#!/usr/bin/python

import serial
import time
import random


ser = serial.Serial('/dev/ttyUSB0',19200)

msg="WAll work and no play makes Jack a dull boy.\r"

while True:
    ser.write(msg)
    ser.flush()
    ser.readline()
    ser.write("cr\r")
    ser.flush()
    ser.readline()
ser.close
