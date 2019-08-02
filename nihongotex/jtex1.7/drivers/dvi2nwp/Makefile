#
# Makefile for dvi previewer.
#
# $Header: Makefile,v 1.2 89/09/16 16:51:40 kono Locked $
#

CC=cc
CFLAGS = -O

# Program's directory
JXDVI_BIN=/usr/local/bin/jxdvi
JX11DVI_BIN=/usr/bin/X11/jx11dvi
DVI2NWP_BIN=/usr/sony/lib/dvi2nwp

# Fonts' directory
NWF_FONT_DIR = /usr/sony/lib/font/devnwp533
SONY_FONT_DIR= /usr/sony/lib/font/devnwp533

all: jx11dvi dvi2nwp

install: all
	-install -c jxdvi   $(JXDVI_BIN)
	-install -c jx11dvi $(JX11DVI_BIN)
	-install -c dvi2nwp $(DVI2NWP_BIN)

install_jxdvi: jxdvi
	-install -c jxdvi   $(JXDVI_BIN)
install_jx11dvi: jx11dvi
	-install -c jx11dvi $(JX11DVI_BIN)
install_dvi2nwp: dvi2nwp
	-install -c dvi2nwp $(DVI2NWP_BIN)


JXDVI_CFLAGS   = -DJXDVI -DX10 $(CFLAGS)
JX11DVI_CFLAGS = -DJXDVI -DX11 $(CFLAGS)
DVI2NWP_CFLAGS = -DDVI2NWP $(CFLAGS)

# Remove
#   /usr/sony/lib/FSlib.a
# if  you do not have NWF-605.
JXDVI_LIBS   = -L/usr/sony/lib -lFSlib -lX -lm
JX11DVI_LIBS = -L/usr/sony/lib -lFSlib -lX11 -lm
DVI2NWP_LIBS = -L/usr/sony/lib -lFSlib -lm
# JXDVI_LIBS   = -L/usr/sony/lib  -lX -lm
# JX11DVI_LIBS = -L/usr/sony/lib  -lX11 -lm
# DVI2NWP_LIBS = -L/usr/sony/lib  -lm


JXDVI_OBJS = dv_jxdvi.o font_jxdvi.o font_db.o c2i.o font_load.o \
	window_jxdvi.o table_jxdvi.o read_pk_jxdvi.o \
	read_pxl_jxdvi.o read_nwf_jxdvi.o read_v_nwf_jxdvi.o \
	read_dnf_jxdvi.o
JX11DVI_OBJS = dv_jx11dvi.o font_jx11dvi.o font_db.o c2i.o font_load.o \
	window_jx11dvi.o read_pk_jx11dvi.o \
	read_pxl_jx11dvi.o read_nwf_jx11dvi.o read_v_nwf_jx11dvi.o \
	read_dnf_jx11dvi.o
DVI2NWP_OBJS = dv.o font.o font_db.o c2i.o add.o font_load.o \
	read_pk.o read_pxl.o read_nwf.o read_v_nwf.o read_dnf.o special.o


jxdvi: $(JXDVI_OBJS)
	$(CC) $(CFLAGS) $(JXDVI_CFLAGS) $(JXDVI_OBJS) $(JXDVI_LIBS) -o $@ 
jx11dvi: $(JX11DVI_OBJS)
	$(CC) $(CFLAGS) $(JX11DVI_CFLAGS) $(JX11DVI_OBJS) $(JX11DVI_LIBS) -o $@ 
dvi2nwp: $(DVI2NWP_OBJS)
	$(CC) $(CFLAGS) $(DVI2NWP_CFLAGS) $(DVI2NWP_OBJS) $(DVI2NWP_LIBS) -o $@ 

# for JXDVI
dv_jxdvi.o : dv.c  config.h jxdvi.h font.h dvi.h add.h window.h
	$(CC) -c $(JXDVI_CFLAGS) -o $@ dv.c
window_jxdvi.o: window.c config.h jxdvi.h window.h font.h
	$(CC) -c $(JXDVI_CFLAGS) -o $@ window.c
table_jxdvi.o : table.c 
	$(CC) -c $(JXDVI_CFLAGS) -o $@ table.c
