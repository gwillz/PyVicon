#!/usr/bin/env python2
#
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
version =  "0.2.4"
#
# Grabs Vicon data, records room calibration on a flash
#
# usage: python2 room_calib.py <object name> {--config <file>}
#

from eagleeye import ViconSocket, Sleeper
from eagleeye.common import *
from datetime import datetime
from elementtree.SimpleXMLWriter import XMLWriter
from serial import Serial
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
SERIAL_DEV = settings.get(CFG_VAR, "serial_device")

FRAMERATE = int(settings.get(CFG_VAR, "framerate")) # per second
sleeper = Sleeper(1.0 / FRAMERATE)

# data directory sanity check
check_directory(settings.get(CFG_VAR, "output_folder"))

# start the serial listener
try:
    serial = Serial(SERIAL_DEV, 19200)
except OSError:
    print "Couldn't open serial device", SERIAL_DEV
    quit(1)

# print status
print ""
print "Using config:", CONFIG
sys.stdout.write("Waiting for flash... ")
sys.stdout.flush() #linux tweaks

# open CSV file
with open(OUTFILE, 'w') as xmlfile:
    
    # open Vicon client
    client = ViconSocket(IP_ADDR, port=int(PORT))
    client.open()
    
    #open xml file
    w = XMLWriter(xmlfile)
    doc = w.start("ViconCalib")
    w.element("version", at=str(version))
    
    #wait for flash
    while True:
        sleeper.stamp()
        
        if serial.getCTS():
            # grab data
            r = client.get("getRotation", OBJECT)
            t = client.get("getTranslation", OBJECT)
        
            # write data
            if len(r) > 0 and len(t) > 0:
                w.element("timestamp", at=str(sleeper.getStamp()))
                w.element("roomOrigin", x=t[0], y=t[1], z=t[2], rx=r[0], ry=r[1], rz=r[2])
                sys.stdout.write("got it!\n")
                break
            
        # sleep until next timestamp
        sleeper.sleep()
        sys.stdout.flush()
    
    w.close(doc)
    client.close()
    
print "\nComplete."
exit(0)

# End of file
