===========================================================================
$BF|K\8l(B TeX $B%P!<%8%g%s(B 3.14159, p2.1.10
===========================================================================
$B"((B $BI,MW$J$i$P!"%$%s%9%H!<%k:n6H$r$9$kA0$K!"(B
   $B5l%P!<%8%g%s$N(B TeX $B%7%9%F%`$N%P%C%/%"%C%W$r$7$F$/$@$5$$!#(B

---------------------------------------------------------------------------
$B!|(B $B%=!<%9%U%!%$%k(B
---------------------------------------------------------------------------
pTeX $B$N%$%s%9%H!<%k$K$O!"$D$.$N%U%!%$%k$,I,MW$G$9!#(B

 * ptex-src-2.1.10.tar.gz     (pTeX $B%=!<%9%U%!%$%k(B)
 * ptex-texmf-1.8.tar.gz      (pTeX $B%i%$%V%i%j%U%!%$%k(B)
 * teTeX-src-1.0.7.tar.gz     (teTeX $B%=!<%9%U%!%$%k(B)
 * teTeX-texmf-1.0.2.tar.gz   (teTeX $B%i%$%V%i%j%U%!%$%k(B)

$B$3$l$i$O!"0J2<$N%5%$%H$J$I$+$iF~<j2DG=$G$9!#(B

$B!&(BpTeX $B4XO"(B
    ftp://ftp.ascii.co.jp/pub/TeX/ascii-ptex/
    ftp://ftp.media.kyoto-u.ac.jp/TeX/ASCII-pTeX/
    ftp://bash.cc.keio.ac.jp/pub/TeX/ascii-ptex/
    ftp://ftp.cs.titech.ac.jp/pub/text/TeX/ASCII-pTeX/
    ftp://ftp.tut.ac.jp/TeX/ASCII-pTeX/
    ftp://ftp.u-aizu.ac.jp/pub/tex/ASCII-pTeX/
    ftp://ftp.foretune.co.jp/pub/TeX/ascii-ptex/

$B!&(BteTeX $B4XO"(B
    ftp://ftp.lab.kdd.co.jp/CTAN/
    ftp://ftp.meisei-u.ac.jp/pub/CTAN/
    ftp://ftp.riken.go.jp/pub/tex-archive/
    ftp://ftp.u-aizu.ac.jp/pub/tex/CTAN/
    ftp://sunsite.sut.ac.jp/pub/archives/ctan/
    ftp://ftp.dante.de/tex-archive/
    ftp://ftp.tex.ac.uk/tex-archive/
    ftp://ctan.tug.org/tex-archive/

$B$J$*!"(BteTeX-src $B$N$+$o$j$K!"(B

 * ftp://ftp.cstug.cz/pub/tex/local/cstug/thanh/pdftex/tetex-supp.tgz

$B$rMQ$$$k$H!"(Bsrc-special $B5!G=$rMxMQ$G$-$^$9!#(B

---------------------------------------------------------------------------
$B!|(B $B%$%s%9%H!<%k(B
---------------------------------------------------------------------------
1. teTeX $B$N%i%$%V%i%j%U%!%$%k$rE83+$7$^$9!#(B

     mkdir -p /usr/local/teTeX/share/texmf
     tar zxf teTeX-texmf-1.0.2.tar.gz -C /usr/local/teTeX/share/texmf

2. ptex $B$N%i%$%V%i%j%U%!%$%k$rE83+$7$^$9!#(B

     tar zxf ptex-texmf-1.8.tar.gz -C /usr/local/teTeX/share/texmf

3. teTeX $B$N%=!<%9%U%!%$%k$rE83+$7$^$9!#(B

     tar zxf teTeX-src-1.0.7.tar.gz -C /usr/src

4. ptex $B$N%=!<%9%U%!%$%k$rE83+$7$^$9!#(B

     tar zxvf ptex-src-2.1.9-1.tar.gz -C /usr/src/teTeX-1.0/texk/web2c

5. $B%=!<%9%U%!%$%k$rE83+$7$?%G%#%l%/%H%j$K0\F0$7!"(Bconfigure $B%3%^%s%I$r(B
   $B<B9T$7$^$9!#(Bprefix $B%*%W%7%g%s$K$O!"(BteTeX $B%i%$%V%i%j$r%$%s%9%H!<%k(B
   $B$7$?%G%#%l%/%H%j$r;XDj$7$^$9!#(B

     cd /usr/src/teTeX-1.0
     ./configure --prefix=/usr/local/teTeX
	 make
	 make install

6. ptex $B$N%=!<%9%G%#%l%/%H%j$K0\F0$7!"(Bconfigure $B%3%^%s%I$r<B9T$7$^$9!#(B
   $B$3$N$H$-!"C<Kv$K=PNO$9$k4A;z%3!<%I$r;XDj$7$F$/$@$5$$!#(B

     cd /usr/src/teTeX-1.0/texk/web2c/ptex-2.1.10
     ./configure EUC     (EUC$B%3!<%I$N>l9g(B)
     ./configure JIS     (JIS$B%3!<%I$N>l9g(B)
     ./configure SJIS    ($B%7%U%H(BJIS$B%3!<%I$N>l9g(B)
	 make
	 make install

---------------------------------------------------------------------------
$B<+F0@8@.$5$l$k(B ascgrp, ascii10, ascii36 $B$N(B pk $B%U%!%$%k$r(B
$TEXMF/fonts/pk/<$B%b!<%I(B>/ascgrp $B$N2<$KCV$/$h$&$K$9$k>l9g$O!"(B
$TEXMF/fontname/special.map $B$K$D$.$NFs9T$r2C$($^$9!#(B

    ascgrp ascgrp
    ascii  ascgrp

---------------------------------------------------------------------------
$B!|(B $BLd$$9g$o$;@h$J$I(B
---------------------------------------------------------------------------
pTeX $B$K$D$$$F$N$*Ld$$9g$o$;$O!"EE;R%a!<%k$G(B www-ptex@ascii.co.jp $B08$F(B
$B$K$*4j$$$7$^$9!#(B

pTeX $B$N(B WEB $B%Z!<%8!J(Bhttp://www.ascii.co.jp/pb/ptex/$B!K$G$O!"%P%0>pJs$d(B
$B%$%s%9%H!<%k$N%R%s%H!"(BpTeX $B$GDI2C$5$l$?%W%j%_%F%#%V0lMw$J$I$r7G:\$7$F(B
$B$$$^$9!#$3$A$i$b$4Mw$/$@$5$$!#(B

-------------------------------------
$BCfLn(B $B8-(B (ken-na@ascii.co.jp)
$B3t<02q<R(B $B%"%9%-!<(B $B=PHG5;=QIt(B
=============================================================== EOT =====
