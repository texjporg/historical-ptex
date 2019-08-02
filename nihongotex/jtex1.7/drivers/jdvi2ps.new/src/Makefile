# The following file and directory specifications may need changing at
# your site:
#
# where is the header file to be found?
HDRDIR=/usr/lib/tex
HDRFILE=${HDRDIR}/jtex.ps
PSPATH=.
#
# where is the software to be installed
BINDIR=/tex/TeX/VAXbin2.9
OPT = BINDIR=${BINDIR} HDRDIR=${HDRDIR} HDRFILE=${HDRFILE}
#
#
# By default we will use PK > GF > PXL format files.
#
# how to global font registration
# include a -DGLOBAL_FONT below
# and edit "jtex.ps" file	/bop0  -> /bop1
#				/bop1  -> /bop0
#
CFLAGS = -DHDRFILE=\"${HDRFILE}\" -DPS_PATH=\"${PSPATH}\" -O
LDFLAGS = -s -O

FONTDIR = ../fonts
FONTLIB = fonts.a
DVIDIR	= ../dvi
DVILIB	= dvi.a

TARGET = jdvi2kps
#
# object file name 
COMMON 	= jdvi2kps.o dvi_private.o fopenp.o postscript.o
OBJ = ${COMMON} ${DVIDIR}/${DVILIB} ${FONTLIB}

# text file

SRC	= jdvi2kps.c dvi_private.c fopenp.c postscript.c init_font.c\
	  jdvi2kps.h Makefile jdvi2kps.1 jtex.ps

all:;
	-(i386 && ${MAKE} ${OPT} 'LDFLAGS=${LDFLAGS} -lmalloc' ${TARGET}) ||\
	${MAKE} ${OPT} ${TARGET}

${COMMON}: jdvi2kps.h ${DVIDIR}/dvi_public.h
dvi_private.o: ${FONTDIR}/public.h
${TARGET}: ${OBJ}
	${CC} ${LDFLAGS} -o $@ ${OBJ}

${FONTLIB}: ${FONTDIR}/init_font.c dummy
	(cd ${FONTDIR}; ${MAKE} 'KFONT = ' $@)
	mv ${FONTDIR}/$@ $@
${FONTDIR}/init_font.c: dummy
	-(cmp $@ init_font.c || cp init_font.c ${FONTDIR})
${DVIDIR}/${DVILIB}: dummy
	(cd ${DVIDIR}; ${MAKE} ${DVILIB})
dummy:

lint:	
	lint ${SRC}

ci:;
	ci -l ${SRC}

co:;
	co -q -l ${SRC}

clean:
	/bin/rm -f ${COMMON} *~

veryclean: clean
	/bin/rm -f ${TARGET}
	(cd ${DVIDIR}; ${MAKE} $@)
	(cd ${FONTDIR}; ${MAKE} $@)

install:
	cp ${TARGET} ${BINDIR}
	cp jtex.ps ${HDRDIR}
