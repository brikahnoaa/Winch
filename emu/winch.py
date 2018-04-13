# emulate winch v3
from laraSer import Serial
from serial.tools.list_ports import comports
from threading import Thread, Event
from time import time, sleep
from design import *
import floats

# globals set in init(), start()

# amodRate measured about 6.5 sec 
name = 'winch'
baudrate = 4800
eol = '\r\n'
# select port 0-n of multiport serial
portSelect = 3

amodDelay = 5.5
serThreadObj = None

def info():
    global go, cableLen, motorRunState
    "globals which may be externally set"
    print "winch:  go=%s  winch.motor('%s')  winch.cable(%.2f)" % \
        (go.isSet(), motorRunState, cableLen)

def init():
    "set global vars to defaults"
    global go, cableLen, motorRunState, motorOn
    cableLen = 0
    # motorRunState off, down, up
    motorRunState = 'off' 
    motorOn = Event()
    go = Event()

def start(portSel=portSelect):
    "start serial and reader thread"
    global go, ser, buffOut, serThreadObj
    buffOut = ''
    # threads run while go is set
    go.set()
    motorOn.clear()
    try:
        # select port 0-n of multiport serial
        port = comports()[portSel].device
        ser = Serial(port=port,baudrate=baudrate,name=name,eol=eol)
        print "serial %s for %s" % (port, name)
    except:
        print "no serial for %s" % name
        ser = None
        return
    obj = Thread(target=motorThread)
    obj.daemon = True
    obj.name = 'motor'
    obj.start()
    serThreadObj = Thread(target=serThread)
    serThreadObj.daemon = True
    serThreadObj.name = name
    serThreadObj.start()

def stop():
    global go, motorOn, serThreadObj
    "stop threads, close serial"
    if not serThreadObj: return
    go.clear()
    # motorOn thread is in wait()
    motorOn.set()
    # wait until thread ends, allows daemon to close clean
    serThreadObj.join(3.0)
    if serThreadObj.is_alive():
        print "stop(): fail on %s" % serThreadObj.name

def serThread():
    "thread: looks for serial input, output; sleeps to simulate amodDelay"
    global ser, go
    if not ser.is_open: ser.open()
    try:
        while go.isSet():
            # acoustic modem. up, stop, down.
            if ser.in_waiting:
                amodInput()
            if buffOut:
                amodOutput()
        # while go
    except IOError, e:
        print "IOError on serial, calling stop() ..."
        stop()
    if ser.is_open: ser.close()

# #R,01,03
surfCmd = "#R,%s,00" % winchID
riseCmd = "#R,%s,03" % winchID
fallCmd = "#F,%s,00" % winchID
quitRsp = "%%S,%s,00" % winchID
stopCmd = "#S,%s,00" % winchID
buoyRsp = "%%S,%s,00" % winchID

riseRsp = "%%R,%s,00" % buoyID
fallRsp = "%%F,%s,00" % buoyID
quitCmd = "#S,%s,00" % buoyID
stopRsp = "%%S,%s,00" % buoyID
buoyCmd = "#S,%s,00" % buoyID

def amodInput():
    "process input at serial, sleeps to simulate amodDelay"
    l = ser.getline()
    if not l: return
    ser.log( "hearing %s" % l )
    if "OK" in l:
        return
    if len(l) > 6: 
        ser.putline( "OK" )
        sleep(amodDelay)
    # rise
    if riseCmd in l:
        motor('up')
        sleep(amodDelay)
        ser.putline(riseRsp)
    # stop
    elif stopCmd in l:
        motor('off')
        sleep(amodDelay)
        ser.putline(stopRsp)
    # fall
    elif fallCmd in l:
        motor('down')
        sleep(amodDelay)
        ser.putline(fallRsp)
    elif quitRsp in l:
        ser.log( "buoy stop response %s" % l )
    # buoy responds to stop after dock or slack
    elif buoyRsp in l:
        ser.log( "buoy stat response %s" % l )
    # something strange
    elif l:
        ser.log("amod: unexpected %r" % l)

def amodOutput():
    "Sleep to emulate amodDelay, put buffOut"
    global buffOut
    # check
    if not buffOut: return ser.log("err: amodOutput(): empty buffOut")
    # one line out
    sleep(amodDelay)
    b = buffOut
    # end of line
    e = b.find(ser.eol)
    if e>0: 
        # found eol, include
        e += len(ser.eol)
    else: e = len(b)
    ser.put( b[:e] )
    # repeat twice
    i = 2
    while (i > 0):
        sleep(1)
        if ser.in_waiting:
            amodInput()
            break
        else:
            ser.put( b[:e] )
            i -= 1
    buffOut = b[e:]

def amodPut(s):
    "Buffer output for slow sending by amodOutput"
    global buffOut
    # note - should use threading.Timer for this
    buffOut += s

def motor(state):
    "set motorRunState to off, down, up; motorOn event"
    global ser, motorRunState, motorOn
    if state not in ( 'off', 'down', 'up'):
        return ser.log( "motor(up|off|down), not '%s'" % state )
    #
    motorRunState = state
    ser.log( "motor %s with cableLen at %.2f" % (state,cableLen) )
    if motorRunState=='off': motorOn.clear()
    else: motorOn.set()

def motorThread():
    "when motor is on: update cableLen, check dock and slack"
    global ser, go, cableLen, motorRunState, motorOn
    # motor could be on when emulation starts
    while go.isSet(): 
        motorOn.wait()
        motorLastTime = time()
        sleep(.1)
        # up
        if motorRunState=='up':
            cableLen += (time() - motorLastTime) * .331
            if slack():
                ser.log( "surfaced" )
                motor('off')
                amodPut(quitCmd + ser.eol)
        # down
        if motorRunState=='down':
            cableLen -= (time() - motorLastTime) * .2
            if docked():
                ser.log( "docked" )
                cableLen=0
                motor('off')
                amodPut(quitCmd + ser.eol)

def slack():
    "determine if the cableLen is slack"
    return floats.depth()<.1

def docked():
    "are we docked?"
    global cableLen
    return cableLen<.1

def cable(new=None):
    "set or get cableLen"
    global cableLen
    if new is not None:
        cableLen = new
    return cableLen

init()
