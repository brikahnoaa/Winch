#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# $Id: makefile,v 1.33.0.0 2014/12/08 20:05:10 H.Matsumoto Exp $
# $Id: makefile,v 1.32.2.1 2008/09/11 20:05:10 dbliudnikas Exp $
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# RCS Log:
#
# $Log: makefile,v $
# Revision 1.33      2014/12/8  H.Matsumoto
# Changed for Cygwin enviroment
#
# Revision 1.32.2.1  2008/09/11 20:05:10  dbliudnikas
# Change fwname to Apf9iSeascan.
#
# Revision 1.32  2008/07/14 16:57:04  swift
# Facilitate auto-revision control to avoid conflict with WRC.
#
# Revision 1.31  2007/06/08 02:13:43  swift
# Added automatic management of FormatNotes target.
#
# Revision 1.30  2006/12/06 17:09:24  swift
# Changed executable name used in WRC target.
#
# Revision 1.29  2006/10/13 18:54:28  swift
# Modifications to account for different SAMBA configuration.
#
# Revision 1.28  2006/03/24 23:06:38  swift
# Changed copy-to name in the wrc target.
#
# Revision 1.27  2005/10/11 20:46:19  swift
# Added a target to copy the executable to web site for access by WRC.
#
# Revision 1.26  2005/02/22 21:43:07  swift
# Eliminated use of Motorola 9500 phone in favor of LBT9522.
#
# Revision 1.25  2005/01/06 00:59:22  swift
# Implemented automatic firmware revisioning.
#
# Revision 1.24  2004/12/29 23:13:48  swift
# Fixed firmware revision 123004.
#
# Revision 1.23  2004/07/14 22:53:46  swift
# Modifications that reflect the transition to the new stdio library.
#
# Revision 1.22  2004/06/07 21:22:04  swift
# Reverted back to auotmatic firmware revisioning.
#
# Revision 1.21  2004/05/03 18:07:29  swift
# Set revision manually.
#
# Revision 1.20  2004/04/30 00:22:05  swift
# Modifications to account for changes in the installation directores for
# HiTech and PsdExpress.
#
# Revision 1.19  2004/04/26 16:02:57  swift
# Added automatic revisioning.
#
# Revision 1.18  2004/04/14 18:16:54  swift
# Changed back to auto-rev'ing firmware.
#
# Revision 1.17  2004/03/26 16:36:50  swift
# Revision modification.
#
# Revision 1.16  2004/03/24 00:25:05  swift
# Firmware revision update.
#
# Revision 1.15  2004/02/05 23:56:03  swift
# Revision modification.
#
# Revision 1.14  2003/12/24 17:14:00  swift
# Updated firmware revision.
#
# Revision 1.13  2003/12/17 20:43:28  swift
# Added target to flash the APF9 using the command line version of flink.
#
# Revision 1.12  2003/11/20 18:59:47  swift
# Added GNU Lesser General Public License to source file.
#
# Revision 1.11  2003/11/18 16:55:49  swift
# Modifications to make sure the include directory exists.
#
# Revision 1.10  2003/11/14 19:23:01  swift
# Addition of a target to clean the PSD directory.
#
# Revision 1.9  2003/11/14 15:03:48  swift
# Modifications to move the psd directory to the parent directory.
#
# Revision 1.8  2003/11/13 19:48:52  swift
# Updated the revision number.
#
# Revision 1.7  2003/11/13 19:45:58  swift
# Modifications to account for refactored directory structure.
#
# Revision 1.6  2003/11/13 00:50:31  swift
# Modifications to revision specifier.
#
# Revision 1.4  2003/11/12 23:07:03  swift
# Modifications to linking and revision specification.
#
# Revision 1.3  2003/06/11 18:37:59  swift
# Modifications to use 64-bit doubles rather than 32-bit doubles.
# The drop-in replacement package for services in <time.h> requires
# more precision than 32-bit floats can supply.
#
# Revision 1.2  2003/06/07 20:35:32  swift
# Modifications to automatically generate header files.
#
# Revision 1.1  2003/06/05 13:03:13  swift
# Initial revision
#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.DEFAULT:
.SUFFIXES:
.PRECIOUS: %.obj %.hex %.h $(psdDir)/%.hex
.PHONY: headers force
SHELL=/bin/bash

HiTech = /cygdrive/c/HT-XA#
PsdExpress = /cygdrive/c/PSDexpress#
binDir = ../bin#
psdDir = ../psd#
incDir = ../include#

FwRev := $(shell $(binDir)/FwRev.exe -o)
fwrev = $(shell perl -ne 's/.*revision: ([0-9]{6})/\1/g;print' FirmwareRevision)
fwname := Apf9iSeascan

