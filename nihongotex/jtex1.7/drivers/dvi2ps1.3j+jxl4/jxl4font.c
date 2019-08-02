/* this module have functions for a jxl4 font format */
/* derived rastfont.c and pxlfont.c */

#include	<stdio.h>
#include	"defs.h"
#include	"global.h"
#include	"rastfont.h"  /* for rastchar_entry  */
#include	"jxl4info.h"
#include	"jxl4font.h"

void read_jxl_fontinfo();
struct fontop jxlop = {
    "jxl",
    jxl4type_access,
    read_jxl_fontinfo,
};

#define	NHALFPXLCHARS	128
#define	NJXL4CHARS	(94*94) /* <= NFNTCHARS */

int
jxl4info(fentry)
struct	font_entry *fentry;
{
    int	jxl4_id;
    unsigned int	size;
    int	rsize, i;
    register struct JXL4	*jxl4p;
    FILE *font_fd;

    jxl4p = jxl4finfo(fentry)->jxl4;
    font_fd = fentry->openfile;

    /*
     * Get all tfm-width table entry.
     */

    size = sizeof(struct TFM_TABLE ) * jxl4p->tfm_size;
    rsize = jxl4p->tfm_size;

    if ((jxl4finfo(fentry)->tfmtable = (struct TFM_TABLE *)malloc(size)) == NULL)
	Fatal("tfm_width");	/* exit */

    (void) fseek(font_fd, (long)jxl4p->tfm_ptr, 0);
    for(i=0;i<rsize;i++)
        jxl4finfo(fentry)->tfmtable[i].entry 
             = getuint(font_fd, 4);

/*    (void)fread(font_fd, (char *)jxl4finfo(fentry)->tfmtable, rsize); */

    /*
     * Get X_info & Y_info.
     */

    size = sizeof(struct X_INFO) * jxl4p->x_info_size;
    rsize = jxl4p->x_info_size;

    if((jxl4finfo(fentry)->xinfo = (struct X_INFO *)malloc(size)) == NULL)
	Fatal("x_info");	/* exit */

    (void)fseek(font_fd, (long)jxl4p->x_info_ptr, 0);
    for(i=0;i<rsize;i++) {
        jxl4finfo(fentry)->xinfo[i].x_offset = getint(font_fd, 2);
        jxl4finfo(fentry)->xinfo[i].x_pix    = getint(font_fd, 2);
        jxl4finfo(fentry)->xinfo[i].x_offset_p = getint(font_fd, 2);
    }
/*    (void)fread(font_fd, (char *)jxl4finfo(fentry)->xinfo, rsize); */

    size = sizeof(struct Y_INFO) * jxl4p->y_info_size;
    rsize = jxl4p->y_info_size;

    if ((jxl4finfo(fentry)->yinfo = (struct Y_INFO *)malloc(size)) == NULL)
	Fatal("y_info");	/* exit */

    (void)fseek(font_fd, (long)jxl4p->y_info_ptr, 0);
    for(i=0;i<rsize;i++) {
        jxl4finfo(fentry)->yinfo[i].y_offset = getint(font_fd, 2);
        jxl4finfo(fentry)->yinfo[i].y_pix    = getint(font_fd, 2);
        jxl4finfo(fentry)->yinfo[i].y_offset_p = getint(font_fd, 2);
    }
/*    (void)fread(font_fd, (char *)jxl4finfo(fentry)->yinfo, rsize); */

    /*
     * Get pxl_dir information.
     */

    size = sizeof(struct PXL_DIR) * jxl4p->dir_size;

    if ((jxl4finfo(fentry)->pxldir = (struct PXL_DIR *)malloc(size)) == NULL)
	Fatal("pxl_dir");	/* exit */
}


