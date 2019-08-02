/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

#include <stdio.h>
#include "../common/def.h"

FILE	*dvi;


put_int(v, n)
register v;
{
	switch(n) {
	case 4:	putchar(v >> 24);
	case 3:	putchar(v >> 16);
	case 2:	putchar(v >> 8);
	case 1: putchar(v);
	}
	return v;
}


fcopy(n)
register n;
{
	while(--n >= 0)
		putchar(getc(dvi));
}


atoi(pp)
char	**pp;
{
	register char	*p;
	register n, sign;

	p = *pp;
	sign = (*p == '-');
	if(*p == '+' || *p == '-')	++p;
	for(n=0; *p >= '0' && *p <= '9'; )
		n = n * 10 + *p++ - '0';
	if(sign) n = -n;
	*pp = p;
	return n;
}


/*	redirect stdout to lpr	*/

spool()
{
	int	pp[2];

	pipe(pp);
	if(fork() == 0) {	/* child */
		close(0);
		dup2(pp[0], 0);
		close(pp[0]);
		close(pp[1]);
		execl(LPR, 0);
		error("can't exec lpr!\n");
	}
	close(1);
	dup2(pp[1], 1);
	close(pp[0]);
	close(pp[1]);
}
