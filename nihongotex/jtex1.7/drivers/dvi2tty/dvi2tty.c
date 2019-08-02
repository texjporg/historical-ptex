/******************************************************************************
 * Marcel Mol: 1989-02-14  (UUCP: duteca!marcel)
 *               Fixed check for .dvi extension.
 *               Allowed more ligatures.
 *               Fixed side effect bugs (2 gets as function arguments).
 *               Version 3.2.
 * Marcel Mol: 1989-01-03  (UUCP: duteca!marcel)
 *               Fixed a bugs concerning pager programs
 *               and scanning environment variable DVI2TTY.
 * Marcel Mol: 1988-10-25  (UUCP: duteca!marcel)
 *        dvi2tty.c dvi2tty.h dvistuff.c commands.h
 *               Converted program to C.
 *               improved spacing between words/characters.
 * bogart:/usr/alla/zap/dvitty/dvitty.p  1986-08-15 20:24:31,
 *               Version to be sent to mod.sources ready.
 * New option since last version:
 *   -Fprog      Pipe output to prog. Can be used to get a different
 *               pager than the default.
 * bogart:/usr/alla/zap/dvitty/dvitty.p  1986-01-13 21:49:31,
 *   Environment variable DVITTY is read and options can be set from it.
 *   These are the currently implemented options:
 *      -ofile   Write output to file, else write to stdout,
 *               possibly piped through a pager if stdout is a tty.
 *      -plist   Print pages whos TeX-page-number are in list.
 *               List is on the form  1,3:6,8  to choose pages
 *               1,3-6 and 8. TeX-nrs can be negative: -p-1:-4,4
 *      -Plist   Print pages whos sequential number are in list.
 *      -wn      Print the lines with width n characters, default is
 *               80. Wider lines gives better results.
 *      -q       Don't try to pipe to a pager.
 *      -f       Try to pipe to a pager if output is a tty.
 *      -Fname   Specify a pager program.                  
 *               Default of -q and -f is a compile time option, a constant.
 *      -l       Write '^L' instead of formfeed between pages.
 *      -u       Don't try to find Scandinavian characters (they will
 *               print as a:s and o:s if this option is choosen).
 *      -s       Scandinavian characters printed as }{|][\.
 *               Default of -s and -u is a compile time option, a constant.
 * bogart:/usr/alla/zap/dvitty/dvitty.p  1986-01-10 18:51:03,
 *   Argument parsing, and random access functions (external, in C)
 *   and other OS-dependent stuff (in C). Removed private 'pager' &
 *   tries to pipe through PAGER (environment var) or, if PAGER not
 *   defined, /usr/ucb/more. Some changes for efficency.
 * bogart:/usr/alla/svante/dvitty/dvitty.p  1985-07-15 20:51:00,
 *   The code for processing dvi-files running on UNIX (UCB-Pascal)
 *   but no argument parsing.
 * VERA::SS:<SVANTE-LINDAHL.WORK>DVITTY.PAS.140, 30-Mar-85 05:43:56,
 *   Edit: Svante Lindahl
 * VERA::SS:<SVANTE-LINDAHL.WORK>DVITTY.PAS.136, 15-Jan-85 13:52:59,
 *   Edit: Svante Lindahl, final Twenex version !!!??
 * VERA::SS:<SVANTE-LINDAHL.WORK>DVITTY.PAS.121, 14-Jan-85 03:10:22,
 *   Edit: Svante Lindahl, cleaned up and fixed a lot of little things
 * VERA::SS:<SVANTE-LINDAHL.WORK>DVITTY.PAS.25, 15-Dec-84 05:29:56,
 *   Edit: Svante Lindahl, COMND-interface, including command line scanning
 * VERA::SS:<SVANTE-LINDAHL.WORK>DVITTY.PAS.23, 10-Dec-84 21:24:41,
 *   Edit: Svante Lindahl, added command line scanning with Rscan-JSYS
 * VERA::<SVANTE-LINDAHL.DVITTY>DVITTY.PAS.48,  8-Oct-84 13:26:30,
 *  Edit: Svante Lindahl, fixed switch-parsing, destroyed by earlier patches
 * VERA::<SVANTE-LINDAHL.DVITTY>DVITTY.PAS.45, 29-Sep-84 18:29:53,
 *  Edit: Svante Lindahl
 *
 * dvitty - get an ascii representation of a dvi-file, suitable for ttys
 *
 * This program, and any documentation for it, is copyrighted by Svante
 * Lindahl. It may be copied for non-commercial use only, provided that
 * any and all copyright notices are preserved.
 *
 * Please report any bugs and/or fixes to:
 *
 * UUCP: {seismo,mcvax,cernvax,diku,ukc,unido}!enea!ttds!zap
 * ARPA: enea!ttds!zap@seismo.CSS.GOV
 *  or   Svante_Lindahl_NADA%QZCOM.MAILNET@MIT-MULTICS.ARPA
 * EAN:  zap@cs.kth.sunet
 */


