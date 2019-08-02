/*
 * DEVICE dependent, built-in font dependent
 */

#include	<stdio.h>
#include	"defs.h"
#include	"emit.h"
#include	"global.h"
#include	"bifont.h"
#include	"ps.h"


/* TYPE: tfm
 */
dev_tfm_fontdict(fe, c)
struct font_entry *fe;
int c;
{
    if ((fe->ncdl)++ == 0)	/* open font dict before first char */
	EMIT(outfp, "/%s /%s %.3f %.3f LWF\n",
	     psfname(fe->k), fe->n,
	     (float)fe->s/(float)hconv, -(float)fe->s/(float)vconv);
    return (fe->k);
}

/* bikanji
 */
void getbikanji();
struct confop bkcop = {
    "bikanji",
    getbikanji
};

struct bikanji {
    char *bk_name;
    int bk_len;
    int bk_jfam;
    int bk_fixed;
    char *bk_psname;
    struct bikanji *bk_next;
};
struct bikanji *bikanjis = NULL;
struct bikanji **nextbk = &bikanjis;
int bkjfam = 1;

void
getbikanji(c, f)
int c;
FILE *f;
{
    char field_name[STRSIZE];
    char field_fixed[STRSIZE];
    char field_psname[STRSIZE];
    register struct bikanji *bk;

    c = getfield(field_name, c, f);
    c = getfield(field_fixed, c, f);
    c = getfield(field_psname, c, f);
    skipline(c, f);
    bk = NEW(struct bikanji, "bikanji");
    bk->bk_name = strsave(field_name);
    bk->bk_len = strlen(field_name);
    bk->bk_jfam = bkjfam++;
    bk->bk_fixed = strcmp(field_fixed, "fixed") == 0;
    bk->bk_psname = strsave(field_psname);
    bk->bk_next = NULL;
    *nextbk = bk;
    nextbk = &(bk->bk_next);
}

struct bikanji *
findbikanji(name)
register char *name;
{
    register struct bikanji *bk;

    for (bk = bikanjis; bk != NULL; bk = bk->bk_next)
	if (strncmp(name, bk->bk_name, bk->bk_len) == 0)
	    return (bk);
    Fatal("%s is not built-in kanji", name);
}

dev_isbikanji(name)
register char *name;
{
    register struct bikanji *bk;

    for (bk = bikanjis; bk != NULL; bk = bk->bk_next)
	if (strncmp(name, bk->bk_name, bk->bk_len) == 0)
	    return (TRUE);
    return (FALSE);
}

char *
dev_bikanjiname(name)
char *name;
{
    return (findbikanji(name)->bk_psname);
}


/* TYPE: jstfm
 */
struct jstfmfdict {
    int jd_jfam;
    int jd_s;
    int jd_jfont;
    int jd_open;
    struct jstfmfdict *jd_next;
};
struct jstfmfdict *jstfmfdicts = NULL;
struct jstfmfdict **nextjd = &jstfmfdicts;

dev_getjfont(fe)
register struct font_entry *fe;
{
    register int s, jfam;
    register struct jstfmfdict *jd;
    struct bikanji *bk;

    s = fe->s;
    jfam = (bk = findbikanji(fe->n))->bk_jfam;
    for (jd = jstfmfdicts; jd != NULL; jd = jd->jd_next)
	if (jd->jd_s == s && jd->jd_jfam == jfam) {
	    jstfmfinfo(fe)->dev_font = jd->jd_jfont;
	    jstfmfinfo(fe)->dictopen = &(jd->jd_open);
	    jstfmfinfo(fe)->dev_name = bk->bk_psname;
	    return (getjsubfont(fe->n+bk->bk_len));
	}
    jd = NEW(struct jstfmfdict, "jstfmfdict");
    jd->jd_s = s;
    jd->jd_jfam = jfam;
    jd->jd_jfont = jstfmfinfo(fe)->dev_font = dev_nextdevfont();
    jd->jd_open = FALSE;
    jd->jd_next = NULL;
    *nextjd = jd;
    nextjd = &(jd->jd_next);
    jstfmfinfo(fe)->dictopen = &(jd->jd_open);
    jstfmfinfo(fe)->dev_name = bk->bk_psname;
    return (getjsubfont(fe->n+bk->bk_len));
}

