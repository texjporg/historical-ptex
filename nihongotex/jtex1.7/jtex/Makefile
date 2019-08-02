#################################
# Overall Makefile for WEB to C	#
#				#
# Tim Morgan Jan 15, 1988	#
#################################

# Installation directories
BINDIR=/usr/local/bin
TEXPOOLDIR=/usr/lib/tex
MFPOOLDIR=/usr/lib/mf

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

# add install-jtex and install-jlatex if you wish to use undumped versions
install-all:	install-initex install-virtex

install-tex:	ctex/jtex
		cp ctex/jtex ${BINDIR}/jtex

install-latex:	ctex/jlatex
		cp ctex/jlatex ${BINDIR}/jlatex

install-virtex:	ctex/virtex
		cp ctex/virtex ${BINDIR}/virtex

install-initex:	ctex/initex
		cp ctex/initex ${BINDIR}/initex
		cp ctex/jtex.pool ${TEXPOOLDIR}/jtex.pool

web2c:
		(cd web2cdir; make)

#triptex:	web2c jtangle/jtangle ctex/tex0.c ctex/triptex
#ctex/triptex:
#		(cd ctex; make triptex)

initex:		web2c jtangle/jtangle ctex/initex
ctex/initex:	ctex/tex0.c
		(cd ctex; make initex)

virtex:		web2c jtangle/jtangle ctex/virtex
ctex/virtex:	ctex/tex0.c
		(cd ctex; make virtex)

biginitex:	web2c jtangle/jtangle bigctex/initex
bigctex/initex:	bigctex/tex0.c
		(cd bigctex; make initex)

bigvirtex:	web2c jtangle/jtangle bigctex/virtex
bigctex/virtex:	bigctex/tex0.c
		(cd bigctex; make virtex)

ctex/tex0.c:	texdir/jtex.p
		(cd texdir; convert)

bigctex/tex0.c:	texdir/bigjtex.p
		(cd texdir; bigconvert)

texdir/jtex.p:	texdir/tex.web texdir/jctex.ch
		(cd texdir; make jtex.p)

texdir/bigjtex.p:	texdir/tex.web texdir/bigjctex.ch
		(cd texdir; make bigjtex.p)

jtex:		ctex/jtex
ctex/jtex:	initex virtex
		(cd ctex; make jtex)

jlatex:		ctex/jlatex
ctex/jlatex:	initex virtex
		(cd ctex; make jlatex)

jtangle/jtangle:
		(cd jtangle; make jtangle)


clean:
		(cd texdir; make clean)
		(cd web2cdir; make clean)
		(cd ctex; make clean)
		(cd bigctex; make clean)
		(cd jtangle; make clean)