#include "dvi2tty.h"

    /*-----------------------------------------------------------------------*/
    /* The following constants may be toggled before compilation to          */
    /* customize the default behaviour of the program for your site.         */
    /* Whichever their settings are, the defaults can be overridden at       */
    /* runtime.                                                              */
    /*-----------------------------------------------------------------------*/

#define DEFSCAND    FALSE     /* default is Scandinavian, toggle this if you */
                              /* don't have terminals with Scand. nat. chars */
#define WANTPAGER   TRUE      /* default: try to pipe through a pager (like  */
                              /* more) if stdout is tty and no -o switch     */
#define DEFPAGER    "/usr/ucb/more"   /* CHANGE TO YOUR LOCAL PAGER            */

    /*------------------ end of customization constants ---------------------*/

#define MAXLEN          100    /* size of char-arrays for strings            */

/*
 * USAGE CODES
 */

#define wrnge  1                /* width switch arg out of range     */
#define ign    2                /* ignore cause, print 'Usage:..'    */
#define nan    3                /* not a number where one expected   */
#define gae    4                /* garbage at end                    */
#define bdlst  5                /* bad page-numberlist               */
#define onef   6                /* only one dvifile allowed          */
#define bdopt  7                /* bad option                        */
#define onepp  8                /* only one page list allowed        */
#define noarg  9                /* argument expected                 */

char *dvi2tty = "@(#) dvi2tty.c  3.2 14/02/89 M.J.E. Mol (c) 1989";

/*---------------------------------------------------------------------------*/

printlisttype * currentpage;    /* current page to print                     */
printlisttype * firstpage;      /* first page selected                       */
printlisttype * lastpage;       /* last page selected                        */

FILE *          DVIfile;
FILE *          output;
bool            outputtofile;   /* tells if output goes to file or stdout    */
int             ttywidth;       /* max nr of chars per printed line          */

int             foo;            /* utility variable, "register"              */
bool            pager;          /* tells if output is piped to a pager       */
char  *         path;           /* name of the pager to run                  */
char  *         progname;       /* our name                                  */
int             Argc;
char **         Argv;
char            DVIfilename[MAXLEN];
char *          OUTfilename;

/*---------------------------------------------------------------------------*/

char *  getenv();
FILE *  popen();

void    main(); 
void    setoption();
void    getargs();
void    getpages();
void    plcnxt();
void    getname();
int     getinteger();
void    usage();


/****************************************************************************/
/*                                                                          */
/*                                 M A I N                                  */
/*                                                                          */
/****************************************************************************/

void main(argc, argv)
int argc;
char ** argv;
{

    progname = *argv;
    Argc = argc;
    Argv = argv;

    getargs();                              /* read command line arguments   */

    if ((DVIfile = fopen(DVIfilename, "r")) == NULL)
        errorexit(filop);                   /* can't open dvifile            */

    if (outputtofile) {                     /* open the outfile, if needed   */
        if ((output = fopen(OUTfilename, "w")) == NULL)
            errorexit(filcr);
        pager = FALSE;
    }
    else {
        output = stdout;
        if (isatty(fileno(output)) && pager) {   /* try to pipe to a pager   */
            if ((output = popen(path, "w")) == NULL) /* get pipe to pager    */
                errorexit(pipcr);                /* make output to output    */
        }
        else
            pager = FALSE;
    }

    dvimain();

    if (pager)
        pclose(output);                     /* close pipe to pager            */

    exit(0);

} /* main */

/*----------------------------------------------------------------------------*/

