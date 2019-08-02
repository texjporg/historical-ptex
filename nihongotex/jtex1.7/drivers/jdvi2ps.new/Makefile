BINDIR = /usr/local/bin
HDRDIR = /usr/lib/tex
HDRFILE = /usr/lib/tex/jtex.ps

MANIFESTS = MANIFEST.new MANIFEST.fonts dvi/MANIFEST

all install clean veryclean:
	(cd src; make BINDIR=${BINDIR} HDRDIR=${HDRDIR} HDRFILE=${HDRFILE} $@)
shar:;
	/bin/rm -rf kit/*
#	cat ${MANIFESTS} | makekit -s50k -nkit/Part -oMANIFEST
	mailsplit -A -V kit/Mail `cat ${MANIFESTS}`

