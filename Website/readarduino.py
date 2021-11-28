import serial

arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=.1)
while True: 
    data = arduino.readline().strip()
    print(data)