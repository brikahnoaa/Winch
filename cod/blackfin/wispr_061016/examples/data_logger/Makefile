#
# Makefile for WISPR using the GNU Windows Blackfin Toochain
#

TARGET = __ADSPBF537__

UCLINUX_OPTS = -Wall -D$(TARGET)

# Cross-compiler is typically installed in 
# C:/Program Files (x86)/Analog Devices/GNU Toolchain\2010R1\uclinux\bin
# Add this path to your environ variables
#CC=/opt/uClinux/bfin-uclinux/bin/bfin-uclinux-gcc
CC=bfin-uclinux-gcc

# Set the locations of the include and lib files
INCS = -I../../../linux-2.6.x/blackfin/include
INCS += -I../../libwispr
INCS += -L../../libwispr
INCS += -I../../libwispr_flac_1.3.1/include
INCS += -L../../libwispr_flac_1.3.1

# Libraries
LIBS = -lwispr -lwispr_flac -lbfdsp -lm -lpthread

CFLAGS = $(INCS) $(UCLINUX_OPTS) -O2 -mfast-fp
LDFLAGS = -Wl,-elf2flt $(INCS) $(UCLINUX_OPTS) -O2 -mfast-fp

#OBJS = 

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

wispr_data_logger:  wispr_data_logger.c $(OBJS) 
	$(CC) $(LDFLAGS) -o wispr_data_logger wispr_data_logger.c $(OBJS) $(LIBS)

win_clean: 
	del *.o
	del *.gdb

clean: 
	rm *.o
	rm *.gdb
