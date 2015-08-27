#!/usr/bin/env python2
#
# pyserial test

from serial import Serial
from eagleye import EasyConfig
import time, sys

config = EasyConfig("serial_test.cfg")

try:
    ser = Serial(config.serial_device, 192000)
except OSError:
    print "Cannot open serial device"
    exit(1)


while True:
    if raw_input("eh?:").startswith("y"):    
        ser.setRTS()
    
exit(0)

#print ser.name
while True:
    if ser.getCTS():
        print "True!"
        time.sleep(0.3)
    time.sleep(0.00001)
exit(0)
