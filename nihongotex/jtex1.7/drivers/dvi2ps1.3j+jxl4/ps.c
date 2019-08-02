/*
 * DEVICE dependent, font independent
 */

#include	<stdio.h>
#include	"defs.h"
#include	"emit.h"
#include	"global.h"
#include	"ps.h"

/* define for "optimal" relative postioning, rather
   than absolute.  Relative can reduce size of postcript
   output 20% (and reduce print time by almost as much */
#define	USERELPOS	1

#ifndef	RESOLUTION
#define	RESOLUTION	300
#endif

int resolution = RESOLUTION;
int hconvresolution, vconvresolution;

char *PScmd[100];               /* PostScript commands to send */
int nps = 0;			/* number of PostScript commands to send */

int ordinal = 0;		/* page's position in the page sequence */

int hconv, vconv;		/* converts DVI units to pixels */
int ps_h = 0;			/* current h on device */
int ps_v = 0;			/* current v on device */

int cur_psf;
char psfnamebuf[256];
int devicefont = 0;
int jcompfont = -1;

/*
 * >= 0		dvi
 * -1		NONFONT
 * -2,-4,...	pack, jstfm
 * -3,-5,...	jcomp
 */
char *
psfname(psf)
int psf;
{
    if (psf >= 0)
	sprintf(psfnamebuf, "f%d", psf);
    else if (psf%2 == 0)
	sprintf(psfnamebuf, "F%d", -psf/2-1);
    else
	sprintf(psfnamebuf, "JF%d", -psf/2-1);
    return (psfnamebuf);
}

dev_nextdevfont()
{
    return (devicefont -= 2);
}

dev_nextjcompfont()
{
    return (jcompfont -= 2);
}

dev_arg(option, c)
char option;
char *c;
{
    switch (option) {
    case 'R':
	resolution = atoi(c);
	break;
    case 'o':
	if (ChkOpt(c))
	    PScmd[nps++] = c;
	else
	    Fatal("%s is an invalid option\n", c);
	break;
    }
}

/*-->ChkOpt*/   /* check a user supplied option for validity */
#define ISOPT(s) if (strcmp(str,s)==0) return (TRUE)

ChkOpt(str)
char *str;
{
    ISOPT("note");	/* its a shame to build this into the program */
    ISOPT("letter");
    ISOPT("letternote");
    ISOPT("legal");
    ISOPT("landscape");
    ISOPT("manualfeed");
    ISOPT("envelope");
    ISOPT("a3");
    ISOPT("a4");
    ISOPT("b4");
    ISOPT("b5");
    ISOPT("a3landscape");
    ISOPT("a4landscape");
    ISOPT("b4landscape");
    ISOPT("b5landscape");
    ISOPT("letterlandscape");
    ISOPT("notelandscape");
    ISOPT("legallandscape");
    return (FALSE);
}

dev_init()
{
    int i;

    EMIT(outfp, "%%!PS-Adobe-2.0\n");
    EMIT(outfp, "%%%%Creator: %s\n", G_progname);
    EMIT(outfp, "%%%%Title: %s\n", filename);
    EMIT(outfp, "%%%%Pages: (atend)\n");
    EMIT(outfp, "%%%%EndComments\n");
    if (G_header)
	ps_copyfile(HDRFILE);
    for (i = 0; i < nif; i++)	/* copy all included files */
	ps_copyfile(Ifile[i]);
    EMIT(outfp, "%%%%EndProlog\n");

    hconvresolution = resolution;
    vconvresolution = resolution;
    EMIT(outfp, "%%%%BeginSetup\n");
    EMIT(outfp, "TeXDict begin\n");
    EMIT(outfp, "/Resolution %d def\n", resolution);
    EMIT(outfp, "START\n");
    for (i = 0; i < nps; i++)	/* prefix valid PostScript options with a "@" */
	EMIT(outfp, "@%s\n", PScmd[i]);
    if (ncopies > 1)
	EMIT(outfp, "%d COPIES\n", ncopies);
    /* include file given in fontdesc */
    do_include(ps_copyfile);
    EMIT(outfp, "%%%%EndSetup\n");
}

