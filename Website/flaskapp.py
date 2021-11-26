from flask_sqlalchemy import SQLAlchemy
from datetime import datetime, timedelta
import serial
from flask import Flask, render_template
import smtplib, ssl
import time
import atexit
from apscheduler.schedulers.background import BackgroundScheduler

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

def timepairs(on_query, off_query):
	'''returns a list of tuples, each tuple is (on_time, off_time)'''
	pairs = []
	for on in on_query:
		ontime = on.time
		for off in off_query:
			offtime= off.time
			if offtime > ontime: break
		pairs.append((ontime, offtime))
	return pairs

def sumtime(pairs):
	return sum([off-on for on, off in pairs], timedelta())

mail_sent = True
def send_email():
	last_on_anim=Actions.query.filter_by(action='animation').order_by(Actions.time.desc()).first().time
	last_off_anim=Actions.query.filter_by(action='anim_off').order_by(Actions.time.desc()).first().time
	global mail_sent
	timeD=timedelta(seconds=15)
	if last_off_anim > last_on_anim: 
		mail_sent = False
	else:
		if (last_on_anim + timeD < datetime.now()) and mail_sent == False:
			print('mail sent!')
			mail_sent = True
			port = 465  # For SSL
			smtp_server = "smtp.gmail.com"
			sender_email = "bkraspi3@gmail.com"  # Enter your address
			receiver_email = "babos.kristof@gmail.com"  # Enter receiver address
			password = "vTBk8HkoWbuzgX"
			message = f"""\
			Subject: Riasztas


			A LED mar tobb, mint 3 oraja be van kapcsolva."""

			context = ssl.create_default_context()
			with smtplib.SMTP_SSL(smtp_server, port, context=context) as server:
				server.login(sender_email, password)
				server.sendmail(sender_email, receiver_email, message)
	

@app.route('/')
def index():
	sw1pairs = timepairs(Actions.query.filter_by(action='sw1On').all(),Actions.query.filter_by(action='sw1Off').all())
	sw2pairs = timepairs(Actions.query.filter_by(action='sw2On').all(),Actions.query.filter_by(action='sw2Off').all())
	animpairs = timepairs(Actions.query.filter_by(action='animation').all(),Actions.query.filter_by(action='anim_off').all())
	data = [sumtime(animpairs).total_seconds(),sumtime(sw1pairs).total_seconds(),sumtime(sw2pairs).total_seconds()]
	labels = ['LED szalag', 'Switch 1', 'Switch 2']
	print(data)
	return render_template('index.html', values=data, labels=labels,
						   last_action_anim=Actions.query.filter_by(action='animation').order_by(
							   Actions.time.desc()).first().time.strftime('%Y. %b. %d. %H:%M:%S'),
						   last_action_sw1=Actions.query.filter(Actions.action.like('sw1%')).order_by(
							   Actions.time.desc()).first().time.strftime('%Y. %b. %d. %H:%M:%S'),
						   last_action_sw2=Actions.query.filter(Actions.action.like('sw2%')).order_by(Actions.time.desc()).first().time.strftime('%Y. %b. %d. %H:%M:%S'))

scheduler = BackgroundScheduler()
scheduler.add_job(func = send_email, trigger = "interval", seconds = 5)
scheduler.start()

# Shut down the scheduler when exiting the app
atexit.register(lambda: scheduler.shutdown())

@app.route('/animation_1')
def anim1():
	arduino.write(bytes('1', 'utf-8'))
	db.session.add(Actions(time=datetime.now(), action='animation'))
	db.session.commit()
	return ('animation1')


@app.route('/animation_2')
def anim2():
	arduino.write(bytes('2', 'utf-8'))
	db.session.add(Actions(time=datetime.now(), action='animation'))
	db.session.commit()
	return ('animation2')


@app.route('/animation_3')
def anim3():
	arduino.write(bytes('3', 'utf-8'))
	db.session.add(Actions(time=datetime.now(), action='animation'))
	db.session.commit()
	return ('animation3')


@app.route('/brightness_down')
def brdn():
	arduino.write(bytes('*', 'utf-8'))
	db.session.add(Actions(time=datetime.now(), action='anim_br'))
	db.session.commit()
	return ('brightness down')


@app.route('/brightness_up')
def brup():
	arduino.write(bytes('#', 'utf-8'))
	db.session.add(Actions(time=datetime.now(), action='anim_br'))
	db.session.commit()
	return ('brightness up')


@app.route('/led_off')
def ledoff():
	arduino.write(bytes('0', 'utf-8'))
	db.session.add(Actions(time=datetime.now(), action='anim_off'))
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


@app.route('/sw2_on')
def sw2on():
	arduino.write(bytes('sw2On', 'utf-8'))
	db.session.add(Actions(time=datetime.now(), action='sw2On'))
	db.session.commit()
	return ('switch2 on')


@app.route('/sw2_off')
def sw2off():
	arduino.write(bytes('sw2Off', 'utf-8'))
	db.session.add(Actions(time=datetime.now(), action='sw2Off'))
	db.session.commit()
	return ('switch2 off')


if __name__ == '__main__':
	db.create_all()
	app.run(debug=True, host='192.168.0.30')
