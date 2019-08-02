/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

#include <stdio.h>


/* get signed integer from fp */

get_int(fp, n)
register FILE	*fp;
register n;
{
	register i;

	if((i=getc(fp)) >= 128)
		i -= 256;
	while(--n > 0)
		i = (i<<8) + getc(fp);
	return i;
}


/* get unsigned integer from fp */

get_uint(fp, n)
register FILE	*fp;
register n;
{
	register i;

	for(i = 0; --n >= 0; )
		i = (i<<8) + getc(fp);
	return i;
}


/* get string of length n from fp */

char *get_str(fp, n)
register FILE	*fp;
register n;
{
	register char	*p;
	char	*malloc();

	if((p = malloc(n+1)) == NULL)
		error("can't allocate memory for string.\n");
	fread(p, 1, n, fp);
	p[n] = '\0';
	return p;
}
