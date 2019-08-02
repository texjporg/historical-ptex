#include	<stdio.h>
#include	"file.h"
#include	"defs.h"
#include	"global.h"
#include	"rastfont.h"

#define	PXLID		1001
#define JXL4ID		101157

struct rastfntinfo *
alloc_rastfinfo(nchars, init, prfi)
int nchars, init;
struct  passrastfntinfo *prfi;
{
    register struct rastfntinfo *rfi;
    register struct rastchar_entry *ce;	/* temporary char_entry pointer  */
    register int i;

    rfi = (struct rastfntinfo *)
	  alloc_check(malloc(sizeof(struct rastfntinfo)+
			     (nchars-1)*sizeof(struct rastchar_entry)),
		      "rastfont info");
    if (init)
	for (i = 0; i < nchars; i++) {
	    ce = &(rfi->ch[i]);
	    ce->width = 0;
	    ce->height = 0;
	    ce->xOffset= 0;
	    ce->yOffset = 0;
	    ce->dev_font = NONFONT;
	    ce->dev_char = NONCHAR;
	    ce->where.fileOffset = NONEXISTANT;
	    ce->tfmw = 0;
	}
    rfi->corrfact = prfi->corrfact;
    return (rfi);
}

pktype_access(proto, fe, acca)
char *proto;
struct font_entry *fe;
struct accarg *acca;
{
    return (rast_access(proto, fe, acca, 1));
}

pxltype_access(proto, fe, acca)
char *proto;
struct font_entry *fe;
struct accarg *acca;
{
    if (rast_access(proto, fe, acca, 5))
	return (check_id(fe->name, PXLID));
    else
	return (FALSE);
}

jxl4type_access(proto, fe, acca)
char *proto;
struct font_entry *fe;
struct accarg *acca;
{
    if (rast_access(proto, fe, acca, 5))
	return (check_id(fe->name, JXL4ID));
    else
	return (FALSE);
}

struct passrastfntinfo passrfinfo;

rast_access(proto, fe, acca, t)
char *proto;
register struct font_entry *fe;
register struct accarg *acca;
int t;
{
    fe->finfo = (struct finfo *)&passrfinfo;
    switch (acca->acc_mode) {
    case ACC_EXACT:
	acca->pv_mag = ROUND(acca->actmagfact*resolution*t);
	passrfinfo.corrfact = 1;
	break;
    case ACC_SUBST:
	if (ROUND(acca->actmagfact*resolution*t) != acca->reqmag)
	    return (FALSE);
	acca->pv_mag = acca->submag;
	passrfinfo.corrfact = (float)acca->reqmag/(float)acca->submag;
	break;
    case ACC_MAGSTEP:
	acca->pv_mag = ROUND(acca->stepmagfact*resolution*t);
	passrfinfo.corrfact = acca->rawmagfact/acca->stepmagfact;
	break;
    }
    pave(fe->name, proto, acca);
    return (access(fe->name, R_OK) == 0);
}


/* generic fontdict for raster font */
rast_fontdict(fe, c, loadchar)
register struct font_entry *fe;
int c;
void (*loadchar)();
{
    register struct rastchar_entry *ce;
    int bytewidth;

    /* char range check omitted */
    ce = &(rastfinfo(fe)->ch[c]);
    if (ce->dev_char != NONCHAR)
	return (ce->dev_font);
    if (ce->where.fileOffset == NONEXISTANT) {
	ce->dev_font = NONFONT;
	ce->dev_char = DUMMYCHAR;
	return (NONFONT);
    }
    (*loadchar)(fe, c);
    dev_rast_fontdict(fe, c);
    /* we should really free the space used by the PXL data after this
       point, but it is not large, and besides, we may want to be
       more clever in the future, about sending bitmaps.  So keep
       the data around */
#ifdef STATS
    Snbpxl += (bytewidth = (ce->width + 7) >> 3)*ce->height;
    fe->nbpxl += bytewidth*ce->height;
    Sonbpx += (ce->width*ce->height + 7) >> 3;
    Sndc += 1;
#endif
    return (ce->dev_font);
}


check_id(name, id)
char	*name;
int	id;
{
    FILE *tfp;
    int t;

    tfp = BINARYOPEN(name, "r");
    if ((tfp = BINARYOPEN(name, "r")) == NULL)
	Fatal("FNT file %s could not be opened", name);
    if ((t = getuint(tfp, 4)) != id) {
	fclose(tfp);
	return (FALSE);
    }
    fseek(tfp, -4L, 2);
    if ((t = getuint(tfp, 4)) != id) {
	fclose(tfp);
	return (FALSE);
    }
    fclose(tfp);
    return (TRUE);
}
