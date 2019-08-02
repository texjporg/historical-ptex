/*
 * built-in font
 */

#include	<stdio.h>
#include	"file.h"
#include	"defs.h"
#include	"global.h"
#include	"bifont.h"
#include	"jsub.h"

int nomag_access();
int nomag_bk_access();
void read_tfm_fontinfo();
void read_jstfm_fontinfo();
void read_jfm_fontinfo();
void read_bitex_fontinfo();

struct fontop tfmop = {
    "tfm",
    nomag_access,
    read_tfm_fontinfo,
};

struct fontop jstfmop = {
    "jstfm",
    nomag_bk_access,
    read_jstfm_fontinfo,
};

struct fontop jfmop = {
    "jfm",
    nomag_bk_access,
    read_jfm_fontinfo,
};

struct fontop bitexop = {
    "bitex",
    nomag_access,
    read_bitex_fontinfo,
};


nomag_access(proto, fe, pva)
char *proto;
struct font_entry *fe;
struct pavearg *pva;
{
    /* This may be device dependent, because not all devices allow
       arbitrary magnification. */
    pave(fe->name, proto, pva);
    return (access(fe->name, R_OK) == 0);
}

nomag_bk_access(proto, fe, pva)
char *proto;
struct font_entry *fe;
struct pavearg *pva;
{
    /* This may be device dependent, because not all devices allow
       arbitrary magnification. */
    pave(fe->name, proto, pva);
    if (access(fe->name, R_OK) == 0)
	if (dev_isbikanji(fe->n))
	    return (TRUE);
	/*else
	    Warning("%s is not buit-in kanji", fe->n);*/
    return (FALSE);
}

/*
    There are three kinds of tfm files, e.g.
	Times-Roman.tfm, Courier.tfm, ...
	XXjhira.tfm, XXja.tfm, ... (XX: japanese font)
	XX.tfm, ... (jfm format, XX: japanese font)
    The first one: (TYPE tfm)
	easy.
    The second one: (TYPE jstfm)
	dev_font: assgined when XX first appears
	dev_ku, dev_ten: calculated from j?? and charcode
    The third one: (TYPE jfm)
	easy, but tfmw information can be stored efficiently
	by using jfm structure
*/

/* set by readtfm, readjfm */
char width[4*256];
int nt, lh, bc, ec, nw;
#define	JSTFMHEAD	4*2
char header[JSTFMHEAD];

/* TYPE: tfm
 */
void
read_tfm_fontinfo(fe)
register struct font_entry *fe;
{
    register FILE *fntfp;
    register struct tfmchar_entry *ce;
    register int i;
    float s;
    int dev_tfm_fontdict();

    openfontfile(fe);
    readtfm(fntfp = fe->openfile);
    tfmfinfo(fe) = NEW(struct tfmfntinfo, "tfmfont info");
    tfmfinfo(fe)->lastfntchar = ec;
    
    s = (float)fe->s;
    for (i = bc, ce = (tfmfinfo(fe)->ch)+bc; i <= ec; i++, ce++) {
	/* ce->dev_font = fe->k;	/* Not used */
	ce->dev_char = i;
	ce->tfmw = ((float)makeuint(width+4*getuint(fntfp,1),4)*s) /
		   (float)(1<<20);
	fseek(fntfp, 3, 1);
    }
    fe->dev_fontdict = dev_tfm_fontdict;
    dev_tfm_initfe(fe);
}

/* TYPE: jstfm
 */
void
read_jstfm_fontinfo(fe)
register struct font_entry *fe;
{
    register FILE *fntfp;
    register struct jstfmchar_entry *ce;
    register int i;
    float s;
    int subfont;
    int dev_jstfm_fontdict();

    openfontfile(fe);
    readtfm(fntfp = fe->openfile);
    if (lh <= 2 || makeuint(header, 4) != JSTFM_MAGIC)
	Fatal("%s is not japanese subfont", fe->name);
    subfont = header[JSTFMHEAD-1];
    jstfmfinfo(fe) = NEW(struct jstfmfntinfo, "jstfmfont info");
    jstfmfinfo(fe)->lastfntchar = ec;
    if (subfont != dev_getjfont(fe))
	Warning("%s has subfont number %d", fe->name, subfont);
    s = (float)fe->s;
    for (i = bc, ce = (jstfmfinfo(fe)->ch)+bc; i <= ec; i++, ce++) {
	compute_jis(subfont, i, &(ce->dev_ku), &(ce->dev_ten));
	ce->tfmw = ((float)makeuint(width+4*getuint(fntfp,1),4)*s) /
		   (float)(1<<20);
	fseek(fntfp, 3, 1);
    }
    fe->dev_fontdict = dev_jstfm_fontdict;
    dev_jstfm_initfe(fe);
}

/* TYPE: jfm
 */
