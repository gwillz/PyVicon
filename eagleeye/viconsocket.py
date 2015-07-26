# 
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# version 0.1.2
# 
# Wraps the socket object and provides default values
# returns an array of strings for use in writing CSV rows
# 

import socket

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
            
    def get(self, command, object=None):
        cmd = command if object is None else command + " " + object
        self._sock.send(cmd)
        return self._sock.recv(self._buffer).split(self._split)
        
    def close(self):
        self._sock.close()
