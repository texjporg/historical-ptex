/*
 * DEVICE dependent, raster font dependent
 */

#include	<stdio.h>
#include	"defs.h"
#include	"emit.h"
#include	"global.h"
#include	"rastfont.h"
#include	"jxl4font.h"
#include	"jxl4info.h"
#include	"ps.h"


/* form of dictionary */
#define	PACK		0
#define	DVI		1 


#define	NPACKPSCHARS	256
#define	LASTPACKPSCHAR	037
#define	FIRSTPACKPSCHAR	040

#define	CORRNORM	1000


struct pdlist {
    int pl_corr;
    short pl_font, pl_char;
    struct pdlist *pl_next;
};

dev_jxl4_fontdict(fe, c)		/* output a character bitmap */
struct font_entry *fe;
int c;
{
    register struct rastchar_entry *ce;
    struct jxl4fntinfo *rfi;
    struct pdlist *pl;
    struct pdlist *getpdlist();
    int corr, bytewidth, i;
    float cf;
    register int j;
    register unsigned char *sl;
    register int cc;
    float cw;	/* char width, in "dots"
		   - we rely on PostScript maintaining sufficient accuracy */
    
    end_string();
    ce = &((rfi = jxl4finfo(fe))->ch[c]);
    /* open font dict before first char */
    corr = (int)(rfi->corrfact*CORRNORM);
    cf = (float)corr/CORRNORM;
    if ((pl = getpdlist(corr))->pl_char == FIRSTPACKPSCHAR)
        EMIT(outfp, "%.3f %d /%s NF\n",
    cf, NPACKPSCHARS, psfname(pl->pl_font));
    ce->dev_font = pl->pl_font;
    ce->dev_char = pl->pl_char;
    dev_setfont(ce->dev_font);	/* because this isn't done on pass 0 */
    
    /* Output in PostScript coord system (y +ive up, x +ive right)
       (0,0) of char bitmap at lower left.  Output scan lines
       from bottom to top */
    EMITS("[<");
    bytewidth = (ce->width + 7) >> 3;
    for (i = ce->height-1, cc = 2;  i >= 0;  i--) {
	sl = (unsigned char *)(ce->where.pixptr + i*ce->nbpl);
	for (j = 0;  j < bytewidth;  j++, sl++) {
	    if (cc > 100) {
		EMITS("\n  ");   cc = 2;
	    }
	    EMITH(*sl);
	    cc += 2;
	}
    }
    cw = (float )ce->tfmw / (float) hconv;
    EMIT(outfp, "> %d %d %d %d %.3f] %d D\n", 
	 bytewidth<<3, ce->height,
	 ce->xOffset, (((int)ce->height)-ce->yOffset)-1, cw/cf,
	 ce->dev_char);
    (fe->ncdl)++;
}

dev_jxl4_initfe(fe)
register struct font_entry *fe;
{
    int ps_jxl4_setchar(), ps_jxl4_setstring();

    fe->dev_setchar = ps_jxl4_setchar;
    fe->dev_setstring = ps_jxl4_setstring;
    jxl4finfo(fe)->dictform = PACK;
    /* rastfinfo(fe)->font_space = fe->s/6;	/* never used */
}

ps_jxl4_setchar(c)
int c;
{
    register struct rastchar_entry *ce;
    register int cw;
    c = jis_to_dnf_index(c);
    ce = &(jxl4finfo(fontptr)->ch[c]);
    begin_string();
    pschar(ce->dev_char);
    ps_h += (cw = ce->tfmw);
    return (cw);
}

ps_jxl4_setstring(s, len)
char *s;
int len;
{
    Fatal("dvi2ps implementation error: ps_jxl4_setstring");
}
