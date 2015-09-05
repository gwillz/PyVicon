#!python
# PyVicon class implementation
# Written for the EagleEye project at the University of South Australia
#
# 2015-09-05
# Gwilyn Saunders
# 
# Wraps the static functions made in the .cpp into a Python class object.
# 

import pyvicon

class PyVicon:
    SM_ClientPull = 0
    SM_ClientPullPreFetch = 1
    SM_ServerPush = 2
    
    def __init__(self):
        self._c = pyvicon.new_client()
        self.__version__ = ".".join(pyvicon.version(self._c))
    
    def __del__(self):
        # explicitly deleting probably does nothing
        # I don't know how to clean up a pyCapsule
        # assume it's automatic..?
        del self._c
    
    def connect(self, ip, port):
        stat = pyvicon.connect(self._c, "{}:{}".format(ip, port))
        if stat:
            pyvicon.enableSegmentData(self._c)
            pyvicon.enableMarkerData(self._c)
            pyvicon.setStreamMode(self.SM_ClientPull)
        return stat
    
    def disconnect(self):
        return pyvicon.disconnect(self._c)
    
    def isConnected(self):
        return pyvicon.isConnected(self._c)
    
    def subjectCount(self):
        return pyvicon.subjectCount(self._c)
    
    def subjectName(self, index):
        return pyvicon.subjectName(self._c, index)
    
    def subjects(self):
        return pyvicon.subjects(self._c)
    
    def rotation(self, name):
        return pyvicon.globalRotation(self._c, name)
    
    def translation(self, name):
        return pyvicon.globalTranslation(self._c, name)
    
    def frame(self):
        return pyvicon.frame(self._c)
    
    def frameNumber(self):
        return pyvicon.frameNumber(self._c)
    
    def setStreamMode(self, streamMode):
        return pyvicon.setStreamMode(self._c, streamMode)
    
    def enableSegmentData(self):
        return pyvicon.enableSegmentData(self._c)
    
    def enableMarkerData(self):
        return pyvicon.enableMarkerData(self._c)
    

if __name__ == "__main__":
    client = PyVicon()
    print client.__version__
    print client.isConnected()
    print client.connect("192.168.10.1", 801)
    print client.isConnected()
    print client.disconnect()
    
    exit(0)
