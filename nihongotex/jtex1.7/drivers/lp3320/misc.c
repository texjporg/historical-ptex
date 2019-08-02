#include	<stdio.h>
#include	"dvi2ricoh.h"

int
calcconvfact(num, den, mag, res)
	int	num;
	int	den;
	int	mag;
	int	res;
{
	double	val;

	val = ((double)num / (double)den) *
	      ((double)mag / 1000.0) *
	      ((double)res / 254000.0);
	return (int)(1.0 / val + 0.5);
}

int
calcpixel(tfmval, fact)
	int	tfmval;
	int	fact;
{
	/*
	 * round(tfmval / fact)
	 */
	return (tfmval + (fact / 2)) / fact;
}

int
calctfmwidth(pxlval, fact)
	int	pxlval;
	int	fact;
{
	return pxlval * fact;
}

double
actualfactor(size)
	int	size;
{
	switch (size) {
	case 1095:	/*stephalf*/
		return 1.095445;
	case 1315:	/*stepihalf*/
		return 1.314534;
	case 1577:	/*stepiihalf*/
		return 1.577441;
	case 1893:	/*stepiiihalf*/
		return 1.892929;
	case 2074:	/*stepiv*/
		return 2.0736;
	case 2488:	/*stepv*/
		return 2.48832;
	case 2986:	/*stepiv*/
		return 2.985984;
	default:
		return size / 1000.0;
	}
}
	
fatal(str, arg1, arg2, arg3, arg4, arg5, arg6)
	char	*str;
	char	*arg1;
	char	*arg2;
	char	*arg3;
	char	*arg4;
	char	*arg5;
	char	*arg6;
{
	fprintf(stderr, "%s: ", progname);
	fprintf(stderr, str, arg1, arg2, arg3, arg4, arg5, arg6);
	putc('\n', stderr);
	doexit(2);
}

warning(str, arg1, arg2, arg3, arg4, arg5, arg6)
	char	*str;
	char	*arg1;
	char	*arg2;
	char	*arg3;
	char	*arg4;
	char	*arg5;
	char	*arg6;
{
	if (!warnflag)
		return;
	fprintf(stderr, "%s: ", progname);
	fprintf(stderr, str, arg1, arg2, arg3, arg4, arg5, arg6);
	putc('\n', stderr);
}

int
getsint(fp, size)
	register FILE	*fp;
	register int	size;
{
	register int	val, ch, first;

	first = TRUE;
	while (--size >= 0) {
		if ((ch = getc(fp)) == EOF)
			fatal("unexpected EOF");
		if (first) {
			if (ch & 0x80)
				val = 0xffffff00 | ch;
			else
				val = ch;
			first = FALSE;
		} else
			val = (val << 8) | ch;
	}
	return val;
}

int
getuint(fp, size)
	register FILE	*fp;
	register int	size;
{
	register int	val, ch;

	val = 0;
	while (--size >= 0) {
		if ((ch = getc(fp)) == EOF)
			fatal("unexpected EOF");
		val = (val << 8) | ch;
	}
	return val;
}

skipspecial(fp, len)
	register FILE	*fp;
	register int	len;
{
	/*
	 * special
	 * just now, treat them as nop's
	 */
	fseek(fp, len, 1);
}
