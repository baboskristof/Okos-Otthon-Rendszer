import serial
import time
arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=.1)
while True:
    arduino.write(bytes(str(input("INPUT: ")), 'utf-8'))