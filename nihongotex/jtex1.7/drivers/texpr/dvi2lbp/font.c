/*
 *	Copyright (c) Japan TeX Users Group, 1986, 1987
 */

#include <stdio.h>
#include <strings.h>
#include "../common/def.h"

float	mag;
FONT	font[];


/*	load pixel file of font no. fn	*/

load_pixel(fn)
{
	register FONT *t;
	register FONT_CH *p;
	int	i, c;
	float	d;
	char	*q, *get_str();
	FILE	*pxl;

	t = &font[fn];
	if((pxl = fopen(t->a, "r")) == NULL){
		if(!read_jfm(t))
			error("can't open pixel file %s.\n", t->a);
		return;
	}
	c = get_int(pxl, 4);
	fseek(pxl, -4L, 2);
	if(c != get_int(pxl, 4) || c != PXL_ID)
		error("bad pixel file %s.\n", t->a);
/*
	fseek(pxl, -20L, 2);
	if((c = get_int(pxl, 4)) && t->c && c != t->c)
		error("pixel file %s version error.\n", t->a);
	fseek(pxl, 8L, 1);
*/
	fseek(pxl, -8L, 2);
	c = get_int(pxl, 4) * 4;
	fseek(pxl, 0L, 0);
	q = get_str(pxl, c);
	d = (float)t->d * t->fmag / (float)(1<<20);
	t->ch = (FONT_CH *)malloc(sizeof(FONT_CH [NCHAR]));
	if(t->ch == NULL)
		error("can't allocate memory for font table.\n");
	for(p=t->ch; p < &t->ch[NCHAR]; ++p) {
		register char *r, *s;
		register x, y;
	
		p->width = (get_uint(pxl, 2)+7)/8;
		p->height = get_uint(pxl, 2);
		p->xoff = get_int(pxl, 2);
		p->yoff = get_int(pxl, 2);
		i = get_int(pxl, 4);
		p->pxl = i ? q + i * 4 : NULL;
		p->tfmw = fix((float)get_int(pxl, 4) * d);

		r = s = p->pxl;	/* byte pack */
		for(y=p->height; --y>=0; s+=(-p->width)&3)
			for(x=p->width; --x>=0; )
				*r++ = *s++;
	}
	fclose(pxl);
}


/*	Is fn same as .down_font ?	*/

same_font(fn)
{
	register FILE	*fp;
	char	dfname[80], buf[80];

	sprintf(dfname, "%s/.down_font", LIB);
	if((fp = fopen(dfname, "r")) == NULL)
		return 0;
	fgets(buf, 80, fp);
	fclose(fp);
	return (strcmp(buf, font[fn].n) == 0);
}


/*	memorize name of font fn	*/

memo_font(fn)
{
	register FILE	*fp;
	char	dfname[80];

	sprintf(dfname, "%s/.down_font", LIB);
	if(fp = fopen(dfname, "w")) {
		fputs(font[fn].n, fp);
		fclose(fp);
	}
}

#define	ID	para[0]
#define	NT	para[1]
#define	LF	para[2]
#define	LH	para[3]
#define	BC	para[4]
#define	EC	para[5]
#define	NW	para[6]
#define	NH	para[7]
#define	ND	para[8]
#define	NI	para[9]
#define	NL	para[10]
#define	NK	para[11]
#define	NG	para[12]
#define	NP	para[13]

static	long	info[256], wid[256], ht[16], dp[16], glue[256];
static	unsigned long gluekern[256];