void
jxl4charinfo(fe)
register struct font_entry *fe;
{
    FILE *jxlfp;
    char    *byte_map, *word_map;
    register struct rastchar_entry *ce;
    struct rastchar_entry tchare;
    int     b_width;
    int    long_ptr;
    int    nchars;
    int    i, code;
    register int    size, h;
    register char   *op, *np;
    int nbytes;
    unsigned int tmp;

    nchars  = jxl4finfo(fe)->nfntchars;
    jxlfp   = fe->openfile;
    for (i = 0; i < nchars; i++) {
       size = i;
       ce = &(jxl4finfo(fe)->ch[i]);
       if (size < 1410) {  /* non kanji */
           long_ptr = jxl4finfo(fe)->jxl4->dir_ptr_0;
       }
       else if (size < 4418) {       /* level-1 kanji */
           size -= 1410;
           long_ptr = jxl4finfo(fe)->jxl4->dir_ptr_1;
       }
       else {        /* level-2 kanji */
           size -= 4418;
           long_ptr = jxl4finfo(fe)->jxl4->dir_ptr_2;
       }

       (void)fseek(jxlfp, (long)(long_ptr + (SIZEof_PXL_DIR * size)), 0);

/*       (void)fread((char *)&(jxl4finfo(fe)->pxldir[i]),sizeof(struct PXL_DIR), 1,  jxlfp); */
       jxl4finfo(fe)->pxldir[i].tfm_index       = getint(jxlfp, 1);
       jxl4finfo(fe)->pxldir[i].rat_index       = getint(jxlfp, 3);
       jxl4finfo(fe)->pxldir[i].x_info_index    = getint(jxlfp, 2);
       jxl4finfo(fe)->pxldir[i].y_info_index    = getint(jxlfp, 2);
       if((tchare.where.fileOffset = jxl4finfo(fe)->pxldir[i].rat_index) == 0)
	   tchare.where.fileOffset = NONEXISTANT;
       ce->where.fileOffset = tchare.where.fileOffset;

       size = (int) jxl4finfo(fe)->pxldir[i].x_info_index;
       tchare.width = ce->width = jxl4finfo(fe)->xinfo[size].x_pix;
       ce->nbpl = (tchare.width + 7) >> 3;
       tchare.xOffset = ce->xOffset = jxl4finfo(fe)->xinfo[size].x_offset;

       size = (int) jxl4finfo(fe)->pxldir[i].y_info_index;
       tchare.height = ce->height = jxl4finfo(fe)->yinfo[size].y_pix;
       tchare.yOffset = ce->yOffset = jxl4finfo(fe)->yinfo[size].y_offset;

       size = (int) jxl4finfo(fe)->pxldir[i].tfm_index;
        tmp = ((float)jxl4finfo(fe)->tfmtable[size].entry *(float)fe->s) /
            (float)(1<<20);

       tchare.tfmw = ce->tfmw = tmp;

    }
}