void setoption(optch, optset, optwarg, str)
char optch;
char *optset, *optwarg;
char *str;
{
    int j = 0;
   
    while (strchr(optset, optch) != NULL) {
        foo = optch;
        switch (optch) {
	    case 'q' : pager = FALSE; break;
	    case 'f' : pager = TRUE; break;
	    case 'l' : noffd = TRUE; break;
	    case 's' : scascii = TRUE; break;
	    case 'u' : scascii = FALSE; break;
            case 'P' :
		       sequenceon = TRUE;     /* fall through */
            case 'p' :
                       if (pageswitchon)
                           usage(onepp);
		       getpages(j, str);
                       break;
            case 'w' :
                       if (((str[j] < '0') || (str[j] > '9'))
                           && (str[j] != '-')) 
                           usage(nan);
		       ttywidth = getinteger(&j, str, 80);
                       if (str[j] != '\0') 
                           usage(gae);
                       if ((ttywidth < 16) || (ttywidth > 132))
                           usage(wrnge);
                       break;
            case 'o' :
                       OUTfilename = str;
		       outputtofile = TRUE;
                       j = strlen(str);
                       break;
            case 'F' :
                       pager = TRUE;
                       path = str;
                       j = strlen(str);
                       break;
            default  : 
                       usage(bdopt);
        }
        if ((optch = str[j]) == '\0')
            break;
        j++;
        if ((strchr(optwarg, optch) != NULL) && (str[j]='\0')) {
                if (--Argc <= 0)
                    usage(noarg);
                str = *++Argv;
                j = 0;
            }
    }
} /* setoption */

char      KANJI_IN[] = "\033$@";
char      KANJI_OUT[]= "\033(J";
/*----------------------------------------------------------------------------*/

void getargs()
{
    char *str, *envp;
    bool DVIfound;                      /* if a dvi filename found           */
    char *optset, *optwarg;
    char optch, *sc_kanji;

    if (Argc <= 1)
        usage(ign);

    pageswitchon = FALSE;       /* show all pages                            */
    sequenceon   = FALSE;       /* selected pages are TeX-numbered           */
    outputtofile = FALSE;       /* write to stdout                           */
    pager        = WANTPAGER;   /* want paging, compile time option          */
    noffd        = FALSE;       /* print formfeed between pages              */
    scascii      = DEFSCAND;    /* scandinavian, compile time option         */
    ttywidth     = 80;          /* default terminal width                    */
    DVIfound     = FALSE;
    optset       = "wpPousqlfF";/* legal options                             */
    optwarg      = "wpPoF";     /* options with argument                     */

    if ((path = getenv("PAGER")) == NULL)   /* find default pathname of page */
            path = DEFPAGER;             /* program in case paging is wanted */
	/*
	          KJ=J@B
                     ^------ SEJ
                      ^------JISKANJI Begin
                        ^----Turn to ASCII
	 */
    if (sc_kanji = getenv("RNKJ") ) {
	if (sc_kanji[0] == 'J' || sc_kanji[0] == 'j') {
	  KANJI_IN[2] = sc_kanji[1];
	  KANJI_OUT[2]= sc_kanji[2];
	  OType = JIS;
	} else if (sc_kanji[0] == 'S' || sc_kanji[0] == 's') {
	  OType = SJIS;
	} else if (sc_kanji[0] == 'E' || sc_kanji[0] == 'e') {
	  OType = EUC;
	}
    }

    if ((envp = getenv("DVI2TTY")) != NULL) {
        while ((*envp == ' ') && (*envp != '\0'))
             envp++;
        while (*envp) {                     /* environment var args          */
            if (strchr(optset, optch = *envp++) != NULL) {
                /*
                 * we always pass one option, and arrange for optarg ourselfves,
                 * so setoption does not mesh up Argv
                 */
                if (strchr(optwarg, optch) != NULL) {
                    while ((*envp == ' ') && (*envp != '\0'))
                        envp++;
                    if (*envp == '\0')
                        usage(noarg);
                    str = envp;             /* str points to optarg          */
                    while ((*envp != ' ') && (*envp != '\0'))
                        *envp++;            /* set envp just after optarg    */
                    if (*envp != '\0')
                        *envp++ = '\0';     /* end optarg string             */
                }
                setoption(optch, optset, optwarg, str);
            }
            else
                usage(bdopt);
            while ((*envp == ' ') && (*envp != '\0'))
                 envp++;
        }
    }

    while (--Argc > 0) {                    /* command line args             */
        str = *++Argv;
        if (*str != '-') {                  /* argument is not an option     */
            if (DVIfound)
                usage(onef);
            getname(str);
            DVIfound = TRUE;
        }
        else if (strchr(optset, optch = *++str) != NULL) {
            str++;                      /* point to rest of argument if any  */
            if ((strchr(optwarg, optch) != NULL) && (*str == NULL)) {
                if (--Argc <= 0)
                    usage(noarg);
                str = *++Argv;
            }
            setoption(optch, optset, optwarg, str);
        }
        else
            usage(bdopt);
    }

    if (!DVIfound)
        usage(ign);

} /* getargs */

