#########################################
# Makefile for initex, virtex, etc.	#
#					#
# Tim Morgan 5/24/88			#
#########################################

SITEDIR=..
#CC=gcc
OPT=-O
CFLAGS=$(OPT) -I$(SITEDIR)
LDFLAGS=
RM=/bin/rm

IOBJS= itex0.o itex1.o itex2.o itex3.o itex4.o itex5.o \
	itex6.o itex7.o itex8.o itex9.o iextra.o initex.o code.o jextra.o
VOBJS= itex0.o itex1.o itex2.o itex3.o itex4.o itex5.o \
	itex6.o itex7.o itex8.o itex9.o vextra.o virtex.o code.o jextra.o
TOBJS= ttex0.o ttex1.o ttex2.o ttex3.o ttex4.o ttex5.o \
	ttex6.o ttex7.o ttex8.o itex9.o textra.o triptex.o code.o jextra.o

all:		initex virtex jtex jlatex

triptex:	$(TOBJS)
		$(CC) $(LDFLAGS) -o triptex $(TOBJS)

initex:		$(IOBJS)
		$(CC) $(LDFLAGS) -o initex $(IOBJS)

virtex:		$(VOBJS)
		$(CC) $(LDFLAGS) -o virtex $(VOBJS)

# extra.c and itex.c contain "#ifdef INITEX", so they have to be
# compiled both with and without -DINITEX.
iextra.o:	extra.c orig_texd.h tex.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) -c $(CFLAGS) -DINITEX extra.c && mv -f extra.o iextra.o
vextra.o:	extra.c orig_texd.h tex.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) -c $(CFLAGS) extra.c && mv -f extra.o vextra.o
textra.o:	extra.c trip_texd.h tex.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=trip_texd.h texd-h
		$(CC) -c $(CFLAGS) -DINITEX extra.c && mv -f extra.o textra.o

initex.o:	itex.c orig_texd.h tex.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) $(CFLAGS) -DINITEX -c itex.c && mv -f itex.o initex.o
virtex.o:	itex.c orig_texd.h tex.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) $(CFLAGS) -c itex.c && mv -f itex.o virtex.o
triptex.o:	itex.c trip_texd.h tex.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=trip_texd.h texd-h
		$(CC) $(CFLAGS) -DINITEX -c itex.c && mv -f itex.o triptex.o

code.o:
		$(CC) $(CFLAGS) -c code.c

jextra.o:
		$(CC) $(CFLAGS) -c jextra.c

# The rest of the files don't contain INITEX-dependent code, so they
# are just compiled once for both initex and virtex, but they still
# have to be made separately for triptex.
itex0.o:	tex0.c orig_texd.h tex.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) $(CFLAGS) -c tex0.c && mv -f tex0.o itex0.o
itex1.o:	tex1.c tex.h orig_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) $(CFLAGS) -c tex1.c && mv -f tex1.o itex1.o
itex2.o:	tex2.c tex.h orig_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) $(CFLAGS) -c tex2.c && mv -f tex2.o itex2.o
itex3.o:	tex3.c tex.h orig_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) $(CFLAGS) -c tex3.c && mv -f tex3.o itex3.o
itex4.o:	tex4.c tex.h orig_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) $(CFLAGS) -c tex4.c && mv -f tex4.o itex4.o
itex5.o:	tex5.c tex.h orig_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) $(CFLAGS) -c tex5.c && mv -f tex5.o itex5.o
itex6.o:	tex6.c tex.h orig_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) $(CFLAGS) -c tex6.c && mv -f tex6.o itex6.o
itex7.o:	tex7.c tex.h orig_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) $(CFLAGS) -c tex7.c && mv -f tex7.o itex7.o
itex8.o:	tex8.c tex.h orig_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) $(CFLAGS) -c tex8.c && mv -f tex8.o itex8.o
itex9.o:	tex9.c tex.h orig_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=orig_texd.h texd-h
		$(CC) $(CFLAGS) -c tex9.c && mv -f tex9.o itex9.o

# These .o files are used only to build triptex
ttex0.o:	tex0.c tex.h trip_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=trip_texd.h texd-h
		$(CC) $(CFLAGS) -c tex0.c && mv -f tex0.o ttex0.o
ttex1.o:	tex1.c tex.h trip_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=trip_texd.h texd-h
		$(CC) $(CFLAGS) -c tex1.c && mv -f tex1.o ttex1.o
ttex2.o:	tex2.c tex.h trip_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=trip_texd.h texd-h
		$(CC) $(CFLAGS) -c tex2.c && mv -f tex2.o ttex2.o
ttex3.o:	tex3.c tex.h trip_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=trip_texd.h texd-h
		$(CC) $(CFLAGS) -c tex3.c && mv -f tex3.o ttex3.o
ttex4.o:	tex4.c tex.h trip_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=trip_texd.h texd-h
		$(CC) $(CFLAGS) -c tex4.c && mv -f tex4.o ttex4.o
ttex5.o:	tex5.c tex.h trip_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=trip_texd.h texd-h
		$(CC) $(CFLAGS) -c tex5.c && mv -f tex5.o ttex5.o
ttex6.o:	tex6.c tex.h trip_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=trip_texd.h texd-h
		$(CC) $(CFLAGS) -c tex6.c && mv -f tex6.o ttex6.o
ttex7.o:	tex7.c tex.h trip_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=trip_texd.h texd-h
		$(CC) $(CFLAGS) -c tex7.c && mv -f tex7.o ttex7.o
ttex8.o:	tex8.c tex.h trip_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=trip_texd.h texd-h
		$(CC) $(CFLAGS) -c tex8.c && mv -f tex8.o ttex8.o
ttex9.o:	tex9.c tex.h trip_texd.h coerce.h ${SITEDIR}/site.h
		@make texdsrc=trip_texd.h texd-h
		$(CC) $(CFLAGS) -c tex9.c && mv -f tex9.o ttex9.o

texd-h:		$(texdsrc)
		-@cmp $(texdsrc) texd.h || cp $(texdsrc) texd.h


veryclean:	clean
		$(RM) -f tex?.c itex.c texd.h coerce.h jtex.pool \
			jtex jlatex virtex initex triptex
clean:
		$(RM) -f *.o *.fmt core

jplain.fmt:	initex
		./initex 'jplain \dump'

jlplain.fmt:	initex
		./initex 'jlplain \dump'

splain.fmt:	initex
		./initex 'splain \dump'

# xtex is for use with the experimental fmtdump program.
# Use at your own risk!
#xtex:		$(VOBJS) plain.o
#		$(CC) $(LDFLAGS) -o xtex $(VOBJS) plain.o

# The tex and latex entries are for systems with working undump's
jtex:		virtex jplain.fmt
		./virtex '&jplain \input HackyInputFileNameForCoreDump.tex'
		@echo ''
		undump jtex virtex core
		$(RM) -f core

jlatex:		virtex jlplain.fmt
		./virtex '&jlplain \input HackyInputFileNameForCoreDump.tex'
		@echo ''
		undump jlatex virtex core
		$(RM) -f core

slitex:		virtex splain.fmt
		./virtex '&splain \input HackyInputFileNameForCoreDump.tex'
		@echo ''
		undump slitex virtex core
		$(RM) -f core
