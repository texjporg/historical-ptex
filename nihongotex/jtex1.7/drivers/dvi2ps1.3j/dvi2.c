#include	"commands.h"
#include	<sys/param.h>
#include	<signal.h>
#include	<stdio.h>
#include	"defs.h"
#include	"global.h"

void ReadPreAmble();
void ReadPostAmble();
void FindPostAmblePtr();
void GetFontDef();
void SkipFontDef();
void SetFntNum();
/*void MoveDown();*/
/*void MoveOver();*/
void SetRule();
void SetChar();
void SetString();

#define	MoveDown(a)	v += a
#define	MoveOver(b)	h += b

/**********************************************************************/
/*************************  Global Variables  *************************/
/**********************************************************************/

int   FirstPage = -1000000;     /* first page to print (uses count0)   */
int   LastPage = 1000000;       /* last page to print                    */

int   h;			/* current horizontal position              */
int   v;			/* current vertical position                */
int   den;			/* denominator specified in preamble        */
int   mag;			/* magnification specified in preamble      */
int   num;			/* numerator specified in preamble          */
int   usermag = 0;              /* user specified magnification */

struct font_entry *fontptr;	/* font_entry pointer			*/
struct font_entry *hfontptr = NULL; /* font_entry pointer		*/
int (*fontdict)();
int (*setchar)();
int (*setstring)();
char *fontdescfile;		/* Fontdesc file name			*/

long  postambleptr;		/* Pointer to the postamble		*/
long  ppagep;	                /* previous page pointer		*/

char filename[PATHLEN];		/* DVI file name			*/
FILE *dvifp  = NULL;		/* DVI file pointer			*/
char rootname[PATHLEN];		/* DVI filename without extension	*/
FILE  *outfp = NULL;            /* output file				*/

char *Ifile[100];		/* files to include			*/
int   ncopies = 1;              /* number of copies to print		*/
int   ndone = 0;                /* number of pages converted		*/
int   nif = 0;                  /* number of files to include		*/

#ifdef DEBUG
int Debug = 0;
#endif
int   PreLoad = TRUE;		/* preload the font descriptions?	     */
int   Reverse = TRUE;		/* process DVI pages in reverse order ? */
int   G_create = FALSE;         /* create an output file in spool area ?   */
int   G_errenc = FALSE;	        /* has an error been encountered?          */
int   G_header = TRUE;          /* copy header file to output?             */
char  G_Logname[PATHLEN];       /* name of log file, if created            */
int   G_interactive = FALSE;    /* is the program running interactively    */
				/* (i.e., standard output not redirected)? */
int   G_logging = 0;            /* Are we logging warning messages?        */
int   G_logfile = FALSE;        /* Are these messages going to a log file? */
FILE *G_logfp;                  /* log file pointer (for errors)           */
char *G_progname;		/* program name                            */
int   G_quiet = FALSE;          /* for quiet operation                     */
int   G_nowarn = FALSE;         /* don't print out warnings                */
#ifdef STATS
int   Stats = FALSE;          /* are we reporting stats ?                 */
int   Snbpxl = 0;             /* # of bytes of pixel data                 */
int   Sonbpx = 0;             /* "optimal" number of bytes of pixel data  */
int   Sndc = 0;               /* # of different characters typeset        */
int   Stnc = 0;               /* total # of chars typeset                 */
int   Snbpx0, Sndc0, Stnc0;   /* used for printing incremental changes per dvi page */
#endif


