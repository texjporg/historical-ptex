#include	<stdio.h>

char *
alloc_check(p, e)
char *p, *e;
{
    if (p == NULL)
	Fatal("can't malloc space for %s", e);
    return (p);
}

makeuint(s, n)		/* return n byte quantity from string */
register unsigned char *s;
register int n;
{
    register int x;	/* number being constructed */

    x = 0;
    while (n--)  {
	x <<= 8;
	x |= *s++;
    }
    return (x);
}

#ifdef SYSV
#define	bcopy(s, t, n)	memcpy(t, s, n)
#endif

char *
strsave(s)
char *s;
{
    register char *t;
    register int len;
    char *malloc();

    if ((t = malloc(len = strlen(s)+1)) == NULL)
	Fatal("cannot save string %s", s);
    bcopy(s, t, len);
    return (t);
}
