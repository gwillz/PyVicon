# Eagle Eye /// Capture Tool /// Documentation

## Overview
This software records raw Vicon data into CSV files. The CSV files include 
timestamps and flash sync. The software is intended to be run a system running 
the Vicon Tracker software. This may require administrative priviledges to 
install support libraries or driver software.

#### Procedure
The procedure is as follows:
* Create physical objects
* [Prepare](#preparation) the camera, sync, Vicon tracker
* [Configure](#configuration-file) software accordingly
* [Calibrate](#room-calibration) the room
* [Capture](#data-capture) data

#### Outputs
The software outputs two file types.
* A [CSV file](#csv-data-output) from _run_capture.bat_
* An [XML file](#xml-data-output) from _run_calib.bat_

## Procedure
### Preparation
#### Vicon objects
The Vicon system captures objects that are defined as unique "constellations" 
of dots placed on physical objects within the lab. These objects are configured 
in the _Vicon Tracker v1.2_ software. Some experimental/working objects are 
stored in the [objects](ojects) folder.

Each object is given a name in Vicon, this name must be specified each time 
this software is run. This is done in the batch executable files (.bat). Simply 
right click to edit, double click to execute.

#### Configuration file
Most of these setting don't need to change. Different settings may apply to 
different Vicon Systems. Typically only the _framerate_ or _serial_device_ 
variables would need tweaking. The configuration file must contain all of the 
variables as specified in this [documentation here](#configuration). 

A list of available serial ports can be found with this command:
```
python2 -m serial.tools.list_ports
```

#### Syncronization
The video file and Vicon data need to be syncronized. This is done with a 
[sync circuit](#flash-sync-circuit). The circuit signals the software to create 
a marker in the data and simultaneously triggers a camera flash to be seen in 
the video.

_IMPORTANT:_ a dataset can only contain two flashes. These two flashes represent 
the start and end of a dataset. 
The [Formatter Tool](http://git.gwillz.com.au/eagleeye/formattertool) 
will ignore any data outside of these flashes.

### Room Calibration
The 

### Data Capture
* 

## Supporting Documentation
### Prerequisites
* Windows 7+
* Python 2.7
* Vicon Tracker System (tested with v1.2)
* Serial RS232 Serial connection
* Flash Sync circuit
* A camera flash w/ PC-SYNC connection

### Legal
* The CRAPL License
* http://matt.might.net/articles/crapl/CRAPL-LICENSE.txt

### Flash Sync Circuit
This is a circuit by design of Peter Barsznica that triggers a camera flash and 
an identifier in the software, in order to syncronise the video and data feeds.

This is the circuit detail:

![Circuit Sync](assets/sync_circuit_small.png)

The circuit connects to the serial GND and CTS pins:

![Serial Pinout](assets/pinouts_serial.gif)

### Configuration
The configuration file (.cfg) specifies common settings that don't need to 
change during a recording session or within a lab. These are the variables it 
contains and the corresponding defaults.

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

### CSV Data Output
| Row | Data      | Type  | Examples |
| --- | --------- | ----- | -------- |
| 0   | Timestamp | float | 0.144    |
| 1   | Sync      | char  | F, L, .  |
| 2   | X-axis    | float | 5121.54  |
| 3   | Y-axis    | float | 1543.33  |
| 4   | Z-axis    | float | 45.1431  |
| 5   | X-rotate  | float | 0.1123   |
| 6   | Y-rotate  | float | 0.2323   |
| 7   | Z-rotate  | float | 2.1102   |

#### Sync
* . (dot) - is a regular frame
* - F - is a flash frame, they may be sequential but must only appear twice 
within a dataset
* - L - is a late frame, this means the data was not recieved before the next 
frame was due

#### Notes
* The timestamp is 0 (zero) on the first frame
* The X, Y, Z rotational data corresponds to pitch, yaw, roll (which is which 
is unknown)

### XML Data Output
```xml
<?xml version="1.0"?>
<ViconCalib>
  <timestamp at="1.4960000515"/>
  <roomOrigin
      x="6877.87615416716"
      y="895.554984200831"
      z="64.0212346077015"
      rx="-0.00035405915893805"
      ry="0.0813275486771378"
      rz="-0.528133124177889"
  />
</ViconCalib>
```

This XML data must be kept with the CSV data, it specifies where the 
[common world-coordinate](http://git.gwillz.com.au/eagleeye/formattertool/blob/master/README.md#common-world-coordinates) 
origin lies. This is necessary to convert Vicon world-coordinates to the 
common world-coordinates.
