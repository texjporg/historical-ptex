/*
 *	This file is based on `texpr/dvi2lpb/pkfont.c'.
 *	The copyright of the original is:
 *		Copyright (c) Japan TeX Users Group, 1986 1987
 */

#include	<stdio.h>
#include	"defs.h"
#include	"global.h"
#include	"rastfont.h"
#include	"pkcodes.h"

void read_pk_fontinfo();
struct fontop pkop = {
    "pk",
    pktype_access,
    read_pk_fontinfo,
};

unsigned int maxcc;

void
read_pk_fontinfo(fe)
register struct font_entry *fe;
{
    register FILE *fntfp;
    register int cmd, i, t;
    struct rastfntinfo *rfi;
    int ds;
    int pk_fontdict();

    openfontfile(fe);
    fntfp = fe->openfile;
    i = 0;
    do {
	if (fseek(fntfp, --i, 2) == -1)
	    Fatal("can't seek to the end of pk file %s\n", fe->name);
    } while ((t=getuint(fntfp, 1)) == PK_NO_OP);
    fseek(fntfp, 0, 0);
    if (t != PK_POST ||
	getuint(fntfp,1) != PK_PRE || getuint(fntfp,1) != PK_ID)
	Fatal("bad pk file %s\n", fe->name);
    fseek(fntfp, getuint(fntfp, 1), 1);		/* comment */
    ds = getuint(fntfp, 4);	/* design size */
    if ((t = getuint(fntfp, 4)) && fe->c && t != fe->c)
	Warning("font = \"%s\",\n-->font checksum = %d,\n-->dvi checksum = %d",
		fe->name, fe->c, t);
    fseek(fntfp, 8, 1);			/* hppp,vppp */

    rastfinfo(fe) = rfi = alloc_rastfinfo(NFNTCHARS, TRUE, rastfinfo(fe));
    /* rfi->designsize = ds; */
    maxcc = 0;
    for (;;) {
	switch (cmd = getuint(fntfp, 1)) {

	case PK_XXX1: case PK_XXX2: case PK_XXX3: case PK_XXX4:
	    fseek(fntfp, getuint(fntfp, cmd-PK_XXX1+1), 1);
	    continue;

	case PK_YYY:
	    getuint(fntfp,4);
	    continue;

	case PK_NO_OP:
	    continue;

	case PK_POST:
	    break;

	default:
	    if (cmd <= PK_FLAG)
		pkcharoffset(cmd, fe, fntfp);
	    else
		Fatal("illegal pk command %d in %s\n", cmd, fe->name);
	    continue;
	}
	break;
    }
    rfi->nfntchars = maxcc+1;
    fe->dev_fontdict = pk_fontdict;
    dev_rast_initfe(fe);
}

pkcharoffset(flag, fe, fntfp)
int flag;
register struct font_entry *fe;
register FILE *fntfp;
{
    int off, pl;
    unsigned int cc;
    register int n;
    register struct rastchar_entry *tcharptr;/* temporary char_entry pointer  */

    off = ftell(fntfp) - 1;
    if ((n = flag&7) == 7) {	/* long form */
	pl = getuint(fntfp, 4) - 8;	/* without tfmw,dx */
	if ((cc = getuint(fntfp, 4)) >= NFNTCHARS) {
	    Warning("cc %d in pk file %s (skipped)", cc, fe->name);
	    fseek(fntfp, pl+8, 1);
	    return;
	}
	tcharptr = &rastfinfo(fe)->ch[cc];
	tcharptr->tfmw = ((float)getuint(fntfp, 4)*(float)fe->s) /
	    (float)(1<<20);
	getuint(fntfp, 4); 	/* dx */
	n = 4;
    } else if (n >= 4) {		/* extended short form */
	pl = (n-4)*65536 + getuint(fntfp, 2) - 3;	/* without tfmw */
	tcharptr = &rastfinfo(fe)->ch[cc = getuint(fntfp, 1)];
	tcharptr->tfmw = ((float)getuint(fntfp, 3)*(float)fe->s) /
	    (float)(1<<20);
	n = 2;
    } else {			/* short form */
	pl = n*256 + getuint(fntfp, 1) - 3;	/* without tfmw */
	tcharptr = &rastfinfo(fe)->ch[cc = getuint(fntfp, 1)];
	tcharptr->tfmw = ((float)getuint(fntfp, 3)*(float)fe->s) /
	    (float)(1<<20);
	n = 1;
    }
    if (cc > maxcc)
	maxcc = cc;
    getuint(fntfp, n); 		/* dy or dm */
    tcharptr->width = getuint(fntfp, n);
    tcharptr->height = getuint(fntfp, n);
    tcharptr->xOffset = getint(fntfp, n);
    tcharptr->yOffset = getint(fntfp, n);
    tcharptr->nbpl = (tcharptr->width + 7) >> 3;
    tcharptr->where.fileOffset = off;
    fseek(fntfp, pl-5*n, 1);
}

