/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

#include <stdio.h>
#include "../common/pkcom.h"
#include "../common/def.h"
#define  min(x,y)	((x)<=(y) ? (x) : (y))


FONT	font[];
int	width, height, dyn_f;
int	b_width, black;
float	ds;
char	*pixel;
char	pat[8] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01};
FILE	*pk;

load_pk(fn)
{
	register FONT *t;
	register cmd, i, c;

	t = &font[fn];
	if((pk = fopen(t->a, "r")) == NULL)
		return(-1);
/*
		error("can't open pk file %s\n", t->a);
*/
	i = 0;
	do {
		if(fseek(pk, --i, 2) == -1)
			error("can't seek to the end of pk file %s\n", t->a);
	} while((c=get_uint(pk, 1)) == PK_NO_OP);
	fseek(pk, 0, 0);
	if(c != PK_POST || get_uint(pk,1) != PK_PRE || get_uint(pk,1) != PK_ID)
		error("bad pk file %s\n", t->a);
	fseek(pk, get_uint(pk, 1), 1);		/* comment */
	get_int(pk, 4);				/* design size */
	if((c = get_int(pk, 4)) && t->c && c != t->c)
		error("pk file %s version error\n", t->a);
	fseek(pk, 8, 1);			/* hppp,vppp */
	if((t->ch = (FONT_CH *)malloc(sizeof(FONT_CH [NCHAR]))) == NULL)
		error("can't allocate memory for font table.\n");
	ds = (float)t->d * t->fmag / (float)(1<<20);
	
	for( ; ; ) {
		switch(cmd = get_uint(pk, 1)) {

		case PK_XXX1: case PK_XXX2: case PK_XXX3: case PK_XXX4:
			fseek(pk, get_uint(pk, cmd-PK_XXX1+1), 1);
			continue;

		case PK_YYY:
			get_int(pk,4);
			continue;

		case PK_NO_OP:
			continue;

		case PK_POST:
			break;

		default:
			if(cmd <= PK_FLAG)
				def_char(cmd, t);
			else
				error("illegal pk command %d\n", cmd);
			continue;
		}
		break;
	}
	fclose(pk);
	return(0);
}

/*	character shape definition	*/

def_char(n, t)
register FONT *t;
{
	register FONT_CH *p;

	dyn_f = n/16;
	black = (n&8) >> 3;
	if((n &= 7) == 7) {		/* long form */
		/*pl =*/ get_int(pk, 4);
		p = &t->ch[get_int(pk, 4)];
		p->tfmw = fix((float)get_int(pk, 4) * ds);
		get_int(pk, 4); 	/* dx */
		n = 4;
	} else if(n >= 4) {		/* extended short form */
		/*pl = (n-4)*65536 +*/ get_uint(pk, 2);
		p = &t->ch[get_uint(pk, 1)];
		p->tfmw = fix((float)get_uint(pk, 3) * ds);
		n = 2;
	} else {			/* short form */
		/*pl = n*256 +*/ get_uint(pk, 1);
		p = &t->ch[get_uint(pk, 1)];
		p->tfmw = fix((float)get_uint(pk, 3) * ds);
		n = 1;
	}
	get_uint(pk, n); 		/* dy/dm */
	width = get_uint(pk, n);
	b_width = p->width = (width+7)/8;
	height = p->height = get_uint(pk, n);
	p->xoff = get_int(pk, n);
	p->yoff = get_int(pk, n);
	pixel = p->pxl = (char *)calloc(height, b_width);
	if(pixel == NULL)
		error("can't allocate memory for pixel\n");
	if(dyn_f == 14)
		unpack_raster();
	else
		unpack_run();
}

/*	get nybble	*/

#define get_nyb()  ((nyb_f^=1)? (nyb_w=get_uint(pk,1))/16: nyb_w&15)

/*	get run_count/repeat_count	*/

#define get_run()\
	for( ; ; ) {\
		if((i=get_nyb()) >= 14) {\
			rep = i*2-29;\
			continue;\
		} else if(i > dyn_f)\
			run = (i-dyn_f-1)*16 + get_nyb() + dyn_f + 1;\
		else if(i > 0)\
			run = i;\
		else /* i==0 */ {\
			do	++i;\
			while((run = get_nyb()) == 0);\
			while(--i >= 0)\
				run = run*16 + get_nyb();\
			run += (13-dyn_f)*16 + dyn_f - 15;\
		}\
		if(rep >= 0)\
			break;\
		rep = run;\
	}\

/*	unpack run_encoded packet	*/

unpack_run()
{
	register char *p, *q;
	register i, run, rep, h, h1, v;
	int	nyb_f, nyb_w;

	nyb_f = 0;
	p = pixel;
	black ^= 1;
	run = 0;
	for(v=0; v < height; v += rep+1) {
		rep = 0;
		for(h=0; h < width; h=h1) {
			if(run == 0) {
				get_run();
				black ^= 1;
			}
			h1 = min(h+run, width);
			if(black) {
				h1 = min(h1, (h+8)&~7);
				p[h/8] |= pat[h&7];
				if(h1 & 7)
					p[h1/8] &= ~pat[h1&7];
			}
			run -= h1-h;
		}
		q = p;	p += b_width;
		for(h=rep*b_width; --h >= 0; )
			*p++ = *q++;
	}
}

/*	unpack raster packet	*/

unpack_raster()
{
	register char *p;
	register n, m, t, h, v;

	p = pixel;
	n = 0;
	for(v=0; v < height; ++v) {
		for(h=0; h < width; h+=m) {
			if(n == 0) {
				t = get_uint(pk, 1);
				n = 8;
			}
			m = min(n, 8-(h&7));
			m = min(m, width-h);
			p[h/8] |= t >> (h&7);
			t <<= m; n -= m;
		}
		if(width & 7)
			p[width/8] &= ~pat[width&7];
		p += b_width;
	}
}
