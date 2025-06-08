from scapy.all import *
from scapy.layers.dot11 import Dot11
import threading
import queue
import struct
from mjpeg_streamer import MjpegServer, Stream
from PIL import Image
import numpy as np

# This script listens to the interface <arg1> and receives the Images sent by the ESP32
# in custom raw format, then provides these images in a mjpeg stream server.

# Interface requirements:
#   Interface must be configured in monitor mode. (See scripts 00_showIfaces.sh and 01_initIface.sh)
#   Interface must be set to use the same channel as the emitter. (I use wireshark)

# Packets format explained:
# * Packets are received with the following format:
#   - link id                : 4 bits
#   - message id             : 12 bits
#   - number of fragments    : 16 bits 
#   - current fragment index : 16 bits
#   - payload length         : 16 bits
#   - payload                : variable length, max 1468 bytes
# * Each link id is used for a data type:
#   - ESP32 will send images with link id = 1
#   - Other link ids are reserved for other data types
#   - (e.g link id 0 is for testing purposes,
#   -  link ids 2 and 3 are for MAV protocol messages)
# * Images are trasmitted in JPEG format as bytes 

# Check script requirements
if os.geteuid() != 0:
    print("This script shall be run as root")
    sys.exit(1)

if len(sys.argv) < 2:
    print(f"Usage: python {sys.argv[0]} interface")
    sys.exit(1)

# Interface must be in monitor mode
iface = sys.argv[1]
print(f"Interface to be used: {iface}")

# MAC to be monitorized, ESP32 mac
target_mac = "02:6A:9C:1F:3B:E8".lower()
print(f"Listening for messages from: {target_mac}")

# Data queues
packetQueue = queue.Queue()
imageQueue  = queue.Queue()

# Handler for packets received from the interface
def packet_handler(pkt):
    try:
        packetPayload = pkt[Raw].load
        packetQueue.put(packetPayload) # Send to the next stage
    except Exception:
        pass  # Ignore errors

# This class will parse the fragment header of a packet
class FragmentHeader:
    STRUCT_FORMAT = '<HHHH'  # Little-endian: 4x uint16_t

    def __init__(self, raw_data):
        header_size = struct.calcsize(self.STRUCT_FORMAT)
        if len(raw_data) < header_size:
            raise ValueError("Not enough data for header")

        # Unpack the first 8 bytes of the payload:
        # * Link ID (4 bits)
        # * Message ID (12 bits)
        # * Amount of fragments that form the complete message
        # * ID of current fragment
        # * Size of payload in fragment
        self.ids_set, self.total_frags, self.frag_index, self.payload_len = struct.unpack(
            self.STRUCT_FORMAT, raw_data[:header_size]
        )
        self.link_id = (self.ids_set >> 12) & 0xF
        self.message_id = self.ids_set & 0xFFF

        # Validate payload length
        if len(raw_data) < header_size + self.payload_len:
            raise ValueError("Payload length does not match data received")

        # Extract actual payload
        self.payload = raw_data[header_size:header_size + self.payload_len]

    def __repr__(self):
        return (f"<FragmentHeader link_id={self.link_id} message_id={self.message_id} total_frags={self.total_frags} "
                f"frag_index={self.frag_index} payload_len={self.payload_len}>")

# This class manages packets and composes the messages
class MessageAssembler:
    def __init__(self, buffer_len=None):
        self.messages = {}  # message_id → dict con { 'total', 'received', 'data' }
        self.buffer_len = buffer_len

    def add_fragment(self, fragment: FragmentHeader):
        mid = fragment.message_id

        # Remove old frames that will not receive more fragments
        if self.buffer_len is not None:
            self.messages = {k: v for k, v in self.messages.items() if k >= mid-self.buffer_len}

        # Initialize message buffers, if not already initialized 
        if mid not in self.messages:
            self.messages[mid] = {
                'total': fragment.total_frags,
                'received': 0,
                'fragments': {}
            }
        msg = self.messages[mid]

        # Do not override already received fragments
        if fragment.frag_index not in msg['fragments']:
            msg['fragments'][fragment.frag_index] = fragment.payload
            msg['received'] += 1

        # Check if message is fully received
        if msg['received'] == msg['total']:
            # Compose complete payload in order
            full_payload = b''.join(
                msg['fragments'][i] for i in range(msg['total'])
            )

            # Remove buffer for completed message
            del self.messages[mid]
            # Return complete message payload
            return full_payload

        # Return None for non-completed messages
        return None

# Thread to assembly messages
#   This thread receives all packets from and uses the assembler to recompose images
#   When an image is fully received, it is sent to the image viewer
imageAssember = MessageAssembler(10)
def packet_assembler():
    while True:
        if not packetQueue.empty():
            raw = packetQueue.get()
            fragment = FragmentHeader(raw)
            if fragment.link_id == 1: # Raw image link
                print("Image link fragment:", fragment)
                fullPayload = imageAssember.add_fragment(fragment)
                if fullPayload:
                    print("Full image received")
                    imageQueue.put(fullPayload)

# Image streaming server
last_image = None
stream = Stream("icaro_camera", size=(640, 480), quality=50, fps=20)
server = MjpegServer("localhost", 48485)
server.add_stream(stream)

# Thread to update the latest received image and provide it to the streaming server
def image_forwarder():
    global last_image
    while True:
        if not imageQueue.empty():
            try:
                img_data = imageQueue.get()
                img = Image.open(io.BytesIO(img_data)).convert('RGB')
                image = np.array(img)[:, :, ::-1].copy()
                stream.set_frame(image)
            except Exception as e:
                print(f"Unable to convert image: {e}")
            finally:
                pass

# Thread to receive data from the interface 
def interface_sniffer():
    print(f"📡 Listening to {iface}...")
    sniff(iface=iface, prn=packet_handler, store=0, filter=f"ether src {target_mac}")


# Start program
if __name__=="__main__":
    # Initialize threads
    threads = {}
    threads["image_forwarder"] = threading.Thread(target=image_forwarder, daemon=True)
    threads["packet_assembler"] = threading.Thread(target=packet_assembler, daemon=True)
    threads["interface_sniffer"] = threading.Thread(target=interface_sniffer, daemon=True)
    
    threads["image_forwarder"].start()
    threads["packet_assembler"].start()
    threads["interface_sniffer"].start()

    # Start streaming server
    server.start()
    
    # Supervise threads to keep them running
    while True:
        for name, thread in list(threads.items()):
            if not thread.is_alive():
                print(f"Thread '{name}' stopped. Restarting...")
    
                if name == "image_forwarder":
                    threads[name] = threading.Thread(target=image_forwarder, daemon=True)
                elif name == "packet_assembler":
                    threads[name] = threading.Thread(target=packet_assembler, daemon=True)
                elif name == "interface_sniffer":
                    threads[name] = threading.Thread(target=interface_sniffer, daemon=True)

                threads[name].start()
                print(f"Restarted thread: {name}")
        time.sleep(2)
