# The following file and directory specifications may need changing at
# your site:
#
# directory for dvi2ps: where the auxiliary files are installed
DVI2PSLIB=/usr/local/lib/dvi2ps
#
# where the software is installed
BINAREA=/usr/local/bin
#
# name of default fontdesc file in your site
FDNAME=fontdesc
#
# default page type, use a4 if your printer has enough memory (e.g. IMAGEN)
PAGETYPE=note
#
# resolution of the printer
RESOLUTION=300
#
# top margin and left margin (unit 1/72 inch)
TOPMARGIN=72
LEFTMARGIN=72
#
FONTDESC = ${DVI2PSLIB}/${FDNAME}
HDRFILE = ${DVI2PSLIB}/tex.ps
OLDHDRFILE = ${DVI2PSLIB}/tex.ps.old
FIXFONTS = LBP-LW-fix.ps

CFLAGS = -O #-DSYSV
CNFCFLAGS = ${CFLAGS} -DFONTDESC=\"${FONTDESC}\"
DEVCFLAGS = ${CFLAGS} -DHDRFILE=\"${HDRFILE}\" -DRESOLUTION=${RESOLUTION}

DVI2OBJS = dvi2.o run.o mag.o fontdesc.o \
	pkfont.o gffont.o pxlfont.o rastfont.o \
	bifont.o \
	compfont.o \
	jsub.o open.o io.o util.o \
	version.o
PSOBJS = ps.o psspecial.o psrast.o psbi.o

all: dvi2ps

dvi2ps:	${DVI2OBJS} ${PSOBJS}
	${CC} -o $@ ${DVI2OBJS} ${PSOBJS} -lm

install: dvi2ps tex.psdoc.old
	install -s -m 755 dvi2ps ${BINAREA}/dvi2ps
	sed -e s,'%%DVI2PSLIB%%',${DVI2PSLIB}, < ${FDNAME} > ${FONTDESC}
	./psstrip < tex.psdoc | \
	sed -e s/'##XCOORD##'/${LEFTMARGIN}/ -e s/'##YCOORD##'/${TOPMARGIN}/ \
	    -e s/'##PAGETYPE##'/${PAGETYPE}/ \
		> ${HDRFILE}
	./psstrip < tex.psdoc.old | \
	sed -e s/'##XCOORD##'/${LEFTMARGIN}/ -e s/'##YCOORD##'/${TOPMARGIN}/ \
	    -e s/'##PAGETYPE##'/${PAGETYPE}/ \
		> ${OLDHDRFILE}
	cp ${FIXFONTS} ${DVI2PSLIB}

tex.psdoc.old: tex.psdoc
	cp tex.psdoc tex.psdoc.old
	patch tex.psdoc.old tex.psdoc.patch
	rm tex.psdoc.old.orig

clean:	
	rm -f dvi2ps *.o *.orig tex.psdoc.old core

dvi2.o:
	${CC} -c ${CNFCFLAGS} dvi2.c
ps.o:
	${CC} -c ${DEVCFLAGS} ps.c

dvi2.o: defs.h
run.o: defs.h
mag.o: defs.h
fontdesc.o: defs.h
pkfont.o: defs.h rastfont.h pkcodes.h
gffont.o: defs.h rastfont.h gfcodes.h
pxlfont.o: defs.h rastfont.h
rastfont.o: file.h defs.h rastfont.h
compfont.o: defs.h compfont.h
bifont.o: file.h defs.h bifont.h jsub.h
jsub.o: defs.h jsub.h
open.o: defs.h
io.o: defs.h emit.h

ps.o: defs.h emit.h ps.h
psspecial.o: file.h defs.h emit.h
psrast.o: defs.h emit.h rastfont.h ps.h
psbi.o: defs.h emit.h bifont.h ps.h


install-lprdvi:
	sed -e s,'%%DVI2PSLIB%%',${DVI2PSLIB}, \
	    -e s,'%%BINAREA%%',${BINAREA}, \
	    -e s,'%%LEFTMARGIN%%',${LEFTMARGIN}, \
	    -e s,'%%TOPMARGIN%%',${TOPMARGIN}, \
	    -e s,'%%PAGETYPE%%',${PAGETYPE}, \
		lprdvi.csh > ${BINAREA}/lprdvi
	chmod a+x ${BINAREA}/lprdvi
	cp fontdesc-pk ${DVI2PSLIB}/fontdesc-pk
	cp fontdesc-js ${DVI2PSLIB}/fontdesc-js
	./psstrip < tex.psdoc > ${DVI2PSLIB}/tex.ps-templ
