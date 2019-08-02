#
# This is a makefile for dvi libraries.
#
# Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
#
# $Header: Makefile,v 1.2 90/01/10 18:29:17 kakiuchi Exp $
# $Author: kakiuchi $
# $Date: 90/01/10 18:29:17 $
#

#CFLAGS = -g -p -DDEBUG
CFLAGS = -O -s
FONTDIR = ../fonts
SRC = dvi.c dvi.h dvi_public.h types.h Makefile
OBJS = dvi.o
TARGET = dvi.a

all: ${TARGET}

${TARGET}: ${OBJS}
	ar rv $@ ${OBJS}
	-ranlib $@

dvi.o: dvi.h ${FONTDIR}/public.h dvi_public.h types.h

ci:
	ci -l ${SRC}
co:
	co -q -l ${SRC}

clean:
	/bin/rm -f ${OBJS} *~

veryclean: clean
	/bin/rm -f ${TARGET}
