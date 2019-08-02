#include	<stdio.h>
#include	"defs.h"
#include	"global.h"
#include	"rastfont.h"

void read_pxl_fontinfo();
struct fontop pxlop = {
    "pxl",
    pxltype_access,
    read_pxl_fontinfo,
};

#define	PXLID		1001

#define	NHALFPXLCHARS	128
#define	NPXLCHARS	256	/* <= NFNTCHARS */

void
read_pxl_fontinfo(fe)
register struct font_entry *fe;
{
    register FILE *fntfp;
    int t, i, fdir, nchars;
    register struct rastchar_entry *tcharptr;/* temporary char_entry pointer  */
    struct rastfntinfo *rfi;
    int ds, mg;
    int pxl_fontdict();

    openfontfile(fe);
    fntfp = fe->openfile;
    if ((t = getuint(fntfp, 4)) != PXLID)
	Fatal("PXL ID = %d, can only process PXL ID = %d files",
	      t, PXLID);
    fseek(fntfp, -20, 2);
    t = getuint(fntfp, 4);
    if ((fe->c != 0) && (t != 0) && (fe->c != t))
	Warning("font = \"%s\",\n-->font checksum = %d,\n-->dvi checksum = %d",
		fe->name, fe->c, t);
    mg = getuint(fntfp, 4);
    ds = getuint(fntfp, 4);
    
    t = ftell(fntfp) - 12;
    fseek(fntfp, fdir = getuint(fntfp, 4) * 4, 0);
    nchars = (t-fdir)/(4*4);
    if (nchars > NPXLCHARS) {
	Fatal("PXL-font dir size of %s is %d\n", fe->name, nchars);
    } else if (nchars != NHALFPXLCHARS && nchars != NPXLCHARS) {
	Warning("PXL-font dir size of %s is %d\n", fe->name, nchars);
    }

    rastfinfo(fe) = rfi = alloc_rastfinfo(nchars, FALSE, rastfinfo(fe));
    /* rfi->designsize = ds; */
    /* rfi->magnification = mg; */
    rfi->nfntchars = nchars;
    for (i = 0; i < nchars; i++) {
	tcharptr = &(rfi->ch[i]);
	tcharptr->width = getuint(fntfp, 2);
	tcharptr->height = getuint(fntfp, 2);
	tcharptr->xOffset = getint(fntfp, 2);
	tcharptr->yOffset = getint(fntfp, 2);
	tcharptr->dev_char = NONCHAR; 
	tcharptr->nbpl = ((tcharptr->width + 31) >> 5) * 4;
	if ((tcharptr->where.fileOffset = getuint(fntfp, 4) * 4) == 0)
	    tcharptr->where.fileOffset = NONEXISTANT;
	tcharptr->tfmw = ((float)getuint(fntfp, 4)*(float)fe->s) /
                                      (float)(1<<20);

    }
    fe->dev_fontdict = pxl_fontdict;
    dev_rast_initfe(fe);
}

pxl_fontdict(fe, c)
struct font_entry *fe;
int c;
{
    void loadpxlchar();
    return (rast_fontdict(fe, c, loadpxlchar));
}

void
loadpxlchar(fe, c)
register struct font_entry *fe;
int c;
{
    char *pixel;
    int nbytes;
    register struct rastchar_entry *ce = &(rastfinfo(fe)->ch[c]);
    FILE *pxlfp;

    openfontfile(fe);
    fseek(pxlfp = fe->openfile, ce->where.fileOffset, 0);
    nbytes = ce->nbpl * ce->height;
    if((pixel = (char *)malloc(nbytes)) == NULL )
	Fatal("Unable to allocate memory for char\n");
    fread(pixel, 1, nbytes, pxlfp);
    ce->where.pixptr = pixel;
}
