# pyvicon testing
import pyvicon

address = "192.168.10.1:801"

print "connected?:", pyvicon.isConnected()
print "\nConnecting.."

if pyvicon.connect(address):
    print "Connected:", address
else:
    print "Could not connect:", address

if pyvicon.isConnected():
    print "version:", pyvicon.version()
    print "subject count:", pyvicon.subjectCount()
    sub0 = pyvicon.subjectName(0)
    print "subject index 0:", sub0
    print "all subjects:", pyvicon.subjects()
    
    print "marker count of subject 0", pyvicon.markerCount(sub0)
    
    pyvicon.disconnect()

exit(0)