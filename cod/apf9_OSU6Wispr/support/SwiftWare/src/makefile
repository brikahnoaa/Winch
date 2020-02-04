#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# $Id: makefile,v 1.1.1.1 2003/11/12 23:22:58 swift Exp $
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# RCS Log:
#
# $Log: makefile,v $
# Revision 1.1.1.1  2003/11/12 23:22:58  swift
# Apf9 firmware for a 260ml Apex with SBE41 CTD.
#
# Revision 1.1  2001/06/03 07:23:37  navo
# Initial revision
#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.SUFFIXES:
.DEFAULT:
.PRECIOUS: %.o 
.PHONY: all test fresh clean force

SHELL=/bin/csh
libDir      = ../..#
binDir      = ../bin#

# construct a list of all source files in source directory
srcFiles = $(wildcard *.cpp *.c)

# construct a list of base names
baseNames = $(basename $(notdir $(srcFiles)))

# construct a list of binary file names
binFiles = $(addprefix $(binDir)/, $(baseNames))

all: $(binFiles)
	@echo done making $@ ...

test:
	@echo libDir    = $(libDir)
	@echo binDir    = $(binDir)
	@echo baseNames = $(baseNames)
	@echo binFiles  = $(binFiles)
	@echo srcFiles  = $(srcFiles)

fresh: clean all

clean: force
	- rm -f $(binFiles) 

$(binDir)/%: %.cpp
	gcc $< -x c++ -D$(shell echo $*.cpp|tr a-z. A-Z_) -gstabs+ -I{.,../c,../c++,..}/ \
       -Wall -Wmissing-prototypes -L$(libDir)/ -lSwiftWare++ -lSwiftWare \
       -lSwiftWare++ -lstdc++ -o$@

$(binDir)/%: %.c
	gcc $< -x c -D$(shell echo $*.cpp|tr a-z. A-Z_) -gstabs -I{.,../c,..}/ \
       -Wall -Wmissing-prototypes -L$(libDir)/ -lSwiftWare -o$@
