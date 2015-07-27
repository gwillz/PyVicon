@echo off
start /b cmd /c "vicon_binaries\ViconDataStreamServer.exe & pause"
C:\Python27\python.exe vicon_capture.py --time 1 --config unisa-ml_vicon.cfg
