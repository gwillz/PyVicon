#!/usr/bin/env python2

import sys, xml.etree.ElementTree as ET

tree = ET.parse(sys.argv[1])
root = tree.getroot()

offset = root.find("roomOrigin").attrib

print offset

print offset['x']
print offset['y']
print offset['z']

exit(0)