dev_jstfm_fontdict(fe, c)
struct font_entry *fe;
int c;
{
    register struct jstfmfntinfo *jstfmfi;
    int size;

    (fe->ncdl)++;
    if (!*((jstfmfi = jstfmfinfo(fe))->dictopen)) {
	size = findbikanji(fe->n)->bk_fixed
		? (jstfmfi->ch+jstfmfi->lastfntchar)->tfmw
		: fe->s;
	EMIT(outfp, "/%s /%s %.3f %.3f LWF\n",
	     psfname(jstfmfi->dev_font), jstfmfi->dev_name,
	     (float)size/(float)hconv, -(float)size/(float)vconv);
	*(jstfmfi->dictopen) = TRUE;
    }
    return (jstfmfi->dev_font);
}

/* TYPE: jfm
 */
dev_jfm_fontdict(fe, c)
struct font_entry *fe;
int c;
{
    register struct jfmfntinfo *jfmfi;
    int size;

    if ((fe->ncdl)++ == 0) {	/* open font dict before first char */
	jfmfi = jfmfinfo(fe);
	size = findbikanji(fe->n)->bk_fixed ? jfmfi->ch->tfmw : fe->s;
	EMIT(outfp, "/%s /%s %.3f %.3f LWF\n",
	     psfname(fe->k), jfmfi->dev_name,
	     (float)size/(float)hconv, -(float)size/(float)vconv);
    }
    return (fe->k);
}

/* TYPE: bitex
 */
dev_bitex_fontdict(fe, c)
struct font_entry *fe;
int c;
{
    if ((fe->ncdl)++ == 0)	/* open font dict before first char */
	EMIT(outfp, "/%s /%s %.3f %.3f LWF\n",
	     psfname(fe->k), bitexfinfo(fe)->dev_name,
	     (float)fe->s/(float)hconv, -(float)fe->s/(float)vconv);
    return (fe->k);
}


/*
 * LW dependent setchar/setstring
 */
dev_tfm_initfe(fe)
register struct font_entry *fe;
{
    int tfm_setchar(), tfm_setstring();

    fe->dev_setchar = tfm_setchar;
    fe->dev_setstring = tfm_setstring;
}

tfm_setchar(c)
int c;
{
    register struct tfmchar_entry *ce = &(tfmfinfo(fontptr)->ch[c]);
    register int cw;

    begin_string();
    pschar(ce->dev_char);
    ps_h += (cw = ce->tfmw);
    return (cw);
}

tfm_setstring(s, len)
char *s;
int len;
{
    register char *sp;
    register struct tfmchar_entry *ce = tfmfinfo(fontptr)->ch;
    register int cw = 0;

    begin_string();
    for (sp=s; sp < s+len; sp++) {
	pschar((ce+*sp)->dev_char);
	cw += (ce+*sp)->tfmw;
    }
    ps_h += cw;
    return (cw);
}

dev_jstfm_initfe(fe)
register struct font_entry *fe;
{
    int jstfm_setchar(), jstfm_setstring();

    fe->dev_setchar = jstfm_setchar;
    fe->dev_setstring = jstfm_setstring;
}

jstfm_setchar(c)
int c;
{
    register struct jstfmchar_entry *ce = &(jstfmfinfo(fontptr)->ch[c]);
    register int cw;

    begin_string();
    pschar(ce->dev_ku);
    pschar(ce->dev_ten);
    ps_h += (cw = ce->tfmw);
    return (cw);
}

jstfm_setstring(s, len)
char *s;
int len;
{
    register char *sp;
    register struct jstfmchar_entry *ce = jstfmfinfo(fontptr)->ch;
    register int cw = 0;

    begin_string();
    for (sp=s; sp < s+len; sp++) {
	pschar((ce+*sp)->dev_ku);
	pschar((ce+*sp)->dev_ten);
	cw += (ce+*sp)->tfmw;
    }
    ps_h += cw;
    return (cw);
}

dev_jfm_initfe(fe)
register struct font_entry *fe;
{
    int jfm_setchar(), jfm_setstring();

    fe->dev_setchar = jfm_setchar;
    fe->dev_setstring = jfm_setstring;
}

jfm_setchar(c)
register int c;
{
    register int cw;
    register struct jfmfntinfo *jfmfi = jfmfinfo(fontptr);

    begin_string();
    pschar(c>>8);
    pschar(c&0xff);
    ps_h += (cw = jfmfi->ch[getctype(c,jfmfi)].tfmw);
    /* quick hack to disable USERELPOS */
    ps_v = -1;
    return (cw);
}

jfm_setstring(s, len)
char *s;
int len;
{
    Fatal("dvi2ps implementation error: jfm_setstring");
}
