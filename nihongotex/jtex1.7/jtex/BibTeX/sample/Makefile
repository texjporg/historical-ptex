RM = /bin/rm -f
JLATEX = /usr/local/bin/jlatex

all:	test

test:
	cp ../bib/jxampl.bib .
	cp ../bst/jplain.bst .
	$(JLATEX) test
	-../jbibtex test
	$(JLATEX) test
	$(JLATEX) test
	diff test.bbl jplain.sav
	@echo \*\*\*  TESTING SUCCESS  \*\*\*

clean:
	$(RM) *.dvi *.aux *.bbl *.blg *.log *.bib *.bst
