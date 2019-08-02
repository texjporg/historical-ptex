/*
 * External procedures for pltotf.c etc.
 *
 * Tim Morgan, 3/22/88
 * Hamano Hisato, 9/??/90
 */

#include <stdio.h>
#include "site.h"
#ifdef	SYSV
#include <string.h>
extern int sprintf();
#else
#include <strings.h>
extern char *sprintf();
#endif

#define	TRUE	1
#define	FALSE	0

integer argc;

static char **gargv;
argv(n, buf)
int n;
char buf[];
{
    (void) strcpy(buf+1, gargv[n]);
    (void) strcat(buf+1, " ");
}

main(ac, av)
char *av[];
{
    argc = ac;
    gargv = av;
    main_body();
}

/* Same routine as index() aka strchr() */
#ifndef	SYSV
char *char_index(cp, c)
char *cp, c;
{
    while (*cp != c && *cp) ++cp;
    if (*cp) return(cp);
    return(0);
}
#else	/* not SYSV */
#define	char_index	strchr
#endif

/* Open a file; don't return if error */
FILE *openf(name, mode)
char *name, *mode;
{
    FILE *result;
    char *cp;

    cp = char_index(name, ' ');
    if (cp) *cp = '\0';
    result = fopen(name, mode);
    if (result) return(result);
    perror(name);
    exit(1);
    /*NOTREACHED*/
}

/* Print real number r in format n:m */
printreal(r, n, m)
double r;
int n,m;
{
    char fmt[50];

    (void) sprintf(fmt, "%%%d.%df", n, m);
    (void) printf(fmt, r);
}

/* Return true on end of line or eof of file, else false */
eoln(f)
FILE *f;
{
    register int c;

    if (feof(f)) return(1);
    c = getc(f);
    if (c != EOF)
	(void) ungetc(c, f);
    return (c == '\n' || c == EOF);
}

integer zround(f)
double f;
{
    if (f >= 0.0) return(f + 0.5);
    return(f - 0.5);
}

/* Read an integer in from file f; read past the subsequent end of line */
integer inputint(f)
FILE *f;
{
    char buffer[20];

    if (fgets(buffer, sizeof(buffer), f)) return(atoi(buffer));
    return(0);
}

zinput_3ints(a,b,c)
integer *a, *b, *c;
{
    while (scanf("%ld %ld %ld\n", a, b, c) != 3)
	(void) fprintf(stderr, "Please enter three integers\n");
}

char
xchr(c)
char	c;
{
	return(c);
}

char
kanjiord(c)
char	c;
{
	return(c);
}

#ifdef SJIS
mstojis(kcode)
{
	register short	byte1, byte2;

	byte1 = (kcode >> 8) & 0xff;
	byte2 = kcode & 0xff;
	byte1 -= (byte1 >= 0xa0) ? 0xc1 : 0x81;
	kcode = ((byte1 << 1) + 0x21) << 8;
	if( byte2 >= 0x9f ) {
		kcode += 0x0100;
		kcode += byte2 - 0x7e;
	} else {
		kcode += byte2 - ( (byte2<=0x7e) ? 0x1f : 0x20 );
	}
	return(kcode);
}
#endif

#ifdef EUC
isfbkanji(c)
register int	c;
{
	c &= 0377;
	return (c >= 0xa1 && c <= 0xfe);
}
#else
isfbkanji(c)
register int	c;
{
	c &= 0377;
	return ((c > 0x80 && c < 0xa0)||(c > 0xdf && c < 0xfd));
}
#endif