/**********************************************************************/
/*******************************  main  *******************************/
/**********************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
    struct stack_entry {  /* stack entry */
	int h, v, w, x, y, z;  /* what's on stack */
    };

    int command;	    /* current command			     */
    int count[10];          /* the 10 counters at begining of each page */
    long cpagep;	    /* current page pointer		     */
    int  Emitting = FALSE;  /* are we outputting typsetting instructions? */
    register int i;	    /* command parameter; loop index	     */
    int k;		    /* temporary parameter		     */
    char n[STRSIZE];	    /* command parameter		     */
    int PassNo = 0;         /* which pass over the DVI page are we on?  */
    int SkipMode = FALSE;   /* in skip mode flag                     */
    int sp;		    /* stack pointer			     */
    struct stack_entry stack[STACKSIZE];   /* stack		     */
    int t;		    /* temporary			     */
    char SpecialStr[STRSIZE]; /* "\special" strings                  */
    int val, val2;          /* temporarys to hold command information*/
    int w;		    /* current horizontal spacing	     */
    int x;		    /* current horizontal spacing	     */
    int y;		    /* current vertical spacing		     */
    int z;		    /* current vertical spacing		     */

    G_progname = argv[0];
    if ((fontdescfile = getenv(FONTDESCENV)) == NULL)
	fontdescfile = FONTDESC;
    DecodeArgs(argc, argv);
    read_fontdesc(fontdescfile);

    if ((i = getuint(dvifp, 1)) != PRE)
	Fatal("PRE doesn't occur first--are you sure this is a DVI file?");
    if ((i = getint(dvifp, 1)) != DVIFORMAT)
	Fatal("DVI format = %d, can only process DVI format %d files",
	      i, DVIFORMAT);

    outfp = stdout;

    dev_init();			/* output prologue and setup */

    if (Reverse) {
	ReadPostAmble(PreLoad);
	fseek(dvifp, ppagep, 0);
    } else {
	if (PreLoad) {
	    ReadPostAmble(TRUE);
	    fseek(dvifp, (long) 14, 0);
	} else
	    ReadPreAmble();
	k = getuint(dvifp, 1);
	getbytes(dvifp, n, k);
    }
    dev_conv(num, den, mag);

    PassNo = 0;
    while (TRUE)
	switch (command=getuint(dvifp, 1))  {

	case SET1:case SET2:case SET3:case SET4:
	    val = getuint(dvifp, command-SET1+1);
	    if (!SkipMode) SetChar(val, PassNo, TRUE);
	    break;

	case SET_RULE:
	    val = getuint(dvifp, 4);
	    val2 = getuint(dvifp, 4);
	    if (Emitting) SetRule(val, val2, 1);
	    break;

	case PUT1:case PUT2:case PUT3:case PUT4:
	    val = getuint(dvifp,command-PUT1+1);
	    if (!SkipMode) SetChar(val, PassNo, FALSE);
	    break;

	case PUT_RULE:
	    val = getuint(dvifp, 4);
	    val2 = getuint(dvifp, 4);
	    if (Emitting) SetRule(val, val2, 0);
	    break;

	case NOP:
	    break;

	case BOP:
	    cpagep = ftell(dvifp) - 1;
	    for (i=0; i<=9; i++)
		count[i] = getuint(dvifp, 4);
	    ppagep = getuint(dvifp, 4);

	    h = v = w = x = y = z = 0;
	    sp = 0;
	    fontptr = NULL;
	    dev_initpage();

	    if (count[0] < FirstPage || count[0] > LastPage)
		SkipMode = TRUE;
	    else
		SkipMode = FALSE;

	    Emitting = (PassNo != 0) && !SkipMode;

	    if (!SkipMode) {
		if (PassNo == 0) {
		    dev_bop0(count[0]);
#ifdef STATS
		    if (Stats) {
			Sndc0 = Sndc;
			Stnc0 = Stnc;
			Snbpx0 = Snbpxl;
		    }
#endif
		    if (!G_quiet)
			fprintf(stderr, "[%d", count[0]);
		} else
		    dev_bop1();
	    }
	    break;

	case EOP:
	    if (!SkipMode) {
		if (PassNo == 0) {	/* start second pass on current page */
		    fseek(dvifp,cpagep,0);
		    PassNo = 1;
		} else {	/* end of second pass, and of page processing */
		    dev_eop();
#ifdef STATS
		    if (Stats)
			fprintf(stderr," - %d total ch,  %d diff ch,  %d pxl bytes]\n",
				Stnc-Stnc0, Sndc-Sndc0, Snbpxl-Snbpx0);
		    else
#endif
		    if (!G_quiet) {
			fprintf(stderr,"] ");
			if ((++ndone % 10) == 0) fprintf(stderr,"\n");
			fflush(stderr);
		    }
		    PassNo = 0;
		}
	    } else
		PassNo = 0;
	    if (PassNo == 0 && Reverse) {
		if (ppagep > 0)
		    fseek(dvifp, ppagep, 0);
		else
		    AllDone();
	    }
	    break;

	case PUSH:
	    if (sp >= STACKSIZE)
		Fatal("stack overflow");
	    stack[sp].h = h;
	    stack[sp].v = v;
	    stack[sp].w = w;
	    stack[sp].x = x;
	    stack[sp].y = y;
	    stack[sp].z = z;
	    sp++;
	    break;

	case POP:
	    --sp;
	    if (sp < 0)
		Fatal("stack underflow");
	    h = stack[sp].h;
	    v = stack[sp].v;
	    w = stack[sp].w;
	    x = stack[sp].x;
	    y = stack[sp].y;
	    z = stack[sp].z;
	    break;

	case RIGHT1:case RIGHT2:case RIGHT3:case RIGHT4:
	    val = getint(dvifp,command-RIGHT1+1);
	    if (Emitting) MoveOver(val);
	    break;

	case W0:
	    if (Emitting) MoveOver(w);
	    break;

	case W1:case W2:case W3:case W4:
	    w = getint(dvifp,command-W1+1);
	    if (Emitting) MoveOver(w);
	    break;

	case X0:
	    if (Emitting) MoveOver(x);
	    break;

	case X1:case X2:case X3:case X4:
	    x = getint(dvifp,command-X1+1);
	    if (Emitting) MoveOver(x);
	    break;

	case DOWN1:case DOWN2:case DOWN3:case DOWN4:
	    val = getint(dvifp,command-DOWN1+1);
	    if (Emitting) MoveDown(val);
	    break;

	case Y0:
	    if (Emitting) MoveDown(y);
	    break;

	case Y1:case Y2:case Y3:case Y4:
	    y = getint(dvifp,command-Y1+1);
	    if (Emitting) MoveDown(y);
	    break;

	case Z0:
	    if (Emitting) MoveDown(z);
	    break;

	case Z1:case Z2:case Z3:case Z4:
	    z = getint(dvifp,command-Z1+1);
	    if (Emitting) MoveDown(z);
	    break;

	case FNT1:case FNT2:case FNT3:case FNT4:
	    k = getuint(dvifp, command-FNT1+1);
	    if (!SkipMode)
		SetFntNum(k, Emitting);
	    break;

	case XXX1:case XXX2:case XXX3:case XXX4:
	    k = getuint(dvifp,command-XXX1+1);
	    getbytes(dvifp, SpecialStr, k);
	    if (Emitting) dev_dospecial(SpecialStr, k);
	    break;

	case FNT_DEF1:case FNT_DEF2:case FNT_DEF3:case FNT_DEF4:
	    k = getuint(dvifp, command-FNT_DEF1+1);
	    if (PreLoad || HasBeenRead(k))
		SkipFontDef(k);
	    else
		ReadFontDef(k);
	    break;

	case PRE:
	    Fatal("PRE occurs within file");
	    break;

	case POST:
	    AllDone();
	    break;

	case POST_POST:
	    Fatal("POST_POST with no preceding POST");
	    break;

	default:
	    if (command >= FONT_00 && command <= FONT_63) {
		if (!SkipMode)
		     SetFntNum(command - FONT_00, Emitting);
	    } else if (command >= SETC_000 && command <= SETC_127) {
		if (!SkipMode) SetString(command, PassNo);
	    } else
		Fatal("%d is an undefined command", command);
	    break;

	}

}


