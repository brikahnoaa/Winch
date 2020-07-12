# emulator. v3
from design import *
import winch, sbe16, sbe39, ant, floats

# this should be set from env var if present
Offset = 2

def init():
    "init all"
    sbe16.init()
    winch.init()
    sbe39.init()
    # ant.init()

def start():
    "start all"
    winch.start(3+Offset)
    sbe16.start(2+Offset)
    # sbe39.start(1+Offset)
    ant.start(1+Offset)

def stop():
    "stop all"
    winch.stop()
    sbe16.stop()
    sbe39.stop()
    # ant.stop()

def info():
    "info all"
    winch.info()
    sbe16.info()
    sbe39.info()
    # ant.info()
    
def restart():
    "stop init start"
    stop()
    init()
    start()

import atexit
atexit.register(stop)

print "start() stop() restart() info() mooring"
info()
