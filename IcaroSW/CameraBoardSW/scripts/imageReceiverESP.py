from scapy.all import *
from scapy.layers.dot11 import Dot11
import time
import threading
import queue
import struct
import tkinter as tk
from PIL import Image, ImageTk

# Configurations
STORE_IMAGE=False

# Interfaz en modo monitor
iface = "wlo1mon" # Cambiar a la interfaz de recepción

# Dirección MAC del ESP32
target_mac = "02:6A:9C:1F:3B:E8".lower()

# Package queue
packetQueue = queue.Queue()
imageQueue = queue.Queue()

# Función para manejar paquetes capturados
def packet_handler(pkt):
    if pkt.haslayer(Dot11) and pkt.type == 2:  # Solo paquetes de datos
        src_mac = pkt.addr2.lower() if pkt.addr2 else None
        if src_mac == target_mac: # Filtrar paquetes del ESP32
            if pkt.haslayer(Raw):
                try:
                    mensaje = pkt[Raw].load
                    packetQueue.put(mensaje)
                    # print(f"📥 Paquete recibido de {pkt.addr2}: {mensaje}")
                except Exception:
                    pass  # Silenciar errores de decodificación

class FragmentHeader:
    STRUCT_FORMAT = '<HHHH'  # Little-endian: 4x uint16_t

    def __init__(self, raw_data):
        header_size = struct.calcsize(self.STRUCT_FORMAT)
        if len(raw_data) < header_size:
            raise ValueError("Datos insuficientes para el encabezado")

        # Desempaquetar los primeros 8 bytes
        self.message_id, self.total_frags, self.frag_index, self.payload_len = struct.unpack(
            self.STRUCT_FORMAT, raw_data[:header_size]
        )

        # Validar longitud de payload
        if len(raw_data) < header_size + self.payload_len:
            raise ValueError("Datos insuficientes para el payload declarado")

        # Extraer payload
        self.payload = raw_data[header_size:header_size + self.payload_len]

    def __repr__(self):
        return (f"<FragmentHeader message_id={self.message_id} total_frags={self.total_frags} "
                f"frag_index={self.frag_index} payload_len={self.payload_len}>")

class MessageAssembler:
    def __init__(self):
        self.messages = {}  # message_id → dict con { 'total', 'received', 'data' }

    def add_fragment(self, fragment: FragmentHeader):
        mid = fragment.message_id

        # Inicializar si es la primera vez que vemos este mensaje
        if mid not in self.messages:
            self.messages[mid] = {
                'total': fragment.total_frags,
                'received': 0,
                'fragments': {}
            }

        msg = self.messages[mid]

        # Evitar sobrescribir si ya se recibió
        if fragment.frag_index not in msg['fragments']:
            msg['fragments'][fragment.frag_index] = fragment.payload
            msg['received'] += 1

        # ¿Está completo?
        if msg['received'] == msg['total']:
            # Reconstruir en orden
            full_payload = b''.join(
                msg['fragments'][i] for i in range(msg['total'])
            )
            if STORE_IMAGE:
                with open(f"debug_{fragment.message_id}.jpg", "wb") as f:
                    f.write(full_payload)
            del self.messages[mid]  # limpiar
            return full_payload  # Mensaje completo listo

        return None  # Aún incompleto

assembler = MessageAssembler()
# Función para recomponer los paquetes capturados
def packet_assembler():
    while True:
        if not packetQueue.empty():
            raw = packetQueue.get()
            fragment = FragmentHeader(raw)
            print("Procesando:", fragment)
            fullPayload = assembler.add_fragment(fragment)
            if fullPayload:
                imageQueue.put(fullPayload)

# Clase para la GUI principal
class ImageViewer:
    def __init__(self, root):
        self.root = root
        self.root.title("Imagen recibida")
        self.root.geometry("640x480")
        self.label = tk.Label(self.root)
        self.label.pack()
        self.root.after(100, self.check_queue)

    def check_queue(self):
        try:
            while not imageQueue.empty():
                img_data = imageQueue.get()
                img = Image.open(io.BytesIO(img_data))
                tk_img = ImageTk.PhotoImage(img)
                self.label.config(image=tk_img)
                self.label.image = tk_img  # evitar garbage collection
        except Exception as e:
            print("Error al mostrar imagen:", e)
        finally:
            self.root.after(100, self.check_queue)

# Función para recomponer los paquetes capturados
def packet_sniffer():
    # Iniciar captura
    print(f"📡 Escuchando en {iface}...")
    sniff(iface=iface, prn=packet_handler, store=0)


# Iniciar hilos procesamiento
threading.Thread(target=packet_sniffer, daemon=True).start()
threading.Thread(target=packet_assembler, daemon=True).start()
# threading.Thread(target=payload_handler, daemon=True).start()

root = tk.Tk()
viewer = ImageViewer(root)
root.mainloop()
