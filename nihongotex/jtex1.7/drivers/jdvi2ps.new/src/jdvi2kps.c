#define		VERSION		"2.10gf"

#include	"../dvi/dvi_public.h"
#include	"jdvi2kps.h"

/*
 *
 * AUTHOR(s)
 *     Mark Senn wrote the early versions of this program for the
 *     BBN BitGraph.  Stephan Bechtolsheim, Bob Brown, Richard
 *     Furuta, James Schaad and Robert Wells improved it.  Norm
 *     Hutchinson ported the program to the Sun.  Neal Holtz ported
 *     it to the Apollo, and from there to producing PostScript
 *     output. Scott Jones added intelligent font substitution.
 *     Howard Trickey made it read GF files instead of PXL ones.
 *
 */

/* Basic method:
 * Using the local font cacheing machinery that was in the previewer,
 * we can easily manage to send the bitmap for each chracter only once.
 * Two passes are made over each page in the DVI file.  The first pass
 * simply outputs the bitmaps for all characters on that page that haven't
 * been sent before.  The second pass outputs all the character setting
 * and positioning commands.  This allows us to bracket the setting portion
 * with PostScript save's and restore's, thus reclaiming considerable
 * virtual memory after each page.
 *
 * All coordinates are output in the PostScript system (TeX origin),
 * and in integer units of rasters (300/inch) -- except for character
 * widths, which are sent as floating point numbers.
 *
 * About 2 pages of PostScript support code must be sent to the LaserWriter
 * before this stuff goes.  It is automatically included unless the
 * -h option is given.
 */


/* Change log:
 *
 * Early 1985, (nmh) -- ported sun version to Apollo. 
 * A little later (nmh) -- changed to continue on in the event of missing
 *                      font files.
 * 30-Mar-85 (nmh) -- added -a option to specify a different PXL area
 * 30-Mar-85 (nmh) -- changed default PXL area to /pxl118
 * 31-Mar-85 (nmh) -- fixed bug in OpenFontFile() regarding more than MAXOPEN
 *                    PXL files -- changed to mark files as closed in font_entry.
 *  7-Apr-85 (nmh) -- made command line argument decoding case insensitive.
 *                    cleaned up handling of DVI file name argument.
 * 30-May-85 (nmh) -- new version; hacked to output PostScript commands
 *  6-Jun-85 (nmh) -- added relative positioning (20% smaller PostScript output)
 *                    add -m option to specify mag
 * 11-Jun-85 (nmh) -- fixed bug regarding char spacings in very long "words"
 * 12-Jun-85 (nmh) -- v1.02 - process DVI pages in reverse order
 * 13-Jun-85 (nmh) -- fixed bug re PXL files getting opened too often when no PreLoad
 * 14-Jun-85 (nmh) -- font dict created in PostScript only when 1st char of font downloaded
 *                    add -m0 -mh -m1 etc. to specify magsteps
 * 16-Aug-85 (nmh) -- added -c option t0 create output file in spool area (Apollo specific)
 *                    added -h option to copy header file to output
 *                    added -o option to pass options through to PostScript
 * 20-Aug-85 (nmh) -- v1.03
 * 24-Aug-85 (nmh) -- add -q option (for quiet operation).
 *                    changed -o option to check PostScript option
 *                    changed to output coordinates in TeX system (but
 *                    scaled to raster units) -- (0,0) at 1" in and down from
 *                      top left (better for use with different size paper).
 *                 -- v2.00
 * 25-Aug-85 (nmh) -- added dictionary enclosure to Tex.ps, and output
 *                      suitable prolog here.
 * 26-Aug-85 (nmh) -- changes to tex.ps to support Macintosh documents.
 * 14-Sep-85 (nmh) -- added keyword=value decoding to \special;
 * 15-Sep-85 (nmh) -- added -i file option.
 * 23-Sep-85 (saj) -- added font substitution for case when font is
 *                    unavailable at requested mag. (a frequent occurrence
 *                    with some macro packages like LaTeX)
 *  7-Jun-86 (hwt) -- use gf files ("pxl" survives in variable names)
 *  8-Aug-86 (rkf) -- merged together the pxl and the gf versions.  Define
 *                    USEPXL should you want PXL files---gf is the default
 * 22-Dec-87 (pam) -- suppress (but do not remove) the function of the -d
 *		      flag for debugging, so that it can be used to specify
 *		      print-engine resolution (inspired by the VariTyper 600).
 *  5-Jan-88 (pam) -- provide for 256 character gf fonts.
 */


