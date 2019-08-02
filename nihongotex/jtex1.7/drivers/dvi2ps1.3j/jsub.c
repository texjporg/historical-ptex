/*
 * utilities for jsubfont
 */

#include	<stdio.h>
#include	"defs.h"
#include	"global.h"
#include	"jsub.h"

char *jsf_names[] = {
    "",
    "jsy",
    "jroma",
    "jhira",
    "jkata",
    "jgreek",
    "jrussian",
    "jkeisen",
    "jka",
    "jkb",
    "jkc",
    "jkd",
    "jke",
    "jkf",
    "jkg",
    "jkh",
    "jki",
    "jkj",
    "jkk",
    "jkl",
    "jkm",
    "jkn",
    "jko",
    "jkp",
    "jkq",
    "jkr",
    "jks",
    "jkt",
    "jku",
    "jkv",
    "jkw",
    "jkx",
    "jky",
    "jkz",
    NULL
};

char *
jsubfontname(f)
{
    return (jsf_names[f]);
}

getjsubfont(n)
register char *n;
{
    register int f;

    for (f = 1; f < NJSUBS; f++)
	if (!strncmp(n, jsf_names[f], strlen(jsf_names[f])))
	    return (f);
    return (0);
}

#define	kushift(c)	c+0x20
#define	tenshift(c)	c+0x20

compute_jis(f, c, ku, ten)
register short f, c;
unsigned short *ku, *ten;
{
    register int n;

    if (f <= 7) {
	if (f == 1) {
	    if (c >= 100) {
		*ku = kushift(2);
		*ten = tenshift(c-100);
	    } else {
		*ku = kushift(1);
		*ten = tenshift(c);
	    }
	} else if (f == 2) {
	    *ku = kushift(3);
	    *ten = tenshift(c-32);
	} else {
	    *ku = kushift(f+1);
	    *ten = tenshift(c);
	}
    } else if (f <= 19) {	/* Daiichi Suijun */
	n = (f-8)*256+c;
	*ku = kushift((n/94)+16);
	*ten = tenshift((n%94)+1);
    } else {			/* Daini Suijun */
	n = (f-20)*256+c;
	*ku = kushift((n/94)+48);
	*ten = tenshift((n%94)+1);
    }
}

compute_fc(ku, ten, f, c)
register short ku, ten;
unsigned short *f, *c;
{
    register int n;

    ku -= 0x20;
    ten -= 0x20;
    *f = 1;
    *c = 1;
    if (ku <= 0 || (9 <= ku && ku <= 15) || ku > 84) {
	Warning("invalid ku in jis (%x, %x)", ku+0x20, ten+0x20);
	return;
    }
    if (ten < 1 || ten > 94) {
	Warning("invalid ten in jis (%x, %x)", ku+0x20, ten+0x20);
	return;
    }
    if (ku <= 8) {
	if (ku == 1) {
	    *f = 1;
	    *c = ten;
	} else if (ku == 2) {
	    *f = 1;
	    *c = ten+100;
	} else if (ku == 3) {
	    *f = 2;
	    *c = ten+32;
	} else {
	    *f = ku-1;
	    *c = ten;
	}
    } else if (ku <= 47) {	/* Daiich Suijun */
	n = (ku-16)*94+ten-1;
	*f = (n/256)+8;
	*c = n%256;
    } else {			/* Daini Suijun */
	n = (ku-48)*94+ten-1;
	*f = (n/256)+20;
	*c = n%256;
    }
}
