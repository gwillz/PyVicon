@echo off
start /b cmd /c "vicon_binaries\ViconDataStreamServer.exe & pause"
start /b cmd /c "C:\Python27\python.exe room_calib.py Wand --config unisa-ml_vicon.cfg & pause"
