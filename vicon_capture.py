#!/usr/bin/env python2
#
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# version 0.11.26
#
# Retrieves Vicon data via PyVicon
# Includes syncronized timestamp data via a R232 COM port.
#
# usage: python2 vicon_capture.py {--output <folder> | --time <in minutes> | --config <file> | --training <file>}
#

from eagleeye import Sleeper, EasyConfig, EasyArgs
from datetime import datetime
from serial import Serial
import csv, sys, os
from python_vicon import PyVicon

def main(sysargs):
    # set arguments
    args = EasyArgs(sysargs)
    cfg = EasyConfig(args.config, group="capture")
    time = args.time or cfg.default_time
    output_folder = args.output or cfg.default_output
    outpath = os.path.join(output_folder, datetime.now().strftime(cfg.date_format))
    
    num_frames = int(time * cfg.framerate) + (cfg.flash_delay * 2)
    flash_at = [cfg.flash_delay, num_frames - cfg.flash_delay]
    sleeper = Sleeper(1.0 / cfg.framerate)
    
    # data directory sanity check
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)
    
    # start the serial listener
    if cfg.run_serial:
        try:
            serial = Serial(cfg.serial_device, 9600)
            serial.setRTS(0) # set at zero
        except OSError:
            print "Couldn't open serial device", cfg.serial_device
            return 1
    else:
        print "Not listening to serial"
    
    # open Vicon connection
    print "Connecting to Vicon..."
    client = PyVicon()
    client.connect(cfg.ip_address, cfg.port)
    
    if not client.isConnected():
        print "Failed to connect to Vicon! {}:{}".format(
                                            cfg.ip_address, cfg.port)
        return 1
    
    csvfiles = []
    csvwriters = {}
    
    # determine training or capture mode
    if args.training:
        # test target existance
        client.frame()
        if not cfg.trainer_target in client.subjects():
            print "Cannot find:", cfg.trainer_target
            return 1
        
        f = open(args.training, 'wb')
        csvfiles.append(f)
        csvwriters[cfg.trainer_target] = csv.writer(f, delimiter=cfg.output_delimiter, quoting=csv.QUOTE_MINIMAL)
        subjects = [cfg.trainer_target]
    else:
        client.frame()
        subjects = client.subjects()
        
        # open CSV files
        for sub in subjects:
            path = "{0}_{1}.csv".format(outpath, sub)
            f = open(path, 'wb')
            w = csv.writer(f, delimiter=cfg.output_delimiter, quoting=csv.QUOTE_MINIMAL)
            csvfiles.append(f)
            csvwriters[sub] = w
        
    
    # print status
    print ""
    print "Using config:", cfg._path
    print "Running for", time, "seconds ({} frames)".format(num_frames)
    print "Flash delay at:", cfg.flash_delay, " ({} seconds)".format(int(cfg.flash_delay / cfg.framerate))
    print "Capturing at", cfg.framerate, "frames per second"
    
    if args.training:
        print "Recording training target:", cfg.trainer_target, "into:", args.training
    else:
        print "Saving data into:", output_folder
        print "Recording these subjects:\n", ", ".join(subjects)
    
    print ""
    
    # main loop
    for c in range(0, num_frames):
        sleeper.stamp()
        
        # run flash
        flash = "."
        if cfg.run_serial:
            if c in flash_at:
                serial.setRTS(1)
                flash = "F"
                sys.stdout.write("\r - - - - - - - Flash!\r")
            else: 
                serial.setRTS(0)
        
        client.frame()
        for s in subjects:
            csvwriters[s].writerow(
                [sleeper.getStamp(), flash] + 
                list(client.translation(s)) + 
                list(client.rotation(s)) + 
                list(client.markerStatus(s)))
        
        # sleep until next timestamp
        sys.stdout.write("{}/{}\r".format(c, num_frames))
        sleeper.sleep("\r - - - - - - - - - Late!\r")
        sys.stdout.flush()
        
    # clean up
    for f in csvfiles: f.close()
    client.disconnect()
    
    print "\nComplete."
    return 0

if __name__ == '__main__':
    exit(main(sys.argv))
