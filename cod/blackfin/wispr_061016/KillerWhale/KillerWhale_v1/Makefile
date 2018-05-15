#
# Makefile for WISPR using the Windows Blackfin Toochain
# You will need to change the KERN path for your location  
#

TARGET = __ADSPBF537__

UCLINUX_OPTS = -Wall -D$(TARGET)
#UCLINUX_OPTS += -D_CYCLE_COUNTS

# Cross-compiler is in C:/Program Files (x86)/Analog Devices/GNU Toolchain\2010R1\uclinux\bin
# Add this path to your environ variables
CC=bfin-uclinux-gcc

# Set the locations of the include files
INCS = -I../../../linux-2.6.x/blackfin/include
INCS += -I../../libwispr
INCS += -L../../libwispr
INCS += -I../../libwispr_flac_1.3.1/include
INCS += -L../../libwispr_flac_1.3.1

LIBS = -lwispr -lwispr_flac -lbfdsp -lm -lpthread

CFLAGS = $(INCS) $(UCLINUX_OPTS) -O2 -mfast-fp
LDFLAGS = -Wl,-elf2flt $(INCS) $(UCLINUX_OPTS) -O2 -mfast-fp

OBJS = detect_kw.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

wispr_kw:  main_all_modes_kw.c $(OBJS) 
	$(CC) $(LDFLAGS) -o wispr_kw main_all_modes_kw.c $(OBJS) $(LIBS)

test_filter:  test_filter.c
	$(CC) $(LDFLAGS) -o test_filter test_filter.c  $(LIBS)

test_detect:  test_detect.c $(OBJS)
	$(CC) $(LDFLAGS) -o test_detect test_detect.c $(OBJS) $(LIBS)

clean: 
	rm *.o
	rm *.gdb

win_clean: 
	del *.o
	del *.gdb