/**********************************************************************/
/************************  Global Definitions  ************************/
/**********************************************************************/

char	*digit		= "0123456789ABCDEF";
static char version[] = "This is jdvi2kps version 2.1.";

/**********************************************************************/
/*************************  Global Variables  *************************/
/**********************************************************************/

OptionSet	Option;		/* jdvi2kps Option data struct */

/*-->AbortRun*/
/**********************************************************************/
/***************************  AbortRun  *******************************/
/**********************************************************************/

static void
AbortRun(code)
int code;
{
	exit(code);
}

/*-->DecodeArgs*/
/*********************************************************************/
/***************************** DecodeArgs ****************************/
/*********************************************************************/

static void
DecodeArgs( argc, argv )
int argc;
char *argv[];
{
	int argind;		/* argument index for flags */
	char curarea[STRSIZE];	/* current file area */
	char curname[STRSIZE];	/* current file name */
	char *tcp, *tcp1;	/* temporary character pointers	*/
	char *getenv();

	strcpy(Option.G_progname, argv[0]);	/* set this program name */

	/* default option set */

	Option.G_errenc	= ERRCOUNT;	/* has an error been encountered */
	Option.G_header	= HEADER;	/* output default header file ? */
	Option.G_logfile = LOGFILE;	/* make log file ? */
	Option.G_quiet	 = QUIET;	/* changing page number output ? */
	Option.G_nowarn	 = NOWARN;	/* print warning ? */
	Option.Debug	 = DEBUGMODE;	/* print changing status ? */
	Option.FirstPage = FIRSTPAGE;	/* first page to print
						   (uses count0) */
	Option.LastPage	 = LASTPAGE;	/* last page to print */
	Option.FontDensity = DEFAULTRESOLUTION;
					/* the usual Canon 300 dpi */
	Option.PreLoad	= PRELOAD;	/* preload the font descriptions ? */
	Option.Reverse	= REVERSE;/* process DVI pages in reverse order ? */
	Option.usermag	= USERMAG;/* user specified magnification */
	Option.n_copies	= N_COPIES;	/* copies */
	Option.G_logging = 0;		/* 1 : openning log file */
					/* 0 : before open log file */
					/*-1 : not create log file */
	Option.n_Ifile	= 0;	/* reset count of files to include */
	Option.n_PS = 0;	/* reset count PostScript commands */
	Option.Tombo = 0;       /* reset tombo flag */
	/* Set PostScript file path */
	Option.ps_path = getenv("DVIPSPATH");
	if (!Option.ps_path) Option.ps_path = PS_PATH;
	Option.Stats = False;
	Option.ordinal_page = 0;

	argind = 1;
	while (argind < argc) {
	    tcp = argv[argind];
	    if (*tcp == '-') {
		switch(isupper(*++tcp) ? (*tcp-'A')+'a' : *tcp) {

		case 'd':	/* d selects font pixel density */
#ifdef DEBUG
		    Option.Debug = True;	/* and maybe Debug output too */
#endif
                    if( ++argind >= argc ||
			sscanf(argv[argind], "%d", &Option.FontDensity) != 1 ) {
                        Fatal("Argument is not a valid integer\n", 0);
		    }
                    Option.G_quiet = False;
		    break;

                case 'f':	/* next arg is starting pagenumber */
                    if( ++argind >= argc ||
			sscanf(argv[argind], "%d", &Option.FirstPage) != 1 ) {
                        Fatal("Argument is not a valid integer\n", 0);
		    }
                    break;

                case 'h':
		/* don't copy PostScript header file through to output */
                    Option.G_header = False;
                    break;

                case 'i':	/* next arg is a PostScript file to copy */
                    if( ++argind >= argc )
                        Fatal("No argument following -i\n", 0);
                    Option.Ifile[Option.n_Ifile++] = argv[argind];
                    break;

		case 'l':	/* l prohibits logging of errors */
		    Option.G_logging = -1;
		    break;
#ifdef USEGLOBALMAG
                case 'm':       /* specify magnification to use */
                    switch( tolower(*++tcp) ) {

                    case '\0':       /* next arg is a magnification to use */
                        if( ++argind >= argc ||
			    sscanf(argv[argind], "%d", &Option.usermag) != 1 ) {
                            Fatal("Argument is not a valid integer\n", 0);
			}
                        break; 
                    case '0': Option.usermag = 1000; break;
                    case 'h': Option.usermag = 1095; break;
                    case '1': Option.usermag = 1200; break;
                    case '2': Option.usermag = 1440; break;
                    case '3': Option.usermag = 1728; break;
                    case '4': Option.usermag = 2074; break;
                    case '5': Option.usermag = 2488; break;
                    default: Fatal("%c is a bad mag step\n", *tcp);
                    }
                    break;
#endif
                case 'n':       /* next arg is number of copies to print */
                    if( ++argind >= argc ||
			sscanf(argv[argind], "%d", &Option.n_copies) != 1 ) {
                        Fatal("Argument is not a valid integer\n", 0);
		    }
		    if (Option.n_copies < MIN_N_COPIES
			|| MAX_N_COPIES < Option.n_copies) {
			Option.n_copies = 0;
		    }
                    break;    

                case 'o':       /* next arg is a PostScript command to send */
                    if( ++argind >= argc ) {
                        Fatal("No argument following -o\n", 0);
		    }
                    Option.PScmd[Option.n_PS++] = argv[argind];
                    break;

		case 'p':	/* p prohibits pre-font loading */
		    Option.PreLoad = 0;
                    Option.Reverse = False;    /* must then process in forward order */
		    break;

                case 'q':       /* quiet operation */
                    Option.G_quiet = True;
                    break;

                case 'r':       /* don't process pages in reverse order */
                    Option.Reverse = False;
                    break;
                case 's':       /* print some statistics */
                    Option.Stats = True;
                    break;
                case 't':       /* next arg is ending pagenumber */
                    if( ++argind >= argc ||
			sscanf(argv[argind], "%d", &Option.LastPage) != 1 ) {
                        Fatal("Argument is not a valid integer\n", 0);
		    }
                    break;

                case 'w':       /* don't print out warnings */
                    Option.G_nowarn = True;
                    break;

		case 'x':
		    Option.Tombo = True;
		    break;

		default:
		    (void) fprintf(stderr, "\n%s\n", version);
		    prerror("\n usage : %s %s", Option.G_progname,ERRMSG);
		    AbortRun(1);
		}
	    } else {
		tcp = strrchr(argv[argind], '/');
					/* split into directory + file name */
		if (tcp == NULL)  {
		    curarea[0] = '\0';
		    tcp = argv[argind];
                } else  {
		    strcpy(curarea, argv[argind]);
		    curarea[tcp-argv[argind]+1] = '\0';
		    tcp += 1;
                }
        
		strcpy(curname, tcp);
		tcp1 = strrchr(tcp, '.');   /* split into file name + extension */
		if (tcp1 == NULL) {
		    strcat(curname, ".dvi");
                }

		strcpy(Option.dvi_fname, curarea);
		strcat(Option.dvi_fname, curname);
        
		strcpy(Option.G_Logname, curname);
		strcat(Option.G_Logname, ".log");
	    }
	    argind++;
        }
}

