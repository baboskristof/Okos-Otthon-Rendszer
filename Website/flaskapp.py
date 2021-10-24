from typing import Text
from flask import Flask, render_template
import serial

arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=.1)
app = Flask(__name__)

@app.route('/')
def hello_world():
    arduino.write(bytes('0', 'utf-8'))
    return render_template('index.html')

@app.route('/1.html')
def one():
    arduino.write(bytes('4', 'utf-8'))
    return render_template('1.html')

@app.route('/navbar.html')
def navbar():
    arduino.write(bytes('4', 'utf-8'))
    return render_template('navbar.html')

@app.route('/2')
def two():
    arduino.write(bytes('5', 'utf-8'))
    return 'two'

if __name__ == '__main__':
    app.run(debug=True, host='192.168.0.30')
    