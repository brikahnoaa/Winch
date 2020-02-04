.DEFAULT:
.SUFFIXES:
.PHONY: force
.PRECIOUS: %.o
SHELL=/bin/bash

binFiles :=  ../bin/swh ../bin/rx ../bin/sx ../bin/chkconfig ../bin/chkconfigN2 \
             ../bin/Lbt9522IccidMsisdn  ../bin/FwRev
symFiles := chat.c crc16bit.c expect.c logger.c pkt.c serial.c xmodem.c \
            extract.c strtoul.c 
hdrFiles := chat.h crc16bit.h expect.h logger.h pkt.h Rx.h Rx5036.h serial.h Tx.h xmodem.h \
            config.h configN2.h extract.h strtoul.h cstring.h serial.h linux.h cstring.h \
            chat.h lbt9522.h modem.h
objFiles := $(subst .c,.o, $(symFiles))

all: swiftware symfiles ../bin/swh headers $(binFiles) ebm manual
	@echo Done making all...

../bin/chkconfig: chkconfig.o config.o logger.o crc16bit.o ds2404.o cstring.o
	gcc -gstabs -L. $^ -lm -lstdc++ -lSwiftWare -o $@

../bin/FwRev: FwRev.o 
	gcc -gstabs -L. FwRev.o -lm -lstdc++ -lSwiftWare -o $@ 

../bin/Lbt9522IccidMsisdn: Lbt9522IccidMsisdn.o serial.o logger.o linux.o cstring.o lbt9522.o modem.o chat.o
	gcc -gstabs -L. $^ -lm -lSwiftWare -o $@

../bin/chkconfigN2: chkconfigN2.o configN2.o logger.o crc16bit.o ds2404.o cstring.o
	gcc -gstabs -L. $^ -lm -lstdc++ -lSwiftWare -o $@

../bin/rx: rx.o logger.o crc16bit.o serial.o Rx.o xmodem.o StdioPort.o ds2404.o cstring.o
	gcc $^ -L. -lg++ -lSwiftWare++ -lSwiftWare -lstdc++ -lm -o $@

../bin/swh: swh.o 
	gcc -gstabs -L. swh.o -lm -lstdc++ -lSwiftWare -o $@ 

../bin/sx: sx.o crc16bit.o logger.o pkt.o serial.o Tx.o xmodem.o StdioPort.o ds2404.o cstring.o
	gcc $^ -L. -lg++ -lSwiftWare++ -lSwiftWare -lstdc++ -lm -o $@

clean: clean-swiftware clean-ebm clean-manual force
	-rm -f $(binFiles) $(addsuffix .o,$(binFiles)) $(symFiles) $(objFiles) $(hdrFiles) \
          swh swh.o Tx.o Rx.o chkconfig.o config.o ds2404.o rx.o sx.o StdioPort.o cstring.o \
          lbt9522.o linux.o modem.o configN2.o chkconfigN2.o rx_pid.o Lbt9522IccidMsisdn.o \
          FwRev.o

%.o: %.c
	gcc -D$(shell echo $*.c|tr a-z.- A-Z__) -Dcode="" -Dpersistent="" -c -x c -I{.,SwiftWare/c} -gstabs \
       -Wall -Wmissing-prototypes $< -o $@

%.o: %.cpp
	gcc -D$(shell echo $*.cpp|tr a-z.- A-Z__) -c -x c++ -gstabs+ -I{.,SwiftWare/{c,c++,g++}} \
       -Wall $*.cpp -o $*.o 

headers: $(hdrFiles)

%.h: %.c 
	../bin/swh D=$(shell echo $*.h|tr a-z.- A-Z__) if=$< of=$@

symfiles: $(symFiles)

$(symFiles):
	-ln -s ../lib/$@

config.c: ../src/config.c
	-ln -s ../src/$@

manual: force
	-cd ../manual; make pdf

clean-manual: force
	-cd ../manual; make clean

ebm: force
	-cd ../ebm; make

clean-ebm: force
	-cd ../ebm; make clean

swiftware: force
	-cd SwiftWare/c; make
	-cd SwiftWare/c++; make
	-cd SwiftWare/g++; make
	-cd SwiftWare/src; make

clean-swiftware: force
	-cd SwiftWare/c; make clean
	-cd SwiftWare/c++; make clean
	-cd SwiftWare/g++; make clean
	-cd SwiftWare/src; make clean

Rx.o: Rx.c xmodem.c

Tx.o: Tx.c xmodem.c

Tx.c: ../lib/tx.c
	ln -s ../lib/tx.c Tx.c

Rx.c: ../lib/rx.c
	ln -s ../lib/rx.c Rx.c

cstring.o: cstring.c
	gcc -D$(shell echo $*.c|tr a-z.- A-Z__) -Dcode="" -c -x c -I{.,SwiftWare/c} -gstabs \
       -Wall -Wmissing-prototypes $< -o $@

linux.o: linux.c
	gcc -D$(shell echo $@.c|tr a-z.- A-Z__) -Dpersistent= -Dfar= -c -x c -I. \
       -gstabs  -I. -Wall -Wmissing-prototypes $< -o $@

chkconfig.o: chkconfig.c
	gcc -D$(shell echo $*.c|tr a-z.- A-Z__) -Dcode="" -Dpersistent="" \
       -DFWREV=0x`perl -ne 's/.*revision:[ ]*([0-9]+)/\1/g;print' ../src/FirmwareRevision` \
       -c -x c -I{.,SwiftWare/c} -gstabs -Wall -Wmissing-prototypes $< -o $@

chkconfigN2.o: chkconfigN2.c
	gcc -D$(shell echo $*.c|tr a-z.- A-Z__) -Dcode="" -Dpersistent="" \
       -DFWREV=0x`perl -ne 's/.*revision:[ ]*([0-9]+)/\1/g;print' ../src/FirmwareRevision` \
       -c -x c -I{.,SwiftWare/c} -gstabs -Wall -Wmissing-prototypes $< -o $@
