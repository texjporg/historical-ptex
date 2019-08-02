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

/* encode the jis code  to dnf font index */
/* from a module of dvi2nwp as c2i.c */
jis_to_dnf_index(jis)
register unsigned int jis;
{
	register int	kanji;

	kanji = jis - 0x2121;
	return ( ((kanji >> 8) & 0x7f ) * 94 + ( kanji & 0x7f ) );
}

