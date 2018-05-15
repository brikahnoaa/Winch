#
# Makefile for WISPR using the Windows Blackfin Toochain
#

TARGET = __ADSPBF537__

UCLINUX_OPTS = -Wall -D$(TARGET)
#UCLINUX_OPTS += -DDO_CYCLE_COUNTS

# Cross-compiler is in C:/Program Files (x86)/Analog Devices/GNU Toolchain\2010R1\uclinux\bin
# Add this path to your environ variables
#CC=/opt/uClinux/bfin-uclinux/bin/bfin-uclinux-gcc
CC=bfin-uclinux-gcc

# Set the locations of the include and lib files
INCS = -I../../../linux-2.6.x/blackfin/include
INCS += -I../../libwispr
INCS += -L../../libwispr
INCS += -I../../libwispr_flac_1.3.1/include
INCS += -L../../libwispr_flac_1.3.1

LIBS = -lwispr -lwispr_flac -lbfdsp -lm -lpthread

CFLAGS = $(INCS) $(UCLINUX_OPTS) -O2 -mfast-fp
LDFLAGS = -Wl,-elf2flt $(INCS) $(UCLINUX_OPTS) -O2 -mfast-fp

OBJS = detect_bw.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

wispr_bw_socal:  main_all_modes_bw_socal.c $(OBJS) 
	$(CC) $(LDFLAGS) -o wispr_bw_socal main_all_modes_bw_socal.c $(OBJS) $(LIBS)
#	
#wispr_bw:  main_all_modes_bw_at.c $(OBJS) 
#	$(CC) $(LDFLAGS) -o wispr_bw main_all_modes_bw_at.c $(OBJS) $(LIBS)
		
clean: 
	rm *.o
	rm *.gdb
