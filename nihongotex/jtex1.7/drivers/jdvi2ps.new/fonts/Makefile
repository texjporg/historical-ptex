#
# This is a makefile for font libraries.
#
# Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
#
# $Header: Makefile,v 1.14 90/01/08 14:50:59 kakiuchi Exp $
# $Author: kakiuchi $
# $Date: 90/01/08 14:50:59 $
#

FONTPATHS = '-DDEFAULT_FONT_PATH="/usr/lib/tex/fonts"'\
	    '-DDEFAULT_TFM_PATH="/usr/lib/tex/fonts"'
#CFLAGS = -g -p -DDEBUG ${FONTPATHS}
CFLAGS = -O ${FONTPATHS}
SRC = font.c pxl.c jxl4.c gf.c pk.c xfont.c tfm.c tfmfont.c jfmfont.c\
      utils.c kfont.c init_font_skel.c\
      common.h font.h pxl.h jxl4.h gf.h jfmfont.h kfont.h pk.h\
      public.h tfm.h tfmfont.h xfont.h Makefile
COMMON = font.o pxl.o gf.o pk.o tfm.o tfmfont.o jfmfont.o utils.o
#XFONT = xfont.o
XFONT =
KFONT = jxl4.o kfont.o
KFONT_S = jxl4.o kfont_s.o
OPT = ${KFONT} ${XFONT}
OPT_S = ${KFONT_S} ${XFONT}
OBJS = ${COMMON} ${OPT}
OBJS_S = ${COMMON} ${OPT_S}

fonts.a: ${OBJS}
	ar rv $@ ${OBJS}
	-ranlib $@

fonts_s.a: ${OBJS_S}
	ar rv $@ ${OBJS_S}
	-ranlib $@

all: fonts.a fonts_s.a

${COMMON} kfont.o kfont_s.o: font.h common.h
font.o: init_font.c public.h
pxl.o: pxl.h
jxl4.o: jxl4.h
gf.o: gf.h
pk.o: pk.h
tfmfont.o: tfmfont.h tfm.h
jfmfont.o: jfmfont.h tfm.h
tfm.o: tfm.h
xfont.o: xfont.h tfm.h ../src/jdvi2x.h
kfont.o: kfont.h tfm.h
kfont_s.o: kfont.h tfm.h
	${CC} -c kfont.c -DSON ${CFLAGS}
	mv kfont.o kfont_s.o

ci:
	ci -l ${SRC}
co:
	co -q -l ${SRC}

clean:
	/bin/rm -f *.o *~

veryclean: clean
	/bin/rm -f fonts.a fonts_s.a

