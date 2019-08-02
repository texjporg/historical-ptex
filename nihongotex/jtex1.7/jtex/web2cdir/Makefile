#################################################
# Makefile for TeX to C conversion programs	#
#################################################

SITEDIR=..
OPT=-O
CFLAGS=$(OPT) -I$(SITEDIR)
LDFLAGS=
#LDFLAGS=-s
RM=/bin/rm
# You can substitute "bison -y" for "yacc" if you like
YACC=yacc
# Note: If you're going to use flex, compile web2cl.c with "-DFLEX"
LEX=lex

all:		web2c fixwrites splitup regfix

web2c:		web2cy.o web2cl.o web2c.o
		$(CC) $(LDFLAGS) -o web2c web2c.o web2cy.o web2cl.o -ll

web2cl.o:	web2cl.c web2c.h web2cy.h symtab.h $(SITEDIR)/site.h
web2cy.o:	web2cy.c web2c.h symtab.h $(SITEDIR)/site.h
web2c.o:	web2c.c symtab.h web2cy.h $(SITEDIR)/site.h

web2cl.c:	web2c.lex
		$(LEX) web2c.lex
		mv -f lex.yy.c web2cl.c

web2cy.c:	web2c.yacc
		$(YACC) -d web2c.yacc
		mv -f y.tab.c web2cy.c
		-@cmp y.tab.h web2cy.h || cp y.tab.h web2cy.h

fixwrites:	fixwrites.c
		$(CC) $(CFLAGS) $(LDFLAGS) -o fixwrites fixwrites.c

splitup:	splitup.c $(SITEDIR)/site.h
		$(CC) $(CFLAGS) $(LDFLAGS) -o splitup splitup.c

regfix:		regfix.c $(SITEDIR)/site.h
		$(CC) $(CFLAGS) $(LDFLAGS) -o regfix regfix.c

veryclean:	clean
clean:
		$(RM) -f *.o regfix splitup fixwrites web2c \
			web2cy.c web2cl.c web2cy.h
