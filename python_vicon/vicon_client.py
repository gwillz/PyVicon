
# vicon_client.py


import pyvicon

print pyvicon.isconnected()
#pyvicon.connect("192.168.10.1:801")


"""
#print pyvicon.hello("")

class PyVicon:
    
    def __init__(self):
        
        pass
        
    def connect(self, ip, port=801):
        pyvicon.connect("{}:{}".format(ip,port))
        pyvicon.enableSegmentData()
        pyvicon.setStreamMode(pyvicon.StreamMode.ClientPull)
        
    
    
    
"""