read_jfm(f)
FONT	*f;
{
	char	jfm_name[100];
	int	para[14];
	char	*p, *q, *r;
	register int	i, j, k, xsize, ysize, rsize, offset;
	float	dsize, d, vad;
	FILE	*jfm;
	register unsigned long *u;
	register CHAR_TYPE	*s;
	register FONT_CH	*t;

	p = jfm_name;
	r = f->a;
	while(*r){
		if(*r == '.')
			q = p;
		*p++ = *r++;
	}
	strcpy(q, ".tfm");
	if((jfm = fopen(jfm_name, "r")) == NULL)
		return(0);
	for(i = 0; i < 14; i++){
		para[i] = get_int(jfm, 2);
	}
	if(ID != 11)
		error("bad jfm file %s.\n", jfm_name);
	f->id = JFM_ID;
	fseek(jfm, (long)((LH)*4 - 1), 1);
	f->face = get_int(jfm, 1);
	if((s = f->ctype = (CHAR_TYPE*)malloc((NT + 1) * 4))==NULL)
		error("can't allocate memory for font table.\n");
	(s++)->type = NT;
	for(i = 0 ; i < NT; i++, s++){
		s->code = get_int(jfm, 2);
		s->type = get_int(jfm, 2);
	}
	for(i = BC; i <= EC; i++) 
		info[i] = get_int(jfm, 4);
	if((t = f->ch = (FONT_CH *)malloc(sizeof(FONT_CH)*(EC-BC+1)))==NULL)
		error("can't allocate memory for font table.\n");
	d = (float)f->d * f->fmag / (1<<20);
	for(i = 0; i < NW; i++)
		wid[i] = fix((float)get_int(jfm, 4) * d);
	for(i = 0; i < NH; i++)
		ht[i] = fix((float)get_int(jfm, 4) * d);
	for(i = 0; i < ND; i++)
		dp[i] = fix((float)get_int(jfm, 4) * d);
	fseek(jfm, (long)(NI*4), 1);
	for(i = 0; i < NL; i++)
		gluekern[i] = get_int(jfm, 4);
	fseek(jfm, (long)(NK*4), 1);
	for(i = 0; i < NG; i++){
		glue[i] = get_int(jfm, 4);
	}
	j = ((unsigned long)info[0] >> 16) & 0xf;
	k = ((unsigned long)info[0] >> 20) & 0xf;
	ysize = sp_to_dot(dp[j] + ht[k]);
	vad = (float)(dp[j])/(dp[j] + ht[k]);
	xsize = sp_to_dot(wid[(unsigned long)info[0] >> 24]);
	f->size = rsize = calc_pxl_size(ysize, &f->face);
	offset = (xsize - rsize)/2;

	for(i = BC; i <= EC; i++, t++){
		t->tfmw = wid[(unsigned)info[i] >> 24];
		t->yoff = adjust_yoffset(ysize) - ((int)(rsize * vad));
		t->xoff = -offset + adjust_xoffset(ysize);
		if(((info[0] >> 8) & 3) == 1){
			u = &gluekern[info[0] & 0xff];
			do{
				if((((*u >> 16) & 0xff) == i) &&
						(((*u >> 8) & 0xff) < 128)){
					t->xoff += sp_to_dot(fix(
					    (float)glue[(*u & 0xff)*3] * d));
					break;
				}
			}while((*u++ >> 24) < 128);
		}
	}
	fclose(jfm);
	return(1);
}

char
get_char_type(f, jc)
register unsigned short jc;
{
	register int	n, r;
	register unsigned half;
	register CHAR_TYPE	*dp, *sp, *ep, *mp;

	dp = font[f].ctype;
	n = dp->type;
	dp++;
	sp = dp + 1;
	ep = dp + --n;
	n--;
	if(ep->code < jc)
		return((char)dp->type);
	while(sp <= ep){
		if(half = n >> 1){
			mp = sp + (n & 1 ? half : (half - 1));
			if(!(r = jc - mp->code)){
				return((char)mp->type);
			}else{
				if(r < 0){
					ep = mp - 1;
					n = n & 1 ? half : half - 1;
				}else{
					sp = mp + 1;
					n = half;
				}
			}
		}else{
			if(jc == sp->code)
				return((char)sp->type);
			return((char)dp->type);
		}
	}
	return((char)dp->type);
}
