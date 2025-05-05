from scapy.all import *
from scapy.layers.dot11 import Dot11
import time
import threading
import queue
import struct
import socket
from pymavlink.dialects.v20 import common as mavlink2

# This script listens to an interface and displays processes the MAV
# messages received from an ESP32 with an specific 802.11 message format.

# Interface requirements:
#   Interface must be configured in monitor mode. (See scripts 00_showIfaces.sh and 01_initIface.sh)
#   Interface must be set to use the same channel as the emitter. (I use wireshark)

if os.geteuid() != 0:
    print("This script shall be run as root")
    sys.exit(1)

# if len(sys.argv) < 2:
#     print(f"Usage: python {sys.argv[0]} interface")
#     sys.exit(1)


# Interface must be in monitor mode
# iface = sys.argv[1]
iface = "wlo1mon"
print(f"Interface to be used: {iface}")

# MAC to be monitorized, ESP32 mac
target_mac = "02:6A:9C:1F:3B:E8".lower()

# Data queues
packetQueue = queue.Queue()
mavQueue = queue.Queue()

# Handler for packets received from the interface
def packet_handler(pkt):
    if pkt.haslayer(Dot11) and pkt.type == 2: # Filter data packets
        src_mac = pkt.addr2.lower() if pkt.addr2 else None
        if src_mac == target_mac: # Filtrar paquetes del ESP32
            if pkt.haslayer(Raw):
                try:
                    packetPayload = pkt[Raw].load
                    packetQueue.put(packetPayload) # Send to the next stage
                except Exception:
                    pass  # Ignore errors

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

class MessageAssembler:
    def __init__(self):
        self.messages = {}  # message_id → dict con { 'total', 'received', 'data' }

    def add_fragment(self, fragment: FragmentHeader):
        mid = fragment.message_id

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
assembler = MessageAssembler()
def packet_assembler():
    while True:
        if not packetQueue.empty():
            raw = packetQueue.get()
            fragment = FragmentHeader(raw)
            if fragment.link_id == 1: # Raw image link
                pass # Ignore
            if fragment.link_id == 2: # MAV link
                print("Processing:", fragment)
                fullPayload = assembler.add_fragment(fragment)
                if fullPayload:
                    mavQueue.put(fullPayload)

class fifo(object):
    def __init__(self):
        self.buf = []
    def write(self, data):
        self.buf += data
        return len(data)
    def read(self):
        return self.buf.pop(0)
    

# Thread to forward messages to the MAV recepient application
def mav_forwarder():
    host = '127.0.0.1'  # MAV server ip
    port = 48484        # MAV server port
    f = fifo()
    while True:
        mav = mavlink2.MAVLink(f)
        if not mavQueue.empty():
            mavMessage = mavQueue.get()
            # try:
            #     m2 = mav.decode(mavMessage)
            #     print("Got a message with id %u and fields %s" % (m2.get_msgId(), m2.get_fieldnames()))
            # except Exception as ex:
            #     print(f"Unable to parse mavMessage: ", ex)
            # finally:
            #     pass

            try:
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                    s.connect((host, port))
                    s.sendall(mavMessage)
                    respuesta = s.recv(1024)
                    print('Server response:', respuesta.decode())
            except:
                print(f"Unable to forward mavMessage: MAV connection error")
            finally:
                pass

# Start processing threads
threading.Thread(target=packet_assembler, daemon=True).start()
threading.Thread(target=mav_forwarder, daemon=True).start()

# Start monitoring interface
print(f"📡 Listening to {iface}...")
sniff(iface=iface, prn=packet_handler, store=0)