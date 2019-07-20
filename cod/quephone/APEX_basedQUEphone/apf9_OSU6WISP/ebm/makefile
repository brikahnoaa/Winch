#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# $Id: makefile,v 1.5 2008/07/14 17:04:53 swift Exp $
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# RCS Log:
#
# $Log: makefile,v $
# Revision 1.5  2008/07/14 17:04:53  swift
# Statically link the energy budget utility for easier distribution of the binary.
#
# Revision 1.4  2006/11/19 17:12:33  swift
# Change the name of the energy budget model source code file to follow
# current name conventions.
#
# Revision 1.3  2006/11/18 17:40:29  swift
# Change makefile to work with iridium floats.
#
# Revision 1.2  2006/11/11 18:58:56  swift
# Added targets to make symbolic links.
#
# Revision 1.1  2006/11/11 18:29:34  swift
# Added energy budget model to the project.
#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.DEFAULT:
.SUFFIXES:
.PRECIOUS: %.o
.PHONY: force

DATE="01/01/80"
SHELL=/bin/tcsh
CFLAGS= -gstabs+ -I. -I../support/SwiftWare/c -L../support -Wall -Wmissing-prototypes $(F)
C++FLAGS= -x c++ -gstabs+ -I. -I../support/SwiftWare/{c,c++,g++} -Wall $(F)
CLIBS=-lSwiftWare -lfl -lstdc++
C++LIBS= -lg++ -lSwiftWare++ 
SYSLIBS= -lm -ltermcap 

srcDir = .#
binDir = ../bin#

# construct a list of all source files in source directory
srcFiles  = Apex260Sbe41cpApf9i.cpp

# construct a list of base names
baseNames = $(basename $(notdir $(srcFiles)))

# construct a list of executables for bin directory
binFiles  =  $(addprefix $(binDir)/, $(baseNames))

# construct a list of symbolic links
symFiles = HydroObs.oop AlkalineBatteryPack.oop LithiumBatteryPack.oop

OBJS=HydroObs.o AlkalineBatteryPack.o LithiumBatteryPack.o

all: $(symFiles) $(binFiles)

$(binDir)/%: $(srcDir)/%.o $(OBJS)
	gcc  $< $(LIBS) $(OBJS) $(CLIBS) $(C++LIBS) $(CLIBS) $(C++LIBS) $(SYSLIBS) $(CFLAGS) $(LIBS) -static -o $@

%.o: %.c
	gcc -D$(shell echo $*.c|tr a-z.- A-Z__) -c $(CFLAGS) $*.c

%.o: %.cc
	gcc -c $(C++FLAGS) -DUSE_LIBGXX_INLINES $*.cc -o $*.o 

%.o: %.cpp
	gcc -D$(shell echo $*.cpp|tr a-z.- A-Z__) -c $(C++FLAGS) $*.cpp -o $*.o 

%.cpp: %.l
	flex -o$*.cpp $*.l

%.c: %.l
	flex -o$*.c $*.l

empty:
	@echo Make argument missing...

clean: force
	-rm -f $(OBJS) $(binFiles) $(addsuffix .o, $(baseNames) ) 

swiftware:
	- cd ~swift/procs; make tar DATE="$(DATE)"
	- cd ~swift/oop; make tar DATE="$(DATE)"
	- cd ~swift/program.lib; make tar DATE="$(DATE)"
	- cd ~swift; tar -Pcvf swiftware.tar {procs/procs.tar,oop/oop.tar,program.lib/progs.tar}; gzip swiftware.tar

HydroObs.oop:
	ln -s HydroObs.{cpp,oop}

AlkalineBatteryPack.oop:
	ln -s AlkalineBatteryPack.{cpp,oop}

LithiumBatteryPack.oop:
	ln -s LithiumBatteryPack.{cpp,oop}
