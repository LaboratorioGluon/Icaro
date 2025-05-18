#!/usr/bin/python3

import struct
import sys
import time
from datetime import datetime
from smbus2 import SMBus, i2c_msg

# Variables
useCustomMessages = False

def sendI2C(data):
    I2C_BUS        = 1
    DEVICE_ADDRESS = 0x28
    REGISTER       = 0x00
    print(f"Sending message: {data}")
    message = i2c_msg.write(DEVICE_ADDRESS, data)
    with SMBus(I2C_BUS) as bus:
        bus.i2c_rdwr(message)

def sendMessage(tag, payload):
    timestamp   = int(time.time())
    source      = 42 
    length      = len(payload)
    dummy       = 0xA5

    header = struct.pack('<QBBBB',
                         timestamp,
                         tag,
                         source,
                         length,
                         dummy)
    data = header + payload
    sendI2C(data)

def sendBoardStatusMessage(boardStatus=1, commsStatus=2, powerStatus=3):
    global useCustomMessages

    tag = 1 # Status TAG
    if useCustomMessages:
        pass # TODO: Ask for values

    payload = struct.pack('<BBB', boardStatus, commsStatus, powerStatus)

    sendMessage(tag, payload) 

def sendPowerStatusMessage(status3v3=1, status5v0=1,batteryLevel=100.0):
    global useCustomMessages

    tag = 2 # Power TAG
    if useCustomMessages:
        pass # TODO: Ask for values

    payload = struct.pack('<BBf', status3v3, status5v0, batteryLevel)

    sendMessage(tag, payload) 
    
def sendSensorsStatusMessage(externalTemp=10.0, internalTemp=20.0, onBoardTemp=30.0, humidity=40.0):
    global useCustomMessages

    tag = 3 # SENSORS TAG
    if useCustomMessages:
        pass # TODO: Ask for values

    payload = struct.pack('<ffff', externalTemp, internalTemp, onBoardTemp, humidity)

    sendMessage(tag, payload) 

def sendAccelerometerStatusMessage(x=0.0, y=0.0, z=0.0):
    global useCustomMessages

    tag = 4 # Accelerometer TAG
    if useCustomMessages:
        pass # TODO: Ask for values

    payload = struct.pack('<fff', x, y, z)

    sendMessage(tag, payload) 

def sendGyroscopeStatusMessage(x=0.0, y=0.0, z=0.0):
    global useCustomMessages

    tag = 5 # Gyroscope TAG
    if useCustomMessages:
        pass # TODO: Ask for values

    payload = struct.pack('<fff', x, y, z)

    sendMessage(tag, payload) 

def sendGPSStatusMessage(lat=0.0, lon=0.0, alt=0.0):
    global useCustomMessages

    tag = 6 # GPS TAG
    if useCustomMessages:
        pass # TODO: Ask for values

    payload = struct.pack('<ddd', lat, lon, alt)

    sendMessage(tag, payload) 

def sendTimeStatusMessage(year=2025, month=1, day=1, hour=0, minute=0, sec=0, msec=0):
    global useCustomMessages

    tag = 7 # Time TAG
    if useCustomMessages:
        pass # TODO: Ask for values

    payload = struct.pack('<HBBBBBH', year, month, day, hour, minute, sec, msec)

    sendMessage(tag, payload) 

if __name__=="__main__":
    print ("Icaro I2C simulator")
    
    def printMenu():
        global useCustomMessages
        print("Select message to send:")
        print(" A) Board status message")
        print(" B) Power status message")
        print(" C) Sensors status message")
        print(" D) Accelerometer status message")
        print(" E) Gyroscope status message")
        print(" F) GPS status message")
        print(" G) Time status message")
        if useCustomMessages:
            print(" *) Change to default messages")
        else:
            print(" *) Change to custom messages")
        print(" .) exit")
    
        option = input()
        if option == 'A' or option == 'a':
            sendBoardStatusMessage()
        elif option == 'B' or option == 'b':
            sendPowerStatusMessage()
        elif option == 'C' or option == 'c':
            sendSensorsStatusMessage()
        elif option == 'D' or option == 'd':
            sendAccelerometerStatusMessage()
        elif option == 'E' or option == 'e':
            sendGyroscopeStatusMessage()
        elif option == 'F' or option == 'f':
            sendGPSStatusMessage()
        elif option == 'G' or option == 'g':
            now = datetime.now()
            sendTimeStatusMessage(now.year, now.month, now.day, now.hour, now.minute, now.second, int(now.microsecond/1000))
        elif option == '*':
            useCustomMessages = not useCustomMessages
        elif option == '.':
            exit()

    while 1:
        printMenu()