font_jxdvi.o : font.c config.h jxdvi.h font.h dvi.h
	$(CC) -c $(JXDVI_CFLAGS) -o $@ font.c
read_pk_jxdvi.o: read_pk.c config.h pk.h jxdvi.h font.h
	$(CC) -c $(JXDVI_CFLAGS) -o $@ read_pk.c
read_pxl_jxdvi.o: read_pxl.c config.h jxdvi.h font.h
	$(CC) -c $(JXDVI_CFLAGS) -o $@ read_pxl.c
read_nwf_jxdvi.o: read_nwf.c config.h jxdvi.h font.h
	$(CC) -c $(JXDVI_CFLAGS) -o $@ read_nwf.c
read_v_nwf_jxdvi.o: read_v_nwf.c config.h font.h
	$(CC) -c $(JXDVI_CFLAGS) -o $@ read_v_nwf.c
read_dnf_jxdvi.o: read_dnf.c config.h font.h
	$(CC) -c $(JXDVI_CFLAGS) -o $@ read_dnf.c

# for JX11DVI
dv_jx11dvi.o : dv.c  config.h jxdvi.h font.h dvi.h add.h window.h
	$(CC) -c $(JX11DVI_CFLAGS) -o $@ dv.c
table_jx11dvi.o : table.c 
	$(CC) -c $(JX11DVI_CFLAGS) -o $@ table.c
font_jx11dvi.o : font.c config.h jxdvi.h font.h dvi.h
	$(CC) -c $(JX11DVI_CFLAGS) -o $@ font.c
read_pk_jx11dvi.o: read_pk.c config.h pk.h jxdvi.h font.h
	$(CC) -c $(JX11DVI_CFLAGS) -o $@ read_pk.c
read_pxl_jx11dvi.o: read_pxl.c config.h jxdvi.h font.h
	$(CC) -c $(JX11DVI_CFLAGS) -o $@ read_pxl.c
read_nwf_jx11dvi.o: read_nwf.c config.h jxdvi.h font.h
	$(CC) -c $(JX11DVI_CFLAGS) -o $@ read_nwf.c
read_v_nwf_jx11dvi.o: read_v_nwf.c config.h font.h
	$(CC) -c $(JX11DVI_CFLAGS) -o $@ read_v_nwf.c
read_dnf_jx11dvi.o: read_dnf.c config.h font.h
	$(CC) -c $(JX11DVI_CFLAGS) -o $@ read_dnf.c
window_jx11dvi.o: window.c config.h jxdvi.h window.h font.h
	$(CC) -c $(JX11DVI_CFLAGS) -o $@ window.c

# for DVI2NWP
dv.o: dv.c config.h jxdvi.h font.h dvi.h add.h
	$(CC) -c $(DVI2NWP_CFLAGS) -o $@ dv.c
font.o: font.c  config.h jxdvi.h font.h dvi.h
	$(CC) -c $(DVI2NWP_CFLAGS) -o $@ font.c
add.o: add.c config.h font.h add.h
	$(CC) -c $(DVI2NWP_CFLAGS) -o $@ add.c
read_pxl.o: read_pxl.c config.h jxdvi.h font.h
	$(CC) -c $(DVI2NWP_CFLAGS) -o $@ read_pxl.c
read_pk.o: read_pk.c config.h pk.h jxdvi.h font.h
	$(CC) -c $(DVI2NWP_CFLAGS) -o $@ read_pk.c
read_nwf.o: read_nwf.c config.h jxdvi.h font.h
	$(CC) -c $(DVI2NWP_CFLAGS) -o $@ read_nwf.c
read_v_nwf.o: read_v_nwf.c config.h font.h
	$(CC) -c $(DVI2NWP_CFLAGS) -o $@ read_v_nwf.c
read_dnf.o: read_dnf.c config.h font.h
	$(CC) -c $(DVI2NWP_CFLAGS) -o $@ read_dnf.c
special.o: special.c config.h add.h
	$(CC) -c $(DVI2NWP_CFLAGS) -o $@ special.c

# Common objects
font_db.o: font_db.c config.h font.h
	$(CC) -c $(CFLAGS) -o $@ font_db.c
