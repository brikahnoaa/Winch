# emulate buoy v3
import time
from laraSer import Serial
from serial.tools.list_ports import comports
from threading import Thread, Event
from design import *
import winch

# globals set in init(), start()
# serThread should be a class in laraSerial.py

name = 'sbe16'
portSelect = 2      # select port 0-n of multiport serial
baudrate = 9600

CTD_DELAY = 4.3     # delay readings from sbe16
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
    global ser, go, sleepMode, syncMode, syncModePending
    stamp = time.time()
    # goes to sleep if no input for 2:00 or QS
    # 0.8 sec to wake
    if not ser.is_open: ser.open()
    ser.buff = ''
    try:
        while go.isSet():
            if not sleepMode and not syncMode:
                if (time.time()-stamp)>120:
                    ser.put('time out\r\n')
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
                        ser.put('SBE 16plus\r\nS>')
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
                                (dt, sbe16DateTime()) )
                        elif 'SYNCMODE=Y' in l:
                            syncModePending = True
                            ser.log( "syncMode pending (when ctd sleeps)")
                        elif 'QS' in l:
                            gotoSleepMode()
                        if sleepMode != True: 
                            ser.put('S>')
        # while go:
    except IOError, e:
        print "IOError on serial, calling buoy.stop() ..."
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

def sbe16DateTime():
    "use global timeOff set by setDateTime() to make a date"
    global timeOff
    # sbe16 has no comma, sbe39 has one
    f='%d %b %Y %H:%M:%S'
    return time.strftime(f,time.localtime(time.time()-timeOff))

def ctdDelay():
    "Delay for response. TBD, variance"
    global CTD_DELAY
    return CTD_DELAY

# Temp, conductivity, depth, fluromtr, PAR, salinity, time
#'20.7301,''0.00832,''''0.243,'0.0098,'0.0106,'''0.0495,'14'May'2017'23:18:20
# 20.5476,  0.01495,    0.300, 1.8187, 0.0139,   0.0801, 24 Oct 2017 00:32:38
def ctdOut():
    "instrument sample"
    # ctd delay to process. Add variance?
    time.sleep(ctdDelay())
    # note: modify temp for ice tests
    ser.put(" %7.4f, %8.5f, %8.3f, %6.4f, %6.4f, %8.4f, %s\r\n" %
        (20.1, 0.1, depth(), 0.1, 0.1, 0.1, sbe16DateTime() ))

def depth():
    "mooring - (cable+buoyLine)"
    # TBD
    d = mooring-(winch.cable()+buoyLine)
    if d<0: return 0
    else: return d

init()
