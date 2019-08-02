#include	<stdio.h>
#include	<ctype.h>
#include	"defs.h"
#include	"global.h"

/*-->DecodeArgs*/
/*********************************************************************/
/***************************** DecodeArgs ****************************/
/*********************************************************************/

void
DecodeArgs( argc, argv )
int argc;
char *argv[];
{
    int argind;             /* argument index for flags              */
    char curarea[STRSIZE];  /* current file area		     */
    char curname[STRSIZE];  /* current file name		     */
    char *tcp, *tcp1;	    /* temporary character pointers	     */

    argind = 1;
    while (argind < argc) {
	tcp = argv[argind];
	if (*tcp == '-')
	    switch (*++tcp) {

		case 'F':	/* F selects different fontdesc */
		    fontdescfile = argv[++argind];
		    break;
		case 'R':	/* R selects different resolution */
		    dev_arg('R', argv[++argind]);
		    break;
#ifdef CREOPT
		case 'c':       /* create an output file in spool area */
		    G_create = TRUE;
		    break;
#endif CREOPT
#ifdef DEBUG
		case 'd':	/* d selects Debug output */
		    Debug = TRUE;
		    break;
#endif
		case 'f':       /* next arg is starting pagenumber */
		    if (++argind >= argc || sscanf(argv[argind], "%d", &FirstPage) != 1)
			Fatal("Argument is not a valid integer\n");
		    break;

		case 'h':       /* don't copy PostScript header file through to output */
		    G_header = FALSE;
		    break;

		case 'i':       /* next arg is a PostScript file to copy */
		    if (++argind >= argc)
			Fatal("No argument following -i\n");
		    Ifile[nif++] = argv[argind];
		    break;

		case 'l':	/* l prohibits logging of errors */
		    G_logging = -1;
		    break;
#ifdef USEGLOBALMAG
		case 'm':       /* specify magnification to use */
		    switch(*++tcp) {

		    case '\0':       /* next arg is a magnification to use */
			if (++argind >= argc || sscanf(argv[argind], "%d", &usermag) != 1)
			    Fatal("Argument is not a valid integer\n", 0);
			break;
		    case '0': usermag = 1000; break;
		    case 'h': usermag = 1095; break;
		    case '1': usermag = 1200; break;
		    case '2': usermag = 1440; break;
		    case '3': usermag = 1728; break;
		    case '4': usermag = 2074; break;
		    case '5': usermag = 2488; break;
		    default: Fatal("%c is a bad mag step\n", *tcp);
		    }
		    break;
#endif
		case 'n':       /* next arg is number of copies to print */
		    if (++argind >= argc || sscanf(argv[argind], "%d", &ncopies) != 1)
			Fatal("Argument is not a valid integer\n");
		    break;

		case 'o':       /* next arg is a PostScript command to send */
		    if (++argind >= argc)
			Fatal("No argument following -o\n");
		    dev_arg('o', argv[argind]);
		    break;

		case 'p':	/* p prohibits pre-font loading */
		    PreLoad = FALSE;
		    Reverse = FALSE;    /* must then process in forward order */
		    break;

		case 'q':       /* quiet operation */
		    G_quiet = TRUE;
		    break;

		case 'r':       /* don't process pages in reverse order */
		    Reverse = FALSE;
		    break;
#ifdef STATS
		case 's':       /* print some statistics */
		    Stats = TRUE;
		    break;
#endif
		case 't':       /* next arg is ending pagenumber */
		    if (++argind >= argc || sscanf(argv[argind], "%d", &LastPage) != 1)
			Fatal("Argument is not a valid integer\n");
		    break;

		case 'w':       /* don't print out warnings */
		    G_nowarn = TRUE;
		    break;

		default:
		    printf("%c is not a legal flag\n", *tcp);
		}

	else {

	    tcp = rindex(argv[argind], '/');    /* split into directory + file name */
	    if (tcp == NULL)  {
		curarea[0] = '\0';
		tcp = argv[argind];
	    } else  {
		strcpy(curarea, argv[argind]);
		curarea[tcp-argv[argind]+1] = '\0';
		tcp += 1;
	    }

	    strcpy(curname, tcp);
	    tcp1 = rindex(tcp, '.');   /* split into file name + extension */
	    if (tcp1 == NULL) {
		strcpy(rootname, curname);
		strcat(curname, ".dvi");
	    } else {
		*tcp1 = '\0';
		strcpy(rootname, curname);
		*tcp1 = '.';
	    }

	    strcpy(filename, curarea);
	    strcat(filename, curname);

	    if ((dvifp = BINARYOPEN(filename,"r")) == NULL)
		Fatal("can't find DVI file \"%s\"", filename);

	    strcpy(G_Logname, curname);
	    strcat(G_Logname, ".log");

	    break;
	}
	argind++;
    }

    if (dvifp == NULL)  {
	fprintf(stderr,
		"\nusage: %s [-F fontdesc] [-R resolution] [-c] [-h] [-o option] [-p] [-s] [-r] [-f n] [-t n] [-m{0|h|1|2|3|4}] [-m mag] dvifile\n\n",
		G_progname);
	AbortRun(2);
    }
}


