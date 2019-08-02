#
#   Makefile for pTeX distribution, subdirectory `'
#
#   This file makes pDVItype from its WEB sources.
#

SITEDIR	=../..
RM=/bin/rm
OPT=-O
CFLAGS=$(OPT) -Dlink=link_array -I$(SITEDIR)
LDFLAGS=-s

.SUFFIXES:
.SUFFIXES: .o .c .p .ch

.p.c:
	./convert $*.p $*.c

.c.o:
	$(CC) $(CFLAGS) -c $*.c

all:	jdvitype

# jDVItype
jdvitype: jdvitype.o jdvityext.o
	$(CC) $(CFLAGS) -o jdvitype jdvitype.o jdvityext.o
jdvityext.o: jdvityext.c ${SITEDIR}/site.h
jdvitype.o: jdvitype.c web2c.h
jdvitype.p: dvitype.web jdvitype.ch
	../../jtangle/jtangle dvitype.web jdvitype.ch
	mv dvitype.p jdvitype.p
jdvitype.c: jdvitype.p

clean:
	$(RM) -f jdvitype.p dvitype.pool jdvitype.o jdvityext.o
veryclean:	clean
	$(RM) -f jdvitype.c jdvitype.h jdvitype
