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
* Vicon Tracker System (tested with v1.2)
* Serial RS232 Serial connection
* Flash Sync circuit
* A camera flash w/ PC-SYNC connection

### Flash Sync Circuit
This is a circuit by design of Peter Barsznica that triggers a camera flash and 
an identifier in the software, in order to syncronise the video and data feeds.

This is the circuit detail:

![Circuit Sync](assets/sync_circuit_small.png)

The circuit connects to the serial GND and CTS pins:

![Serial Pinout](assets/pinouts_serial.gif)

A list of available serial ports can be found with:
```
python2 -m serial.tools.list_ports
```

### Configuration
config file description

| Setting          | Default       |
| ---------------- | ------------- |
| ip_address       | 127.0.0.1     |
| port             | 802           |
| date_format      | Y-%m-%d_%H-%M |
| framerate        | 30            |
| output_folder    | data          |
| output_delimiter | ,             |
| serial_device    | COM4          |
| run_serial       | True          |

### Preparation
* Vicon objects
* configure
* .bat files

### Room Calibration
* 

### Data Capture
* 


