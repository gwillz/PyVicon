#!/usr/bin/env python2
#
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# version 0.5.13
#
# Retrieves Vicon data directly via TCP sockets.
# Includes syncronized timestamp data via a R232 COM port.
#
# usage: python2 vicon_capture.py <object name> {--time <in minutes> | --config <file>}
#

from common import *
from subprocess import *
from datetime import datetime
import csv, sys

# args sanity check
args_check("python2 vicon_capture.py <object name> \
            {--time <in minutes> | --config <file>}")

# set arguments
OBJECT = sys.argv[1]
TIME = int(find_arg("time", "5"))
CONFIG = find_arg("config", find_cfg())
settings = load_cfg(CONFIG)
CFG_VAR = "settings"

# config load
SERVER_PATH = win_cwd() + settings.get(CFG_VAR, "server_path")
IP_ADDR = settings.get(CFG_VAR, "ip_address")
PORT = settings.get(CFG_VAR, "port")
OUTFILE = win_cwd() + settings.get(CFG_VAR, "output_folder") + "/" + \
          str(datetime.now().strftime(settings.get(CFG_VAR, "date_format"))) + \
          "_" + OBJECT + ".csv"
DELIMITER = settings.get(CFG_VAR, "output_delimiter")
FRAMERATE = int(settings.get(CFG_VAR, "framerate")) # per second
SERIAL_DEV = settings.get(CFG_VAR, "serial_device")
DO_FLASH = settings.get(CFG_VAR, "run_serial") == "True"
DO_SERVER = settings.get(CFG_VAR, "run_server") == "True"

num_frames = TIME * 60 * FRAMERATE
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
if DO_FLASH:
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
print "Running for", TIME, "minutes"
print "Capturing at", FRAMERATE, "per second"
print ""

# open CSV file
with open(OUTFILE, 'wb') as csvfile:
    w = csv.writer(csvfile, delimiter=DELIMITER, quoting=csv.QUOTE_MINIMAL)
    
    # open Vicon client
    client = ViconSocket(IP_ADDR, port=int(PORT))
    client.open()
    
    # main loop
    for i in range(0, num_frames):
        sleeper.stamp()
        
        # grab data
        r = client.get("getRotation", OBJECT)
        t = client.get("getTranslation", OBJECT)
        
        # get flash timestamper
        flash = "."
        if DO_FLASH:
            if serial.getCTS():
                flash = "F"
        
        # write data
        if len(r) > 0 and len(t) > 0:
            w.writerow([sleeper.getStamp(), flash] + t + r)
            sys.stdout.write(flash)
            
        # sleep until next timestamp
        sleeper.sleep("\bL")
        
        sys.stdout.flush()
        
    client.close()
    
print "\nComplete."
exit(0)

# End of file