/*-->AbortRun*/
/**********************************************************************/
/***************************  AbortRun  *******************************/
/**********************************************************************/

void
AbortRun(code)
int code;
{
    exit(code);
}


/*-->AllDone*/
/**********************************************************************/
/****************************** AllDone  ******************************/
/**********************************************************************/

void
AllDone()
{
    char t;
    struct font_entry *p;
    int per;

    dev_finish();
    if (!G_quiet) fprintf(stderr,"\n");

#ifdef CREOPT
    if (G_create) {
	fclose(outfp);
	if (!G_quiet) fprintf(stderr, "Output written on \"%s\"\n", outfname);
    }
#endif CREOPT

#ifdef STATS
    if (Stats) {
	fprintf(stderr, "Total chars   diff chars   raster bytes\n");
	fprintf(stderr, "      #   %%        #   %%       #   %%\n");
	fprintf(stderr, "------- ---   ------ ---   ----- ---\n");
	for (p = hfontptr; p != NULL; p = p->next) {
	    fprintf(stderr, "%7d%4d", p->ncts, 100*p->ncts/Stnc);
	    if (Sndc != 0)
		fprintf(stderr, "%9d%4d", p->ncdl, 100*p->ncdl/Sndc);
	    else
		fprintf(stderr, "%9d  --", p->ncdl);
	    if (Snbpxl != 0)
		fprintf(stderr, "%8d%4d", p->nbpxl, 100*p->nbpxl/Snbpxl);
	    else
		fprintf(stderr, "%8d  --", p->nbpxl);
	    fprintf(stderr, "  %s\n", p->name);
	}
	fprintf(stderr, "\nTotal number of characters typeset: %d\n", Stnc);
	fprintf(stderr, "Number of different characters downloaded: %d\n", Sndc);
	fprintf(stderr, "Number of bytes of raster data downloaded: %d\n", Snbpxl);
	fprintf(stderr, "Optimal # of bytes of raster data: %d\n", Sonbpx);
    }
#endif

    AbortRun(G_errenc ? 1 : 0);
}


/*-->Fatal*/
/**********************************************************************/
/******************************  Fatal  *******************************/
/**********************************************************************/

void
Fatal(fmt, a, b, c)	/* issue a fatal error message */
char *fmt;		/* format */
char *a, *b, *c;	/* arguments */
{
    if (G_logging == 1 && G_logfile) {
	fprintf(G_logfp, "%s: FATAL-- ", G_progname);
	fprintf(G_logfp, fmt, a, b, c);
	fprintf(G_logfp, "\n");
    }

    fprintf(stderr,"\n");
    fprintf(stderr, "%s: FATAL-- ", G_progname);
    fprintf(stderr, fmt, a, b, c);
    fprintf(stderr, "\n\n");
#ifdef CREOPT
    if (G_create && outfp != NULL) {
	fclose(outfp);
	unlink(outfname);
    }
#endif CREOPT
    AbortRun(2);
}


/*-->Warning*/
/**********************************************************************/
/*****************************  Warning  ******************************/
/**********************************************************************/

void
Warning(fmt, a, b, c)	/* issue a warning */
char *fmt;		/* format   */
char *a, *b, *c;	/* arguments */
{
    if (G_logging == 0) {
	if (G_logfile)
	    G_logfp=fopen(G_Logname,"w+");
	else {
	    G_logfp=stderr;
	    if (G_nowarn) return;
	}
	G_logging = 1;
	if (G_logfp == NULL) G_logging = -1;
    }

    G_errenc = TRUE;
    if (G_logging == 1) {
	fprintf(G_logfp, fmt, a, b, c);
	fprintf(G_logfp,"\n");
    }
}
