#!/usr/bin/python

import serial
import time
import random
import sys
import os.path

def waitForSwitch(ser):
    status = 0
    while (status & (1<<3)) == 0:
        ser.write("read\r")
        ser.flush()
        status = ser.readline().strip()
        status = int(status,16)
        print("Switch status : {0:04b}".format(status))
        time.sleep(0.1)
    
def runConnection(ser):
    while 1:
        waitForSwitch(ser)
        f = open(os.path.expanduser("~/prepared-doc.txt"))
        for line in iter(lambda:f.readline(),''):
            ser.write("W"+line.rstrip()+"\r")
            ser.flush()
            print(ser.readline().rstrip() + " : "+line.rstrip())
            ser.write("cr\r")
            ser.flush()
            ser.readline()
        time.sleep(5)


while 1:
    ser = 0
    try:
        ser = 0
        ser = serial.Serial('/dev/ttyUSB0',19200)
        runConnection(ser)
    except serial.SerialException as e:
        if ser != 0:
            ser.close
        print "Couldn't open /dev/ttyUSB0; trying again in 3 seconds."
        time.sleep(3)
    ser = 0
