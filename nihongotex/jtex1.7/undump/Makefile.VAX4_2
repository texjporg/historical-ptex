BINDIR=/usr/local

undump: undump.o scanargs.o
	cc -O -o undump undump.o scanargs.o

undump.o: undump.c
	cc -O -c undump.c

clean:
	rm -f *.o

install: undump
	cp undump $(BINDIR)