void
ReadPreAmble()
{
    num = getuint(dvifp, 4);
    den = getuint(dvifp, 4);
    mag = getuint(dvifp, 4);
#ifdef USEGLOBALMAG
    if (usermag > 0 && usermag != mag)
	fprintf(stderr,
		"DVI magnification of %d over-ridden by user mag of %d\n",
		mag, usermag);
#endif
    if (usermag > 0) mag = usermag;
#ifndef USEGLOBALMAG
    if (mag != 1000) fprintf(stderr, "Magnification of %d ignored.\n", mag);
#endif
}

void
ReadPostAmble(load)
int     load;
/***********************************************************************
    This  routine  is  used  to  read  in  the  postamble  values.    It
    initializes the magnification and checks  the stack height prior  to
    starting printing the document.
***********************************************************************/
{
    FindPostAmblePtr(&postambleptr);
    if (getuint(dvifp, 1) != POST)
	Fatal("POST missing at head of postamble");
#ifdef DEBUG
    if (Debug) fprintf(stderr, "got POST command\n");
#endif
    ppagep = getuint(dvifp, 4);
    num = getuint(dvifp, 4);
    den = getuint(dvifp, 4);
    mag = getuint(dvifp, 4);
#ifdef USEGLOBALMAG
    if (usermag > 0 && usermag != mag)
	fprintf(stderr,
		"DVI magnification of %d over-ridden by user mag of %d\n",
		mag, usermag);
#endif
    if (usermag > 0) mag = usermag;
#ifndef USEGLOBALMAG
    if (mag != 1000) fprintf(stderr, "Magnification of %d ignored.\n", mag);
#endif

    getuint(dvifp, 4);	/* height-plus-depth of tallest page */
    getuint(dvifp, 4);	/* width of widest page */
    if (getuint(dvifp, 2) >= STACKSIZE)
	Fatal("Stack size is too small");
    getuint(dvifp, 2);	/* this reads the number of pages in */
    /* the DVI file */
#ifdef DEBUG
    if (Debug) fprintf(stderr, "now reading font defs");
#endif
    if (load) GetFontDef();
}

