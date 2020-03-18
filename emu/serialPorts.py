#!/usr/bin/python
# list serial ports
from serial.tools.list_ports import comports

for i in comports(): print i.device
