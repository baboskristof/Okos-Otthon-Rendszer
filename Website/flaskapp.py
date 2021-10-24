from typing import Text
from flask import Flask, render_template, jsonify
import serial
import numpy as np

arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=.1)
app = Flask(__name__)

random_data = 0

@app.route('/')
def hello_world():
    arduino.write(bytes('0', 'utf-8'))
    return render_template('index.html')

@app.route('/up_data', methods=['POST'])
def updatedata():
    random_data = np.random.rand()
    return jsonify('', render_template('test_data_template.html', x = random_data))

if __name__ == '__main__':
    app.run(debug=True, host='192.168.0.30')
    