############################################################
# Makefile for tex.p
############################################################

TANGLE	=	../jtangle/jtangle

default:	jtex.p

bigjtex.p:	tex.web bigjctex.ch
		${TANGLE} tex.web bigjctex.ch
		mv tex.p bigjtex.p
		mv tex.pool jtex.pool

jtex.p:		tex.web jctex.ch
		${TANGLE} tex.web jctex.ch
		mv tex.p jtex.p
		mv tex.pool jtex.pool

clean:
		rm -f jtex.p jtex.pool
