#! /bin/csh -f
set xorig=%%LEFTMARGIN%%
set yorig=%%TOPMARGIN%%
set pagetype=%%PAGETYPE%%
set fdescdefault=%%DVI2PSLIB%%/fontdesc-pk
set fdescdraft=%%DVI2PSLIB%%/fontdesc-js
set headtempl=%%DVI2PSLIB%%/tex.ps-templ
set dvi2ps=%%BINAREA%%/dvi2ps
set xcoord=$xorig
set ycoord=$yorig
set nowarn=0
set tmpps=/tmp/LWdvi-$$
set tmphead=/tmp/TeXps-$$
if ($?FONTDESC) then
	set fontdesc=$FONTDESC
else
	set fontdesc=$fdescdefault
endif
top:
	if ($#argv > 0) then
		switch ($argv[1])
		case -Draft:
			set fontdesc=$fdescdraft
			shift argv
			goto top
		case -x:
			if ($#argv < 2) then
				echo -x takes following xorigin
				exit(1)
			endif
			@ xcoord=( $xorig + $argv[2] )
			shift argv
			shift argv
			goto top
		case -y:
			if ($#argv < 2) then
				echo -y takes following xorigin
				exit(1)
			endif
			@ ycoord=( $yorig + $argv[2] )
			shift argv
			shift argv
			goto top
		case -Nowarn:
			set nowarn=1
			shift argv
			goto top
		endsw
	endif

if ($#argv == 0) then
	echo no dvifile
	exit(1)
endif

sed -e s/'##XCOORD##'/$xcoord/ -e s/'##YCOORD##'/$ycoord/ \
    -e s/'##PAGETYPE##'/$pagetype/ \
	< $headtempl > $tmphead
$dvi2ps -F $fontdesc -q -h -i $tmphead $* > $tmpps
if ($status == 0 || ($status == 1 && $nowarn == 0)) lpr -J TeX $tmpps
rm -f $tmphead $tmpps
