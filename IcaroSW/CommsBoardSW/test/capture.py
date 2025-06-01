import serial


with open("icaro.bin","wb") as f:
    with serial.Serial('/dev/ttyACM1', 115200, timeout=1) as ser:
        while True:
            line = ser.readline()
            if line:
                print(line)
                f.write(line)
                # Here you can add code to process the line, e.g., parse it or save it to a file