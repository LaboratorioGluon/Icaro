from scapy.all import *
from scapy.layers.dot11 import Dot11
import threading
import queue
import struct
import socket
#from mjpeg_streamer import MjpegServer, Stream
from PIL import Image
import numpy as np
from crcmod.predefined import mkCrcFun

from local_mavlink import *

import serial
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections

from icaro import Icaro

# This script listens to the interface <arg1> and receives the MAV messages sent by the ESP32
# in custom raw format, then forwards the MAV message to a TCP server.

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
#   - ESP32 will send MAV messages with the following Link ids:
#     - Link id 1 is for jpeg image messages (no MAV)
#     - Link id 2 is for status messages (MAV protocol)
#     - Link id 3 is for data messages (MAV  protocol)
#   - Other link ids are reserved for other data types
#   - (e.g link id 0 is for testing purposes,

# Check script requirements
#if os.geteuid() != 0:
#    print("This script shall be run as root")
#    sys.exit(1)

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
packetQueue    = queue.Queue()
mavStatusQueue = queue.Queue()
mavDataQueue   = queue.Queue()
imageQueue     = queue.Queue()

# Handler for packets received from the interface
def packet_handler(pkt):
    # if pkt.haslayer(Dot11) and pkt.type == 2: # Filter data packets
        # src_mac = pkt.addr2.lower() if pkt.addr2 else None
        # if src_mac == target_mac: # Filtrar paquetes del ESP32
            # if pkt.haslayer(Raw):
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
imageAssember      = MessageAssembler(10)
mavStatusAssembler = MessageAssembler(5)
mavDataAssember    = MessageAssembler(5)
def packet_assembler():
    while True:
        if not packetQueue.empty():
            raw = packetQueue.get()
            fragment = FragmentHeader(raw)
            if fragment.link_id == 1: # JPEG image link
                print("Image link fragment:", fragment)
                fullPayload = imageAssember.add_fragment(fragment)
                if fullPayload:
                    print("Full image received")
                    imageQueue.put(fullPayload)
            elif fragment.link_id == 2: # MAV status link
                print("MAV data link fragment:", fragment)
                fullPayload = mavStatusAssembler.add_fragment(fragment)
                if fullPayload:
                    mavStatusQueue.put(fullPayload)
            elif fragment.link_id == 3: # MAV data link
                print("MAV data link fragment:", fragment)
                fullPayload = mavDataAssember.add_fragment(fragment)
                if fullPayload:
                    mavDataQueue.put(fullPayload)

# Image streaming server
last_image = None
#stream = Stream("icaro_camera", size=(640, 480), quality=50, fps=20)
#image_server = MjpegServer("localhost", 48485)
#image_server.add_stream(stream)

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

# Thread to forward messages to the MAV recepient application
def mav_forwarder():
    host = '127.0.0.1'  # MAV server ip
    port = 48484        # MAV server port

    while True:
        if not mavStatusQueue.empty():
            mavStatusMessage = mavStatusQueue.get()
            try:
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                    s.connect((host, port))
                    s.sendall(mavStatusMessage)
            except:
                print(f"Unable to forward mavMessage: MAV connection error")
            finally:
                pass

        if not mavDataQueue.empty():
            mavDataMessage = mavDataQueue.get()
            try:
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                    s.connect((host, port))
                    s.sendall(mavDataMessage)
            except:
                print(f"Unable to forward mavMessage: MAV connection error")
            finally:
                pass

# Thread to receive data from the interface 
def interface_sniffer():
    print(f"📡 Listening to {iface}...")
    # sniff(iface=iface, prn=packet_handler, store=0)
    sniff(iface=iface, prn=packet_handler, store=0, filter=f"ether src {target_mac}")



