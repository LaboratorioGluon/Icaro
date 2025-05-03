from scapy.all import *
from scapy.layers.dot11 import Dot11
import time
import threading
import queue
import struct
import tkinter as tk
from PIL import Image, ImageTk

# This script listens to an interface and displays the image received from 
# an ESP32 with an specific 802.11 message format.

# Interface requirements:
#   Interface must be configured in monitor mode. (See scripts 00_showIfaces.sh and 01_initIface.sh)
#   Interface must be set to use the same channel as the emitter. (I use wireshark)

# Script Configurations
STORE_IMAGE=False # Set to 'True' to store each received frame as a jpg file 

if len(sys.argv) < 2:
    print(f"Usage: python {sys.argv[0]} interface")
    sys.exit(1)

# Interface must be in monitor mode
iface = sys.argv[1]
print(f"Interface to be used: {iface}")

# MAC to be monitorized, ESP32 mac
target_mac = "02:6A:9C:1F:3B:E8".lower()

# Data queues
packetQueue = queue.Queue()
imageQueue = queue.Queue()

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
        # * Message ID
        # * Amount of fragments that form the complete message
        # * ID of current fragment
        # * Size of payload in fragment
        self.message_id, self.total_frags, self.frag_index, self.payload_len = struct.unpack(
            self.STRUCT_FORMAT, raw_data[:header_size]
        )

        # Validate payload length
        if len(raw_data) < header_size + self.payload_len:
            raise ValueError("Payload length does not match data received")

        # Extract actual payload
        self.payload = raw_data[header_size:header_size + self.payload_len]

    def __repr__(self):
        return (f"<FragmentHeader message_id={self.message_id} total_frags={self.total_frags} "
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
            # Store image if enabled
            if STORE_IMAGE:
                with open(f"debug_{fragment.message_id}.jpg", "wb") as f:
                    f.write(full_payload)

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
            print("Processing:", fragment)
            fullPayload = assembler.add_fragment(fragment)
            if fullPayload:
                imageQueue.put(fullPayload)

# Image viewer to display received images
class ImageViewer:
    def __init__(self, root):
        self.root = root
        self.root.title("Latest image received")
        self.root.geometry("640x510")
        self.label = tk.Label(self.root)
        self.label.pack()
        
        # FPS counter
        self.last_time = time.time()
        self.frame_count = 0
        self.fps = 0
        self.fps_label = tk.Label(self.root, text="FPS: 0", font=("Arial", 12))
        self.fps_label.pack()

        self.root.after(100, self.check_queue)

    def check_queue(self):
        try:
            while not imageQueue.empty():
                img_data = imageQueue.get()
                img = Image.open(io.BytesIO(img_data))
                tk_img = ImageTk.PhotoImage(img)
                self.label.config(image=tk_img)
                self.label.image = tk_img
                self.frame_count += 1

            # Calculate images received every second
            current_time = time.time()
            if current_time - self.last_time >= 1.0:
                self.fps = self.frame_count
                self.frame_count = 0
                self.last_time = current_time
                self.fps_label.config(text=f"FPS: {self.fps}")

        except Exception as e:
            print("Error al mostrar imagen:", e)
        finally:
            self.root.after(100, self.check_queue)

# Thread to receive packets from the interface
def packet_sniffer():
    # Start monitoring interface
    print(f"📡 Listening to {iface}...")
    sniff(iface=iface, prn=packet_handler, store=0)

# Start processing threads
threading.Thread(target=packet_sniffer, daemon=True).start()
threading.Thread(target=packet_assembler, daemon=True).start()

# Start GUI
root = tk.Tk()
viewer = ImageViewer(root)
root.mainloop()
