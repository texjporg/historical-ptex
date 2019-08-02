/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

#include <stdio.h>
#include <strings.h>
#include "def.h"

float	mag;
FONT	font[];
char	pathf;
char	*


/* make real font name */

real_font(fn, pkf)
{
	register FONT *p;
	register char *q;
	char	*malloc();
	int	num;
	char	*suffix, *t;

	p = &font[fn];
	if(pkf){
		suffix = "pk";
		num = fix(DPI*p->fmag*mag);
		pathf = p->a[0];
	}else{
		suffix = "pxl";
		num = fix((DPI*5.0)*p->fmag*mag);
		t = rindex(p->n, '.');
	}
	if((q = malloc(256)) == NULL)
		error("can't allocate memory for string.\n");
	if(pathf)
		sprintf(q, "%s%s.%d%s", p->a, p->n, num, suffix);
	else
		sprintf(q, "%s/%s.%d%s", LIB, p->n, num, suffix);
	p->a = q;
/*
	p->n = rindex(q, '/')+1;
*/
}


/* compute ratio (to get accurate magnification) */

float ratio(i, j)	/* due to TeX's bad design */
{
	float	f;

	f = (float)i / (float)j;
	switch( fix(f*1000.0) ) {
	case 1095: case 1096:	f = 1.0954451;	break;
	case 1314: case 1315:	f = 1.3145341;	break;
	case 1643: case 1644:	f = 1.6431676;	break;
	case 2073: case 2074:	f = 2.0736;	break;
	case 2488: case 2489:	f = 2.48832;	break;
	case 2985: case 2986:	f = 2.985984;	break;
	case 3110: case 3111:	f = 3.1104;	break;
	case 3732: case 3733:	f = 3.73248;	break;
	case 4478: case 4479:	f = 4.478976;	break;
	default:	/* o.k. */;
	}
	return f;
}
