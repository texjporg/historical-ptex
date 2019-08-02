/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

#include <stdio.h>
#include <strings.h>
#include "../common/def.h"

int	vflag;
float	mag;
FONT	font[];

/*	load pixel file of font no. fn	*/

load_pixel(fn)
{
	register FONT *t;
	register FONT_CH *p;
	register i, c;
	char	*get_str();
	FILE	*pxl;

	t = &font[fn];
	if((pxl = fopen(t->a, "r")) == NULL){
		if(!check_jfm(t->a)){
			error("can't open pixel(jfm) file %s.\n", t->a);
		}
		t->id = JFM_ID;
		return;
	}
	c = get_int(pxl, 4);
	fseek(pxl, -4, 2);
	if(c != get_int(pxl,4) || c != PXL_ID)
		error("bad pixel file %s.\n", t->a);
	t->id = c;
/*
	fseek(pxl, -20, 2);
	if((c = get_int(pxl, 4)) && t->c && c != t->c)
		error("pixel file %s version error.\n", t->a);
	fseek(pxl, 8, 1);
*/
	fseek(pxl, -8, 2);
	fseek(pxl, get_int(pxl, 4)*4, 0);
	t->ch = (FONT_CH *)malloc(sizeof(FONT_CH [NCHAR]));
	if(t->ch == NULL)
		error("can't allocate memory for font table.\n");
	for(p=t->ch; p < &t->ch[NCHAR]; ++p) {	
		p->width = (get_uint(pxl, 2)+7)/8;
		p->height = get_uint(pxl, 2);
		p->xoff = get_int(pxl, 2);
		p->yoff = get_int(pxl, 2);
		p->pxl = (char *)get_int(pxl, 4); /* NULL, if not defined */
		fseek(pxl, 4, 1);
	}
	fclose(pxl);
}

/*	search most frequently used font	*/

font_search()
{
	register FONT	*p, *q;
	register FONT_CH *r;
	register m, i;
	int	 maxw, maxh, maxd, nchar;

	if(vflag)
	    for(p=font; p < &font[MAX_FONT]; ++p)
	      if(p->n)
		fprintf(stderr,"%2d: %s (%d)\n", p-font, p->n, p->down);

	for( ; ; ) {
		m = 0; q = NULL;
		for(p=font; p < &font[MAX_FONT]; ++p)
			if((p->down > m) && (p->id != JFM_ID)) {
				m = p->down;
				q = p;
			}
		if(m < NCHAR) {
			if(vflag) fprintf(stderr, "no down load.\n");
			return -1;
		}
		maxw = maxh = maxd = nchar = 0;
		for(i=0; i < NCHAR; ++i) {
			if(i == '\033' || i == ' ')
				continue;
			r = &q->ch[i];
			if(r->pxl == NULL)
				continue;
			++nchar;
			if(r->width > maxw)	maxw = r->width;
			if(r->yoff > maxh)	maxh = r->yoff;
			if(r->height-r->yoff > maxd)
				maxd = r->height-r->yoff;
		}
		if(maxw <= MAX_W/8 && maxh+maxd <= MAX_H
		/*  && (maxw*(maxh+maxd)+(2+OFS))*nchar <= MAX_MEM */   ) {
			if(vflag) fprintf(stderr, "down load %s\n", q->n);
			return q-font;
		}
		if(vflag)
			fprintf(stderr,"Sorry, box of %s is too big.\n", q->n);
		q->down = 0;
	}
}

check_jfm(pxl_name)
char	*pxl_name;
{
	char	jfm_name[100];
	int	id;
	char	*p, *q;
	FILE	*jfm;

	q = p = jfm_name;
	while(*pxl_name){
		if(*pxl_name == '.')
			q = p;
		*p++ = *pxl_name++;
	}
	strcpy(q, ".tfm");
	if((jfm = fopen(jfm_name, "r")) == NULL)
		return(0);
	id = get_int(jfm, 2);
	fclose(jfm);
	if(id != 11)
		return(0);
	return(1);
}
