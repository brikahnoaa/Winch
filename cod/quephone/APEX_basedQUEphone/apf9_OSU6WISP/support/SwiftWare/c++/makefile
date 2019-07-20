#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# $Id: makefile,v 1.2 2008/07/14 16:59:25 swift Exp $
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# RCS Log:
#
# $Log: makefile,v $
# Revision 1.2  2008/07/14 16:59:25  swift
# Modifications for use with Fedora.
#
# Revision 1.1  2006/11/18 17:22:31  swift
# Added new functionality to library.
#
# Revision 1.3  2006/11/11 18:34:34  swift
# Modifications to conform to current C++ standards.
#
# Revision 1.2  2002/09/19 17:46:59  navo
# Modified perl's substitution pattern in the 'calibrat-parser.oop' target
# to be slightly more general.  This fixed a compilation bug.
#
# Revision 1.1  2001/05/13 10:41:14  navo
# Initial revision
#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.SUFFIXES:
.DEFAULT:
.PRECIOUS: %.o
.PHONY: all test fresh clean force

SHELL=/bin/csh
libDir      = ../..#
libName     = SwiftWare++#

# construct a list of all source files in source directory
srcFiles = $(wildcard *.cpp)

# construct a list of base names
baseNames = $(basename $(notdir $(srcFiles)))

# construct a list of object file names
objFiles = $(addsuffix .o, $(baseNames))

# construct a list of symbolic links
symFiles = profile.oop swiftware.oop lsprf.oop utils.oop

all: $(symFiles) $(libDir)/lib$(libName).a
	@echo done making $@ ...

test:
	@echo libDir    = $(libDir)
	@echo baseNames = $(baseNames)
	@echo srcFiles  = $(srcFiles)
	@echo objFiles  = $(objFiles)

fresh: clean all

clean: force
	- rm -f $(objFiles) $(libDir)/lib$(libName).a

$(libDir)/lib$(libName).a: $(objFiles)
	ar rvu $(libDir)/lib$(libName).a $(objFiles)
	ranlib $(libDir)/lib$(libName).a
	@echo done making $@ ...

%.o: %.cpp
	gcc -x c++ -D$(shell echo $*.cpp|tr a-z. A-Z_) -c -gstabs+ -I{.,../c,../g++}/ -Wall $< -o$@

profile.oop:
	ln -s profile.{cpp,oop}

swiftware.oop:
	ln -s swiftware.{cpp,oop}

lsprf.oop:
	ln -s lsprf.{cpp,oop}

utils.oop:
	ln -s utils.{cpp,oop}

