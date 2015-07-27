#
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
#
# Common routines for vicon_capture.py & room_calib.py
#

import sys, os, errno

# Prints a notice if the exit reason was unclean.
def quit(ret_code):
    if ret_code > 0:
        print "Exiting prematurely."
    exit(ret_code)

# Performs argument sanity checks
# prints usage if incorrect
def args_check(usage, args=sys.argv):
    if len(args) <= 1 or args[1].startswith("--"):
        print "Usage:", usage
        quit(1)

# Searches a list for a variable name 
# and returns the next item as the variable value.
# Otherwise returns the default value.
def find_arg(var_name, default, args=sys.argv):
    _arg = ""
    for a in args:
        if a == "--"+var_name:
            _arg = a
        elif _arg != "":
            return a
    return default

# Finds and returns the first file in the 
# current directory that matches the extension.
def find_cfg(extension=".cfg"):
    for f in os.listdir(os.getcwd()):
        if f.endswith(extension):
            return f
    print "Cannot find a config file!"
    quit(1)

# Returns a current working directory 
# that has the correct file separator
def win_cwd(end="/"):
    return os.getcwd().replace("\\", "/") + end

# Test the directory existence, otherwise create it
def check_directory(path):
    try:
        os.makedirs(win_cwd() + path)
    except OSError as ex:
        if ex.errno != errno.EEXIST:
            print "Cannot create data directory!"
            quit(1)
        
# End of file
