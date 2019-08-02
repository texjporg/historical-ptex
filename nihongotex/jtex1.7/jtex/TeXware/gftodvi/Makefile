#
#   Makefile for TeX82 distribution, subdirectory `mfware'
#
#   This file makes GFtoDVI from its WEB sources.
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

.ch.p:
	../../jtangle/jtangle $*.web $*.ch

.c.o:
	$(CC) $(CFLAGS) -c $*.c

all:	gftodvi

# GFtoDVI
gftodvi: gftodvi.o mfwarext.o
	$(CC) $(CFLAGS) -o gftodvi gftodvi.o mfwarext.o
gftodvi.p: gftodvi.web gftodvi.ch
gftodvi.c: gftodvi.p
gftodvi.o: gftodvi.c web2c.h

mfwarext.o:  mfwarext.c ${SITEDIR}/site.h

clean:
	$(RM) -f gftodvi.p gftodvi.pool gftodvi.o mfwarext.o 
veryclean:	clean
	$(RM) -f gftodvi.c gftodvi.h gftodvi
