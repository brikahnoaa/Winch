#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# $Id: makefile,v 1.4 2008/07/14 16:59:19 swift Exp $
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# RCS Log:
#
# $Log: makefile,v $
# Revision 1.4  2008/07/14 16:59:19  swift
# Modifications for use with Fedora.
#
# Revision 1.3  2006/11/18 17:19:44  swift
# Added targets to automatically manage symbolic links.
#
# Revision 1.3  2006/11/11 18:32:44  swift
# Added the makefile to the CVS repository.
#
# Revision 1.2  2005/02/22 22:01:25  swift
# Eliminated GNU regex implementation.
#
# Revision 1.1.1.1  2003/11/12 23:22:58  swift
# Apf9 firmware for a 260ml Apex with SBE41 CTD.
#
# Revision 1.2  2003/06/07 17:33:54  swift
# Eliminate extraneous targets.
#
# Revision 1.1  2001/06/03 07:16:40  navo
# Initial revision
#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.SUFFIXES:
.DEFAULT:
.PRECIOUS: %.o
.PHONY: all test fresh clean force

SHELL=/bin/csh
libDir      = ../..#
libName     = SwiftWare#

# construct a list of all source files in source directory
srcFiles = $(filter-out regengin.c, $(wildcard *.c))

# construct a list of base names
baseNames = $(basename $(notdir $(srcFiles)))

# construct a list of object file names
objFiles = $(addsuffix .o, $(baseNames))

# construct a list of symbolic links
symFiles = defs.p crecipes.p hydro.p math.p oddnend.p 

all: $(symFiles) signature.h $(libDir)/lib$(libName).a
	@echo done making $@ ...

test: force
	@echo libDir    = $(libDir)
	@echo baseNames = $(baseNames)
	@echo srcFiles  = $(srcFiles)
	@echo objFiles  = $(objFiles)

fresh: clean all

clean: force
	- rm -f signature.h $(objFiles) $(libDir)/lib$(libName).a ../bin/machar

$(libDir)/lib$(libName).a: $(objFiles)
	ar rvu $(libDir)/lib$(libName).a $(objFiles)
	ranlib $(libDir)/lib$(libName).a
	@echo done making $@ ...

$(symFiles):
	ln -s SwiftWare.h $@

signature.h: ../bin/machar
	../bin/machar > signature.h

../bin/machar: machar.o dmachar.o
	gcc ./machar.o ./dmachar.o -I./ ../src/machar.c -o ../bin/machar

%.o:  %.c
	gcc -c -gstabs+ -I./ -Wall -Wmissing-prototypes $< -o$@

regengine.o: regengine.c
	touch regengine.o

regcomp.o:  regcomp.c
	gcc -c -gstabs+ -I./ $< -o$@
