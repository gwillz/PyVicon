#!/usr/bin/env python2
# Project Eagle Eye
# UniSA 2015
#
# usage: python2 get_vicon.py <object_name> <time in minutes>

from subprocess import *
from datetime import datetime
import time, csv, sys, os

#variables
OBJECT = sys.argv[1]
TIME_LENGTH = int(sys.argv[2]) # in minutes

#constants
CLIENT_PATH = os.getcwd().replace("\\", "/") + "/vicon_binaries/ViconDataStreamClient.exe"
SERVER_PATH = os.getcwd().replace("\\", "/") + "/vicon_binaries/ViconDataStreamServer.exe"
IP_ADDR = "192.168.10.1"
PORT = "802"
OUTFILE = "vicon_" + str(datetime.now().strftime("%Y-%m-%d_%H-%M")) + "_" + OBJECT + ".csv"
FRAMERATE = 30 # per second

_sleeptime = 1 / FRAMERATE
_timeout = 5 * FRAMERATE

# start the server
server = Popen(SERVER_PATH, shell=True).returncode

if server > 0:
	print "Couldn't start server"
	exit(1)

# open CSV file
with open(OUTFILE, 'wb') as csvfile:
	w = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
	
	# main loop
	for i in range(0, _timeout):
		time.sleep(_sleeptime)
		
		# grab data
		rotate = Popen([CLIENT_PATH, IP_ADDR, PORT, "getRotation", OBJECT], stdout=PIPE)
		translate = Popen([CLIENT_PATH, IP_ADDR, PORT, "getTranslation", OBJECT], stdout=PIPE)
		r = rotate.stdout.read()
		t = translate.stdout.read()
		
		#write data
		if r != "" and t != "":
			w.writerow([time.clock()] + t.split(" ") + r.split(" "))
			sys.stdout.write(".")
		
exit(0)