void
read_jfm_fontinfo(fe)
struct font_entry *fe;
{
    register FILE *fntfp;
    register struct jfmtype_entry *te;
    register struct jfmchar_entry *ce;
    register int i;
    float s;
    int dev_jfm_fontdict();

    openfontfile(fe);
    if (!readjfm(fntfp = fe->openfile))
	Fatal("%s is not jfm file", fe->name);
    jfmfinfo(fe) = NEW(struct jfmfntinfo, "jfmfont info");
    jfmfinfo(fe)->nctype = nt;
    jfmfinfo(fe)->ctype = NEWTAB(struct jfmtype_entry, nt, "jfmtype table");
    for (i = 0, te = jfmfinfo(fe)->ctype; i < nt; i++, te++) {
	te->jfm_code = getuint(fntfp, 2);
	te->jfm_type = getuint(fntfp, 2);
    }
    jfmfinfo(fe)->lasttypecode = (te-1)->jfm_code;
    jfmfinfo(fe)->ch = NEWTAB(struct jfmchar_entry, ec+1, "jfmchar table");
    s = (float)fe->s;
    for (i = bc, ce = (jfmfinfo(fe)->ch)+bc; i <= ec; i++, ce++) {
	ce->tfmw = ((float)makeuint(width+4*getuint(fntfp,1),4)*s) /
		   (float)(1<<20);
	fseek(fntfp, 3, 1);
    }
    jfmfinfo(fe)->dev_name = dev_bikanjiname(fe->n);
    fe->dev_fontdict = dev_jfm_fontdict;
    dev_jfm_initfe(fe);
}

getctype(c, jfmfi)
int c;
struct jfmfntinfo *jfmfi;
{
    register int m;
    register int left, right;
    register struct jfmtype_entry *te;
    int code;

    if (c > jfmfi->lasttypecode)
	return (0);
    for (left = 0, right = jfmfi->nctype, te = jfmfi->ctype; left <= right;) {
	m = (left+right)/2;
	if (c < (code = (te+m)->jfm_code))
	    right = m-1;
	else if (c > code)
	    left = m+1;
	else if (c == code)
	    return ((te+m)->jfm_type);
    }
    return (0);
}

/* TYPE: bitex (built-in tex)
 */
void
read_bitex_fontinfo(fe)
register struct font_entry *fe;
{
/* PLAN
    It would be nice if multiple LW built-in fonts are allowed
    in a single bitex file to make the approximation of TeX font
    more perfect, but I'm not sure if it's worth efforts. (sakurai)
*/
    register FILE *fntfp;
    register struct tfmchar_entry *ce;
    register int i;
    float s;
    int dev_bitex_fontdict();

    openfontfile(fe);
    fntfp = fe->openfile;
    bitexfinfo(fe) = NEW(struct bitexfntinfo, "bitexfont info"); 
    bitexfinfo(fe)->lastfntchar = HALFTFMCHAR;
    getbytes(fntfp, bitexfinfo(fe)->dev_name, LWFSIZE);
    
    s = (float)fe->s;
    for (i = 0, ce = bitexfinfo(fe)->ch; i <= HALFTFMCHAR; i++, ce++) {
	ce->dev_font = fe->k;
	ce->dev_char = getuint(fntfp, 4);
	ce->tfmw = ((float)getuint(fntfp, 4)*s)/(float)(1<<20);
    }
    fe->dev_fontdict = dev_bitex_fontdict;
    dev_tfm_initfe(fe);
}


/* utilities
 */
readtfm(fntfp)
register FILE *fntfp;
{
    fseek(fntfp, 2, 0);		/* lf */
    lh = getuint(fntfp, 2);
    bc = getuint(fntfp, 2);
    ec = getuint(fntfp, 2);
    nw = getuint(fntfp, 2);
    fseek(fntfp, 8*4, 0);
    getbytes(fntfp, header, (lh-2)*4 > JSTFMHEAD ? JSTFMHEAD : (lh-2)*4);
    fseek(fntfp, (6+lh+(ec-bc)+1)*4, 0);
    getbytes(fntfp, width, 4*nw);
    fseek(fntfp, (6+lh)*4, 0);	/* ready to read char_info */
}

readjfm(fntfp)
register FILE *fntfp;
{
    if (getuint(fntfp, 2) != JFM_ID)
	return (FALSE);
    nt = getuint(fntfp, 2);
    fseek(fntfp, 2, 1);		/* lf */
    lh = getuint(fntfp, 2);
    bc = getuint(fntfp, 2);	/* bc should be 0 */
    ec = getuint(fntfp, 2);
    nw = getuint(fntfp, 2);
    fseek(fntfp, (7+lh+nt+(ec-bc)+1)*4, 0);
    getbytes(fntfp, width, 4*nw);
    fseek(fntfp, (7+lh)*4, 0);	/* ready to read char_type and char_info */
    return (TRUE);
}
