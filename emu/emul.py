# emulator. v3
# import winch, sbe16, sbe39, floats, ant
import winch, sbe16, sbe39, floats

def init():
    "init all"
    # sbe16.init()
    winch.init()
    sbe39.init()
    # ant.init()

def start():
    "start all"
    # sbe16.start()
    winch.start()
    sbe39.start()
    # ant.start()

def stop():
    "stop all"
    # sbe16.stop()
    winch.stop()
    sbe39.stop()
    # ant.stop()

def info():
    "info all"
    # sbe16.info()
    winch.info()
    sbe39.info()
    # ant.info()
    
def restart():
    "stop init start"
    stop()
    init()
    start()

import atexit
atexit.register(stop)

print "start() stop() restart() sbe16.info() winch.cable(0)"
info()
