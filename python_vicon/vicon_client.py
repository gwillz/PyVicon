
# vicon_client.py

import hello as pyvicon

print pyvicon.hello("")

class PyVicon:
    
    def __init__(self):
        
        pass
        
    def connect(self, ip, port=801):
        pyvicon.connect("{}:{}".format(ip,port))
        pyvicon.enableSegmentData()
        pyvicon.setStreamMode(pyvicon.StreamMode.ClientPull)
        
    
    
    