/*---------------------------------------------------------------------------*/

void getpages(j, str)
int j;
char *str;
{
    int i;

    pageswitchon = TRUE;
    firstpage = (printlisttype *) malloc(sizeof(printlisttype));
    firstpage->all = FALSE;
    firstpage->nxt = nil;
    firstpage->pag = 0;
    lastpage = firstpage;
    currentpage = firstpage;
    if ((str[j] >= '0') && (str[j] <= '9') || (str[j] == '-'))
        foo = getinteger(&j, str, 0);
    else
        usage(nan);
    while (foo != 0) {
        plcnxt(foo);
        if (str[j] == ',') {
            j++;
            if (((str[j] < '0') || (str[j] > '9')) && (str[j] != '-'))
                usage(nan);
        }
        else if (str[j] == ':') {
            j++;
            if (((str[j] < '0') || (str[j] > '9')) && (str[j] != '-'))
                usage(nan);
            foo = getinteger(&j, str, 0);
            if (currentpage->pag < 0) {
                if (foo > 0) {
                    currentpage->all = TRUE;
                    plcnxt(foo);
                }
                else if (foo < currentpage->pag)
                    for (i = currentpage->pag - 1; i >= foo; i--)
                        plcnxt(i);
                else
                    usage(bdlst);
            }
            else {
                if (foo < currentpage->pag)
                    usage(bdlst);
                for (i = currentpage->pag + 1; i <= foo; i++)
                    plcnxt(i);
            }
            if (str[j] == ',') {
                j++;
                if (((str[j] < '0') || (str[j] > '9')) && (str[j] != '-'))
                    usage(nan);
            }
        }
        foo = getinteger(&j, str, 0);
    }
    if ((str[j] != ' ') || (str[j] != NULL))
        usage(gae);
    currentpage = firstpage;

} /* getpages */


void plcnxt(pagnr)      /* place page-nr next in list */
int pagnr;
{
    currentpage = lastpage;
    currentpage->pag = pagnr;
    lastpage = (printlisttype *) malloc(sizeof(printlisttype));
    lastpage->all = FALSE;
    lastpage->nxt = nil;
    lastpage->pag = 0;
    currentpage->nxt = lastpage;

} /* plcnxt */

/*----------------------------------------------------------------------------*/

void getname(str)
char *str;
{
    int   i;

    i = strlen(str);
    if (i == 0)
        usage(ign);
    strcpy(DVIfilename, str);
    if (!((i >= 5) && (str[i-1] == 'i') && (str[i-2] == 'v') &&
          (str[i-3] == 'd') && (str[i-4] == '.'))) {
        strcat(DVIfilename, ".dvi");
    }

} /* getname */

/*----------------------------------------------------------------------------*/

int getinteger(j, str, def)
int *j;
char *str;
int def;
{
    int  cum;
    int  sgn;
    char ch;

    ch = str[*j];
    if (ch == '-') {
        sgn = -1;
        ch  = str[++(*j)];
    }
    else
        sgn = 1;
    if ((ch >= '0') && (ch <= '9')) {
        cum = 0;
        while ((ch >= '0') && (ch <= '9')) {
            cum = cum*10 + ch - '0';
            ch = str[++(*j)];
        }
        return sgn * cum;
    }
    return def;

}   /* getinteger */

/*----------------------------------------------------------------------------*/

