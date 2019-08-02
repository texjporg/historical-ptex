# Copyright (c) Japan TeX Users Group, 1986 1987
#
#	TeX print (front end)
#

CFLAGS= -O
#CFLAGS= -g -DDEBUG
#CFLAGS= -p -DDEBUG

DEST= /usr/local

OBJ= main.o page.o font.o pkfont.o etc.o util.o util1.o

texpr:		${OBJ}
		cc ${CFLAGS} -o texpr ${OBJ}

main.o:		../common/dvicom.h ../common/def.h

page.o:		../common/dvicom.h

font.o:		../common/def.h

pkfont.o:	../common/def.h ../common/pkcom.h

etc.o:		../common/def.h

util.o:		../common/util.c
		cc ${CFLAGS} -c ../common/util.c

util1.o:	../common/util1.c ../common/def.h
		cc ${CFLAGS} -c ../common/util1.c

install:
		install -c -s texpr ${DEST}

clean:
		rm *.o  *~

