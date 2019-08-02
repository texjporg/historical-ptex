/*
 * DEVICE dependent, raster font dependent
 */

#include	<stdio.h>
#include	"defs.h"
#include	"emit.h"
#include	"global.h"
#include	"rastfont.h"
#include	"ps.h"


void getpackdict();
struct confop pdcop = {
    "packdict",
    getpackdict
};

/* form of dictionary */
#define	PACK		0
#define	DVI		1

struct packdict {
    char *pd_name;
    int pd_len;
    struct packdict *pd_next;
};
struct packdict *packdicts = NULL;
struct packdict **nextpd = &packdicts;

void
getpackdict(c, f)
int c;
FILE *f;
{
    char field[STRSIZE];
    register struct packdict *pd;

    c = getfield(field, c, f);
    skipline(c, f);
    pd = NEW(struct packdict, "packdict");
    pd->pd_name = strsave(field);
    pd->pd_len = strlen(field);
    pd->pd_next = NULL;
    *nextpd = pd;
    nextpd = &(pd->pd_next);
}

dictform(name)
register char *name;
{
    register struct packdict *pd;

    for (pd = packdicts; pd != NULL; pd = pd->pd_next)
	if (strncmp(name, pd->pd_name, pd->pd_len) == 0)
	    return (PACK);
    return (DVI);
}

#define	NPACKPSCHARS	256
#define	LASTPACKPSCHAR	037
#define	FIRSTPACKPSCHAR	040

#define	CORRNORM	1000

struct pdlist {
    int pl_corr;
    /* int pl_id; *//* may be used for finer classification */
    short pl_font, pl_char;
    struct pdlist *pl_next;
};
struct pdlist *pdlists = NULL;
struct pdlist **nextpl = &pdlists;

struct pdlist *
getpdlist(corr)
register int corr;
{
    register struct pdlist *pl;

    for (pl = pdlists; pl != NULL; pl = pl->pl_next)
	if (corr == pl->pl_corr) {
	    if (pl->pl_char++ == LASTPACKPSCHAR) {
		pl->pl_font = dev_nextdevfont();
		pl->pl_char = FIRSTPACKPSCHAR;
	    } else if (pl->pl_char == NPACKPSCHARS)
		pl->pl_char = 0;
	    return (pl);
	}
    pl = NEW(struct pdlist, "pdlist");
    pl->pl_corr = corr;
    pl->pl_font = dev_nextdevfont();
    pl->pl_char = FIRSTPACKPSCHAR;
    pl->pl_next = NULL;
    *nextpl = pl;
    nextpl = &(pl->pl_next);
    return (pl);
}

dev_rast_fontdict(fe, c)		/* output a character bitmap */
struct font_entry *fe;
int c;
{
    register struct rastchar_entry *ce;
    struct rastfntinfo *rfi;
    struct pdlist *pl;
    int corr, bytewidth, i;
    float cf;
    register int j;
    register unsigned char *sl;
    register int cc;
    float cw;	/* char width, in "dots"
		   - we rely on PostScript maintaining sufficient accuracy */
    
    end_string();

    ce = &((rfi = rastfinfo(fe))->ch[c]);
    /* open font dict before first char */
    if (rfi->dictform == PACK) {
	corr = (int)(rfi->corrfact*CORRNORM);
	cf = (float)corr/CORRNORM;
	if ((pl = getpdlist(corr))->pl_char == FIRSTPACKPSCHAR)
	    EMIT(outfp, "%.3f %d /%s NF\n",
		 cf, NPACKPSCHARS, psfname(pl->pl_font));
	ce->dev_font = pl->pl_font;
	ce->dev_char = pl->pl_char;
    } else /* if (rfi->dictform == DVI) */ {
	cf = rfi->corrfact;
	ce->dev_font = fe->k;
	ce->dev_char = c;
	if (fe->ncdl == 0)
	    EMIT(outfp, "%.3f %d /%s NF\n",
		 cf, rfi->nfntchars, psfname(fe->k));
    }
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
    cw = (float)ce->tfmw / (float)hconv;
    EMIT(outfp, "> %d %d %d %d %.3f] %d D\n", 
	 bytewidth<<3, ce->height,
	 ce->xOffset, (((int)ce->height)-ce->yOffset)-1, cw/cf,
	 ce->dev_char);
    (fe->ncdl)++;
}

dev_rast_initfe(fe)
register struct font_entry *fe;
{
    int ps_setchar(), ps_setstring();

    fe->dev_setchar = ps_setchar;
    fe->dev_setstring = ps_setstring;
    rastfinfo(fe)->dictform = dictform(fe->n);
    /* rastfinfo(fe)->font_space = fe->s/6;	/* never used */
}

ps_setchar(c)
int c;
{
    register struct rastchar_entry *ce = &(rastfinfo(fontptr)->ch[c]);
    register int cw;

    begin_string();
    pschar(ce->dev_char);
    ps_h += (cw = ce->tfmw);
    return (cw);
}

ps_setstring(s, len)
char *s;
int len;
{
    register char *sp;
    register struct rastchar_entry *ce = rastfinfo(fontptr)->ch;
    register int cw = 0;

    begin_string();
    for (sp = s; sp < s+len; sp++) {
	pschar((ce+*sp)->dev_char);
	cw += (ce+*sp)->tfmw;
    }
    ps_h += cw;
    return (cw);
}

pschar(c)
register char c;
{
    if (c < ' ' || c >= 0177)
	EMITO(c);
    else if (c == '(' || c == ')' || c == '\\') {
	EMITC('\\');
	EMITC(c);
    } else
	EMITC(c);
}
