import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections

from icaro import Icaro


if "__main__" == __name__:
    import sys
    if len(sys.argv) != 2:
        print("Usage: python -m test.icaro FILENAME")
        sys.exit(1)

    with open(sys.argv[1], "rb") as f:
        data = f.read()
        io = KaitaiStream(BytesIO(data))
        instance = Icaro(io)
        instance._read()
        print(instance)
        print(instance._debug)
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
        