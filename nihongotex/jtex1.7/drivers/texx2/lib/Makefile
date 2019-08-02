#
# Copyright (c) 1987 University of Maryland Department of Computer Science.
# All rights reserved.  Permission to copy for any purpose is hereby granted
# so long as this copyright notice remains intact.
#
# Makefile for ctex/lib (C-TeX library routines)
#
# $Header: /home/reed/grunwald/Projects/Iptex/lib/RCS/Makefile,v 1.6 89/02/13 14:31:29 grunwald Exp Locker: grunwald $
#
DESTDIR=
# Alas, -R makes ALL initialised variables read-only, and we cannot
# use it on font files; hence, FCFLAGS.
CC=cc
CFLAGS=	$(CDEBUGFLAGS) -I../h $(JTEX) 
FCFLAGS=$(CDEBUGFLAGS) -I../h $(JTEX) 

# 4.1BSD Vax:
#ASSRC=	bcopy.s bzero.s
#ASOBJ=	bcopy.o bzero.o
#MISCC=	getopt.c
#MISCO=	getopt.o

# 4.2BSD Vax:
#ASSRC=
#ASOBJ=
#MISCC=	getopt.c
#MISCO=	getopt.o

# 4.2BSD Sun:
#ASSRC=
#ASOBJ=
#MISCC=
#MISCO=

# 4.2BSD Pyramid:
#ASSRC=
#ASOBJ=
#MISCC=	getopt.c
#MISCO=	getopt.o

# 4.3BSD Vax:
ASSRC=
ASOBJ=
MISCC=
MISCO=
# all
CSRC=	conv.c dviclass.c error.c findpost.c fio.c font.c font_subr.c \
	gfclass.c gripes.c magfactor.c rotate.c scaletfm.c scanpost.c \
	search.c seek.c split.c strsave.c tfm.c ${MISCC}
COBJ=	conv.o dviclass.o error.o findpost.o fio.o font.o font_subr.o \
	gfclass.o gripes.o magfactor.o rotate.o scaletfm.o scanpost.o \
	search.o seek.o split.o strsave.o tfm.o ${MISCO}
FSRC=	gffont.c pkfont.c pxlfont.c tfmfont.c
FOBJ=	gffont.o pkfont.o pxlfont.o tfmfont.o

OBJS=	${COBJ} ${ASOBJ} ${FOBJ}

all: lib.a

lib.a: ${OBJS}
	ar cr lib.a ${OBJS}
	ranlib lib.a

$(CSRC) $(FSRC):
	co -q $@

src: $(CSRC) $(FSRC)
	@echo Done

# no installation is necessary; this entry is just for standardisation
install:

clean:
	rm -f *.o lib.a

#pxl.o:
#	${CC} ${CFLAGS} -c -DPXLPATH=\"${PXLPATH}\" pxl.c

# font.o needs to know where to find the font description file
font.o:
	${CC} ${CFLAGS} -c -DFONTDESC=\"${FONTDESC}\" font.c

# special rules for font objects
#	-- actually, we don't use this anymore
#	${FOBJ}:
#	${CC} ${FCFLAGS} -c $*.c

# DO NOT DELETE THIS LINE -- make depend uses it
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
# DO NOT DELETE THIS LINE -- make depend depends on it.

conv.o: ../h/types.h ../h/conv.h
dviclass.o: ../h/dviclass.h
error.o: /usr/include/stdio.h /usr/include/varargs.h
findpost.o: /usr/include/stdio.h ../h/types.h ../h/dvicodes.h ../h/fio.h
fio.o: /usr/include/stdio.h ../h/types.h ../h/fio.h
font.o: /usr/include/stdio.h /usr/include/errno.h /usr/include/sys/errno.h
font.o: ../h/types.h ../h/conv.h ../h/font.h
font_subr.o: ../h/font.h
getopt.o: /usr/include/stdio.h
gfclass.o: ../h/gfclass.h
gffont.o: /usr/include/stdio.h /usr/include/sys/types.h
gffont.o: /usr/include/sys/stat.h ../h/types.h
gffont.o: ../h/font.h ../h/gfcodes.h ../h/gfclass.h ../h/num.h
gripes.o: /usr/include/stdio.h ../h/types.h
pkfont.o: /usr/include/stdio.h /usr/include/sys/types.h
pkfont.o: /usr/include/sys/stat.h ../h/types.h
pkfont.o: ../h/font.h ../h/num.h
pxlfont.o: /usr/include/stdio.h /usr/include/sys/types.h
pxlfont.o: /usr/include/sys/stat.h
pxlfont.o: /usr/include/errno.h /usr/include/sys/errno.h ../h/types.h
pxlfont.o: ../h/font.h ../h/fio.h
rotate.o: ../h/font.h
scaletfm.o: ../h/types.h ../h/font.h
scanpost.o: /usr/include/stdio.h ../h/types.h ../h/dvicodes.h ../h/fio.h
scanpost.o: ../h/postamble.h
search.o: ../h/types.h ../h/search.h
seek.o: /usr/include/stdio.h /usr/include/sys/param.h
seek.o: /usr/include/sys/signal.h
seek.o: /usr/include/sys/types.h 
seek.o: /usr/include/sys/file.h
seek.o: /usr/include/sys/stat.h
split.o: /usr/include/ctype.h
tfm.o: /usr/include/stdio.h ../h/types.h ../h/fio.h ../h/tfm.h
tfmfont.o: /usr/include/stdio.h /usr/include/sys/types.h
tfmfont.o: /usr/include/sys/stat.h ../h/types.h
tfmfont.o: ../h/conv.h ../h/font.h ../h/tfm.h
