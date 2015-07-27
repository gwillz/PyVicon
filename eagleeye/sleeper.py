# 
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# version 0.1.1
# 
# This sleeper object will sleep _until_ the next timestamp is due
# This way there is no timestamp discrepencies when performing 
# time-intensive operations in a loop.
#
# Example:
#   sleeper = common.Sleeper(0.1)
#   for i in range(0, 10):
#       sleeper.stamp()
#       %% do stuff %%
#       print sleeper.getStamp()
#       sleeper.sleep()
#

import time, sys

class Sleeper:
    # sleeptime: time to sleep until releasing the thread
    # resolution: time between comparing the stamps
    # epoch: time that the loop starts, the first time stamp() is called
    def __init__(self, sleeptime, resolution=0.000001):
        self._sleeptime = sleeptime
        self._resolution = resolution
        self._epoch = 0.0
    
    # sleep until next timestamp
    # will print err variable if called after the timestamp is due
    def sleep(self, err="The loop is late!\n"):
        if time.time() > self._stamp + self._sleeptime:
            sys.stdout.write(err)
            return
        while time.time() < self._stamp + self._sleeptime:
            time.sleep(self._resolution)
    
    # record a timestamp
    def stamp(self):
        self._stamp = time.time()
        # record first timestamp seperately
        if self._epoch < 1:
            self._epoch = self._stamp
            
    # returns the current timestamp
    def getRawStamp(self):
        return self._stamp
    
    # returns time since the epoch
    def getStamp(self):
        return self._stamp - self._epoch
