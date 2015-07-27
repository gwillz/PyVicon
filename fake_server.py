#!/usr/bin/env python2
#
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# 
# A fake vicon server for testing outside of the lab.
#

import SocketServer

class ViconHandler(SocketServer.BaseRequestHandler):
    
    delim = " "
    objects = {'EE1': {'r': ['0.1', '0.2', '0.3'], 't': ['100.1', '-100.2', '100.3']},
               'EE2': {'r': ['1.1', '1.2', '1.3'], 't': ['-200.1', '200.2', '150.3']},
               'EE3': {'r': ['2.1', '2.2', '2.3'], 't': ['-300.1', '-300.2', '200.3']},
               'Wand': {'r': ['-1.1', '-2.2', '-0.3'], 't': ['400.1', '400.2', '250.3']}
               }
    
    def handle(self):
        self.data = self.request.recv(1024).split(self.delim)
        print "{}:".format(self.client_address[0]), self.data
        
        cmd = self.data[0].strip()
        
        if cmd == "getRotation":
            obj = self.data[1].strip()
            reply = "{0} {1}".format(obj, self.delim.join(self.objects[obj]['r']))
            
        elif cmd == "getTranslation":
            obj = self.data[1].strip()
            reply = "{0} {1}".format(obj, self.delim.join(self.objects[obj]['t']))
            
        elif cmd == "getSubjects":
            reply = "{0} {1}".format(len(self.objects), self.delim.join(self.objects.keys()))
            
        else:
            reply = "err"
        
        self.request.sendall(reply)
    
if __name__ == "__main__":
    HOST, PORT = "localhost", 1802
    
    print "listening on:", HOST, PORT
    server = SocketServer.TCPServer((HOST, PORT), ViconHandler)
    server.allow_reuse_address = True
    server.serve_forever()
    
    