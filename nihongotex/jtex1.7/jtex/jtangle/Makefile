#
#   Makefile for pTeX distribution, subdirectory `./jtangle'
#
#   This file makes jTANGLE from its WEB sources.
#

BINDIR=/usr/local/bin

CFLAGS=-O -Dlink=link_array -I..
LDFLAGS=-s

.SUFFIXES:
.SUFFIXES: .o .c .p .ch

#.p.c:
#	convert $*.p $*.c

.c.o:
	$(CC) $(CFLAGS) -c $*.c

all: jtangle

install: all
	mv jtangle ${BINDIR}


# Tangle
jtangle: jtangle.o tanext.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o jtangle jtangle.o tanext.o
jtangle.o: jtangle.c jtangle.h web2c.h

#jtangle.c: jtangle.p
jtangle.c:
	cp SAVEjtangle.c jtangle.c

jtangle.h:
	cp SAVEjtangle.h jtangle.h

jtangle.p: tangle.web jtangle.ch
	tangle tangle.web jtangle.ch
	mv tangle.p jtangle.p

tanext.o: tanext.c ../site.h

clean:
	rm -f *.pool *.o *.p
