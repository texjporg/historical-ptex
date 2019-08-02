#	@(#)Makefile	4.1	(Berkeley)	10/19/82
#
DESTDIR=/usr/local
DEFSFILE=/usr/lib/vgrindefs
TEXINPUTS=/usr/lib/tex/macros

CFLAGS=-O 
SOURCES=jtfontedpr.c vgrindefs.c regexp.c retest.c
CMDS=jtfontedpr retest
OBJS=retest.o regexp.o jtfontedpr.o vgrindefs.o

all:	$(CMDS)

jtfontedpr: jtfontedpr.o vgrindefs.o regexp.o
	cc ${CFLAGS} -o jtfontedpr jtfontedpr.o vgrindefs.o regexp.o

jtfontedpr.o: jtfontedpr.c
	cc ${CFLAGS} -DDEFSFILE=\"${DEFSFILE}\" -c jtfontedpr.c

retest: retest.o regexp.o
	cc ${CFLAGS} -o retest retest.o regexp.o

install: all
	install -s jtfontedpr ${DESTDIR}/lib/jtfontedpr
	install -c jtgrind ${DESTDIR}/bin/jtgrind
	install -c tgrindmac.tex ${TEXINPUTS}/tgrindmac.tex
	install -c vgrindefs.src ${DEFSFILE}

clean:
	rm -f ${CMDS} ${OBJS}
