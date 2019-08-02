# Makefile for documentation

SRC	= main.tex iptex.tex texx.tex installation.tex fontdesc.tex

goal:	main.dvi

main.dvi: $(SRC)
	latex main

$(SRC):
	co -q $@

src:	$(SRC)