/*-->AllDone*/
/**********************************************************************/
/****************************** AllDone  ******************************/
/**********************************************************************/
static void
AllDone()
{
	char t;

	EMITS("@end\n");
	EMITS("%%Trailer\n");
	EMIT(outfp, "%%%%Pages: %d ", Option.ordinal_page);
	if (Option.Reverse) EMITS("-1\n");
	else EMITS("1\n");

	if( !Option.G_quiet ) fprintf(stderr,"\n");

	if (Option.G_errenc && Option.G_logging == 1 && Option.G_logfile)  {
	    fseek(Option.G_logfp, 0, 0);
	    while ((t = (char) getc(Option.G_logfp)) != EOF)
		putchar(t);
	}
	if (Option.G_logging == 1 && Option.G_logfile) printf("Log file created\n");

#ifdef GLOBAL_FONT
	if (Option.Stats) {
	    font_status();
	}
#endif
	AbortRun(Option.G_errenc);
}

/**********************************************************************/
/*******************************  main  *******************************/
/**********************************************************************/

main(argc, argv)
int	argc;
char	*argv[];
{
  register int i;		/* command parameter; loop index */
  extern AppInfo app_info;

  DecodeArgs( argc, argv );	/* environment set */

  InitDVIFile(Option.dvi_fname, Option.FontDensity, Option.usermag,
	      Option.PreLoad, NULL, NULL, &app_info);

  --Option.LastPage; --Option.FirstPage;
  Option.FirstPage = Option.FirstPage < 0 ? 0 : Option.FirstPage;
  Option.LastPage = Option.LastPage < 0 ? 0 :
    (Option.LastPage < dvi_info.NumberOfPage ? Option.LastPage : dvi_info.NumberOfPage - 1);
  if (Option.Reverse) {
    for (i = Option.LastPage; i >= Option.FirstPage; --i) {
      ProcessPage(i);
    }
  } else {
    for (i = Option.FirstPage; i <= Option.LastPage; ++i) {
      ProcessPage(i);
    }
  }
  AllDone();
}

