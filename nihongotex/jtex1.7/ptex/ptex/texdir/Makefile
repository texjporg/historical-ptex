############################################################
# Makefile for tex.p
############################################################

TANGLE	=	../jtangle/jtangle

default:	ptex.p

bigptex.p:	tex.web bigpctex.ch
		${TANGLE} tex.web bigpctex.ch
		mv tex.p bigptex.p
		mv tex.pool ptex.pool

ptex.p:		tex.web pctex.ch
		${TANGLE} tex.web pctex.ch
		mv tex.p ptex.p
		mv tex.pool ptex.pool

clean:
		rm -f ptex.p ptex.pool
