/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

/*			dvi2lbp
 *		(back end of TeX print)
 */

#include <stdio.h>
#include "../common/dvicom.h"
#include "../common/def.h"

float	mag;			/* see "TeX: The Program" Chap. 585 */
int	h, v, w, x, y, z, f;	/* unit = scaled point (2^-16pt) */
int	dh, dv;			/* unit = dot */
int	stack[MAX_STACK*6];

FONT	font[MAX_FONT];
int	t_margin = {DPI*3/5}, l_margin = {DPI*5/6}, copy = {1};
int	dflag = {-1}, dfn, hflag = {0};
FILE	*dvi = { stdin };
char	*myname;


main(argc, argv)
int	argc;
char	*argv[];
{
	register i, a, b, c;
	char	*get_str();
	float	ratio();

	myname = argv[0];

	/* dvi file format check */

	if(get_uint(dvi, 1) != PRE || get_uint(dvi, 1) != DVI_ID)
		error("bad dvi file.\n");
	i = 0;
	do {
		if(fseek(dvi, --i, 2) == -1)	/* may be pipe */
			error("can't seek to the end of dvi file.\n");
	} while ((c=get_uint(dvi, 1)) == DVI_PAD);
	fseek(dvi, i-5, 2);
	if(get_uint(dvi, 1) != POST_POST || c != DVI_ID)
		error("bad dvi file.\n");

	/* postamble */

	fseek(dvi, get_int(dvi, 4)+13, 0);
	mag = ratio(get_int(dvi, 4), 1000);
	i = get_uint(dvi,1);
	if((i & 0xfe) == 0x80) {		/* texpr option */
		hflag = i & 1;
		copy = get_uint(dvi, 1);
		dflag = get_int(dvi, 1);
		dfn = get_uint(dvi, 1);
		t_margin += (get_int(dvi, 2)*DPI+50)/100;
		l_margin += (get_int(dvi, 2)*DPI+50)/100;
	} else
		fseek(dvi, 7, 1);
	if(get_uint(dvi, 2) > MAX_STACK)
		error("stack overflow.\n");
	get_uint(dvi, 2);

	/* font definition */

	while((c=get_uint(dvi, 1)) >= FNT_DEF1 && c <= FNT_DEF4) {
		i = get_uint(dvi, c-FNT_DEF1+1);
		font[i].c = get_int(dvi, 4);	/* check sum */
		a = get_int(dvi, 4);		/* at size */
		b = get_int(dvi, 4);		/* design size */
		font[i].d = b;
		font[i].fmag = ratio(a, b);	/* font mag */
		a = get_uint(dvi, 1);
		b = get_uint(dvi, 1);
		font[i].a = get_str(dvi, a);	/* area (directory) name */
		font[i].n = get_str(dvi, b);	/* font name */
		font[i].down = 0;
		real_font(i, 1);		/* make real font name */
		if(load_pk(i)){
			real_font(i, 0);	/* make real font name */
			load_pixel(i);
		}
	}

	init_lbp();

	fseek(dvi, 14, 0);			/* pre */
	fseek(dvi, get_uint(dvi, 1), 1);

	put_pages();

	exit(0);				/* daemon checks return code */
}


error(s, a1, a2, a3, a4)
char	*s;
{
	fprintf(stderr, "%s: ", myname);
	fprintf(stderr, s, a1, a2, a3, a4);
	exit(2);	/* throw away this job */
}
