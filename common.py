#!/usr/bin/env python2
#
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# version 0.5.13
#
# Common routines for get_vicon.py
# Sleeper and ViconSocket classes
#

import time, sys, os, errno, ConfigParser, socket
from serial import Serial

# Prints a notice if the exit reason was unclean.
def quit(ret_code):
    if ret_code > 0:
        print "Exiting prematurely."
    exit(ret_code)

# Performs argument sanity checks
# prints usage if incorrect
def args_check(usage, args=sys.argv):
    if len(args) <= 1 or args[1].startswith("--"):
        print "Usage:", usage
        quit(1)

# Searches a list for a variable name 
# and returns the next item as the variable value.
# Otherwise returns the default value.
def find_arg(var_name, default, args=sys.argv):
    _arg = ""
    for a in args:
        if a == "--"+var_name:
            _arg = a
        elif _arg != "":
            return a
    return default

# Finds and returns the first file in the 
# current directory that matches the extension.
def find_cfg(extension=".cfg"):
    for f in os.listdir(os.getcwd()):
        if f.endswith(extension):
            return f
    print "Cannot find a config file!"
    quit(1)

# Loads the config file with ConfigParser
# does sanity checks and
# returns a RawConfigParser class
def load_cfg(path):
    if not os.path.isfile(path):
        print "Config not found:", path
        quit(1)
    
    settings = ConfigParser.ConfigParser()
    try:
        settings.read(path)
    except:
        print path
        print sys.exc_info()[0]
        quit(1)
    return settings

# Returns a current working directory 
# that has the correct file separator
def win_cwd(end="/"):
    return os.getcwd().replace("\\", "/") + end

# Test the directory existence, otherwise create it
def check_directory(path):
    try:
        os.makedirs(win_cwd() + path)
    except OSError as ex:
        if ex.errno != errno.EEXIST:
            print "Cannot create data directory!"
            quit(1)
            
# -- ViconSocket
# Wraps the socket object and provides default values
# returns an array of strings for use in writing CSV rows
# 
class ViconSocket():
    def __init__(self, address, port=802, buffer=1024, split=" "):
        self._address = address
        self._port = port
        self._buffer = buffer
        self._split = split
        self._open = False
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        #self._sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        
    def open(self):
        if not self._open:
            self._sock.connect((self._address, self._port))
        else:
            print "Vicon socket already open"
            
    def get(self, command, object):
        self._sock.send(command + " " + object)
        return self._sock.recv(self._buffer).split(self._split)
        
    def close(self):
        self._sock.close()
        
# -- Sleeper
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
class Sleeper():
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
        
# End of file
