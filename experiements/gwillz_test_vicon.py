#python2
from subprocess import *
from datetime import datetime
import time, csv

CLIENT_PATH = "D:/Vicon DataStream Server/ViconDataStreamClient.exe"
IP_ADDR = "192.168.10.1"
PORT = "802"
OBJECT = "Wand"
OUTFILE = "vicon_" + str(datetime.now().strftime("%Y-%m-%d_%H-%M")) + "_" + OBJECT + ".csv"
TIME_LENGTH = 1
FRAMERATE = 30 # per second
_sleeptime = 1 / FRAMERATE
_timeout = 10 * FRAMERATE

with open(OUTFILE, 'wb') as csvfile:
	w = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
	for i in range(0, _timeout+1):
		time.sleep(_sleeptime)
		rotate = Popen([CLIENT_PATH, IP_ADDR, PORT, "getRotation", OBJECT], stdout=PIPE)
		translate = Popen([CLIENT_PATH, IP_ADDR, PORT, "getTranslation", OBJECT], stdout=PIPE)
		r = rotate.stdout.read()
		t = translate.stdout.read()
		if r != "" and t != "":
			w.writerow(t.split(" ") + r.split(" "))
		
exit(0)