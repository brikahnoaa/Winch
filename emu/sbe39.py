# emulate antenna sbe39 v3
import time
from random import random
from laraSer import Serial
from serial.tools.list_ports import comports
from threading import Thread, Event
from design import *
import winch

# globals set in init(), start()

name = 'sbe39'
portSelect = 1      # select port 0-n of multiport serial
baudrate = 9600

CTD_DELAY = 0.53
CTD_WAKE = 0.78

serThreadObj = None

def info():
    "globals which may be externally set"
    print "(go:%s)   syncMode=%s   syncModePending=%s   sleepMode=%s" % \
        (go.isSet(), syncMode, syncModePending, sleepMode)

def init():
    "set globals to defaults"
    global go, sleepMode, syncMode, syncModePending, timeOff
    sleepMode = syncMode = syncModePending = False
    timeOff = 0
    go = Event()

def start(portSel=portSelect):
    "start I/O thread"
    global go, serThreadObj, name, ser
    # threads run while go is set
    go.set()
    try:
        # select port 0-n of multiport serial
        port = comports()[portSel].device
        ser = Serial(port=port,baudrate=baudrate,name=name)
    except: 
        print "no serial for %s" % name
        ser = None
        return
    serThreadObj = Thread(target=serThread)
    serThreadObj.daemon = True
    serThreadObj.name = name
    serThreadObj.start()

def stop():
    global go, serThreadObj
    "stop threads"
    if not serThreadObj: return
    go.clear()
    # wait until thread ends, allows daemon to close clean
    serThreadObj.join(3.0)
    if serThreadObj.is_alive(): 
        print "stop(): fail on %s" % serThreadObj.name

def serThread():
    "thread: loop looks for serial input; to stop set sergo=0"
    global go, ser, syncMode, sleepMode
    stamp = time.time()
    if not ser.is_open: ser.open()
    ser.buff = ''
    try:
        while go.isSet():
            if not sleepMode and not syncMode:
                if (time.time()-stamp)>120:
                    ser.put("<Timeout msg='2 min inactivity timeout, "
                        "returning to sleep'/>\r\n")
                    gotoSleepMode()
            # CTD. syncMode, sample, settings
            if ser.in_waiting:
                stamp = time.time()
                # syncMode is pending until sleepMode
                # syncMode is special, a trigger not a command
                if syncMode:
                    c = ser.get()
                    if '\x00' in c:
                        # serial break, python cannot really see it
                        ser.log( "break; syncMode off, flushing %r" % ser.buff )
                        syncMode = False
                        sleepMode = False
                    else:
                        ctdOut()
                elif sleepMode:
                    c = ser.get()
                    if '\r' in c:
                        # wake
                        ser.log( "waking, flushing %r" % ser.buff )
                        time.sleep(CTD_WAKE)
                        ser.put('<Executed/>\r\n')
                        sleepMode = False
                else: # not sync or sleep. command line
                    # upper case is standard for commands, but optional
                    l = ser.getline(echo=1).upper()
                    if l:
                        l = l[:-len(ser.eol)]
                        if 'TS' in l:
                            ctdOut()
                        elif 'DATE' in l:
                            # trim up to =
                            dt = l[l.find('=')+1:]
                            setDateTime(dt)
                            ser.log( "set date time %s -> %s" % 
                                (dt, sbe39DateTime()) )
                        elif 'SYNCMODE=Y' in l:
                            syncModePending = True
                            ser.log( "syncMode pending (when ctd sleeps)")
                        elif 'QS' in l:
                            gotoSleepMode()
                        if sleepMode != True:
                            ser.put('<Executed/>\r\n')
        # while go:
    except IOError, e:
        print "IOError on serial, calling ant.stop() ..."
        stop()
    if ser.is_open: ser.close()

def gotoSleepMode():
    "CTD enters sleep mode, due to timeout or QS command"
    global ser, sleepMode, syncMode, syncModePending
    ser.log(ser.name + " ctd sleepMode")
    if syncModePending:
        ser.log(ser.name + " ctd syncMode")
        syncModePending = False
        syncMode = True
    sleepMode = True

def setDateTime(dt):
    "set ctdClock global timeOff from command in seabird format"
    global timeOff
    # datetime=mmddyyyyhhmmss to python time struct
    pyTime = time.strptime(dt,"%m%d%Y%H%M%S")
    # python time struct to UTC
    utc = time.mktime(pyTime)
    # offset between emulated ctd and this PC clock
    timeOff = time.time()-utc

def sbe39DateTime():
    "use global timeOff set by setDateTime() to make a date"
    global timeOff
    # sbe16 has no comma, sbe39 has one
    f='%d %b %Y, %H:%M:%S'
    return time.strftime(f,time.localtime(time.time()-timeOff))

def ctdDelay():
    "Delay for response. TBD, variance"
    global CTD_DELAY
    return CTD_DELAY

# Temp, conductivity, depth, fluromtr, PAR, salinity, time
# 16.7301,  0.00832,    0.243, 0.0098, 0.0106,   0.0495, 14 May 2017 23:18:20
def ctdOut():
    "instrument sample"
    # "\r\n  t.t, c.c, d.d, f.f, p.p, s.s,  dd Mmm yyyy, hh:mm:ss\r\n"

    # ctd delay to process, nominal 3.5 sec. Add variance?
    time.sleep(ctdDelay())
    ###
    # note: modify temp for ice simulation
    #  24.2544,    0.182, 24 Oct 2017, 00:21:43
    #''24.2544,''''0.182,'24'Oct'2017,'00:21:43
    ser.put("%8.4f, %8.3f, %s\r\n" % (temper(), depth(), sbe39DateTime() ))

def depth():
    "mooring-(cable+buoyL+floatsL+antL), but always below surface"
    dep=mooring - (winch.cable()+buoyLine+floatsLine+antLine)
    if dep>antSBEpos: 
        return dep
    else: # at surface
        wave = (.8 * random()) - .4
        return antSBEpos + wave

def temper():
    "return 20.1 unless we emulate ice at a certain depth"
    return 20.1

init()
