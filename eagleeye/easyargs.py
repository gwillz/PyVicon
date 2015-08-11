# 
# Project Eagle Eye
# Group 15 - UniSA 2015
# Gwilyn Saunders
# version 0.1
# 
# I didn't like getopt.
# 
#
# Usage example in the tester script
# run tests with:
#  $ python2 easyargs.py test
#
# Special uses include compatibility with EasyConfig's config finder.
# i.e:
#  args = EasyArgs()
#  cfg = EasyConfig(args.config)
#
# args.config will return False if not found, causing EasyConfig to 
# start instead search the working directory for a config file.
# 
# Cheeky special use - default values.
# i.e:
#  time = args.time or 180
#  
# If args.time is False, the 'or' statement will assign time as 180.
#

import os, sys

class EasyArgs:
    def __init__(self, args=sys.argv):
        self._noops = []
        self._ops = {}
        
        o = None
        for v in args:
            if v.startswith("-") and o is None:
                o = v.replace("-", "")
            elif o is not None:
                if v.startswith("-"):
                    self._ops[o] = True
                else:
                    self._ops[o] = self._converttype(v)
                o = None # reset
            else: # o is still None
                self._noops.append(self._converttype(v))
        
    
    def verifyOpts(self, *ops):
        for o in ops:
            if self.__getattr__(o) is None:
                self._missing = o
                return False
        return True
    
    def verifyLen(self, length):
        return length >= len(self._noops)
    
    def __getattr__(self, key):
        if key in self._ops:
            return self._ops[key]
        
        for op in self._ops:
            if op.startswith(key):
                return self._ops[op]
        
        return False
        
    def __getitem__(self, key):
        return self._noops[key]
    
    def _converttype(self, var):
        if "." in var:
            try: return float(var)
            except: pass
        
        try: return int(var)
        except: pass
        
        if var in ["True", "False"]:
            return var == "True"
        
        return var
    
    def __str__(self):
        out = []
        for a in self._noops: out.append(str(a))
        for o in self._ops: out.append("{}:{}".format(o, self._ops[o]))
        return ", ".join(out)
        
    
if __name__ == "__main__" and sys.argv[1] == 'test':
    
    test_args = ["-u", "element 2", "regular", "--special", "variable1", "-hello", "world", "nothing"]
    args = EasyArgs(test_args)
    
    print "testing:", test_args
    print ""
    print "verify len (2):", args.verifyLen(2)
    print "verify opts:", args.verifyOpts("special", "u", "h")
    print "verify opts (bad):", args.verifyOpts("bad")
    print ""
    print "empty:", args.empty
    print "u:", args.u
    print "0:", args[0]
    print "1:", args[1]
    print "s:", args.s
    print "hello:", args.hello
    print ""
    print "all:", args
    