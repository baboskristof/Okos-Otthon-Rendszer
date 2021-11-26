from flaskapp import db, Actions
import datetime
sw1on = Actions.query.filter_by(action='sw1On').all()
sw1off = Actions.query.filter_by(action='sw1Off').all()

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
	return sum([off-on for on, off in pairs], datetime.timedelta())

print(float(sumtime(timepairs(sw1on, sw1off)).total_seconds()))