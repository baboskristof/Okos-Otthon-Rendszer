from datetime import datetime
from flaskapp import db, Actions
allrows = Actions.query.all()
db.session.query(Actions).delete()
db.session.commit()
db.session.add(Actions(time=datetime.now(), action='animation'))
db.session.add(Actions(time=datetime.now(), action='sw1On'))
db.session.add(Actions(time=datetime.now(), action='sw2On'))
db.session.add(Actions(time=datetime.now(), action='anim_off'))
db.session.add(Actions(time=datetime.now(), action='sw1Off'))
db.session.add(Actions(time=datetime.now(), action='sw2Off'))
db.session.commit()