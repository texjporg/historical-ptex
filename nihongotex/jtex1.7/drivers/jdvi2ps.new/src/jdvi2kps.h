/**********************************************************************/
/************************  Global Definitions  ************************/
/**********************************************************************/

#define	ERRCOUNT	False	/* has an error been encounted */
#define	HEADER		True	/* default header file output (True : output)*/
#define	LOGFILE		False	/* make log file ? (False : stderr)*/
#define	QUIET		False	/* changing page number output ?
					(False : output) */
#define	NOWARN		False	/* print warning ? (False : not output) */
#define	DEBUGMODE	False	/* print changing status ?
					(False : not output) */
#define	FIRSTPAGE	-1000000/* first page to print */
#define	LASTPAGE	1000000	/* last page to print */
#define	PRELOAD		True	/* preload the font descriptions ? */
#define	REVERSE		True	/* process DVI pages in reverse order ? */
#define	USERMAG		0	/* not 0 use usermagnification */
#define	N_COPIES	1	/* default copies */

#define	DEFAULTRESOLUTION 300	/* can be changed with [-d integer] */
				/* usually the same, but you can get */
				/* interesting effects if they are not */

#ifndef PS_PATH
#define	PS_PATH		"."
#endif

#define	STRSIZE		256
#define	MIN_N_COPIES	0
#define	MAX_N_COPIES	65535

#define	ERRMSG	"[-d resolution] [-h] [-i file] [-n n] [-o option] [-p] [-r] [-s] [-f n] [-t n]  [-m{0|h|1|2|3|4}] dvifile\n\n"

#define	USEGLOBALMAG 1	/* when defined, the dvi global */
 			/*   magnification is applied   */
      
#ifndef	HDRFILE
#define HDRFILE		"/usr/lib/tex/jtex.ps"
#endif

				/* should be yet another switch option (?) */
/**********************************************************************/
/***********************  external definitions  ***********************/
/**********************************************************************/

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>

#define EQ(a,b)	(strcmp(a,b)==0)

#define EMIT      fprintf	/* output a formatted string */
#define EMITS(s)  fputs(s,outfp)/* output a simple string */
#define EMITO(c)  PutOct(c)	/* output an escaped octal number */
#define EMITN(n)  PutInt(n)	/* output a decimal integer */
#define EMITH(h)  putc(*(digit+((h>>4)&0xF)),outfp),\
                  putc(*(digit+(h&0xF)),outfp)
				/* output a byte value in Hex */
#define EMITC(c)  putc(c,outfp)	/* output a single character */

/**********************************************************************/
/*************************  Global Procedures  ************************/
/**********************************************************************/

extern	void	Fatal();

/* see cautionary note in code, re arithmetic vs logical shifts */
extern	void	Warning();
extern	FILE	*fopenp();
extern	void	copy_file();

/**********************************************************************/
/**************************  Data Structures  *************************/
/**********************************************************************/

typedef struct {
  char	dvi_fname[STRSIZE];	/* File name */
  int	FontDensity;	/* usual Canon 300 dpi */
  int	FirstPage;	/* output start page */
  int	LastPage;	/* output end page */
  int	G_errenc;	/* has an error been encountered? */
  int	G_header;	/* copy header file to output? */
  char	G_Logname[STRSIZE];/* name of log file, if created */
  int	G_logging;	/* Are we logging warning messages? */
  int	G_logfile;	/* Are these messages going to a log file? */
  FILE	*G_logfp;	/* log file pointer (for errors) */
  char	G_progname[STRSIZE];/* program name */
  int	G_quiet;	/* for quiet operation */
  int	G_nowarn;	/* don't print out warnings */
  int	Debug;		/* Flags for debugging */
  int	PreLoad;	/* preamble font file */
  int	Reverse;	/* Reverse output page */
  char	*Ifile[100];	/* include file name pointer */
  int	n_Ifile;	/* count include file */
  int	usermag;	/* user specified magnification */
  int	n_copies;	/* number of copies to print */
  char	*PScmd[100];	/* PostScript commands to send */
  int	n_PS;		/* number of PostScript commands */
  /* following tag is added by Y. Kusumi Feb. 1990 */
  int     Tombo;        /* tombo flag */
  char  *ps_path;       /* PostScript file directory */
  int   Stats;          /* are we reporting stats ? */
  int   ordinal_page;   /* Culculate total number of page */
} OptionSet;

extern	OptionSet	Option;	/* jdvi2kps Option struct */

extern  char            *digit;

extern	FILE	*outfp;			/* output file */

	/* Font library */
extern	double	actual_factor();