dev_finish()
{
    end_string();
    EMIT(outfp,"%%%%Trailer\n");
    EMIT(outfp,"%%%%Pages: %d\n", ordinal);
    EMIT(outfp,"END\n");
}

void
ps_copyfile(str)
char *str;
{
    EMIT(outfp, "%%%%BeginFile: %s\n", str);
    copyfile(str);
    EMIT(outfp, "%%%%EndFile\n");
}

dev_conv(num, den, mag)
int num, den, mag;
{
    hconv = DoConv(num, den, mag, hconvresolution);
    vconv = DoConv(num, den, mag, vconvresolution);
}

float
dev_fontmag(fe)
struct font_entry *fe;
{
    return (((float)fe->s/(float)fe->d)
#ifdef USEGLOBALMAG
	    * ((float)mag/1000.0)
#endif
	    );
}

dev_initpage()
{
    ps_h = ps_v = 0;
    cur_psf = NONFONT;
}

dev_bop0(count)
int count;
{
    EMIT(outfp, "\n%%%%Page: %d %d\n", count, ++ordinal);
}

dev_bop1()
{
    end_string();
    EMIT(outfp, "BP\n");
}

dev_eop()
{
    end_string();
    EMIT(outfp, "EP\n");
}

dev_setfont(psf)
register int psf;
{
    if (cur_psf != psf) {
	end_string();
	EMIT(outfp, "%s SF\n", psfname(psf));
	cur_psf = psf;
    }
}

dev_setposn(x, y)	/* output a positioning command */
int x, y;
{
    int rx, ry;
#ifdef USERELPOS
    if (y == ps_v) {	/* use relative movement if just moving horizontally */
	if (x != ps_h) {
	    if ((rx=pixround(x-ps_h,hconv)) != 0) {
		end_string();
		EMITN(rx);
		EMITS("r ");
		ps_h += rx*hconv;
	    }
	}
    } else {
#endif
	end_string();
	EMITN(rx=pixround(x,hconv));
	EMITN(ry=pixround(y,vconv));
	EMITS("p ");
	ps_h = rx*hconv;	/* must know where device "really" is horizontally, for rel. posning. */
	ps_v = y;		/* but we always use direct positioning for vertical movement */
#ifdef USERELPOS
    }
#endif
}

dev_setrule(a, b)
int a, b;
{
    end_string();
    EMITN(pixround(b,hconv));	/* width */
    EMITN(pixround(a,vconv));	/* height */
    EMITS("ru\n");
}

int instring = FALSE;

begin_string()
{
    if (!instring) {
	instring = TRUE;
	EMITC('(');
    }
}

end_string()
{
    if (instring) {
	instring = FALSE;
	EMITS(")s\n");
    }
}

/*-->DoConv*/
/*********************************************************************/
/********************************  DoConv  ***************************/
/*********************************************************************/
/*
	1 sp = (num/den)*10^(-5) cm
	1 inch = 254*10^(-2) cm
	Therefore, 1 sp = conv pixel-unit
		   return-value sp = 1 pixel-unit
*/

int DoConv(num, den, mag, convResolution)
int num, den, mag, convResolution;
{
    register float conv;
    conv = ((float)num/(float)den) *
#ifdef USEGLOBALMAG
/*	ActualFactor(mag) * why was this in as Actual Factor?  jls */
	((float) mag/1000.0) *
#endif
	((float)convResolution/254000.0);
    return((int) (1.0 / conv + 0.5));
}

int
pixround(x, conv)	/* return rounded number of pixels */
register int x;		/* in DVI units     */
int conv;		/* conversion factor */
{
    return((int)((x + (conv >> 1)) / conv));
}
