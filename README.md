## README - Capture Tool

### Overview
This software records raw Vicon data into CSV files. The CSV files include 
timestamps and flash sync.

The software is intended to be run a system running the Vicon Tracker software. 
This may require administrative priviledges to install support libraries or 
driver software.

### Legal
* The CRAPL License
* http://matt.might.net/articles/crapl/CRAPL-LICENSE.txt

### Prerequisites
* Windows 7+
* Python 2.7
* Vicon Tracker System (tested with v1.4)
* Serial RS232 Serial connection
* Flash Sync circuit
* A camera flash w/ PC-SYNC connection

### Flash Sync Circuit
This is a circuit by design of Peter Barsznica that triggers a camera flash and 
an identifier in the software, in order to syncronise the video and data feeds.

This is the circuit detail:
* image here

The circuit just connect to the serial GND and CTS pins:
* image here

### Configuration
config file description
* config file properties

### Preparation
* Vicon objects
* configure
* .bat files

### Room Calibration
* 

### Data Capture
* 


