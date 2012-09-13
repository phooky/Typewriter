#!/usr/bin/python

import serial
import time
import random
import sys

ser = serial.Serial('/dev/ttyUSB0',19200)


for line in iter(lambda:sys.stdin.readline(),''):
    ser.write("W"+line.rstrip()+"\r")
    ser.flush()
    print(ser.readline() + " : "+line.rstrip())
    ser.write("cr\r")
    ser.flush()
    ser.readline()
ser.close