void errorexit(errorcode)
int errorcode;
{

    fprintf(stderr, "%s: ", progname);
    switch (errorcode) {
        case  illop : fprintf(stderr, "Illegal op-code found: %d\n", opcode);
                      break;
        case  stkof : fprintf(stderr, "Stack overflow\n");
                      break;
        case  stkuf : fprintf(stderr, "Stack underflow\n");
                      break;
        case  stkrq : fprintf(stderr, "Cannot create dvi stack\n");
                      break;
        case  badid : fprintf(stderr, "Id-byte is not correct: %d\n ", opcode);
                      break;
        case  bdsgn : fprintf(stderr, "Bad signature: %d (not 223)\n", foo);
                      break;
        case  fwsgn : fprintf(stderr, "%d signature bytes (min. 4)\n", foo);
                      break;
        case  nopre : fprintf(stderr, "Missing preamble\n");
                      break;
        case  nobop : fprintf(stderr, "Missing beginning-of-page command\n");
                      break;
        case  nopp  : fprintf(stderr, "Missing post-post command\n");
                      break;
        case  bdpre : fprintf(stderr, "Preamble occured inside a page\n");
                      break;
        case  bdbop : fprintf(stderr, "BOP-command occured inside a page\n");
                      break;
        case  bdpst : fprintf(stderr, "Postamble occured before end-of-page\n");
                      break;
        case  bdpp  : fprintf(stderr, "Postpost occured before post-command\n");
                      break;
        case  nopst : fprintf(stderr, "Missing postamble\n");
                      break;
        case  illch : fprintf(stderr, "Character code out of range, 0..127\n");
                      break;
        case  filop : fprintf(stderr, "Cannot open dvifile\n");
                      break;
        case  filcr : fprintf(stderr, "Cannot create outfile\n");
                      break;
        case  pipcr : fprintf(stderr, "Cannot create pipe to pager\n");
                      break;
       default      : fprintf(stderr, "Unkown error code\n");
                      break;
    };
    if (outputtofile)
        unlink(OUTfilename);
    exit(errorcode);

}  /* errorexit */

/*----------------------------------------------------------------------------*/

void usage(uerr)
int uerr;
{

    if (uerr != ign) {
        fprintf(stderr,"%s: ", progname);
        switch (uerr) {
            case   ign    : fprintf(stderr, "%s", Copyright);
                            break;
            case   wrnge  : fprintf(stderr, "width arg out of range:16-132");
                            break;
            case   nan    : fprintf(stderr, "numeric argument expected for %c",
                                              foo);
                            break;
            case   gae    : fprintf(stderr, "garbage at end of argument for %c",
                                              foo);
                            break;
            case   bdlst  : fprintf(stderr, "mal-formed list of pagenumbers");
                            break;
            case   onef   : fprintf(stderr, "only one infile argument allowed");
                            break;
            case   noarg  : fprintf(stderr, "option argument expected for %c",
                                              foo);
                            break;
            case   bdopt  : fprintf(stderr, "bad option %c", foo);
                            break;
            case   onepp  : fprintf(stderr, "only one pagelist allowed");
                            break;
            default       : fprintf(stderr, "unknown usage error");
                            break;
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "Usage: %s [ options ] dvifile[.dvi]\n", progname);
    fprintf(stderr, "Options are:\n");
    fprintf(stderr,
            " -ofile   Write output to file, else write to stdout.\n");
    fprintf(stderr,
            " -plist   Print pages whos TeX-page-number are in list.\n");
    fprintf(stderr,
            " -Plist   Print pages whos sequential number are in list.\n");
    fprintf(stderr,
            " -wn      Print the lines with width n characters, default 80.\n");
    fprintf(stderr, " -f       Try to pipe to a pager if output is a tty");
    if (WANTPAGER)
        fprintf(stderr, " (default).\n");
    else
        fprintf(stderr, ".\n");
    fprintf(stderr, " -q       Don't try to pipe to a pager");
    if (WANTPAGER)
        fprintf(stderr, ".\n");
    else
        fprintf(stderr, " (default).\n");
    fprintf(stderr,
            " -l       Write ''^L'' instead of formfeed between pages.\n");
    fprintf(stderr,
            " -u       National Swedish characters printed as aaoAAO");
    if (DEFSCAND)
        fprintf(stderr, ".\n");
    else
        fprintf(stderr, " (default).\n");
    fprintf(stderr,
            " -s       National Swedish characters printed as }{|][\\");
    if (DEFSCAND)
        fprintf(stderr, " (default).\n");
    else
        fprintf(stderr, ".\n");
    exit(uerr);

} /* usage */

