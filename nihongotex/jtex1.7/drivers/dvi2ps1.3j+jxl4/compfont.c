#include	<stdio.h>
#include	<ctype.h>
#include	"defs.h"
#include	"global.h"
#include	"compfont.h"

void read_jcomp_fontinfo();
int jcomp_access();
struct fontop jcompop = {
    "",
    jcomp_access,
    read_jcomp_fontinfo,
};

jcomp_access(proto, fe, acca, subop)
char *proto;
struct font_entry *fe;
struct accarg *acca;
struct fontop *subop;
{
    register char *n, *p;
    char fam[STRSIZE], dsize[STRSIZE];
    register struct jcompfntinfo *jfi;
    int ds;

    for (n = fe->n, p = fam; isalpha(*n);)
	*p++ = *n++;
    *p = '\0';
    for (p = dsize; isdigit(*n);)
	*p++ = *n++;
    *p = '\0';
    acca->pv_fam = fam;
    acca->pv_ds = ds = atoi(dsize);
    acca->pv_jsub = jsubfontname(JSF_HIRA);
    if ((*(subop->fo_access))(proto, fe, acca)) {
	acca->pv_jsub = "JXX";
	pave(fe->name, proto, acca);
	jcompfinfo(fe) = jfi = NEW(struct jcompfntinfo, "jfont info");
	jfi->acca = *acca;
/*
	jfi->acca.acc_mode = acca->acc_mode;
	jfi->acca.rawmagfact = acca->rawmagfact;
	jfi->acca.actmagfact = acca->actmagfact;
	jfi->acca.submag = acca->submag;
	jfi->acca.reqmag = acca->reqmag;
	jfi->acca.stepmagfact = acca->stepmagfact;
*/
	jfi->acca.pv_name = fe->n;
/*
	jfi->acca.pv_mag = acca->pv_mag;
*/
	jfi->acca.pv_fam = strsave(fam);
	jfi->acca.pv_ds = ds;
	jfi->subop = subop;
	jfi->proto = proto;
	return (TRUE);
    }
    return (FALSE);
}

void
read_jcomp_fontinfo(fe)
register struct font_entry *fe;
{
    struct jcompfntinfo *jfi;
    register struct font_entry *js;
    int f;
    int jcomp_fontdict(), jcomp_setchar(), jcomp_setstring();

    jfi = jcompfinfo(fe);
    for (js = jfi->jsubs, f = 0; f < NJSUBS; js++, f++) {
	/* js->k is assigned in jcomp_fontdict */
	js->c = fe->c;
	js->s = fe->s;
	js->d = fe->d;
	strcpy(js->n, fe->n);
	js->name[0] = '\0';
	js->openfile = NO_FILE;
    }
    fe->dev_fontdict = jcomp_fontdict;
    fe->dev_setchar = jcomp_setchar;
    fe->dev_setstring = jcomp_setstring;
}

jcomp_fontdict(fe, code)
struct font_entry *fe;
int code;
{
    struct jcompfntinfo *jfi;
    register struct font_entry *js;
    unsigned short f, c;
    int null_fontdict(), null_setchar(), null_setstring();

    compute_fc(code>>8, code&0xff, &f, &c);
    js = (jfi = jcompfinfo(fe))->jsubs+f-1;
    if (js->name[0] == '\0') {
	js->k = dev_nextjcompfont();
	jfi->acca.pv_jsub = jsubfontname(f);
	if ((*(jfi->subop->fo_access))(jfi->proto, js, &(jfi->acca))) {
	    (*(jfi->subop->fo_readfontinfo))(js);
	} else {
	    Warning("No jsubfont %s", js->name);
	    js->dev_fontdict = null_fontdict;
	    js->dev_setchar = null_setchar;
	    js->dev_setstring = null_setstring;
	}
    }
    return (js->dev_fontdict(js, c));
}

jcomp_setchar(code)
int code;
{
    struct font_entry *savefe;
    unsigned short f, c;
    int cw;

    compute_fc(code>>8, code&0xff, &f, &c);
    savefe = fontptr;
    cw = (*((fontptr = jcompfinfo(fontptr)->jsubs+f-1)->dev_setchar))(c);
    fontptr = savefe;
    return (cw);
}

jcomp_setstring(s, len)
char *s;
int len;
{
    Fatal("dvi2ps implementation error: jcomp_setstring");
}
