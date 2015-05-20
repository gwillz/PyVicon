#!/usr/bin/env python2
#
# pyserial test

from serial import Serial
from common import *
import time, sys

config = load_config("unisa-ml_vicon.cfg")

try:
    ser = Serial(config.get("settings", "serial_device"), int(config.get("settings", "serial_baud")))
except OSError:
    print "Cannot open serial device"
    exit(1)

#print ser.name
while True:
    if ser.getCTS():
        print "True!"
        time.sleep(0.3)
    time.sleep(0.00001)
exit(0)
