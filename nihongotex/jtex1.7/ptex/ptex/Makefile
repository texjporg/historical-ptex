#################################
# Overall Makefile for WEB to C	#
#				#
# Tim Morgan Jan 15, 1988	#
#################################

# Installation directories
BINDIR=/usr/local/bin
TEXPOOLDIR=/usr/lib/tex
MFPOOLDIR=/usr/lib/mf
CDIR=ctex
CONV=convert

# Libraries needed for X10 support
XLIBS=-lX
# Libraries needed for X11 support
X11LIBS=-lX11
# Libraries needed for SunView support
SUNLIBS=-lsuntools -lsunwindows -lpixrect

# Modify the following line to select only the libraries you need
WLIBS=

all:		web2c initex virtex

BIG:		biginitex bigvirtex

# add install-tex and install-latex if you wish to use undumped versions
install-all:	install-initex install-virtex install-inimf install-virmf

install-tex:	${CDIR}/tex
		cp ${CDIR}/tex ${BINDIR}/tex

install-latex:	${CDIR}/latex
		cp ${CDIR}/latex ${BINDIR}/latex

install-virtex:	${CDIR}/virtex
		cp ${CDIR}/virtex ${BINDIR}/virtex

install-initex:	${CDIR}/initex
		cp ${CDIR}/initex ${BINDIR}/initex
		cp ${CDIR}/tex.pool ${TEXPOOLDIR}/tex.pool

web2c:
		(cd web2cdir; make)

triptex:	web2c jtangle ${CDIR}/tex0.c ${CDIR}/triptex
${CDIR}/triptex:
		(cd ${CDIR}; make triptex)

initex:		web2c jtangle ${CDIR}/initex
${CDIR}/initex:	${CDIR}/tex0.c
		(cd ${CDIR}; make initex)

virtex:		web2c jtangle ${CDIR}/virtex
${CDIR}/virtex:	${CDIR}/tex0.c
		(cd ${CDIR}; make virtex)

biginitex:	web2c jtangle bigctex/initex
bigctex/initex:	bigctex/tex0.c
		(cd bigctex; make initex)

bigvirtex:	web2c jtangle bigctex/virtex
bigctex/virtex:	bigctex/tex0.c
		(cd bigctex; make virtex)

${CDIR}/tex0.c:	texdir/ptex.p
		(cd texdir; ${CONV})

bigctex/tex0.c:	texdir/bigptex.p
		(cd texdir; bigconvert)

texdir/ptex.p:	texdir/tex.web texdir/pctex.ch
		(cd texdir; make ptex.p)

texdir/bigptex.p:	texdir/tex.web texdir/bigpctex.ch
		(cd texdir; make bigptex.p)

tex:		${CDIR}/tex
${CDIR}/tex:	initex virtex
		(cd ${CDIR}; make tex)

latex:		${CDIR}/latex
${CDIR}/latex:	initex virtex
		(cd ${CDIR}; make latex)

jtangle:
		(cd jtangle; make jtangle)


clean:
		(cd texdir; make clean)
		(cd web2cdir; make clean)
		(cd ${CDIR}; make clean)
		(cd jtangle; make clean)
