# 
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# version 0.2.7
# 
# I found ConfigParser a bit messy.
# 

import ConfigParser, os, sys, ast, re

class EasyConfig:
    # opens and parses a config file
    # will search the immediate directory if no path is specified
    def __init__(self, path=False, group='settings', extension='.cfg'):
        if path is not False:
            if not os.path.isfile(path):
                raise IOError("Config not found: {0}".format(path))
            self._path = path
        else:
            self._path = self._find_cfg(extension)
        
        self._group = group
        self._cfg = ConfigParser.ConfigParser()
        self._cfg.read(self._path)
    
    # inner routine for searching for cfg files
    def _find_cfg(self, extension):
        for f in os.listdir(os.getcwd()):
            if f.endswith(extension):
                return f
        raise IOError("Cannot find a config file!")
    
    # get the variable and returns in the correct type
    # will also record the var to the class instance for faster access
    def __getattr__(self, varname):
        var = self._cfg.get(self._group, varname)
        
        if varname not in self.__dict__:
            self.__dict__[varname] = self._converttype(var)
        
        return self.__dict__[varname]
    
    # inner routine function to auto-convert type from strings
    def _converttype(self, var):
        # test float
        if "." in var:
            try: return float(var)
            except: pass
        
        # test int
        try: return int(var)
        except: pass
        
        # test bool
        if var in ["True", "False"]:
            return var == "True"
        
        # test tuple, lists
        if re.match("[\(\[].*[\)\]]", var) is not None:
            return ast.literal_eval(var)
        
        return var
    
    
if __name__ == "__main__" and sys.argv[1] == 'test':
    cfg = EasyConfig()
    print cfg.ip_address, type(cfg.ip_address)
    print cfg.port, type(cfg.port)
    print cfg.date_format, type(cfg.date_format)
    print cfg.serial_device, type(cfg.serial_device)
    print cfg.run_serial, type(cfg.run_serial)
    print cfg._group
    
    cfg = EasyConfig("nothing")