def interface_lora():
    alldata = []
    packet = None
    onlyone = True

    with open("icaro.bin","wb") as f:
        with serial.Serial('/dev/ttyACM0', 115200, timeout=1) as ser:
            while True:
                
                alldata.append(ser.read())            

                line = b''.join(alldata)
                        
                if b"Radio Data:\t\t" in line and b"Radio RSSI:" in line.split(b"Radio Data:\t\t")[1]:
                    packet = line.split(b"Radio Data:\t\t")[1].split(b"Radio RSSI:")[0].strip()

                if packet:
                    alldata = []
                    print(line)
                    #f.write(packet)

                    io = KaitaiStream(BytesIO(packet))
                    instance = Icaro(io)
                    instance._read()

                    print("Timestamp:", instance.timestamp)
                    print("BME280 Temperature:", instance.bme280.temperature)
                    print("BME280 Pressure:", instance.bme280.pressure)
                    print("BME280 Humidity:", instance.bme280.humidity)
                    print("Accelerometer X:", instance.acc.x)
                    print("Accelerometer Y:", instance.acc.y)
                    print("Accelerometer Z:", instance.acc.z)
                    print("Gyroscope X:", instance.gyro.x)

                    print("Gyroscope Y:", instance.gyro.y)
                    print("Gyroscope Z:", instance.gyro.z)
                    print("GPS Valid:", instance.gps.valid)
                    print("GPS Year:", instance.gps.year)
                    print("GPS Month:", instance.gps.month)
                    print("GPS Day:", instance.gps.day)
                    print("GPS Latitude:", instance.gps.latitude)
                    print("GPS Longitude:", instance.gps.longitude)
                    print("GPS Altitude:", instance.gps.altitude)
                    print("GPS Hour:", instance.gps.hour)
                    print("GPS Minute:", instance.gps.minute)
                    print("GPS Satellite:", instance.gps.satellite)
                    print("GPS Dummy:", instance.gps.dummy)
                    print("GPS Speed:", instance.gps.speed)
                    print("PT100 External:", instance.pt100_ext)
                    print("PT100 Internal:", instance.pt100_int)
                    print("VIN:", instance.vin)
                    print("V5:", instance.v5)
                    print("Current V3:", instance.currentv3)
                    print("Current V5:", instance.currentv5)
                    print("Supply Temp", instance.supplytemp)

                    link = MAVLink(f)  # RAW_IMU message id is 27
                    rawimu = MAVLink_raw_imu_message(instance.timestamp,
                                                     int(instance.acc.x*1000.0),
                                                     int(instance.acc.y*1000.0),
                                                     int(instance.acc.z*1000.0),
                                                     int(instance.gyro.x*1000.0),
                                                     int(instance.gyro.y*1000.0),
                                                     int(instance.gyro.z*1000.0),
                                                     0, 0, 0,  # Magnetometer data set to zero)
                    )
                    mavDataQueue.put(rawimu.pack(link))

                    gps_fix = 3 if instance.gps.valid == 1 else 0
                    gps2 = MAVLink_gps2_raw_message(instance.timestamp, gps_fix, instance.gps.latitude,
                                             instance.gps.longitude, instance.gps.altitude,
                                             0xFFFF, 0xFFFF, instance.gps.speed, instance.gps.satellite,
                                             0, 0)
                    mavDataQueue.put(gps2.pack(link))
                    packet = None


    
# Start program
if __name__=="__main__":
    # Initialize threads
    threads = {}

    #threads["image_forwarder"] = threading.Thread(target=image_forwarder, daemon=True)
    threads["mav_forwarder"] = threading.Thread(target=mav_forwarder, daemon=True)
    threads["packet_assembler"] = threading.Thread(target=packet_assembler, daemon=True)
    threads["interface_sniffer"] = threading.Thread(target=interface_sniffer, daemon=True)
    threads["interface_lora"] = threading.Thread(target=interface_lora, daemon=True)
    
    #threads["image_forwarder"].start()
    threads["mav_forwarder"].start()
    threads["packet_assembler"].start()
    threads["interface_sniffer"].start()
    threads["interface_lora"].start()

    # Start streaming server
    #image_server.start()

    # Supervise threads to keep them running
    while True:
        for name, thread in list(threads.items()):
            if not thread.is_alive():
                print(f"Thread '{name}' stopped. Restarting...")
    
                #if name == "image_forwarder":
                #    threads[name] = threading.Thread(target=image_forwarder, daemon=True)
                if name == "mav_forwarder":
                    threads[name] = threading.Thread(target=mav_forwarder, daemon=True)
                elif name == "packet_assembler":
                    threads[name] = threading.Thread(target=packet_assembler, daemon=True)
                elif name == "interface_sniffer":
                    threads[name] = threading.Thread(target=interface_sniffer, daemon=True)
                elif name == "interface_lora":
                    threads[name] = threading.Thread(target=interface_lora, daemon=True)

                threads[name].start()
                print(f"Restarted thread: {name}")
        time.sleep(2)