cFiles := apf9main.c cmds.c control.c telemetr.c profile.c descent.c \
          park.c godeep.c prelude.c eeprom.c engine.c upload.c download.c \
          clogin.c config.c lbt9522.c pactivat.c recovery.c gps.c quecom.c

hFiles := control.h cmds.h telemetr.h profile.h descent.h park.h \
          godeep.h prelude.h eeprom.h engine.h upload.h download.h \
          clogin.h config.h lbt9522.h pactivat.h recovery.h gps.h quecom.h

objFiles := $(subst .c,.obj, $(cFiles))

all: $(incDir) ../lib/apex.lib ../apf9lib/apf9.lib headers $(psdDir)/app.obj \
     FormatNotes
	@echo Done making all ...

wrc: FormatNotes force
	-ssh runt rm -f /www/swift/wrc/Apf9/$(fwname)-\*
	scp ../psd/app.obj runt:/www/swift/wrc/Apf9/$(fwname)-`\
	perl -ne 's/.*revision:[ ]+([0-9]+)/\1/;print' FirmwareRevision`
	-scp $(fwname)-$(fwrev).FormatNotes runt:/www/swift/wrc/Apf9

FormatNotes: force
	@-rm -f $(fwname)-[0-9][0-9][0-9][0-9][0-9][0-9].FormatNotes
	perl -ne 's/\$$FwName\$$-\$$FwRev\$$/$(fwname)-$(fwrev)/g;print' FormatNotes > $(fwname)-$(fwrev).FormatNotes

flash: force
	cd $(psdDir); $(PsdExpress)/flink -i app.jcf -p lpt1 -l flink.log -d OD
	cat $(psdDir)/flink.log
	@echo Done making $@ ...

clean: force
	-rm -f $(objFiles) $(hFiles) {$(psdDir)/,}app.{obj,ob,hex,map,o,sym,lnk}

realclean: cleanlib cleanapf9 clean
	@echo Done making $@...

cleanlib: force
	cd ../lib; make clean

cleanapf9: force
	cd ../apf9lib; make clean

cleanpsd: force
	cd $(psdDir); rm -f app_0.plg* app.{ab*,tt*,afu,as~,er*,xrf*,ppr,pfu,map,frp,lbk,arp,crp,sum,mmf}

%.h: %.c 
	$(binDir)/swh.exe D=$(shell echo $*.h|tr a-z.- A-Z__) if=$< of=$@

%.obj: %.c
#	@rm -f $@
	$(HiTech)/bin/xac -q -E -Bl -LF -Zg -I. -I$(incDir) -C $<

headers: $(incDir) $(hFiles)

$(incDir):
	mkdir $@

apf9main.obj: apf9main.c force
	@rm -f $@
	$(HiTech)/bin/xac -q -E -Bl -LF -Zg -DFWREV=0x$(FwRev)UL -I. -I$(incDir) -C $<
	@echo '(' `date` ") Most recent build of firmware revision: $(FwRev)" > FirmwareRevision

../lib/apex.lib:
	cd ../apf9lib; make include
	cd ../lib; make include
	cd ../lib; make

../apf9lib/apf9.lib:
	cd ../lib; make include
	cd ../apf9lib; make include
	cd ../apf9lib; make

powerup.obj: powerup.as
	$(HiTech)/bin/asxa -x powerup.as

rtxa--l.obj: rtxa--l.as
	$(HiTech)/bin/asxa -x rtxa--l.as

$(psdDir)/app.obj: $(psdDir)/app.hex
	-cd $(psdDir); $(PsdExpress)/utladrm app
	@echo Done making $@ ...

$(psdDir)/app.hex: app.hex
	cp $^ $@
	@echo Done making $@ ...

%.hex: $(objFiles) headers
	@echo -z -Mapp.map -H+app.sym \
         -pvectors=00h,text,stack=2000h \
         -prbit=0110h/022h,rdata/text,rbss/. \
         -pdata=stack/rdata,const,strings,bss/. \
         -pfarbss=010000h,fardata/strings \
         -ga/10000h -pnvram=bss -pheap=nvram \
         -ACODE=00h-0FFFFh,010000h-01FFFFhx255 \
         -pfarnvram=fardata -o$*.o $(objFiles) \
         ../lib/apex.lib ../apf9lib/apf9.lib \
         ../lib/apex.lib ../apf9lib/apf9.lib \
         c:/HT-XA/LIB/XA--LF.LIB \
         c:/HT-XA/LIB/XA--LC.LIB > $*.lnk
	$(HiTech)/bin/hlink @$*.lnk
	$(HiTech)/bin/objtohex $*.o $*.hex
	@echo Done making $@ ...
