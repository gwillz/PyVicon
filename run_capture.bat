@echo off
start /b cmd /c "vicon_binaries\ViconDataStreamServer.exe & pause"
start /b cmd /c "C:\Python27\python.exe vicon_capture.py EE1 --time 1 --config unisa-ml_vicon.cfg & pause"
