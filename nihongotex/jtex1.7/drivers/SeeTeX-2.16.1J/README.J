$B$3$N%G%#%l%/%H%j$O!"(BSeeTeX-2.16.1 $B$r(B ASCII $BF|K\8l(BTeX $B$*$h$S(B NTT JTeX
$B$KBP1~$9$k$h$&$K2~NI$7$?(B SeeTeX-1.16.1J $B$r4^$_$^$9!#$?$@$7!"(BXtex $B0J30$O(B
$BBP1~$7$F$$$^$;$s!#F|K\8lItJ,$K4X$9$k%P%0%l%]!<%H!"2~NI!"%3%a%s%H$J$I$O(B
	kagotani@cs.titech.ac.jp
$B$KAw$C$F2<$5$$!#(B


$B%$%s%9%H!<%k(B

Imake.Config $B$N(B FontDesc $B$H(B CCOPTIONS $B$rE,@Z$K;XDj$7$^$9!#(B
 o ASCIIJTEX :	ASCII $BF|K\8l(BTeX $B$KBP1~$5$;$k(B
 o FAKEKANJI :	ASCII $BF|K\8l(BTeX $B$G!"(BTeX $BMQ$NF|K\8l%U%)%s%H$r$b$?$:!"(B
		$B%W%j%s%?Fb$N%U%)%s%H$J$I$rMxMQ$7$F$$$k$3$H$r;X<($9$k!#(B
		$B$3$N;XDj$r$7$J$$>l9g$NF0:n$O!"A4$/%F%9%H$5$l$F$$$^$;$s!#(B
		$B$*$=$i$/!"I,MW$J%U%)%s%H$r:n$k$3$H$,$G$-$^$;$s(B
 o NTTJTEX :	NTT JTeX $B$KBP1~$5$;$k(B
 o LAZYFREE :	$B%U%)%s%H$N%m!<%I2s?t$r8:$i$9$?$a!"$9$0$K%U%)%s%H$r(B
		free $B$7$J$$(B
$B$"$H$O!"(B
  % xmkmf; make Makefiles; make
$B$K$h$C$F!"I,MW$J%P%$%J%j$O:n$i$l$^$9!#(Broot $B$K$J$C$F!"(B
  # make install
$B$7$F2<$5$$!#(B
$B%U%)%s%H$N%$%s%9%H!<%k$OB?>/Fq$7$$$N$G!"(BDoc $B$N2<$K$"$k%I%-%e%a%s%H$r(B
$BFI$^$l$k$3$H$r$*4+$a$7$^$9!#(B

$BFC$K(B ASCII $BF|K\8l(BTeX $B$G(B FAKEKANJI $B$r;XDj$7$F%U%)%s%H$r;H$&;~$O!"(Bk14
$B$r$5$^$6$^$J%5%$%:$KJQ99$9$k$J$I$7$FBP1~$7$^$9!#Nc$($P!"(B75dpi $B$N%G%#(B
$B%9%W%l%$MQ$K%U%)%s%H$r:n$k;~$O!"(B
	5, 6, 7, 10, 12, 15, 20, 25
$B%I%C%H$N%U%)%s%H$rMQ0U$7(B fonts.alias $B$K!"(B
> min5.75            k14.5
> min6.75            k14.6
> min7.75            k14.7
> min8.75            k14.7
> min9.75            k14.10
> min10.75           k14.10
> min10.82           k14.10
> min10.90           k14.12
> min10.108          k14.15
> min10.130          k14.15
> min10.156          k14.20
> min10.187          k14.25
$B$H$$$&9T$rDI2C$7$^$9!#(Bgoth $B$K4X$7$F$bF1MM$G$9!#$3$l$K$h$C$F!"(BLaserShot
$B$K0u:~$9$k$N$H$[$\F1$8=L<\$NF|K\8l%U%)%s%H$G(B preview $B$9$k$3$H$,$G$-$^$9!#(B
$B$=$NB>$N>l9g$OE,597W;;$7$F:n@.$7$F2<$5$$!#(B
k14.bdf $B$N%5%$%:$NJQ99$O(B bdfresize $B$r;H$&$N$,JXMx$G$7$g$&!#(B

							1990$BG/(B 8$B7n(B 2$BF|(B
							$BdFC+M5?M(B
