from scapy.all import *
from scapy.layers.dot11 import Dot11
import time
import threading
import sys

# This script listens to an interface and outputs the number of 
# received packets from the "target_mac" each second.

# Interface requirements:
#   Interface must be configured in monitor mode. (See scripts 00_showIfaces.sh and 01_initIface.sh)
#   Interface must be set to use the same channel as the emitter. (I use wireshark)

if len(sys.argv) < 2:
    print(f"Usage: python {sys.argv[0]} interface")
    sys.exit(1)

# Interface must be in monitor mode
iface = sys.argv[1]
print(f"Interface to be used: {iface}")

# MAC to be monitorized, ESP32 mac
target_mac = "02:6A:9C:1F:3B:E8".lower()

# Statistics
packet_count = 0
byte_count = 0
lock = threading.Lock()

# Handler for packets received from the interface
def packet_handler(pkt):
    global packet_count, byte_count
    if pkt.haslayer(Dot11) and pkt.type == 2: # Filter data packets
        src_mac = pkt.addr2.lower() if pkt.addr2 else None
        if src_mac == target_mac:
            with lock:
                packet_count += 1
                byte_count += len(pkt)

            if pkt.haslayer(Raw):
                try:
                    # Do whatever with payload
                    payload = pkt[Raw].load
                except Exception:
                    pass  # Ignore errors

# Stats printer
def stats_printer():
    global packet_count, byte_count
    while True:
        time.sleep(1)
        with lock:
            pps = packet_count
            bps = byte_count * 8
            kbps = bps / 1000.0

            print(f"📊 {pps} packets\tThroughput: {kbps:.2f} kbps")

            # Reset count for next cycle
            packet_count = 0
            byte_count = 0

# Start statistics thread
threading.Thread(target=stats_printer, daemon=True).start()

# Start monitoring interface
print(f"📡 Listening to {iface}...")
sniff(iface=iface, prn=packet_handler, store=0)