c2i.o : c2i.c 
	$(CC) -c $(CFLAGS) -o $@ c2i.c
font_load.o: font_load.c config.h font.h font_load.h
	$(CC) -c $(CFLAGS) -o $@ font_load.c

#
# for cleaning up
#
clean:
	-rm -f *.o *.BAK *.CKP jxdvi jx11dvi dvi2nwp a.out core errs *~ *.s

#
# for linking fonts
#
link_604_font:
	-ln -s $(SONY_FONT_DIR)/M10.font $(NWF_FONT_DIR)/min7.500nwf
	-ln -s $(SONY_FONT_DIR)/M11.font $(NWF_FONT_DIR)/min8.500nwf
	-ln -s $(SONY_FONT_DIR)/M12.font $(NWF_FONT_DIR)/min9.500nwf
	-ln -s $(SONY_FONT_DIR)/M14.font $(NWF_FONT_DIR)/min10.500nwf
	-ln -s $(SONY_FONT_DIR)/M17.font $(NWF_FONT_DIR)/min10.600nwf
	-ln -s $(SONY_FONT_DIR)/M17.font $(NWF_FONT_DIR)/min10.720nwf
	-ln -s $(SONY_FONT_DIR)/M17.font $(NWF_FONT_DIR)/min10.864nwf
	-ln -s $(SONY_FONT_DIR)/M17.font $(NWF_FONT_DIR)/min12.500nwf
	-ln -s $(SONY_FONT_DIR)/M39.font $(NWF_FONT_DIR)/min7.2000nwf
	-ln -s $(SONY_FONT_DIR)/M44.font $(NWF_FONT_DIR)/min8.2000nwf
	-ln -s $(SONY_FONT_DIR)/M50.font $(NWF_FONT_DIR)/min9.2000nwf
	-ln -s $(SONY_FONT_DIR)/M55.font $(NWF_FONT_DIR)/min10.2000nwf
	-ln -s $(SONY_FONT_DIR)/M66.font $(NWF_FONT_DIR)/min10.2400nwf
	-ln -s $(SONY_FONT_DIR)/M66.font $(NWF_FONT_DIR)/min10.2880nwf
	-ln -s $(SONY_FONT_DIR)/M66.font $(NWF_FONT_DIR)/min10.3456nwf
	-ln -s $(SONY_FONT_DIR)/M66.font $(NWF_FONT_DIR)/min12.2000nwf
	-ln -s $(SONY_FONT_DIR)/G39.font $(NWF_FONT_DIR)/goth7.2000nwf
	-ln -s $(SONY_FONT_DIR)/G44.font $(NWF_FONT_DIR)/goth8.2000nwf
	-ln -s $(SONY_FONT_DIR)/G50.font $(NWF_FONT_DIR)/goth9.2000nwf
	-ln -s $(SONY_FONT_DIR)/G55.font $(NWF_FONT_DIR)/goth10.2000nwf
	-ln -s $(SONY_FONT_DIR)/G66.font $(NWF_FONT_DIR)/goth10.2400nwf
	-ln -s $(SONY_FONT_DIR)/G66.font $(NWF_FONT_DIR)/goth10.2880nwf
	-ln -s $(SONY_FONT_DIR)/G66.font $(NWF_FONT_DIR)/goth10.3456nwf
	-ln -s $(SONY_FONT_DIR)/G66.font $(NWF_FONT_DIR)/goth12.2000nwf

