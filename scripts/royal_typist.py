#!/usr/bin/python -u

import serial
import time
import random
import sys
import os.path
import os

preparedPath = "~/prepared-doc.txt"
tempPath = "~/temp-doc.txt"

def checkSwitch(ser):
    ser.write("read\r")
    ser.flush()
    status = ser.readline().strip()
    status = int(status,16)
    print("Switch status : {0:04b}".format(status))
    return (status & (1<<3)) != 0

def printFile(ser,f):
    for line in iter(lambda:f.readline(),''):
        ser.write("W"+line.rstrip()+"\r")
        ser.flush()
        print(ser.readline().rstrip() + " : "+line.rstrip())
        ser.write("cr\r")
        ser.flush()
        ser.readline()
    print "Done."
    
def runConnection(ser):
    while 1:
        while not checkSwitch(ser):
            time.sleep(0.1)
            try:
                tp = open(os.path.expanduser(tempPath))
                print("Printing temp file")
                printFile(ser,tp)
                os.unlink(os.path.expanduser(tempPath))
            except IOError as e:
                pass
        f = open(os.path.expanduser(preparedPath))
        printFile(ser,f)
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
