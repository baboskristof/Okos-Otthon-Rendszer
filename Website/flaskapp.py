from typing import Text
from flask import Flask, render_template, jsonify
import serial
import numpy as np

arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=.1)
app = Flask(__name__)

random_data = 0

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/animation_1')
def anim1():
    arduino.write(bytes('2', 'utf-8'))
    return ('animation1')

@app.route('/animation_2')
def anim2():
    arduino.write(bytes('6', 'utf-8'))
    return ('animation2')

@app.route('/animation_3')
def anim3():
    arduino.write(bytes('4', 'utf-8'))
    return ('animation3')

@app.route('/brightness_down')
def brdn():
    arduino.write(bytes('*', 'utf-8'))
    return ('brightness down')

@app.route('/brightness_up')
def brup():
    arduino.write(bytes('#', 'utf-8'))
    return ('brightness up')

@app.route('/led_off')
def ledoff():
    arduino.write(bytes('0', 'utf-8'))
    return ('led off')

# @app.route('/up_data', methods=['POST'])
# def updatedata():
#     random_data = np.random.rand()
#     return jsonify('', render_template('test_data_template.html', x = random_data))

if __name__ == '__main__':
    app.run(debug=True, host='192.168.0.30')
    