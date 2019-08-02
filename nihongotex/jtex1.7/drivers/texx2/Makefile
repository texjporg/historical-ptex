#
#	Set `FONTDESC' to the location of your fontdesc file, unquoted.
#	This is passed to the library so they can find it.
#	TEXFONTS is used by texx2 to find your tfm files. It'll
#	use the env. variable TEXFONTS if a user has that set.
#
FONTDESC	=/usr/lib/tex/fontdesc
TEXFONTS=.:/usr/lib/tex/fonts:/usr/lib/tex/fonts/tfm
#
#	Where binaries should be installed. 
#
BINDIR		=/usr/bin/X11
#
#	Where your version of `ipr' lives
#
IPRBIN		=/usr/local/bin
#
#	Where man pages go
#
MANDIR		=/usr/man
#
#	This is the script to convert an `Imakefile' into a `Makefile'
#
XIMAKE		=imake -s Makefile -I/usr/lib/X11/config -DTOPDIR=/project2/X/X.V11R4/mit
#
#	Your compiler. I use `gcc', but it's not required.
#
CC	=gcc -traditional
CDEBUGFLAGS	=-O
JTEX	=-DASCIIJTEX -DFAKEKANJI
#JTEX	=-DNTTJTEX
#
#	Assemble options for submakes
#
MFLAGS	= -k CC="$(CC)" CDEBUGFLAGS="$(CDEBUGFLAGS)" JTEX="$(JTEX)"\
		  FONTDESC="$(FONTDESC)" BINDIR="$(BINDIR)" MANDIR="$(MANDIR)"\
		  PARALLEL="$(PARALLEL)" TEXFONTS="$(TEXFONTS)"

goal: doall

doall:	lib/lib.a
	cd texx2; $(XIMAKE); $(MAKE) $(MFLAGS) texx2 mftobdf
#	cd drivers; $(MAKE) $(MFLAGS) imagen1 texsun texx
	cd utilities; $(MAKE) $(MFLAGS) dviselect

install:
	cd lib ; $(MAKE) $(MFLAGS)
	cd drivers ; $(MAKE) $(MFLAGS)  inst-imagen1 inst-texx inst-texsun
	cd texx2; $(XIMAKE) ; $(MAKE) $(MFLAGS) install
	cd utilities; $(MAKE) $(MFLAGS) inst-dviselect

dotexx2: lib/lib.a utilities/dviselect
	cd texx2; $(XIMAKE); $(MAKE) $(MFLAGS) texx2 mftobdf

dotexsun: lib/lib.a utilities/dviselect
	cd drivers; $(MAKE) $(MFLAGS) texsun

doiptex: lib/lib.a
	cd drivers ; $(MAKE) $(MFLAGS) imagen1

dolib:	
	cd lib; $(MAKE) $(MFLAGS)

lib/lib.a:
	cd lib; $(MAKE) $(MFLAGS)

utilities/dviselect:
	cd utilities ; $(MAKE) $(MFLAGS) dviselect

clean:
	cd lib ; $(MAKE) $(MFLAGS) clean
	cd texx2 ; $(MAKE) $(MFLAGS) clean
	cd drivers ; $(MAKE) $(MFLAGS) clean
	cd utilities ; $(MAKE) $(MFLAGS) clean

#
#	Build tar files
#
UIUCTEXVERSION	=2.2
TEXX2VERSION	=2.9

texx2tar: FILES.common FILES.texx2
	rm -f texx$(TEXX2VERSION).tar texx$(TEXX2VERSION).tar.Z
	tar crf texx$(TEXX2VERSION).tar `cat FILES.common FILES.texx2`
	compress texx$(TEXX2VERSION).tar

tar: FILES.common FILES.texx2 FILES.drivers
	rm -f uiuctex$(UIUCTEXVERSION).tar uiuctex$(UIUCTEXVERSION).tar.Z
	tar crf uiuctex$(UIUCTEXVERSION).tar `cat FILES.common FILES.texx2 FILES.drivers`
	compress uiuctex$(UIUCTEXVERSION).tar
