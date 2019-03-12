#
# Makefile for WISPR using the Windows Blackfin Toochain
#

TARGET = __ADSPBF537__

UCLINUX_OPTS = -Wall -D$(TARGET)

# Cross-compiler is typically install at:
#   C:/Program Files (x86)/Analog Devices/GNU Toolchain\2010R1\uclinux\bin
# Add this path to your environ variables
CC=bfin-uclinux-gcc

# Set the locations of the include files
INCS = -I../../../linux-2.6.x/blackfin/include
INCS += -I../../libwispr
INCS += -L../../libwispr

LIBS = -lwispr -lbfdsp -lm -lpthread

CFLAGS = $(INCS) $(UCLINUX_OPTS) -O2 -mfast-fp
LDFLAGS = -Wl,-elf2flt $(INCS) $(UCLINUX_OPTS) -O2 -mfast-fp

OBJS = spectrogram.o decimate.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

all: spectrogram8kHz decimate_example

spectrogram8kHz:  spectrogram_example.c $(OBJS)
	$(CC) $(LDFLAGS) -o spectrogram8kHz spectrogram_example.c $(OBJS) $(LIBS)

decimate_example:  decimate_example.c $(OBJS)
	$(CC) $(LDFLAGS) -o decimate_example decimate_example.c $(OBJS) $(LIBS)

win_clean: 
	del *.o
	del *.gdb

clean:
	rm *.o  
	rm *.gdb
