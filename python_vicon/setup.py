from distutils.core import setup, Extension

# the c++ extension module
extension_mod = Extension("hello", ["vicon_python.cpp"])

setup(name = "hello", ext_modules=[extension_mod])