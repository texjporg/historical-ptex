XSRCDIR       = /usr/src/X/X.V11R2

XFONTSRCDIR   = $(XSRCDIR)/fonts/bdf

FC            = $(XSRCDIR)/fonts/compiler/bdftosnf
#FC            = $(XSRCDIR)/fonts/compiler/fc
#FC            = cat

XLIBDIR       = /usr/lib/X11

XFONTLIBDIR   = $(XLIBDIR)/fonts
#XFONTLIBDIR   = ./foo

BDFRESIZE     = ./bdfresize/bdfresize
#BDFRESIZE     = echo

K14X          = 5 6 7 8 9 10 12 15 16 18 #14

XDVIX         = 20 27 30 40 60 80


all:		$(BDFRESIZE) k14.bdf xdvi14.bdf 
		for i in $(K14X); \
		do \
			if [ ! -f k14.$$i.snf ]; \
			then \
				$(BDFRESIZE) -f $$i/14 k14.bdf | $(FC) > k14.$$i.snf; \
			fi; \
		done
		for i in $(XDVIX); \
		do \
			if [ ! -f xdvi$$i.snf ]; \
			then \
				$(BDFRESIZE) -f $$i/14 xdvi14.bdf | $(FC) > xdvi$$i.snf; \
			fi; \
		done

k14.bdf:
		ln -s $(XFONTSRCDIR)/k14.bdf k14.bdf

xdvi14.bdf:	k14.bdf
		sed "/^STARTCHAR.*3021/,\$$d" k14.bdf > tmp.bdf
		echo 'STARTCHAR 0000' >> tmp.bdf
		echo 'ENCODING 0000' >> tmp.bdf
		echo 'SWIDTH 84 0' >> tmp.bdf
		echo 'DWIDTH 14 0' >> tmp.bdf
		echo 'BBX 14 14 0 -2' >> tmp.bdf
		echo 'BITMAP' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'FFFC' >> tmp.bdf
		echo 'ENDCHAR' >> tmp.bdf
		echo 'ENDFONT' >> tmp.bdf
		sed -e "s/^CHARS.*\$$/CHARS `egrep \^BITMAP tmp.bdf | wc -l`/" \
		    -e "s/^DEFAULT_CHAR.*\$$/DEFAULT_CHAR 0000/" \
						tmp.bdf > xdvi14.bdf
		rm -f tmp.bdf

$(BDFRESIZE):
		(cd bdfresize; make)

install:	all
		for i in $(K14X); \
		do \
			install -m 644 k14.$$i.snf $(XFONTLIBDIR)/; \
		done
		for i in $(XDVIX); \
		do \
			install -m 644 xdvi$$i.snf $(XFONTLIBDIR)/; \
		done

clean:
		rm -f k14.bdf xdvi14.bdf *.snf
