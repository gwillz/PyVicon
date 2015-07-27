#!/usr/bin/env python2
#
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# version 0.7.16
#
# Retrieves Vicon data via TCP sockets.
# Includes syncronized timestamp data via a R232 COM port.
#
# usage: python2 vicon_capture.py {--time <in minutes> | --config <file>}
#

from eagleeye import ViconSocket, Sleeper, EasyConfig
from eagleeye.common import *
from datetime import datetime
from serial import Serial
import csv, sys

# set arguments
TIME = int(find_arg("time", "3"))
cfg = EasyConfig(find_arg("config", None))
OUTPATH = win_cwd() + cfg.output_folder + "/" + \
          str(datetime.now().strftime(cfg.date_format))

num_frames = int(TIME * 60 * cfg.framerate)
sleeper = Sleeper(1.0 / cfg.framerate)

# data directory sanity check
check_directory(cfg.output_folder)

# start the serial listener
if cfg.run_serial:
    try:
        serial = Serial(cfg.serial_device, 19200)
    except OSError:
        print "Couldn't open serial device", cfg.serial_device
        quit(1)
else:
    print "Not listening to serial"


# open Vicon client
client = ViconSocket(cfg.ip_address, port=cfg.port)
client.open()
objects = client.get("getSubjects")[1:]

# print status
print ""
print "Using config:", cfg._path
print "Running for", TIME, "minutes"
print "Capturing at", cfg.framerate, "per second"
print "Recording these objects:", objects
print ""

# open CSV file
csvfiles = []
csvwriters = {}
for obj in objects:
    path = "{0}_{1}.csv".format(OUTPATH, obj)
    f = open(path, 'wb')
    w = csv.writer(f, delimiter=cfg.output_delimiter, quoting=csv.QUOTE_MINIMAL)
    csvfiles.append(f)
    csvwriters[obj] = w

# main loop
for i in range(0, num_frames):
    sleeper.stamp()
    
    # get flash timestamper
    flash = "."
    if cfg.run_serial:
        if serial.getCTS():
            flash = "F"
    
    # grab data for each object
    for obj in csvwriters:
        r = client.get("getRotation", obj)
        t = client.get("getTranslation", obj)
    
        # write data to appropriate writer
        if len(r) > 0 and len(t) > 0:
            csvwriters[obj].writerow([sleeper.getStamp(), flash] + t + r)
            sys.stdout.write(flash)
            
    # sleep until next timestamp
    sleeper.sleep("\bL")
    sys.stdout.flush()
    
# clean up
for f in csvfiles: f.close()
client.close()
    
print "\nComplete."
exit(0)

# End of file