void
FindPostAmblePtr(postambleptr)
long *postambleptr;
/* this routine will move to the end of the file and find the start
    of the postamble */
{
    int i;

    fseek(dvifp, (long) 0, 2);   /* goto end of file */
    *postambleptr = ftell (dvifp) - 4;
    fseek(dvifp, *postambleptr, 0);

    while (TRUE) {
	fseek(dvifp, --(*postambleptr), 0);
	if (((i = getuint(dvifp, 1)) != 223) && (i != DVIFORMAT))
	    Fatal("Bad end of DVI file");
	if (i == DVIFORMAT)
	    break;
    }
    fseek(dvifp, (*postambleptr) - 4, 0);
    (*postambleptr) = getuint(dvifp, 4);
    fseek(dvifp, *postambleptr, 0);
}


/*-->GetFontDef*/
/**********************************************************************/
/**************************** GetFontDef  *****************************/
/**********************************************************************/

void
GetFontDef()
/***********************************************************************
   Read the font  definitions as they  are in the  postamble of the  DVI
   file.
***********************************************************************/
{
    char str[50], *calloc ();
    unsigned char byte;
    int i, fnamelen;

    while (((byte = getuint(dvifp, 1)) >= FNT_DEF1) &&
	(byte <= FNT_DEF4)) {
	switch (byte) {
	case FNT_DEF1:
	    ReadFontDef(getuint(dvifp, 1));
	    break;
	case FNT_DEF2:
	    ReadFontDef(getuint(dvifp, 2));
	    break;
	case FNT_DEF3:
	    ReadFontDef(getuint(dvifp, 3));
	    break;
	case FNT_DEF4:
	    ReadFontDef(getuint(dvifp, 4));
	    break;
	default:
	    Fatal("Bad byte value in font defs");
	    break;
	}
    }
    if (byte != POST_POST)
	Fatal("POST_POST missing after fontdefs");
}


/*-->ReadFontDef*/
/**********************************************************************/
/****************************  ReadFontDef  ***************************/
/**********************************************************************/

int
ReadFontDef(k)
int k;
{
    register struct font_entry *fe;	/* temporary font_entry pointer */

    fe = NEW(struct font_entry, "font_entry");
    fe->next = hfontptr;
    fe->openfile = NO_FILE;
    fe->ncdl = 0;
#ifdef STATS
    fe->nbpxl = 0;
    fe->ncts = 0;
#endif

    fe->k = k;
    fe->c = getuint(dvifp, 4); /* checksum */
    fe->s = getuint(dvifp, 4); /* space size */
    fe->d = getuint(dvifp, 4); /* design size */
    fe->a = getuint(dvifp, 1); /* area length for font name */
    fe->l = getuint(dvifp, 1); /* device length */
    getbytes(dvifp, fe->n, fe->a+fe->l);
    fe->n[fe->a+fe->l] = '\0';

    read_fontinfo(fe);

    fontptr = hfontptr = fe;
    fontdict = fe->dev_fontdict;
    setchar = fe->dev_setchar;
    setstring = fe->dev_setstring;
}


