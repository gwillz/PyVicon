#!/usr/bin/env python2
#
# timer test

from common import *

FRAMERATE = 30
TIME = 20

_sleeptime = 1.0 / FRAMERATE
_timeout = TIME * FRAMERATE

sleeper = Sleeper(_sleeptime)

for i in range(0, _timeout):
    sleeper.stamp()
    
    print "i:", i
    print "at:", sleeper.getStamp()
    
    # sleep
    sleeper.sleep()
    