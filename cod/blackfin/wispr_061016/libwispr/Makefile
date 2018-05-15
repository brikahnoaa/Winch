#
# Makefile for LIBWISPR using the Windows Blackfin Toochain
# You will need to change the KERN path for your location  
#

TARGET = __ADSPBF537__

UCLINUX_OPTS = -Wall -D$(TARGET)

# Cross-compiler is in C:/Program Files (x86)/Analog Devices/GNU Toolchain\2010R1\uclinux\bin
# Add this path to your environ variables
CC=bfin-uclinux-gcc

AR=bfin-uclinux-ar

INCS = -I../../linux-2.6.x/blackfin/include/

LIBS = -lbfdsp -lm -lpthread

CFLAGS = $(INCS) $(UCLINUX_OPTS) -O2 -mfast-fp
LDFLAGS = -Wl,-elf2flt $(INCS) $(UCLINUX_OPTS) -O2 -mfast-fp

OBJ = gpio.o com.o rtc.o wdt.o log.o led.o wav_file.o dat_file.o sport_adc.o wav_test.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

console_prompt: console_prompt.c
	$(CC) $(LDFLAGS) -o console_prompt console_prompt.c

libwispr: $(OBJ)
	$(AR) -rcs libwispr.a $(OBJ)

clean: 
	rm *.o

win_clean: 
	del *.o