void
read_jxl_fontinfo(fe)
register struct font_entry *fe;
{
    register FILE *fntfp;
    int t, i, fdir, nchars;
    struct JXL4 *jxl4p;
    register struct rastchar_entry *tcharptr;/* temporary char_entry pointer  */
    struct jxl4fntinfo *rfi;
    int ds, mg;
    int jxl4info();
    int jxl_fontdict();

    openfontfile(fe);
    fntfp = fe->openfile;
    if ((t = getuint(fntfp, 4)) != JXL4ID)
	Fatal("JXL ID = %d, can only process PXL ID = %d files",
	      t, JXL4ID);
    fseek(fntfp, -4L, 2);
    if ((t = getuint(fntfp, 4)) != JXL4ID)
	Fatal("Tail JXL ID = %d, can only process PXL ID = %d files",
	      t, JXL4ID);

    if((jxl4p = (struct JXL4 *)malloc(sizeof(struct JXL4))) == NULL)
	Fatal("can't allocate jxl4 for font"); 
    fseek(fntfp, -64L, 2);
    jxl4p->check_sum = getuint(fntfp, 4);
    jxl4p->magnification = mg = getuint(fntfp, 4);
    jxl4p->design_size = ds = getuint(fntfp, 4);
    jxl4p->fixed_pxl_size = getuint(fntfp, 4);
    jxl4p->fixed_tfm_size = getuint(fntfp, 4);
    jxl4p->tfm_ptr        = getuint(fntfp, 4);
    jxl4p->tfm_size       = getuint(fntfp, 4);
    jxl4p->x_info_ptr     = getuint(fntfp, 4);
    jxl4p->x_info_size    = getuint(fntfp, 4);
    jxl4p->y_info_ptr     = getuint(fntfp, 4);
    jxl4p->y_info_size    = getuint(fntfp, 4);
    jxl4p->dir_size       = getuint(fntfp, 4);
    jxl4p->dir_ptr_0      = getuint(fntfp, 4);
    jxl4p->dir_ptr_1      = getuint(fntfp, 4);
    jxl4p->dir_ptr_2      = getuint(fntfp, 4);

/*  *** jxl4 font format not suport check sum ***

    t = jxl4p->check_sum;
    if ((fe->c != 0) && (t != 0) && (fe->c != t))
	Warning("font = \"%s\",\n-->font checksum = %d,\n-->dvi checksum = %d", 
	fe->name, fe->c, t);
*/
    nchars = jxl4p->dir_size;
    if (nchars > NJXL4CHARS) {
	Fatal("JXL-font dir size of %s is %d\n", fe->name, nchars);
    }/*  else if (nchars != NJXL4CHARS) {
	Warning("JXL-font dir size of %s is %d\n", fe->name, nchars);
    } */

    jxl4finfo(fe) = rfi = alloc_jxl4finfo(nchars, TRUE, jxl4finfo(fe));

    /* rfi->designsize = ds; /* not need */
    /* rfi->font_mag = mg;   /* not need */
    rfi->jxl4 = jxl4p;
    rfi->nfntchars = nchars;
    jxl4info(fe);
    jxl4charinfo(fe);
    fe->dev_fontdict = jxl_fontdict;
    dev_jxl4_initfe(fe);
    free(rfi->pxldir);
    free(rfi->yinfo);
    free(rfi->xinfo);
    free(rfi->tfmtable);
    free(rfi->jxl4);
}

jxl_fontdict(fe, c)
struct font_entry *fe;
int c;
{
    void loadjxlchar();
    int  i;
    i = jis_to_dnf_index(c); /* */
    return (jxl4_fontdict(fe, i, loadjxlchar));
}

void
loadjxlchar(fe, c)
register struct font_entry *fe;
int c;
{
    char *pixel;
    int nbytes;
    int i;
    register struct rastchar_entry *ce;
    FILE *jxlfp;

    int bytewidth, j;
    unsigned char *sl;

    ce = &(jxl4finfo(fe)->ch[c]);
    openfontfile(fe);
    fseek(jxlfp = fe->openfile, (long) ce->where.fileOffset, 0);
    if((pixel = (char *)malloc((unsigned int) ce->width * ce->height)) == NULL )
	Fatal("Unable to allocate memory for char\n");
    jxlexp(jxlfp ,pixel, ce->nbpl, ce->width, ce->height); 
    ce->where.pixptr = pixel;
}

struct jxl4fntinfo *
alloc_jxl4finfo(nchars, init, prfi)
int nchars, init;
struct  passrastfntinfo *prfi;
{
    register struct jxl4fntinfo *rfi;
    register struct rastchar_entry *ce;	/* temporary char_entry pointer  */
    register int i;

    rfi = (struct jxl4fntinfo *)
	  alloc_check(malloc(sizeof(struct jxl4fntinfo)),
		      "jxl4font info");
    rfi->ch = ce  = (struct rastchar_entry *)
          alloc_check(malloc(nchars*sizeof(struct rastchar_entry)),"char entry");
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


/* generic fontdict for jxl4 font */

jxl4_fontdict(fe, c, loadchar)
register struct font_entry *fe;
int c;
void (*loadchar)();
{
    register struct rastchar_entry *ce;
    int bytewidth;

    /* char range check omitted */
    ce = &(jxl4finfo(fe)->ch[c]);
    if (ce->dev_char != NONCHAR)
	return (ce->dev_font);
    if (ce->where.fileOffset == NONEXISTANT) {
	ce->dev_font = NONFONT;
	ce->dev_char = DUMMYCHAR;
	return (NONFONT);
    }
    (*loadchar)(fe, c);
    dev_jxl4_fontdict(fe, c);
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

