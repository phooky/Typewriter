#!/usr/bin/python

import serial
import time
import random


ser = serial.Serial('/dev/ttyUSB0',19200)

msg="All work and no play makes Jack a dull boy.\n"

while True:
    ser.write(msg)
    ser.readline()

ser.close
