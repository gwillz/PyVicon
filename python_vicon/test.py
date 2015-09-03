# pyvicon testing
import pyvicon

print pyvicon.isConnected()
print "Connecting.."
address = "192.168.10.1:801"
pyvicon.connect(address)
print "Connected:", address

if pyvicon.isConnected():
    print "subject count:", pyvicon.subjectCount()
    sub0 = pyvicon.subjectName(0)
    print "subject index 0:", sub0
    print "all subjects:", pyvicon.subjects()
    
    print "marker count of subject 0", pyvicon.markerCount(sub0)
    
    pyvicon.disconnect()
    exit(0)

print "could not connect:", address
exit(1)