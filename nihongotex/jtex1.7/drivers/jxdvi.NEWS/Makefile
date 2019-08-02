#
# Makefile for dvi previewer.
#
# $Header: Makefile,v 10.5 86/02/01 15:44:17 tony Rel $
#
DESTDIR=
CONFDIR= /usr/new
TEXFONTDIR=/usr/lib/tex/fonts
SONYFONTDIR=/usr/sony/lib/font/devnwp533


JXDVI_CFLAGS= -O -DJXDVI -DSTATISTICS
#
# Turn on -DACCT if accounting is desired.
#
# DVI2NWP_CFLAGS= -O -DSTATISTICS -DDVI2NWP -DACCT
#
DVI2NWP_CFLAGS= -O -DSTATISTICS -DDVI2NWP

# JXDVI_CFLAGS= -p -DJXDVI -DSTATISTICS
# DVI2NWP_CFLAGS= -g -DSTATISTICS -DDVI2NWP

JXDVI_LIBS= -lX
# JXDVI_LIBS= -lX_p

JXDVI_OBJS= dv_jxdvi.o table_jxdvi.o font_jxdvi.o font_db_jxdvi.o \
			c2i_jxdvi.o font_load_jxdvi.o XDisplayName.o
DVI2NWP_OBJS= dv.o font.o font_db.o c2i.o add.o font_load.o

JXDVI_H= jxdvi.h dvi.h font.h pxl.h
DVI2NWP_H=jxdvi.h dvi.h font.h pxl.h add.h

SOURCES= $(DVI2NWP_H) dv.c font.c font_db.c c2i.c add.c font_load.c

all: jxdvi dvi2nwp

jxdvi: $(JXDVI_OBJS) $(JXDVI_H)
	cc $(JXDVI_CFLAGS) -o $@ $(JXDVI_OBJS) $(JXDVI_LIBS)
dvi2nwp: $(DVI2NWP_OBJS) $(DVI2NWP_H)
	cc $(DVI2NWP_CFLAGS) -o $@ $(DVI2NWP_OBJS)

# for JXDVI
dv_jxdvi.o : dv.c
	cc -c $(JXDVI_CFLAGS) -o $@ dv.c
table_jxdvi.o : table.c
	cc -c $(JXDVI_CFLAGS) -o $@ table.c
font_jxdvi.o : font.c
	cc -c $(JXDVI_CFLAGS) -o $@ font.c
font_db_jxdvi.o : font_db.c
	cc -c $(JXDVI_CFLAGS) -o $@ font_db.c
c2i_jxdvi.o  : c2i.c
	cc -c $(JXDVI_CFLAGS) -o $@ c2i.c
font_load_jxdvi.o: font_load.c
	cc -c $(JXDVI_CFLAGS) -o $@ font_load.c
XDisplayName.o: XDisplayName.c
	cc -c $(JXDVI_CFLAGS) -o $@ XDisplayName.c

# for DVI2NWP
dv.o: dv.c
	cc -c $(DVI2NWP_CFLAGS) -o $@ dv.c
font.o: font.c
	cc -c $(DVI2NWP_CFLAGS) -o $@ font.c
font_db.o: font_db.c
	cc -c $(DVI2NWP_CFLAGS) -o $@ font_db.c
c2i.o : c2i.c
	cc -c $(DVI2NWP_CFLAGS) -o $@ c2i.c
add.o: add.c
	cc -c $(DVI2NWP_CFLAGS) -o $@ add.c
font_load.o: font_load.c
	cc -c $(DVI2NWP_CFLAGS) -o $@ font_load.c

link_font:
	-ln -s $(SONYFONTDIR)/M10.font $(TEXFONTDIR)/min7.500nwf
	-ln -s $(SONYFONTDIR)/M11.font $(TEXFONTDIR)/min8.500nwf
	-ln -s $(SONYFONTDIR)/M12.font $(TEXFONTDIR)/min9.500nwf
	-ln -s $(SONYFONTDIR)/M14.font $(TEXFONTDIR)/min10.500nwf
	-ln -s $(SONYFONTDIR)/M17.font $(TEXFONTDIR)/min10.600nwf
	-ln -s $(SONYFONTDIR)/M17.font $(TEXFONTDIR)/min10.720nwf
	-ln -s $(SONYFONTDIR)/M17.font $(TEXFONTDIR)/min10.864nwf
	-ln -s $(SONYFONTDIR)/M17.font $(TEXFONTDIR)/min12.500nwf
	-ln -s $(SONYFONTDIR)/M39.font $(TEXFONTDIR)/min7.2000nwf
	-ln -s $(SONYFONTDIR)/M44.font $(TEXFONTDIR)/min8.2000nwf
	-ln -s $(SONYFONTDIR)/M50.font $(TEXFONTDIR)/min9.2000nwf
	-ln -s $(SONYFONTDIR)/M55.font $(TEXFONTDIR)/min10.2000nwf
	-ln -s $(SONYFONTDIR)/M66.font $(TEXFONTDIR)/min10.2400nwf
	-ln -s $(SONYFONTDIR)/M66.font $(TEXFONTDIR)/min10.2880nwf
	-ln -s $(SONYFONTDIR)/M66.font $(TEXFONTDIR)/min10.3456nwf
	-ln -s $(SONYFONTDIR)/M66.font $(TEXFONTDIR)/min12.2000nwf
	-ln -s $(SONYFONTDIR)/G39.font $(TEXFONTDIR)/goth7.2000nwf
	-ln -s $(SONYFONTDIR)/G44.font $(TEXFONTDIR)/goth8.2000nwf
	-ln -s $(SONYFONTDIR)/G50.font $(TEXFONTDIR)/goth9.2000nwf
	-ln -s $(SONYFONTDIR)/G55.font $(TEXFONTDIR)/goth10.2000nwf
	-ln -s $(SONYFONTDIR)/G66.font $(TEXFONTDIR)/goth10.2400nwf
	-ln -s $(SONYFONTDIR)/G66.font $(TEXFONTDIR)/goth10.2880nwf
	-ln -s $(SONYFONTDIR)/G66.font $(TEXFONTDIR)/goth10.3456nwf
	-ln -s $(SONYFONTDIR)/G66.font $(TEXFONTDIR)/goth12.2000nwf

clean:
	-rm -f *.o *.BAK *.CKP jxdvi dvi2nwp a.out core errs *~ *.s

rcs:
	ci $(SOURCES)
	co -l $(SOURCES)

rcs_co:
	co -l $(SOURCES)

test:
	rcp dump news2:~/lbp
	rsh news2 'cd ~/lbp; t'

profile:
	dvi2nwp -di /usr/JTeX/doc/jfm.dvi; mv gmon.out gmon.out.jfm
	dvi2nwp -di /usr/JTeX/doc/jtex.dvi; mv gmon.out gmon.out.jtex
	dvi2nwp -di /usr/JTeX/doc/jtexdoc.dvi; mv gmon.out gmon.out.jtexdoc
	gprof -b dvi2nwp gmon.out.jfm gmon.out.jtex gmon.out.jtexdoc > /tmp/p

install:
	-mv dvi2nwp /usr/TeX/NEWSbin
	-mv jxdvi /usr/TeX/NEWSbin

