#!/usr/bin/env python2
#
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# version 0.1
#
# Grabs Vicon data, records room calibration on a flash
#
# usage: python2 room_calib.py <object name> {--config <file>}
#

from common import *
from subprocess import *
from datetime import datetime
from elementtree.SimpleXMLWriter import XMLWriter
import csv, sys

# args sanity check
args_check("python2 room_calib.py <object name> {--config <file>}")

# set arguments
OBJECT = sys.argv[1]
OUT = find_arg("out", "calib.xml")
CONFIG = find_arg("config", find_cfg())
settings = load_cfg(CONFIG)
CFG_VAR = "settings"

# config load
SERVER_PATH = win_cwd() + settings.get(CFG_VAR, "server_path")
IP_ADDR = settings.get(CFG_VAR, "ip_address")
PORT = settings.get(CFG_VAR, "port")
OUTFILE = win_cwd() + settings.get(CFG_VAR, "output_folder") + "/" + \
          str(datetime.now().strftime(settings.get(CFG_VAR, "date_format"))) + \
          "_calib.xml"
DO_SERVER = settings.get(CFG_VAR, "run_server") == "True"
SERIAL_DEV = settings.get(CFG_VAR, "serial_device")

FRAMERATE = int(settings.get(CFG_VAR, "framerate")) # per second
sleeper = Sleeper(1.0 / FRAMERATE)

# data directory sanity check
check_directory(settings.get(CFG_VAR, "output_folder"))

# start the server
if DO_SERVER:
    server = Popen(SERVER_PATH, shell=True).returncode
    if server > 0:
        print "Couldn't start server"
        quit(1)
else:
    print "Not starting local server"

# start the serial listener
try:
    serial = Serial(SERIAL_DEV, 19200)
except OSError:
    print "Couldn't open serial device", SERIAL_DEV
    quit(1)
else:
    print "Not listening to serial"

# print status
print ""
print "Using config:", CONFIG
print "Waiting for flash...",
sys.stdout.flush() #linux tweaks

# open CSV file
with open(OUTFILE, 'w') as xmlfile:
    
    # open Vicon client
    client = ViconSocket(IP_ADDR, port=int(PORT))
    client.open()
    
    #open xml file
    w = Writer(xmlfile)
    doc = w.start("ViconCalib")
    
    #wait for flash
    while True:
        sleeper.stamp()
        
        if serial.getCTS():
            # grab data
            r = client.get("getRotation", OBJECT)
            t = client.get("getTranslation", OBJECT)
        
            # write data
            if len(r) > 0 and len(t) > 0:
                w.element("timestamp", at=sleeper.getStamp())
                w.element("roomOrigin", x=r[0], y=r[1], z=r[2], pitch=t[0], yaw=t[1], roll=t[2])
                print "got it!"
                break
            
        # sleep until next timestamp
        sleeper.sleep()
        sys.stdout.flush()
    
    w.close(doc)
    client.close()
    
print "\nComplete."
exit(0)

# End of file
