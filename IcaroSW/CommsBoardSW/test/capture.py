import serial


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

            if packet and onlyone:
                print(line)
                f.write(packet)
                packet = None
                onlyone = False
                break
                # Here you can add code to process the line, e.g., parse it or save it to a file