from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
import serial
from flask import Flask, render_template

arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=.1)
app = Flask(__name__)

app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///flaskdb.db"
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)

class Actions(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    time = db.Column(db.DateTime, unique=False)
    action = db.Column(db.String(10), unique=False, nullable=False)

    def __repr__(self):
        return f'{self.time}|{self.action}'


data = [50, 10, 15, 7, 14]
labels = ['nappali 1', 'nappali 2', 'konyha', 'étkező', 'fürdőszoba']

@app.route('/')
def index():
    return render_template('index.html', values=data, labels=labels, last_action_anim=Actions.query.filter_by(action='animation').order_by(Actions.time.desc()).first().time.strftime('%Y. %b. %d. %H:%M:%S'))

@app.route('/animation_1')
def anim1():
    arduino.write(bytes('1', 'utf-8'))
    db.session.add(Actions(time=datetime.now(), action='animation1'))
    db.session.commit()
    return ('animation1')

@app.route('/animation_2')
def anim2():
    arduino.write(bytes('2', 'utf-8'))
    db.session.add(Actions(time=datetime.now(), action='animation2'))
    db.session.commit()
    return ('animation2')

@app.route('/animation_3')
def anim3():
    arduino.write(bytes('3', 'utf-8'))
    db.session.add(Actions(time=datetime.now(), action='animation3'))
    db.session.commit()
    return ('animation3')

@app.route('/brightness_down')
def brdn():
    arduino.write(bytes('*', 'utf-8'))
    # db.session.add(Actions(time=datetime.now(), action='ledBrDown'))
    # db.session.commit()
    return ('brightness down')

@app.route('/brightness_up')
def brup():
    arduino.write(bytes('#', 'utf-8'))
    # db.session.add(Actions(time=datetime.now(), action='ledBrUp'))
    # db.session.commit()
    return ('brightness up')

@app.route('/led_off')
def ledoff():
    arduino.write(bytes('0', 'utf-8'))
    db.session.add(Actions(time=datetime.now(), action='ledOff'))
    db.session.commit()
    return ('led off')

@app.route('/sw1_on')
def sw1on():
    arduino.write(bytes('sw1On', 'utf-8'))
    db.session.add(Actions(time=datetime.now(), action='sw1On'))
    db.session.commit()
    return ('switch1 on')

@app.route('/sw1_off')
def sw1off():
    arduino.write(bytes('sw1Off', 'utf-8'))
    db.session.add(Actions(time=datetime.now(), action='sw1Off'))
    db.session.commit()
    return ('switch1 off')

if __name__ == '__main__':
    db.create_all()
    app.run(debug=True, host='192.168.0.30')
    