# 
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# version 0.1
# 
# I found ConfigParser a bit messy.
# 

import ConfigParser, os, sys

class EasyConfig:
    def __init__(self, path=None, group='settings', extension='.cfg'):
        if path is None:
            self.path = find_cfg(extension)
        else:
            self.path = path
        
        self._group = group
        self._cfg = ConfigParser.ConfigParser()
        self._cfg.read(path)
        
    def find_cfg(extension):
        for f in os.listdir(os.getcwd()):
            if f.endswith(extension):
                return f
        raise Exception("Cannot find a config file!") #TODO
    
    def __getattr__(self, varname):
        return self._cfg.get(self._group, varname)
    
