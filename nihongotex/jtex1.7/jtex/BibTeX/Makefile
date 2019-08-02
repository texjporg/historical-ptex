#############################
# Makefile for JBibTeX in C #
#############################

OPT=-O
SITEDIR=..
LDFLAGS=
CFLAGS=$(OPT) -I$(SITEDIR)
BINDIR=/usr/local/bin
BSTDIR=/usr/lib/tex/inputs

RM=/bin/rm -f

# for NTT jtex
#TANGLE=../texware/tangle
# for ASCII jtex
TANGLE=../jtangle/jtangle

SRCKIT = readme readme.1st.unix Makefile sed.script bibext.c \
	 bibtex.ch bibtex.defines bibtex.h kanji.h

BSTKIT = bst/cpp.awk bst/generate.sh bst/jbtxbst.doc bst/abbrv.bst \
	 bst/alpha.bst bst/plain.bst bst/unsrt.bst

SRC = bibtex0.c bibtex1.c bibtex2.c bibtex3.c bibtex4.c
OBJ = bibtex0.o bibtex1.o bibtex2.o bibtex3.o bibtex4.o

all:		jbibtex bibstyle

jbibtex:	$(OBJ) bibext.o
		$(CC) $(LDFLAGS) $(CFLAGS) -o jbibtex $(OBJ) bibext.o

bibstyle:	bst
		cd bst; make all

$(OBJ):		$(SRC) bibtex.h $(SITEDIR)/site.h kanji.h texd.h

bibext.o:	bibext.c $(SITEDIR)/site.h kanji.h texd.h

$(SRC):		bibtex.p bibtex.defines
		cat bibtex.defines bibtex.p \
			| ../web2cdir/web2c -hbibtex.h \
			| sed -f sed.script \
			| ../web2cdir/fixwrites   \
			| sed -e 's/putc(/Fputc(/g'  \
			| ../web2cdir/splitup
		mv -f coerce.h xcoerce.h
		sed -e 's/(buftype)//g' -e 's/(pdstype)//g' < xcoerce.h > coerce.h
		$(RM) xcoerce.h stack.stack bibtex.pool
		mv -f tex0.c bibtex0.c
		mv -f tex1.c bibtex1.c
		mv -f tex2.c bibtex2.c
		mv -f tex3.c bibtex3.c
		mv -f tex4.c bibtex4.c
		$(RM) itex.c

bibtex.p:	bibtex.web bibtex.ch
		$(TANGLE) bibtex.web bibtex.ch

clean:
		$(RM) core bibtex*.c bibtex*.o bibext.o bibtex.p \
			 coerce.h texd.h *.log *.blg *.aux *.uu *Z
		cd sample; make clean
		cd bst; make clean
		
test:
		cd sample; make test

install:
		install -m 0755 -c jbibtex $(BINDIR)
		install -m 444 -c bst/*.bst $(BSTDIR)

kit:		clean bibkit bstkit dockit srckit 

srckit:		$(SRCKIT)
		shar $(SRCKIT)\
		| compress\
		| uuencode SRC.Z >SRC.Z.uu

dockit:		dockit-ASCII dockit-NTT doc
		shar doc doc/* \
		| compress\
		| uuencode DOC.Z >DOC.Z.uu

dockit-ASCII:	doc-ASCII 
		shar doc-ASCII doc-ASCII/* \
		| compress\
		| uuencode DOC-ASCII.Z >DOC-ASCII.Z.uu

dockit-NTT:	doc-NTT
		shar doc-NTT doc-NTT/* \
		| compress\
		| uuencode DOC-NTT.Z >DOC-NTT.Z.uu

bstkit:		sty $(BSTKIT)
		shar sty sty/* bst bst/* \
		| compress\
		| uuencode BST.Z >BST.Z.uu

bibkit:		bib sample
		shar bib bib/* sample sample/*\
		| compress\
		| uuencode BIB.Z >BIB.Z.uu
