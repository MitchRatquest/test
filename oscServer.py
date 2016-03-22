"""OSC Test Script
Written by Aaron Chamberlain Dec. 2013
The purpose of this script is to make a very simple communication structure to the popular 
application touchOSC. This is achieved through the pyOSC library. However, since the pyOSC 
documentation is scarce and only one large example is included, I am going to strip down 
the basic structures of that file to implement a very simple bi-directional communication.
"""

#!/usr/bin/env python

import socket, OSC, re, time, threading, math, subprocess

receive_address = '192.168.0.13', 8000 #Mac Adress, Outgoing Port
send_address = '192.168.0.35', 9000 #iPhone Adress, Incoming Port

class PiException(Exception):
	def __init__(self, value):
		self.value = value
	def __str__(self):
		return repr(self.value)

##########################
#	OSC
##########################

# Initialize the OSC server and the client.
s = OSC.OSCServer(receive_address)
c = OSC.OSCClient()
c.connect(send_address)

s.addDefaultHandlers()

# define a message-handler function for the server to call.
def test_handler(addr, tags, stuff, source):
	print "---"
	print "received new osc msg from %s" % OSC.getUrlStr(source)
	print "with addr : %s" % addr
	print "typetags %s" % tags
	print "data %s" % stuff
	msg = OSC.OSCMessage()
	msg.setAddress(addr)
	msg.append(stuff)
	c.send(msg)
	print "return message %s" % msg
	print "---"

def moveStop_handler(add, tags, stuff, source):
	addMove(0,0)

def moveJoystick_handler(add, tags, stuff, source):
	print "message received:"
	msg = OSC.OSCMessage()
	msg.setAddress(addr)
	msg.append(stuff)
	c.send(msg)
	print "X Value is: " 
	print stuff[0] 
	print "Y Value is: " 
	print stuff[1]  #stuff is a 'list' variable
def jingo(a, b, c, d):
    #print a
    #print b
    test = c[0]
    test *= 100
    test = int(test)
    print test

def redPWM(add, tags, stuff, source):
	red = stuff[0]
	red *= 100
	red = int(red)
	print red
def greenPWM(add, tags, stuff, source):
	green = stuff[0]
	green *= 100
	green = int(green)
	print green
def bluePWM(add, tags, stuff, source):
	blue = stuff[0]
	blue *= 100
	blue = int(blue)
	print blue
def manualMode(add, tags, stuff, source):
	print "manual mode placeholder"
def autoMode(a, b, c, d):
	print "auto mode placeholder"
	if int(c[0]) == 1:
		subprocess.call('echo "0" > /sys/class/gpio/gpio4/value', shell=True)
def wakeup(a, b, c, d):
	print "i am awake"
	if int(c[0]) == 1:
		subprocess.call('echo "1" > /sys/class/gpio/gpio4/value', shell=True)
	#else:
	#	subprocess.call('echo "0" > /sys/class/gpio/gpio4/value', shell=True)

# adding my functions
s.addMsgHandler("/basic/stop", moveStop_handler)
s.addMsgHandler("/basic/joystick", moveJoystick_handler)
s.addMsgHandler("/1/fader1", jingo)
s.addMsgHandler("/1/red", redPWM)
s.addMsgHandler("/1/green", greenPWM)
s.addMsgHandler("/1/blue", bluePWM)
s.addMsgHandler("/1/manual", manualMode)
s.addMsgHandler("/1/auto", autoMode)
s.addMsgHandler("/1/wakeup", wakeup)
# just checking which handlers we have added
print "Registered Callback-functions are :"
for addr in s.getOSCAddressSpace():
	print addr

# Start OSCServer
print "\nStarting OSCServer. Use ctrl-C to quit."
st = threading.Thread( target = s.serve_forever )
st.start()

#FIRST THINGS FIRST LETS MAKE SURE THE IO PIN IS A FILE

subprocess.call('echo 4 > /sys/class/gpio/export', shell=True)


# Loop while threads are running.
try :
	while 1 :
		time.sleep(10)
 
except KeyboardInterrupt :
	print "\nClosing OSCServer."
	s.close()
	print "Waiting for Server-thread to finish"
	st.join()
	print "Done"
