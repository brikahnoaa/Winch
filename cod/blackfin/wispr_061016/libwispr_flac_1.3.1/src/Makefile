#
# Makefile for WISPR FLAC LIBRARY using the Windows Blackfin Toochain
#

TARGET = __ADSPBF537__

UCLINUX_OPTS = -Wall -D$(TARGET) -DHAVE_CONFIG_H

# Cross-compiler is in C:/Program Files (x86)/Analog Devices/GNU Toolchain\2010R1\uclinux\bin
# Add this path to your environ variables
CC=bfin-uclinux-gcc

AR=bfin-uclinux-ar

INCS = -I../../../linux-2.6.x/blackfin/include/
INCS += -I../include/
INCS += -I../../libwispr/

CFLAGS = $(INCS) $(UCLINUX_OPTS) -O2 -mfast-fp

LDFLAGS = -Wl,-elf2flt $(INCS) $(UCLINUX_OPTS) -O2 -mfast-fp

OGG_SRCS = \
	ogg_decoder_aspect.c \
	ogg_encoder_aspect.c \
	ogg_helper.c \
	ogg_mapping.c

OBJS = \
	bitmath.o \
	bitreader.o \
	bitwriter.o \
	cpu.o \
	crc.o \
	fixed.o \
	fixed_intrin_sse2.o \
	fixed_intrin_ssse3.o \
	float.o \
	format.o \
	lpc.o \
	lpc_intrin_sse.o \
	lpc_intrin_sse2.o \
	lpc_intrin_sse41.o \
	lpc_intrin_avx2.o \
	md5.o \
	memory.o \
	metadata_iterators.o \
	metadata_object.o \
	stream_decoder.o \
	stream_encoder.o \
	stream_encoder_intrin_sse2.o \
	stream_encoder_intrin_ssse3.o \
	stream_encoder_intrin_avx2.o \
	stream_encoder_framing.o \
	window.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

libFLAC: $(OBJS)
	$(AR) -rcs ../libFLAC.a $(OBJS)

libwispr_flac: wispr_flac.o $(OBJS)
	$(AR) -rcs ../libwispr_flac.a wispr_flac.o $(OBJS)