link_605_font:
	-ln -s $(SONY_FONT_DIR)/M7.font $(NWF_FONT_DIR)/min5.590nwf
	-ln -s $(SONY_FONT_DIR)/M9.font $(NWF_FONT_DIR)/min6.590nwf
	-ln -s $(SONY_FONT_DIR)/M10.font $(NWF_FONT_DIR)/min7.590nwf
	-ln -s $(SONY_FONT_DIR)/M12.font $(NWF_FONT_DIR)/min8.590nwf
	-ln -s $(SONY_FONT_DIR)/M13.font $(NWF_FONT_DIR)/min9.590nwf
	-ln -s $(SONY_FONT_DIR)/M15.font $(NWF_FONT_DIR)/min10.590nwf
	-ln -s $(SONY_FONT_DIR)/M16.font $(NWF_FONT_DIR)/min10.646nwf
	-ln -s $(SONY_FONT_DIR)/M18.font $(NWF_FONT_DIR)/min10.708nwf
	-ln -s $(SONY_FONT_DIR)/M22.font $(NWF_FONT_DIR)/min10.850nwf
	-ln -s $(SONY_FONT_DIR)/M26.font $(NWF_FONT_DIR)/min10.1020nwf
	-ln -s $(SONY_FONT_DIR)/M31.font $(NWF_FONT_DIR)/min10.1223nwf
	-ln -s $(SONY_FONT_DIR)/M37.font $(NWF_FONT_DIR)/min10.1468nwf
	-ln -s $(SONY_FONT_DIR)/M25.font $(NWF_FONT_DIR)/min5.2000nwf
	-ln -s $(SONY_FONT_DIR)/M30.font $(NWF_FONT_DIR)/min6.2000nwf
	-ln -s $(SONY_FONT_DIR)/M36.font $(NWF_FONT_DIR)/min7.2000nwf
	-ln -s $(SONY_FONT_DIR)/M41.font $(NWF_FONT_DIR)/min8.2000nwf
	-ln -s $(SONY_FONT_DIR)/M46.font $(NWF_FONT_DIR)/min9.2000nwf
	-ln -s $(SONY_FONT_DIR)/M51.font $(NWF_FONT_DIR)/min10.2000nwf
	-ln -s $(SONY_FONT_DIR)/M56.font $(NWF_FONT_DIR)/min10.2191nwf
	-ln -s $(SONY_FONT_DIR)/M61.font $(NWF_FONT_DIR)/min10.2400nwf
	-ln -s $(SONY_FONT_DIR)/M73.font $(NWF_FONT_DIR)/min10.2880nwf
	-ln -s $(SONY_FONT_DIR)/M88.font $(NWF_FONT_DIR)/min10.3456nwf
	-ln -s $(SONY_FONT_DIR)/M105.font $(NWF_FONT_DIR)/min10.4147nwf
	-ln -s $(SONY_FONT_DIR)/M126.font $(NWF_FONT_DIR)/min10.4977nwf
	-ln -s $(SONY_FONT_DIR)/G7.font $(NWF_FONT_DIR)/goth5.590nwf
	-ln -s $(SONY_FONT_DIR)/G9.font $(NWF_FONT_DIR)/goth6.590nwf
	-ln -s $(SONY_FONT_DIR)/G10.font $(NWF_FONT_DIR)/goth7.590nwf
	-ln -s $(SONY_FONT_DIR)/G12.font $(NWF_FONT_DIR)/goth8.590nwf
	-ln -s $(SONY_FONT_DIR)/G13.font $(NWF_FONT_DIR)/goth9.590nwf
	-ln -s $(SONY_FONT_DIR)/G15.font $(NWF_FONT_DIR)/goth10.590nwf
	-ln -s $(SONY_FONT_DIR)/G16.font $(NWF_FONT_DIR)/goth10.646nwf
	-ln -s $(SONY_FONT_DIR)/G18.font $(NWF_FONT_DIR)/goth10.708nwf
	-ln -s $(SONY_FONT_DIR)/G22.font $(NWF_FONT_DIR)/goth10.850nwf
	-ln -s $(SONY_FONT_DIR)/G26.font $(NWF_FONT_DIR)/goth10.1020nwf
	-ln -s $(SONY_FONT_DIR)/G31.font $(NWF_FONT_DIR)/goth10.1223nwf
	-ln -s $(SONY_FONT_DIR)/G37.font $(NWF_FONT_DIR)/goth10.1468nwf
	-ln -s $(SONY_FONT_DIR)/G25.font $(NWF_FONT_DIR)/goth5.2000nwf
	-ln -s $(SONY_FONT_DIR)/G30.font $(NWF_FONT_DIR)/goth6.2000nwf
	-ln -s $(SONY_FONT_DIR)/G36.font $(NWF_FONT_DIR)/goth7.2000nwf
	-ln -s $(SONY_FONT_DIR)/G41.font $(NWF_FONT_DIR)/goth8.2000nwf
	-ln -s $(SONY_FONT_DIR)/G46.font $(NWF_FONT_DIR)/goth9.2000nwf
	-ln -s $(SONY_FONT_DIR)/G51.font $(NWF_FONT_DIR)/goth10.2000nwf
	-ln -s $(SONY_FONT_DIR)/G56.font $(NWF_FONT_DIR)/goth10.2191nwf
	-ln -s $(SONY_FONT_DIR)/G61.font $(NWF_FONT_DIR)/goth10.2400nwf
	-ln -s $(SONY_FONT_DIR)/G73.font $(NWF_FONT_DIR)/goth10.2880nwf
	-ln -s $(SONY_FONT_DIR)/G88.font $(NWF_FONT_DIR)/goth10.3456nwf
	-ln -s $(SONY_FONT_DIR)/G105.font $(NWF_FONT_DIR)/goth10.4147nwf
	-ln -s $(SONY_FONT_DIR)/G126.font $(NWF_FONT_DIR)/goth10.4977nwf