/*-->Fatal*/
/**********************************************************************/
/******************************  Fatal  *******************************/
/**********************************************************************/

void
Fatal(fmt, a, b, c)		/* issue a fatal error message */
char *fmt;			/* format */
char *a, *b, *c;		/* arguments */

{
	if (Option.G_logging == 1 && Option.G_logfile)
	{
	    fprintf(Option.G_logfp, "%s: FATAL--", Option.G_progname);
	    fprintf(Option.G_logfp, fmt, a, b, c);
	    fprintf(Option.G_logfp, "\n");
	}

	fprintf(stderr,"\n");
	fprintf(stderr, "%s: FATAL--", Option.G_progname);
	fprintf(stderr, fmt, a, b, c);
	fprintf(stderr, "\n\n");
	if (Option.G_logging == 1) fprintf(stderr, "Log file created\n");
	AbortRun(1);
}

/*-->Warning*/
/**********************************************************************/
/*****************************  Warning  ******************************/
/**********************************************************************/

void
Warning(fmt, a, b, c)  /* issue a warning */
char *fmt;	/* format   */
char *a, *b, *c;	/* arguments */
{
	if (Option.G_logging == 0) {
	    if (Option.G_logfile) {
		Option.G_logfp=fopen(Option.G_Logname,"w+");
	    } else {
		Option.G_logfp=stderr;
		if( Option.G_nowarn ) return;
	    }
	    Option.G_logging = 1;
	    if (Option.G_logfp == NULL) Option.G_logging = -1;
	}

	Option.G_errenc = True;
	if (Option.G_logging == 1) {
	    fprintf(Option.G_logfp, fmt, a, b, c);
	    fprintf(Option.G_logfp,"\n");
	}
}
