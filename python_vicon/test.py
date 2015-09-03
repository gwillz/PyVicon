# pyvicon testing
import pyvicon

address = "192.168.10.1:801"

#print "all:", dir(pyvicon)
print "connected?:", pyvicon.isConnected()
print "\nConnecting.."

if pyvicon.connect(address):
    print "Connected:", address
else:
    print "Could not connect:", address

if pyvicon.isConnected():
    print "StreamMode - ClientPull:", pyvicon.setStreamMode(pyvicon.SM_ClientPull)
    print "Enable segment data:", pyvicon.enableSegmentData()
    print "Enable marker data:", pyvicon.enableSegmentData()
    print "version:", pyvicon.version()
    
    print "subject count:", pyvicon.subjectCount()
    sub0 = pyvicon.subjectName(0)
    print "subject index 0:", sub0
    print "all subjects:", pyvicon.subjects()
    
    print "marker count:", pyvicon.markerCount(sub0)
    print "translation:", pyvicon.globalTranslation(sub0)
    print "rotation:", pyvicon.globalRotation(sub0)
    
    pyvicon.disconnect()

exit(0)