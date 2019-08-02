# DEFAULT_KANJI_CODE = JIS/SJIS/EUC
CFLAGS =  -O -DDEFAULT_KANJI_CODE=JIS
# CC = gcc

all:	dvi2tty disdvi

dvi2tty:dvi2tty.o dvistuff.o dvikanji.o
	cc -s -o dvi2tty dvi2tty.o dvistuff.o dvikanji.o

disdvi:disdvi.o
	cc -s -o disdvi disdvi.o

clean:
	rm *.o disdvi dvi2tty
