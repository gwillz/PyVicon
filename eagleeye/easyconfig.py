# 
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# version 0.2.5
# 
# I found ConfigParser a bit messy.
# 

import ConfigParser, os, sys

class EasyConfig:
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
        
    def _find_cfg(self, extension):
        for f in os.listdir(os.getcwd()):
            if f.endswith(extension):
                return f
        raise IOError("Cannot find a config file!")
    
    def __getattr__(self, varname):
        var = self._cfg.get(self._group, varname)
        
        if varname not in self.__dict__:
            self.__dict__[varname] = self._converttype(var)
        
        return self.__dict__[varname]
        
    def _converttype(self, var):
        if "." in var:
            try: return float(var)
            except: pass
        
        try: return int(var)
        except: pass
        
        if var in ["True", "False"]:
            return var == "True"
        
        return var
    
if __name__ == "__main__" and sys.argv[1] == 'test':
    cfg = EasyConfig()
    print cfg.ip_address, type(cfg.ip_address)
    print cfg.port, type(cfg.port)
    print cfg.run_serial, type(cfg.run_serial)
    print cfg._group
    
    cfg = EasyConfig("nothing")