pk_fontdict(fe, c)
struct font_entry *fe;
int c;
{
    void loadpkchar();
    return (rast_fontdict(fe, c, loadpkchar));
}

int	width, height, dyn_f;
int	b_width, black;
char	*pixel;
char	pat[8] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01};
FILE	*pkfp;

/*	character shape definition	*/

void
loadpkchar(fe, c)
register struct font_entry *fe;
int c;
{
    int flag, n, l;
    register struct rastchar_entry *ce;

    ce = &(rastfinfo(fe)->ch[c]);
    openfontfile(fe);
    fseek(pkfp = fe->openfile, ce->where.fileOffset, 0);
    flag = getuint(pkfp, 1);
    dyn_f = flag/16;
    black = (flag&8) >> 3;
    if ((n = flag&7) == 7) {	/* long form */
	l = 16;
	n = 4;
    } else if (n >= 4) {	/* extended short form */
	l = 6;
	n = 2;
    } else {			/* short form */
	l = 5;
	n = 1;
    }
    fseek(pkfp, l+5*n, 1);
    width = ce->width;
    height = ce->height;
    b_width = ce->nbpl;
    if ((pixel = (char *)calloc(height, b_width)) == NULL)
	Fatal("Unable to allocate memory for char\n");
    if (dyn_f == 14)
	unpack_raster();
    else
	unpack_run();
    ce->where.pixptr = pixel;
}

/*	get nybble	*/

#define get_nyb()  ((nyb_f^=1)? (nyb_w=getuint(pkfp,1))/16: nyb_w&15)

/*	get run_count/repeat_count	*/

#define get_run()\
	for (;;) {\
		if ((i=get_nyb()) >= 14) {\
			rep = i*2-29;\
			continue;\
		} else if (i > dyn_f)\
			run = (i-dyn_f-1)*16 + get_nyb() + dyn_f + 1;\
		else if (i > 0)\
			run = i;\
		else /* i==0 */ {\
			do	++i;\
			while ((run = get_nyb()) == 0);\
			while (--i >= 0)\
				run = run*16 + get_nyb();\
			run += (13-dyn_f)*16 + dyn_f - 15;\
		}\
		if (rep >= 0)\
			break;\
		rep = run;\
	}\

#define	min(x,y)	((x)<=(y) ? (x) : (y))

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
    for (v=0; v < height; v += rep+1) {
	rep = 0;
	for (h=0; h < width; h=h1) {
	    if (run == 0) {
		get_run();
		black ^= 1;
	    }
	    h1 = min(h+run, width);
	    if (black) {
		h1 = min(h1, (h+8)&~7);
		p[h/8] |= pat[h&7];
		if(h1 & 7)
		    p[h1/8] &= ~pat[h1&7];
	    }
	    run -= h1-h;
	}
	q = p;	p += b_width;
	for (h=rep*b_width; --h >= 0; )
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
    for (v=0; v < height; ++v) {
	for (h=0; h < width; h+=m) {
	    if (n == 0) {
		t = getuint(pkfp, 1);
		n = 8;
	    }
	    m = min(n, 8-(h&7));
	    m = min(m, width-h);
	    p[h/8] |= t >> (h&7);
	    t <<= m; n -= m;
	}
	if (width & 7)
	    p[width/8] &= ~pat[width&7];
	p += b_width;
    }
}
