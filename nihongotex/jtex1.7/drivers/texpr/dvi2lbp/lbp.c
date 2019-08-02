/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

/*	This file contains device dependent routines	*/

#include <stdio.h>
#include "../common/def.h"
#define	 n_putc(n, c)	for(x=n; --x>=0; putchar(c))
#define	SHIFTIN		017
#define	SHIFTOUT	016

float	mag;
int	f, h, v, dh, dv;
int	t_margin, l_margin, copy, dflag, dfn, hflag, last_x, last_y;
FONT	font[];
int	kmode, last_size;

/*	initialize LBP-8	*/


init_lbp()
{
	if(dflag >= 0 && font[dfn].n) {
		font[dfn].down = 1;
		if(dflag > 0 || !same_font(dfn)) {
			printf("\033c");		/* LBP hard reset */
			printf("\23311h\2337 I");	/* set unit = dot */
			printf("\233?5l");		/* LF only */
			printf("\233?2;3h");
			printf("\2331;1 G");		/* spacing */
			down_pixel(dfn);
			memo_font(dfn);
		}
	}
	printf("\23311h\2337 I");	/* set unit = dot */
	printf("\2331;1 G");		/* spacing */
	printf("\233%dp", hflag);	/* portrait/landscape */
	printf("\233%dv", copy);	/* copy no. */
	printf("\233?1;5;6l");		/* LF only */
	printf("\233?2;3h");
	printf("\033(/ 0");
}
	

flush_page()
{
	if(!kmode){
		printf("\033(,0");
		kmode = 1;
	}
	putchar('\f');			/* eject */
}


/*	down load fn	*/

down_pixel(fn)
{
	register FONT *t;
	register FONT_CH *p;
	register i, x, y;
	int	 maxw, maxh, maxd, nchar;	/* width, height, depth */
	char	 *q;

	t= &font[fn];
	maxw = maxh = maxd = nchar = 0;
	for(i=0; i < NCHAR; ++i) {
#if OFS
		if(i == '\033' || i == ' ')
			continue;
#else
		if(i <=' ' || i>'~')
			continue;
#endif
		p = &t->ch[i];
		if(p->pxl == NULL)
			continue;
		++nchar;
		if(p->width > maxw)	maxw = p->width;
		if(p->yoff > maxh)	maxh = p->yoff;
		if(p->height-p->yoff > maxd)
			maxd = p->height-p->yoff;
	}
	if(maxw > MAX_W/8 || maxh+maxd > MAX_H)
		error("can't down load! too big character.\n");
#if SPC
	i = (maxw*(maxh+maxd)+(2+OFS)) * nchar;
#else
	i = (maxw*(maxh+maxd)+(1+OFS)) * nchar;
#endif
/*
	if(i > MAX_MEM)
		error("can't down load! memory full.\n");
*/
	printf("\233%d;", i);			/* total bytes */
	printf("%d;0;0;3840;%d;", nchar, SPC);	/* no. of char, set no. */
	printf("1200;400;0;0;200;"); 		/* pitch, size, face */
	printf("%d;%d;", maxw*8, maxh+maxd);	/* width, height */
#if OFS
	printf("%d;0;0;1.p", maxd);		/* baseline */
#else
	printf("%d;1.p", maxd);		/* baseline */
#endif
	for(i=0; i < NCHAR; ++i) {
#if OFS
		if(i == '\033' || i == ' ')	/* skip CSI/sp */
			continue;
#else
		if(i <=' ' || i>'~')		/* skip non graphics */
			continue;
#endif
		p = &t->ch[i];
		if((q = p->pxl) == NULL)
			continue;
		putchar(i);			/* code */
#if SPC
		putchar(sp_to_dot0(p->tfmw));	/* pitch */
#endif

#if OFS
		putchar(p->width*8);
		putchar(-p->xoff);		/* offset */
#endif
		n_putc((maxh-p->yoff)*maxw, 0); /* raster data */
		for(y=p->height; --y >= 0; ) {
			n_putc(p->width, *q++);
			n_putc(maxw-p->width, 0);
		}
		n_putc((maxd+p->yoff-p->height)*maxw, 0);
	}
}


/*	set character	*/

set_char(cc, update)
{
	register FONT_CH *p;
	register n;

	int	c;

	c = cc > 255 ? get_char_type(f, (unsigned short)cc) : cc;
	p = &font[f].ch[c];
#if OFS
	if(cc > 255 || ((font[f].down) && (c != '\033') && (c != ' '))) {
#else
	if(cc > 255 || ((font[f].down) && (c > ' ') && (c <= '~'))) {
#endif
		if(cc > 255){
#if OFS
			move(sp_to_dot(h), sp_to_dot(v));
#else
			move(sp_to_dot(h)-(p->xoff), sp_to_dot(v)-(p->yoff));
#endif
			putchar(cc >> 8);
			putchar(cc & 0xff);
		}else{
#if OFS
			move(sp_to_dot(h), sp_to_dot(v));
#else
			move(sp_to_dot(h)-(p->xoff), sp_to_dot(v));
#endif
			putchar(c);
		}
	} else {
		move(sp_to_dot(h)-(p->xoff), sp_to_dot(v)-(p->yoff));
		n = p->width * p->height;
		printf("\233%d;%d.r", n, p->width);	/* rectangle image */
		fwrite(p->pxl, 1, n, stdout);
	}
	if(update) h += p->tfmw;
}


/*	set rule	*/

static	char	ldat[] = {0xff, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe};
set_rule(a, b, update)
{
	long	width;
	int	w, hh,	/* pixel units, device coord */
		xbyte,
		lastbyte,
		i, j, tbpy;
	char	outstr[80];

	hh = sp_to_dot(a);
	w = sp_to_dot(b);
	move(sp_to_dot(h), sp_to_dot(v) - hh + 1);
	printf("\2331x");
	if((hh != 1) && (w != 1)){
		printf("\2333s\233%de\233%da\233r", hh - 1, w - 1);
		/* CSST 2-96, CSED 2-97 */
	}else{
		xbyte = w >> 3;
		if(w & 7){
			lastbyte = ldat[w & 7];
		}else{
			lastbyte = -1;
			xbyte--;
		}
		printf("\233%d;%d;200.r", (xbyte+1)*hh, xbyte+1);
		for(i = 0; i < hh; i++){
			for(j = 0; j < xbyte; j++)
				putchar(-1);
			putchar(lastbyte);
		}
	}
	printf("\233x");
	if (update) {
		h += b;
	}
}

/*	move current point relative	*/

move(x, y)
register x, y;
{
	if(last_x != x){
		printf("\233%d`", x);
		last_x = x;
	}
	if(last_y != y){
		printf("\233%dd", y);
		last_y = y;
	}
}