# 
# for revision control
#
SOURCES = config.h add.h dvi.h font.h font_load.h jxdvi.h pk.h window.h \
	dv.c font.c font_db.c c2i.c add.c font_load.c window.c \
	read_pk.c read_pxl.c read_nwf.c read_v_nwf.c read_dnf.c\
	README.jis
RCSFILES= ./RCS/add.c,v ./RCS/dv.c,v ./RCS/font.h,v ./RCS/jxdvi.h,v \
          ./RCS/read_pk.c,v ./RCS/add.h,v ./RCS/dvi.h,v ./RCS/font_db.c,v \
          ./RCS/pk.h,v ./RCS/c2i.c,v ./RCS/font.c,v ./RCS/font_load.c,v \
          ./RCS/window.h,v ./RCS/window.c,v \
	  ./RCS/README.jis,v ./RCS/read_pxl.c,v ./RCS/read_v_nwf.c,v

ci: $(RCSFILES)

./RCS/add.c,v : add.c
./RCS/dv.c,v : dv.c
./RCS/font.h,v : font.h 
./RCS/jxdvi.h,v : jxdvi.h
./RCS/read_pk.c,v : read_pk.c
./RCS/read_pxl.c,v : read_pxl.c
./RCS/read_v_nwf.c,v : read_v_nwf.c
./RCS/add.h,v : add.h
./RCS/dvi.h,v : dvi.h 
./RCS/font_db.c,v : font_db.c
./RCS/pk.h,v : pk.h
./RCS/c2i.c,v : c2i.c
./RCS/font.c,v : font.c
./RCS/font_load.c,v : font_load.c
./RCS/window.h,v : window.h
./RCS/window.c,v : window.c
$(RCSFILES) : 
	ci $@

co: $(SOURCES)

# add.c : RCS/EmptyFile
# dv.c : RCS/dv.c,v
# font.h  : RCS/font.h,v
# jxdvi.h : RCS/jxdvi.h,v
# read_pk.c : RCS/read_pk.c,v
# read_pxl.c : RCS/read_pxl.c,v
# read_nwf.c : RCS/read_nwf.c,v
# read_v_nwf.c : RCS/read_v_nwf.c,v
# add.h : RCS/add.h,v
# dvi.h  : RCS/dvi.h,v
# font_db.c : RCS/font_db.c,v
# pk.h : RCS/pk.h,v
# c2i.c : RCS/c2i.c,v
# font.c : RCS/font.c,v
# font_load.c : RCS/font_load.c,v
# window.h : RCS/window.h,v
# window.c :  RCS/window.c,v
# README.jis : RCS/README.jis,v
$(SOURCES) :
	co -l $@

profile:
	dvi2nwp -di /usr/JTeX/doc/jfm.dvi; mv gmon.out gmon.out.jfm
	dvi2nwp -di /usr/JTeX/doc/jtex.dvi; mv gmon.out gmon.out.jtex
	dvi2nwp -di /usr/JTeX/doc/jtexdoc.dvi; mv gmon.out gmon.out.jtexdoc
	gprof -b dvi2nwp gmon.out.jfm gmon.out.jtex gmon.out.jtexdoc > /tmp/p
