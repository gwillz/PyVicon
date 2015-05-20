#!/usr/bin/env python2
#
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# version 0.4.5
#
# Retrives Vicon data directly via wrapped server/client binaries.
#
# usage: python2 get_vicon.py <object name> {--time <in minutes> | --config <file>}
#

from common import *
from subprocess import *
from datetime import datetime
import csv, sys

# args sanity check
args_check("python2 get_vicon.py <object name> {--time <in minutes> | --config <file>}")

# set arguments
OBJECT = sys.argv[1]
TIME = int(find_arg("time", "5"))
CONFIG = find_arg("config", find_cfg())
settings = load_config(CONFIG)

# config load
CLIENT_PATH = settings.get("settings", "client_path")
SERVER_PATH = settings.get("settings", "server_path")
IP_ADDR = settings.get("settings", "ip_address")
PORT = settings.get("settings", "port")
OUTFILE = settings.get("settings", "output_folder") + "/" + str(datetime.now().strftime(settings.get("settings", "date_format"))) + "_" + OBJECT + ".csv"
DELIMITER = settings.get("settings", "output_delimiter")
FRAMERATE = int(settings.get("settings", "framerate")) # per second

try:
    serial = Serial(settings.get("settings", "serial_device"), int(settings.get("settings", "serial_baud")))
except OSError:
    print "Cannot open serial:", settings.get("settings","serial_device")
    quit(1)

num_frames = TIME * 60 * FRAMERATE
sleeper = Sleeper(1.0 / FRAMERATE)

# data directory sanity check
check_directory(settings.get("settings", "output_folder"))

# status
print "Using config:", CONFIG
print "Running for", TIME, "minutes"
print "Capturing at", FRAMERATE, "per second"

# start the server
server = Popen(SERVER_PATH, shell=True).returncode
if server > 0:
    print "Couldn't start server"
    quit(1)

# open CSV file
with open(OUTFILE, 'wb') as csvfile:
    w = csv.writer(csvfile, delimiter=DELIMITER, quoting=csv.QUOTE_MINIMAL)
    
    # main loop
    for i in range(0, num_frames):
        sleeper.stamp()
        
        # grab data
        rotate = Popen([CLIENT_PATH, IP_ADDR, PORT, "getRotation", OBJECT],stdout=PIPE)
        translate = Popen([CLIENT_PATH, IP_ADDR, PORT, "getTranslation", OBJECT], stdout=PIPE)
        r = rotate.stdout.read()
        t = translate.stdout.read()
        
        # get flash timestamper
        flash = serial.getCTS()
        
        # write data
        if r != "" and t != "":
            if flash:
                w.writerow([sleep.getStamp(), "flash"] + t.split(" ") + r.split(" "))
                sys.stdout.write("F")
            else:
                w.writerow([sleep.getStamp()] + t.split(" ") + r.split(" "))
                sys.stdout.write(".")
        
        # sleep until next timestamp
        sleeper.sleep()
        

print "Complete."
exit(0)

# End of file
