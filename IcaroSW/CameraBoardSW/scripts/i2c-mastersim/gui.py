#!/usr/bin/python3

import messageGenerator
import requests

def sendSensorsStatusMessage(data):
    pass
    print("Mensaje enviado:", data)
    messageGenerator.sendSensorsStatusMessage(data["ExternalTemp"], data["InternalTemp"], data["OnBoardTemp"], data["Humidity"])

def sendAccelerometerStatusMessage(data):
    pass
    print("Mensaje enviado:", data)
    messageGenerator.sendAccelerometerStatusMessage(data["X"], data["Y"], data["Z"])

def sendGyroscopeStatusMessage(data):
    pass
    print("Mensaje enviado:", data)
    messageGenerator.sendGyroscopeStatusMessage(data["X"], data["Y"], data["Z"])

def sendGPSStatusMessage(data):
    pass
    print("Mensaje enviado:", data)
    if data["Use ISS"]:
        url = "https://api.wheretheiss.at/v1/satellites/25544"
        response = requests.get(url)
        data = response.json()
        messageGenerator.sendGPSStatusMessage(data["latitude"], data["longitude"], data["altitude"])
    else:
        messageGenerator.sendGPSStatusMessage(data["Latitude"], data["Longitude"], data["Altitude"])

import tkinter as tk
from tkinter import ttk

class MessageFrame(ttk.LabelFrame):
    def __init__(self, master, title, fields, send_callback, **kwargs):
        super().__init__(master, text=title, **kwargs)
        self.widgets = {}

        for idx, field in enumerate(fields):
            label = ttk.Label(self, text=field["name"])
            label.grid(row=idx, column=0, sticky="e", padx=5, pady=2)

            field_type = field.get("type", "entry")
            name = field["name"]

            if field_type == "entry":
                widget = ttk.Entry(self)
            elif field_type == "scale":
                widget = tk.Scale(self, from_=field.get("from", 0), to=field.get("to", 100), orient="horizontal", resolution=field.get("resolution", 1.0))
            elif field_type == "check":
                var = tk.BooleanVar()
                widget = ttk.Checkbutton(self, variable=var)
                widget.var = var
            elif field_type == "combo":
                widget = ttk.Combobox(self, values=field.get("values", []))
            else:
                widget = ttk.Entry(self)  # fallback

            widget.grid(row=idx, column=1, padx=5, pady=2)
            self.widgets[name] = widget

        send_button = ttk.Button(self, text="Enviar", command=self.send)
        send_button.grid(row=len(fields), column=0, columnspan=2, pady=5)

        self.send_callback = send_callback

    def send(self):
        data = {}
        for name, widget in self.widgets.items():
            if isinstance(widget, ttk.Entry):
                data[name] = widget.get()
            elif isinstance(widget, tk.Scale):
                data[name] = widget.get()
            elif isinstance(widget, ttk.Checkbutton):
                data[name] = widget.var.get()
            elif isinstance(widget, ttk.Combobox):
                data[name] = widget.get()
            else:
                data[name] = str(widget)
        self.send_callback(data)

def send_message(data):
    print("Mensaje enviado:", data)

def main():
    root = tk.Tk()
    root.title("Mensajes GUI")

    fields_msg4 = [
        {"name": "ExternalTemp", "type": "scale", "from": -50.0, "to": 120.0, "resolution": 0.5},
        {"name": "InternalTemp", "type": "scale", "from": -50.0, "to": 120.0, "resolution": 0.5},
        {"name": "OnBoardTemp", "type": "scale", "from": -50.0, "to": 120.0, "resolution": 0.5},
        {"name": "Humidity", "type": "scale", "from": 0.0, "to": 100.0, "resolution": 0.5},
    ]
    msg4 = MessageFrame(root, "Gyroscope", fields_msg4, send_callback=sendSensorsStatusMessage)
    msg4.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

    fields_msg1 = [
        {"name": "X", "type": "scale", "from": -1.0, "to": 1.0, "resolution": 0.01},
        {"name": "Y", "type": "scale", "from": -1.0, "to": 1.0, "resolution": 0.01},
        {"name": "Z", "type": "scale", "from": -1.0, "to": 1.0, "resolution": 0.01},
    ]
    msg1 = MessageFrame(root, "Accelerometer", fields_msg1, send_callback=sendAccelerometerStatusMessage)
    msg1.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")

    fields_msg2 = [
        {"name": "X", "type": "scale", "from": -1.0, "to": 1.0, "resolution": 0.01},
        {"name": "Y", "type": "scale", "from": -1.0, "to": 1.0, "resolution": 0.01},
        {"name": "Z", "type": "scale", "from": -1.0, "to": 1.0, "resolution": 0.01},
    ]
    msg2 = MessageFrame(root, "Gyroscope", fields_msg2, send_callback=sendGyroscopeStatusMessage)
    msg2.grid(row=0, column=2, padx=10, pady=10, sticky="nsew")

    fields_msg3 = [
        {"name": "Latitude", "type": "scale", "from": -90.0, "to": 90.0, "resolution": 1.0},
        {"name": "Longitude", "type": "scale", "from": -180.0, "to": 180.0, "resolution": 1.0},
        {"name": "Altitude", "type": "scale", "from": 0.0, "to": 25000.0, "resolution": 100.0},
        {"name": "Use ISS", "type": "check"},
    ]
    msg3 = MessageFrame(root, "GPS", fields_msg3, send_callback=sendGPSStatusMessage)
    msg3.grid(row=1, column=0, columnspan=1, padx=10, pady=10, sticky="nsew")

    root.mainloop()

if __name__ == "__main__":
    main()
