.DEFAULT:
.SUFFIXES:
.PRECIOUS: %.aux
.PHONY: all clean force bibtex latex html

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# $Id: makefile,v 1.3 2006/12/27 17:04:12 swift Exp $
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# RCS Log:
#
# $Log: makefile,v $
# Revision 1.3  2006/12/27 17:04:12  swift
# Changed the implicit target %.dvi to depend on %.tex.
#
# Revision 1.2  2006/11/22 02:53:59  swift
# Added targets for automatic revision control management.
#
# Revision 1.1  2006/11/03 19:08:57  swift
# Added user manual to CVS control.
#
# Revision 1.1  2006/11/03 17:56:41  swift
# Added user manual to CVS control.
#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# define the shell to use
SHELL = /bin/csh

# location of the html directory
html-dir = /www/swift

FwRev := $(shell perl -ne 's/.*revision: ([0-9]{6})/\1/g;print' ../src/FirmwareRevision)

all: latex
	@echo Done making $@ ...

clean: clean-latex clean-html
	@echo Done making $@ ...

manual: bibtex dvi
	@echo Done making $@ ...

latex: IridiumApex.tex force #bibtex
	latex IridiumApex

bibtex: IridiumApex.bbl IridiumApex.blg
	@echo Done making $@ ...

ps: IridiumApex.ps
	@echo Done making $@ ...

pdf: IridiumApex.pdf #bibtex
	@echo Done making $@ ...

dvi: IridiumApex.dvi
	@echo Done making $@ ...

IridiumApex.tex: FwRev.tex

%.pdf: %.ps
	ps2pdf $< $@

%.ps: %.dvi
	dvips $< -o $@

%.dvi: %.tex force
	latex $*
	latex $*
	latex $*
	latex $*

%.bbl %.blg: %.aux
	bibtex $*

%.aux: %.tex
	latex $<

clean-latex: force
	-rm -f IridiumApex.{aux,bbl,blg,dvi,lof,log,lot,toc,ps,pdf}
	-rm -f IridiumMsgSample.{aux,bbl,blg,dvi,lof,log,lot,toc,ps,pdf}
	-rm -f IridiumLogSample.{aux,bbl,blg,dvi,lof,log,lot,toc,ps,pdf}
	-rm -f IridiumProcDataSample.{aux,bbl,blg,dvi,lof,log,lot,toc,ps,pdf}
	-rm -f EncodeCSourceCode.{aux,bbl,blg,dvi,lof,log,lot,toc,ps,pdf}
	-rm -f texput.log

html: force
	latex2html -address "Dana Swift, swift@ocean.washington.edu" \
	           -dir $(html-dir)/PTV-manual \
              -t "PTV Operating Guidelines" \
              IridiumApex.tex

clean-html: force
	-rm -rf $(html-dir)/IridiumApexManual

FwRev.tex: ../src/FirmwareRevision
	@perl -i~ -ne 's/\{\\fwrev\}\{[0-9]{6}\}/{\\fwrev}{$(FwRev)}/g;print' FwRev.tex
	@rm -f FwRev.tex~
