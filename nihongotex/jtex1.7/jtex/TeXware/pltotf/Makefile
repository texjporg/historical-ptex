#
#   Makefile for pTeX distribution, subdirectory `./ptex/pltotf'
#
#   This file makes TFtoPL and PLtoTF from their WEB sources.
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

all:	tftopl pltotf

# TFtoPL
tftopl: tftopl.o plext.o
	$(CC) $(CFLAGS) -o tftopl tftopl.o plext.o
tftopl.p: tftopl.web tftopl.ch
tftopl.c: tftopl.p
tftopl.o: tftopl.c web2c.h
	$(CC) $(CFLAGS) -c -Dindex=index_type tftopl.c

# PLtoTF
pltotf: pltotf.o plext.o
	$(CC) $(CFLAGS) -o pltotf pltotf.o plext.o
pltotf.p: pltotf.web pltotf.ch
pltotf.c: pltotf.p
pltotf.o: pltotf.c web2c.h
	$(CC) $(CFLAGS) -c -Dindex=index_array pltotf.c
plext.o:  plext.c ${SITEDIR}/site.h

clean:
	$(RM) -f tftopl.p pltotf.p tftopl.pool pltotf.pool
	$(RM) -f tftopl.o pltotf.o plext.o
veryclean:	clean
	$(RM) -f tftopl.c pltotf.c tftopl.h pltotf.h
	$(RM) -f tftopl pltotf
