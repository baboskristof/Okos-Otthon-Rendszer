import smtplib, ssl

port = 465  # For SSL
smtp_server = "smtp.gmail.com"
sender_email = "bkraspi3@gmail.com"  # Enter your address
receiver_email = "babos.kristof@gmail.com"  # Enter receiver address
password = "vTBk8HkoWbuzgX"
message = f"""\
Subject: Riasztás

A  már több, mint 3 órája be van kapcsolva."""

context = ssl.create_default_context()
with smtplib.SMTP_SSL(smtp_server, port, context=context) as server:
    server.login(sender_email, password)
    server.sendmail(sender_email, receiver_email, message)