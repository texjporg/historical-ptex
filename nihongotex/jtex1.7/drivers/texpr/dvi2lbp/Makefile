# Copyright (c) Japan TeX Users Group, 1986 1987
#
#	dvi to LBP-8
#

CFLAGS= -O
#CFLAGS= -g -DDEBUG
#CFLAGS= -p -DDEBUG

DEST= /usr/lib

OBJ= main.o page.o font.o pkfont.o lbp.o util.o util1.o fsel.o

dvi2lbp:	${OBJ}
		cc ${CFLAGS} -o dvi2lbp ${OBJ}

main.o:		../common/dvicom.h ../common/def.h

page.o:		../common/dvicom.h ../common/def.h

font.o:		../common/def.h

fsel.o:		../common/def.h

pkfont.o:	../common/def.h ../common/pkcom.h

lbp.o:		../common/def.h

util.o:		../common/util.c
		cc ${CFLAGS} -c ../common/util.c

util1.o:	../common/util1.c ../common/def.h
		cc ${CFLAGS} -c ../common/util1.c

install:
		install -c -s dvi2lbp ${DEST}

clean:
		rm *.o  *~
