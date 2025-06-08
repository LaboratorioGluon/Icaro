import os, pty, serial
import serial
import json
import time

tty_path="/dev/pts/3"
ser = serial.Serial(tty_path, 115200)

def getPayload():
    # Sensors
    sensors = {}
    sensors['internalTemp']   = 1
    sensors['externalTemp']   = 2
    sensors['boardTemp']      = 3
    sensors['boardHumidity']  = 4
    sensors['boardBarometer'] = 5
    sensors['imuAcc']         = {1,2,3}
    sensors['imuGyro']        = {4,5,6}
    sensors['GPSLat']         = 1.0
    sensors['GPSLon']         = 2.0
    sensors['GPSAlt']         = 3.0
    # Power
    power = {}
    power['battery'] = 80

    # Payload
    payload = {}
    payload['sensors'] = sensors
    payload['power']   = power
    return json.dumps(payload)

while 1:

    data = "Hello, world!aaa\n"
    ser.write(getPayload())

    time.sleep(1)



ser.close()