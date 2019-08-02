/*
 *	This file is based on Beebe's driver program.
 */

#include	<stdio.h>
#include	"defs.h"
#include	"global.h"
#include	"rastfont.h"
#include	"gfcodes.h"

void read_gf_fontinfo();
struct fontop gfop = {
    "gf",
    pktype_access,
    read_gf_fontinfo,
};

void
read_gf_fontinfo(fe)
register struct font_entry *fe;
{
    register FILE *fntfp;
    int t, i, nchars;
    register struct rastchar_entry *tcharptr;
    struct rastfntinfo *rfi;
    int ds;
    int gf_fontdict();

    openfontfile(fe);
    fntfp = fe->openfile;
    fseek(fntfp, -4L, 2);
    while ((t = getuint(fntfp, 1)) == GF_FILLER)
	fseek(fntfp, -2L, 1);
    if (t != GF_ID)
	Fatal("GF ID = %d, can only process GF ID = %d files",
	      t, GF_ID);
    fseek(fntfp, -6L, 1);
    if ((t = getuint(fntfp, 1)) != GF_POSTPOST)
	Fatal("GF PostPost = %d, can only process GF PostPost = %d files",
	      t, GF_POSTPOST);
    fseek(fntfp, (long)getuint(fntfp, 4), 0);
    if ((t = getuint(fntfp, 1)) != GF_POST)
	Fatal("GF Post = %d, can only process GF Post = %d files",
	      t, GF_POST);
    fseek(fntfp, 4L, 1);
    ds = getuint(fntfp, 4);
    t = getuint(fntfp, 4);
    if ((fe->c != 0) && (t != 0) && (fe->c != t))
	Warning("font = \"%s\",\n-->font checksum = %d,\n-->dvi checksum = %d",
		fe->name, fe->c, t);
    t = getuint(fntfp, 4);
    fseek(fntfp, 20L, 1);

    rastfinfo(fe) = rfi = alloc_rastfinfo(NFNTCHARS, TRUE, rastfinfo(fe));
    /* rfi->designsize = ds; */
    /* rfi->magnification = (0.5 + 5.0 * 72.27 * (float)t) / 65536.0; */
    nchars = 0;
    while (((t = getuint(fntfp, 1)) == GF_CHAR_LOC) || 
	   (t == GF_CHAR_LOC0)) {
	i = getuint(fntfp, 1);
	tcharptr = &(rfi->ch[i]);
	if (t == GF_CHAR_LOC) 
	    fseek(fntfp, 8L, 1);
	else
	    fseek(fntfp, 1L, 1);
	tcharptr->tfmw = ((float)getuint(fntfp, 4) * (fe->s)) /
			  (float)(1 << 20);
	tcharptr->where.fileOffset = getuint(fntfp, 4);
	nchars++;
    }
    if (t != GF_POSTPOST)
	Fatal("There is %d between Post Charloc and PostPost", t);
    rfi->nfntchars = nchars;
    fe->dev_fontdict = gf_fontdict;
    dev_rast_initfe(fe);
}

gf_fontdict(fe, c)
struct font_entry *fe;
int c;
{
    void loadgfchar();
    return (rast_fontdict(fe, c, loadgfchar));
}

#define		WHITE		0
#define		BLACK		1
#define		SETBIT(x,y)	pixel[y+(x>>3)] |= 0x80>>(x&7)
#define		INVERT(x)	x = 1-x

void
loadgfchar(fe, c)
register struct font_entry *fe;
int c;
{
    char *pixel;
    int i, j, k, t;
    int min_m, max_m, del_m, min_n, max_n, del_n;
    int paint, length;
    register struct rastchar_entry *ce;
    FILE *pxlfp;

    ce = &(rastfinfo(fe)->ch[c]);
    openfontfile(fe);
    fseek(pxlfp = fe->openfile, (long)(ce->where.fileOffset), 0);

    if ((t = getuint(pxlfp, 1)) == GF_BOC) {
	if (getuint(pxlfp, 4) != c)
	    Fatal("Postamble %d points glyph %d", c, t);
	fseek(pxlfp, 4L, 1);
	min_m = getint(pxlfp, 4);
	max_m = getint(pxlfp, 4);
	min_n = getint(pxlfp, 4);
	max_n = getint(pxlfp, 4);
	del_m = max_m - min_m;
	del_n = max_n - min_n;
    } else if (t == GF_BOC1) {
	if (getuint(pxlfp, 1) != c)
	    Fatal("Postamble %d points glyph %d", c, t);
	del_m = getuint(pxlfp, 1);
	max_m = getuint(pxlfp, 1);
	del_n = getuint(pxlfp, 1);
	max_n = getuint(pxlfp, 1);
	min_m = max_m - del_m;
    } else
	Fatal("%d should be GF BOC or GF BOC1", t);

    ce->width = del_m;
    ce->height = del_n + 1;
    ce->xOffset = -min_m;
    ce->yOffset = max_n;
    ce->nbpl = ((ce->width + 31) >> 5) * 4;
    if ((pixel = (char *)malloc(ce->nbpl * ce->height)) == NULL)
	Fatal("Unable to allocate memory for char");

    for (i = 0; i < ce->height; i++)
	for (j = 0; j < ce->nbpl; j++)
	    pixel[j+i*ce->nbpl] = 0;

    ce->where.pixptr = pixel;
    i = j = 0;
    paint = WHITE;

    while ((t = getuint(pxlfp, 1)) != GF_EOC) {
	switch (t) {
	case GF_PAINT_0 :
	    INVERT(paint);
	    break;
	case GF_PAINT1 : case GF_PAINT2 : case GF_PAINT3 :
	    length = getuint(pxlfp, t - GF_PAINT1 + 1);
	    if (paint == BLACK)
		for (k = 0; k < length; i++, k++)
		    SETBIT(i, j);
	    else
		i += length;
	    INVERT(paint);
	    break;
	case GF_SKIP0 :
	    j += ce->nbpl;
	    i = 0;
	    paint = WHITE;
	    break;
	case GF_SKIP1 : case GF_SKIP2 : case GF_SKIP3 :
	    j += ce->nbpl * (getuint(pxlfp, t - GF_SKIP0) + 1);
	    i = 0;
	    paint = WHITE;
	    break;
	case GF_XXX1 : case GF_XXX2 : case GF_XXX3 : case GF_XXX4 :
	    fseek(pxlfp, (long)getint(pxlfp, t - GF_XXX1 + 1), 1);
	    break;
	case GF_YYY :
	    fseek(pxlfp, 4L, 1);
	    break;
	case GF_NOP :
	    break;
	default :
	    if ((GF_PAINT_0 < t) && (t < GF_PAINT1)) {
		length = t - GF_PAINT_0;
		if (paint == BLACK)
		    for (k = 0; k < length; i++, k++)
			SETBIT(i,j);
		else
		    i += length;
		INVERT(paint);
	    } else if ((GF_NEW_ROW_0 <= t) && (t <= GF_NEW_ROW_MAX)) {
		j += ce->nbpl;
		paint = BLACK;
		i = t - GF_NEW_ROW_0;
	    } else {
		fprintf(stderr, "GF invalid %d command in %s.\n", t, fe->name);
		fseek(pxlfp, -20L, 1);
		for (k = 0; k < 20; k++)
		    fprintf(stderr, "%3d ", getuint(pxlfp, 1));
		fprintf(stderr, "\n");
		for (k = 0; k < 20; k++)
		    fprintf(stderr, "%3d ", getuint(pxlfp, 1));
		fprintf(stderr, "\n");
	    }
	    break;
	}
    }
}

