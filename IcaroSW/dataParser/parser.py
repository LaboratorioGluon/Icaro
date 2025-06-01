import struct

dataMap = {
    "timestamp":"Q",
    "pressure":"d",
    "temperature":"d",
    "humidity":"d",
    "acc.x": "f",
    "acc.y": "f",
    "acc.z": "f",
    "acc.time": "f",
    "gyro.x": "f",
    "gyro.y": "f",
    "gyro.z": "f",
    "gyro.time": "f",
    "gps.valid": "B",
    "gps.year": "B",
    "gps.month": "B",
    "gps.day": "B",
    "gps.latitude": "f",
    "gps.longitude": "f",
    "gps.altitude": "f",
    "gps.hour": "B",
    "gps.minute": "B",
    "gps.satellites": "B",
    "gps.dummy": "B",
    "gps.speedKmh": "f",
    "pt100_1": "f",
    "pt100_2": "f",
    "vin": "f",
    "v5voltage": "f",
    "v3v3current": "f",
    "v5current": "f"
}



def parse_data(data):
    offset = 0
    parsed_data = {}
    
    for key, fmt in dataMap.items():
        # change endianness if necessary   
        size = struct.calcsize(fmt)
        value = struct.unpack_from(fmt, data, offset)
        parsed_data[key] = value[0] if len(value) == 1 else value
        offset += size
    
    return parsed_data

if __name__ == "__main__":

    data = open("DATA.BIN", "rb").read()
    parsed_data = parse_data(data)
    for key, value in parsed_data.items():
        print(f"{key}: {value}")
    
    # Example of accessing specific data
    print(f"Timestamp: {parsed_data['timestamp']}")
    print(f"Pressure: {parsed_data['pressure']}")
    print(f"Temperature: {parsed_data['temperature']}")
    # Print humidity
    print(f"Humidity: {parsed_data['humidity']}")
    # Print pt100_1 and pt100_2
    print(f"PT100_1: {parsed_data['pt100_1']}")
    print(f"PT100_2: {parsed_data['pt100_2']}")