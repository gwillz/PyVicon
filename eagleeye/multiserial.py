#!/usr/bin/env python2
#
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# 

from serial import Serial
import sys, os, threading


class MultiSerial:
    def __init__(self, device, baud=19200):
        #self._serial = Serial(device, baud)
        self._device = device
        self.baud = baud
        
        #self.server()
        
    def server(self):
        self.lock = threading.Lock()
        
        pid = 0
        while pid == 0:
            pid = Popen(["python2", "multiserial.py", device, baud]).pid
        
    def _pyexec(self):
        platform = sys.platform
        if platform.startWith("linux"):
            return "python2"
        elif platform == "win32":
            return "C:\\Python27\\python.exe"
        else:
            return "python"
        
    def getFlash(self):
        self.lock.acquire()
        
        
        pass
    
    
    
    
    

if __name__ == '__main__':
    serial = MultiSerial("/dev/ttyUSB0")
    serial.server()
    exit(0)