/*-->SkipFontDef*/
/**********************************************************************/
/****************************  SkipFontDef  ***************************/
/**********************************************************************/

void
SkipFontDef(k)
int k;
{
    int a, l;
    char n[STRSIZE];

    getuint(dvifp, 4);
    getuint(dvifp, 4);
    getuint(dvifp, 4);
    a = getuint(dvifp, 1);
    l = getuint(dvifp, 1);
    getbytes(dvifp, n, a+l);
}


/*-->SetFntNum*/
/**********************************************************************/
/****************************  SetFntNum  *****************************/
/**********************************************************************/

void
SetFntNum(k, Emitting)
int k, Emitting;
/*  this routine is used to specify the font to be used in printing future
    characters */
{
    register struct font_entry *ptr;

    ptr = hfontptr;
    while ((ptr!=NULL) && (ptr->k!=k))
	ptr = ptr->next;
    if (ptr == NULL)
	Fatal("font %d undefined", k);
    fontptr = ptr;
    fontdict = ptr->dev_fontdict;
    setchar = ptr->dev_setchar;
    setstring = ptr->dev_setstring;
}


/*-->HasBeenRead*/
/**********************************************************************/
/***************************  HasBeenRead  ****************************/
/**********************************************************************/

int
HasBeenRead(k)
int k;
{
    register struct font_entry *ptr;

    ptr = hfontptr;
    while ((ptr!=NULL) && (ptr->k!=k))
	ptr = ptr->next;
    return (ptr != NULL);
}


/*-->SetRule*/
/**********************************************************************/
/*****************************  SetRule  ******************************/
/**********************************************************************/

void
SetRule(a, b, Set)	/* this routine will draw a rule */
int a, b;
BOOLEAN Set;
{
    if (a > 0 && b > 0) {
	dev_setposn(h, v);	/* lower left corner */
	dev_setrule(a, b);
    }
    if (Set)
	h += b;
}


/*-->SetChar*/
/**********************************************************************/
/*****************************  SetChar  ******************************/
/**********************************************************************/

void
SetChar(c, PassNo, Move)
int c, PassNo;
BOOLEAN Move;
{
    register int devf, cw;

    devf = (*fontdict)(fontptr, c);
    if (PassNo == 0) return;

    if (devf != NONFONT) {	/* ignore missing fonts */
	dev_setfont(devf);
	dev_setposn(h, v);
	cw = (*setchar)(c);
	if (Move)
	    h += cw;
    } else
	dev_setposn(h, v);

#ifdef STATS
    Stnc += 1;
    fontptr->ncts += 1;
#endif
}


/*-->SetString*/
/**********************************************************************/
/*****************************  SetString  ****************************/
/**********************************************************************/

#define	STRLEN		256
void
SetString(firstch, PassNo)	/* read and set a consecutive string of chars */
int firstch, PassNo;
{
    char s[STRSIZE];
    register char *sp;
    register int c, len;
    int devf;

    /* read entire string of chars */
    /* ensure that all characters are loaded, */
    devf = (*fontdict)(fontptr, firstch);
    for (c = firstch, sp = s, len = 0;
	 c >= SETC_000 && c <= SETC_127 && len < STRLEN; len++) {
	if (devf != (*fontdict)(fontptr, c))
	    break;
	*sp++ = c;
	c = getuint(dvifp, 1);
    }
    fseek(dvifp, -1, 1);	/* backup one character */
    if (PassNo == 0) return;

    /* NULL's are valid chars, so cant use for string termination */
    if (devf != NONFONT) {	/* ignore missing fonts */
	dev_setfont(devf);
	dev_setposn(h, v);
	h += (*setstring)(s, len);
    } else
	dev_setposn(h, v);

#ifdef STATS
    Stnc += len;
    fontptr->ncts += len;
#endif
}
