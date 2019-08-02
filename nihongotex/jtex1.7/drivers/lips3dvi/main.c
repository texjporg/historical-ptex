/*
 *	main.c -- lips3dvi main module
 *
 *	Written by H. Nagahara on 2/18/92 09:58:11.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: main.c 10.2
 */
#include	<stdio.h>
#include	<string.h>

#define		_MAIN_
#include	"config.h"
#include	"dvi.h"
#include	"drive.h"
#include	"lbp.h"
#include	"version.h"
#include	"font.h"

/* library and other file functions declaration */
#if MSDOS
# include	<malloc.h>
# include	<stdlib.h>
#else
extern char		*malloc();
extern char		*getenv();
extern long		atol();
#endif

extern int		getopt PP((int, char**, char*));
extern char		*optarg;
extern int		optind, opterr;
extern int		makefontlist PP((FILE *));
extern int		lopen PP((void));

/* driver registers */
int4	h, v,		/* current point */
	w, x,		/* horizontal registers */	
	y, z,		/* vertical reigisters */
	dir;		/* direction for pTeX extension */
int	fno;		/* current font # */

/* font lists */
struct font	*fontp = NULL;		/* pointer to current font information */
struct font	*fontlist = NULL;	/* top of the font-list */
struct metric	*metriclist = NULL;	/* top of the font metric infor */
struct font	*g0font = NULL;		/* font assigned to G0 */
struct font	*g1font = NULL;		/* font assigned to G1 */

/* caching font */
struct fdir	*cachetop = NULL;	/* recently referenced bitmap for cache */
struct fdir	*cachebottom = NULL;	/* last referenced bitmap for cache */
struct fdir	*cachefree = NULL;	/* free element in cache */

/* drivers stack */
STACK	*stack_top = NULL;		/* top of stack area */
STACK	*stackp = NULL;			/* current stack element */

/* global variables for identify DVI */
uint2	stack_size;
uint4	num, den, mag;	/* scaling values */
uint4	max_h, max_w;	/* max printing size */
uint2	pages;		/* number of pages */
double	scalepoint;	/* dimension uint in DVI */
int	current_page;	/* prcessing page # */

/* printer status variables */
int2		prn_max_font =0;
int2		using_gset = 0;
int4		downloadcount = 0;
int4		h_base;		/* horizontal base DOT */
int4		v_base;		/* vertical base DOT */
struct jfatr	*fattr = NULL;	/* font attribute list */

/* options */
int	papersize = A4;		/* paper size */
int	landscape = FALSE;	/* true when landscape */
float	paperwidth = (float)210.0;	/* paper width (mm) */
float	paperlength = (float)297.0;	/* paper hgiht (mm) */
int	copypage = 1;		/* copy page */
int	startpage = -1;		/* printing start page */
int	endpage = MAXINT;	/* pringitn end page */
int4	port_h_base = H_INCHPOINT;	/* portraite mode base */
int4	port_v_base = V_INCHPOINT;
int4	land_h_base = H_INCHPOINT;	/* landscape mode base */
int4	land_v_base = V_INCHPOINT;
float	h_offset = (float)0.0;		/* horizontal offset (mm) */
float	v_offset = (float)0.0;		/* vertical offset (mm) */
int	usejxlfont = FALSE;	/* using JXL font flag */
int4	maxdownsize = -1;	/* max of download data size (byte) */
int	mincho_syotai = MINCHO_FACE;	/* Minchotai syotai # */
int	gothic_syotai = GOTHIC_FACE;	/* Gothictai syotai # */
#if MSDOS
int	verbose = TRUE;		/* verbose flag */
#else
int	verbose = FALSE;	/* verbose flag */
#endif
int	forcedownload = FALSE;	/* forcedownload flag */
float	magnification = (float)1.0;	/* magnifications */
int	cache_size = CACHE_SIZE;	/* default font cache size */
int	prpage = BOTHPAGE;	/* printout pages */

/* directory or file name */
char	*progname;		/* my name */
char	*fontdir = FONTDIR;	/* font exist directory */
char	*tfmdir = TFMDIR;	/* tfm exist directory */
char	*(fontpath[MAXPATH+1]);	/* grif file search path */
char	*(tfmpath[MAXPATH+1]);	/* tfm file search path */
char	*downinfo = DOWNINFO;	/* download information file */
char	*conffile = CONFFILE;	/* configuration file */
char	*infile = NULL;		/* input file name */
FILE	*dvifile = NULL;	/* input file */
FILE	*outfile = stdout;	/* output file */

/* option command name */
#define	OPTSTART	"start"		/* start page */
#define	OPTEND		"end"		/* end page */
#define	OPTPAPER	"paper"		/* paper size */
#define	OPTP_HIGHT	"p_height"	/* user paper length */
#define	OPTP_WIDTH	"p_width"	/* user paper width */
#define	OPTLANDSCAPE	"landscape"	/* landscape mode */
#define	OPTPORTRAIT	"portrait"	/* portrait mode */
#define	OPTPORTH_BASE	"porth_base"	/* portrait horizontal base (dots) */
#define	OPTPORTV_BASE	"portv_base"	/* portrait vertical base (dots) */
#define	OPTLANDH_BASE	"landh_base"	/* landscape horizontal base (dots) */
#define	OPTLANDV_BASE	"landv_base"	/* landscape vertical base (dots) */
#define	OPTH_OFFSET	"h_offset"	/* horizontal offset (mm) */
#define	OPTV_OFFSET	"v_offset"	/* vertical offset (mm) */
#define	OPTMAGNIFICATION "magnification"/* user magnification */
#define	OPTJXLFONT	"jxlfont"	/* using JXL font flag */
#define	OPTDOWNLOADSIZE	"downloadsize"	/* max download size */
#define	OPTFONTDIR	"fontdir"	/* font existing directory name */
#define	OPTTFMDIR	"tfmdir"	/* tfm existin directory name */
#define	OPTINFOFILE	"infofile"	/* download information file name */
#define	OPTFILE		"file"		/* input DVI file */
#define	OPTVERBOSE	"verbose"	/* verbose mode flag */
#define	OPTMINCHOFACE	"minchoface"	/* mincho face # */
#define	OPTGOTHICFACE	"gothicface"	/* gothic face # */
#define	OPTFORCEDOWN	"forcedownload"	/* force download (ignore fontinfo) */
#define	OPTSIDE		"side"		/* printout page side */
#define	OPTFONT		"font"		/* Japanese font attribute */

#define	BUFLEN		2048

#ifdef	toupeer
# undef	toupper
#endif
#define	toupper(c)	((((c) >= 'a') && ((c) <= 'z'))?((c) - 0x20):(c))

#ifdef	isdigit
# undef	isdigit
#endif
#define	isdigit(c)	(((c) >= '0') && ((c) <= '9'))

/* usage display */
void
usage()
{
    char	**p;
    static char	*(umsg[]) = {
	"options:",
	"-c <copy>	# of copy",
	"-s <page>	page # for start printing",
	"-e <page>	page # for end printing",
	"-t {a4,a5,b4,b5,hagaki,legal,letter,user} paper size",
	"-h <height>	paper height in mm (-t user  only)",
	"-w <width>	paper width in mm (-t user  only)",
	"-p		portrait mode",
	"-l		landscape mode",
	"-x <offset>	horizontal paper offset in mm",
	"-y <offset>	vertical paper offset in mm",
	"-m <mag>	magnifications",
	"-j		use JXL4 fonts for Kanji",
	"-i		use internal fonts for Kanji",
	"-d <bytes>	maximum download data size",
	"-f		force font download",
	"-F		use download information file",
	"-S		no message display",
	"-V		messages display",
	"-E		print even page only",
	"-O		print odd page only",
	NULL
    };
	    
    fprintf( stderr, "%s [options] dvi-file\n", progname );
    p = umsg;
    while( *p != NULL )
	fprintf( stderr, "%s\n", *(p++) );
    exit( 1 );
}

/* setup option variables from arguments */
void
set_optarg( argc, argv )
int	argc;
char	**argv;
{
    int	cmd;

    while( (cmd = getopt(argc, argv, "c:s:e:t:h:w:lpx:y:m:jid:fFSVEO")) != -1 ) {
	switch( cmd ) {
	  case 'c':	/* copy */
	    copypage = atoi( optarg );
	    break;
	  case 's':	/* start page */
	    startpage = atoi( optarg );
	    break;
	  case 'e':	/* end page */
	    endpage = atoi( optarg );
	    break;
	  case 't':	/* paper size */
	    if( strcmpn("a4", optarg) == 0 )
		papersize = A4;
	    else if( strcmpn("a5", optarg) == 0 )
		papersize = A5;
	    else if( strcmpn("hagaki", optarg) == 0 )
		papersize = HAGAKI;
	    else if( strcmpn("b4", optarg) == 0 )
		papersize = B4;
	    else if( strcmpn("b5", optarg) == 0 )
		papersize = B5;
	    else if( strcmpn("letter", optarg) == 0 )
		papersize = LETTER;
	    else if( strcmpn("legal", optarg) == 0 )
		papersize = LIGAL;
	    else if( strcmpn("user", optarg) == 0 )
		papersize = USER;
	    else
		usage();
	    break;
	  case 'h':	/* paper height */
	    sscanf( optarg, "%f", &paperlength );
	    if( (paperlength < (float)MINPAPERLEN) || (paperlength > (float)MAXPAPERLEN) ) {
		fprintf( stderr, "%s: cannot use such paper size\n", progname );
		exit( 1 );
	    }
	    break;
	  case 'w':	/* paper width */
	    sscanf( optarg, "%f", &paperwidth );
	    if( (paperwidth < (float)MINPAPERWID) || (paperwidth > (float)MAXPAPERWID) ) {
		fprintf( stderr, "%s: cannot use such paper size\n", progname );
		exit( 1 );
	    }
	    break;
	  case 'p':	/* portrait */
	    landscape = FALSE;
	    break;
	  case 'l':	/* landscape */
	    landscape = TRUE;
	    break;
	  case 'x':	/* horizontal offset */
	    sscanf( optarg, "%f", &h_offset );
	    break;
	  case 'y':	/* vertical offset */
	    sscanf( optarg, "%f", &v_offset );
	    break;
	  case 'm':	/* maginifications */
	    sscanf( optarg, "%f", &magnification );
	    if( magnification < (float)0.0 ) {
		fprintf( stderr, "%s: cannot magnificate %f\n",
			 progname, magnification );
		exit( 1 );
	    }
	    break;
	  case 'j':	/* use JXL font */
	    usejxlfont = TRUE;
	    break;
	  case 'i':	/* not use JXL font */
	    usejxlfont = FALSE;
	    break;
	  case 'd':	/* download size */
	    maxdownsize = (int4)atol( optarg );
	    break;
	  case 'f':	/* forcedownload */
	    forcedownload = TRUE;
	    break;
	  case 'F':	/* forcedownload no */
	    forcedownload = FALSE;
	    break;
	  case 'S':	/* silent */
	    verbose = FALSE;
	    break;
	  case 'V':	/* verbose */
	    verbose = TRUE;
	    break;
	  case 'E':	/* even page only */
	    prpage = EVENPAGE;
	    break;
	  case 'O':	/* odd page only */
	    prpage = ODDPAGE;
	    break;
	  default:
	    usage();
	}
    }

    /* check */
    if( landscape ) {
	h_base = land_h_base;
	v_base = land_v_base;
    }
    else {
	h_base = port_h_base;
	v_base = port_v_base;
    }
    h_base += (int4)( mm2dot( h_offset ) + 0.5 );
    v_base += (int4)( mm2dot( v_offset ) + 0.5 );
    if( v_base < 0 ) {
	fprintf( stderr, "%s: vertical base out of paper\n", progname );
	exit( 1 );
    }
    if( h_base < 0 ) {
	fprintf( stderr, "%s: horizontal base out of paper\n", progname );
	exit( 1 );
    }
    if( optind != (argc - 1) )
	usage();
    infile = malloc( strlen( argv[optind] ) + 1 );
    if( infile == NULL ) {
	fprintf( stderr, "%s: no memory\n", progname );
	exit( 1 );
    }
    strcpy( infile, argv[optind] );
}

/* string compare without case */
int
strcmpn( a, b )
char	*a, *b;
{
    while( *a & *b ) {
	if( toupper(*a) > toupper(*b) )
	    return 1;
	else if( toupper(*a) < toupper(*b) )
	    return -1;
	a++;	b++;
    }
    if( toupper(*a) == toupper(*b) )
	return 0;
    else
	return ((*a == '\0') ? -1 : 1);
}

/* setup search path */
void
setpath( path, str, max )
char	*(path[]);	/* path keep array */
char	*str;		/* bundled path string */
int	max;		/* array size */
{
    int		idx;	/* path index */
    char	*p;

    idx = 0;
    p = strtok( str, "; \t\n" );
    while( p != NULL ) {
	if( *p == '#' )	/* ignore comments */
	    break;
	path[idx] = p;
	idx++;
	if( idx >= max )
	    break;
	p = strtok( NULL, "; \t\n" );
    }

    for( ; idx <= max; idx++ )
	path[idx] = NULL;

    return;
}

/* setup Japanese font attribute list */
int
setjfontinfo( name, attr )
char	*name;
char	*attr;
{
    struct jfatr	*jfp;
    int			cmd;
    int			arg;

    /* allocate Jfont attribute element */
    jfp = (struct jfatr *)malloc( sizeof(struct jfatr) );
    if( jfp == NULL )
	goto MemError;
    jfp->name = malloc( strlen(name) + 1 );
    if( jfp->name == NULL ) {
	free( jfp );
	goto MemError;
    }
    strcpy( jfp->name, name );
    jfp->face = 0;
    jfp->weight = DEFAULT_WEIGHT;
    jfp->direction = YOKO;
    jfp->fillpattern = jfp->shadowpattern = jfp->outline = -1;
    jfp->next = NULL;

    /* set font attribute info. */
    if( attr != NULL ) {
	while( *attr ) {
	    cmd = *(attr++);
	    if( (cmd == '\n') || (cmd == '\t') || (cmd == ' ') )
		break;
	    arg = 0;
	    while( (*attr != '\0') && isdigit(*attr) )
		arg = arg * 10 + (*(attr++) - '0');

	    switch( cmd ) {
	      case 'j':	/* font face */
	      case 'J':
		jfp->face = (int1)arg;
		break;
	      case 'f':	/* fill pattern */
	      case 'F':
		jfp->fillpattern = (int1)arg;
		break;
	      case 'w':	/* storoke weight */
	      case 'W':
		jfp->weight = (int1)arg;
		break;
	      case 's':	/* shadow pattern */
	      case 'S':
		jfp->shadowpattern = (int1)arg;
		break;
	      case 'o':	/* outline size */
	      case 'O':
		jfp->outline = (int1)arg;
		break;
	      case 't':	/* tategaki */
	      case 'T':
		jfp->direction = TATE;
		break;
	      case 'y':	/* yokogaki */
	      case 'Y':
		jfp->direction = YOKO;
		break;
	      default:
		goto SyntaxError;
	    }
	}
    }
    if( jfp->face == 0 )
	goto SyntaxError;

    /* connect an elment to the list */
    jfp->next = fattr;
    fattr = jfp;

    return FALSE;

  SyntaxError:
    free( jfp->name );
    free( jfp );
  MemError:
    return TRUE;
}

/* setup option variables for later use */
void
set_options( cf )
FILE	*cf;	/* option description file descripter */
{
    char	lbuf[BUFLEN];
    char	*token, *arg;
    char	*fontname, *fontattr;

    while( fgets( lbuf, BUFLEN, cf ) != NULL ) {
	if( (lbuf[0] == '\0') || (lbuf[0] == '#') || (lbuf[0] == '\n') )
	    continue;
	token = strtok( lbuf, " \t\n" );
	arg = strtok( NULL, " \t\n" );
	if( strcmpn( OPTSTART, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    startpage = atoi( arg );
	}
	else if( strcmpn( OPTEND, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    endpage = atoi( arg );
	}
	else if( strcmpn( OPTPAPER, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    if( strcmpn("a4", arg) == 0 )
		papersize = A4;
	    else if( strcmpn("a5", arg) == 0 )
		papersize = A5;
	    else if( strcmpn("hagaki", arg) == 0 )
		papersize = HAGAKI;
	    else if( strcmpn("b4", arg) == 0 )
		papersize = B4;
	    else if( strcmpn("b5", arg) == 0 )
		papersize = B5;
	    else if( strcmpn("letter", arg) == 0 )
		papersize = LETTER;
	    else if( strcmpn("legal", arg) == 0 )
		papersize = LIGAL;
	    else if( strcmpn("user", arg) == 0 )
		papersize = USER;
	    else {
		fprintf( stderr, "%s: unknown paper size %s\n", progname, arg );
		exit( 1 );
	    }
	}
	else if( strcmpn( OPTP_HIGHT, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    sscanf( arg, "%f", &paperlength );
	    if( (paperlength < (float)MINPAPERLEN) || (paperlength > (float)MAXPAPERLEN) ) {
		fprintf( stderr, "%s: cannot use such paper size\n", progname );
		exit( 1 );
	    }
	}
	else if( strcmpn( OPTP_WIDTH, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    sscanf( arg, "%f", &paperwidth );
	    if( (paperwidth < (float)MINPAPERWID) || (paperwidth > (float)MAXPAPERWID) ) {
		fprintf( stderr, "%s: cannot use such paper size\n", progname );
		exit( 1 );
	    }
	}
	else if( strcmpn( OPTLANDSCAPE, token ) == 0 )
	    landscape = TRUE;
	else if( strcmpn( OPTPORTRAIT, token ) == 0 )
	    landscape = FALSE;
	else if( strcmpn( OPTPORTH_BASE, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    port_h_base = atoi( arg );
	}
	else if( strcmpn( OPTPORTV_BASE, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    port_v_base = atoi( arg );
	}
	else if( strcmpn( OPTLANDH_BASE, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    land_h_base = atoi( arg );
	}
	else if( strcmpn( OPTLANDV_BASE, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    land_v_base = atoi( arg );
	}
	else if( strcmpn( OPTH_OFFSET, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    sscanf( arg, "%f", &h_offset );
	}
	else if( strcmpn( OPTV_OFFSET, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    sscanf( arg, "%f", &v_offset );
	}
	else if( strcmpn( OPTMAGNIFICATION, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    sscanf( arg, "%f", &magnification );
	    if( magnification < (float)0.0 ) {
		fprintf( stderr, "%s: cannot magnificate %f\n",
			 progname, magnification );
		exit( 1 );
	    }
	}
	else if( strcmpn( OPTJXLFONT, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    if( strcmpn( "yes", arg ) == 0 )
		usejxlfont = TRUE;
	    else if( strcmpn( "no", arg ) == 0 )
		usejxlfont = FALSE;
	    else
		goto NoArg;	/* cause syntax error */
	}
	else if( strcmpn( OPTDOWNLOADSIZE, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    maxdownsize = (int4)atol( arg );
	}
	else if( strcmpn( OPTFONTDIR, token ) == 0 ) {
	    if( (arg == NULL) || (arg[0] == '\0') )	goto NoArg;
	    fontdir = malloc( strlen( arg ) + 1 );
	    if( fontdir == NULL ) {
		fprintf( stderr, "%s: no memory\n", progname );
		exit( 1 );
	    }
	    strcpy( fontdir, arg );
	}
	else if( strcmpn( OPTTFMDIR, token ) == 0 ) {
	    if( (arg == NULL) || (arg[0] == '\0') )	goto NoArg;
	    tfmdir = malloc( strlen( arg ) + 1 );
	    if( tfmdir == NULL ) {
		fprintf( stderr, "%s: no memory\n", progname );
		exit( 1 );
	    }
	    strcpy( tfmdir, arg );
	}
	else if( strcmpn( OPTINFOFILE, token ) == 0 ) {
	    if( (arg == NULL) || (arg[0] == '\0') )	goto NoArg;
	    downinfo = malloc( strlen( arg ) + 1 );
	    if( downinfo == NULL ) {
		fprintf( stderr, "%s: no memory\n", progname );
		exit( 1 );
	    }
	    strcpy( downinfo, arg );
	}
	else if( strcmpn( OPTFILE, token ) == 0 ) {
	    if( (arg == NULL) || (arg[0] == '\0') )	goto NoArg;
	    infile = malloc( strlen( arg ) + 1 );
	    if( infile == NULL ) {
		fprintf( stderr, "%s: no memory\n", progname );
		exit( 1 );
	    }
	    strcpy( infile, arg );
	}
	else if( strcmpn( OPTVERBOSE, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    if( strcmpn( "yes", arg ) == 0 )
		verbose = TRUE;
	    else if( strcmpn( "no", arg ) == 0 )
		verbose = FALSE;
	    else
		goto NoArg;	/* cuase syntax error */
	}
	else if( strcmpn( OPTMINCHOFACE, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    mincho_syotai = atoi( arg );
	    if( mincho_syotai < 0 ) {
		fprintf( stderr, "%s: negative Mincho face number %d\n",
			 progname, mincho_syotai );
		exit( 1 );
	    }
	}
	else if( strcmpn( OPTGOTHICFACE, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    gothic_syotai = atoi( arg );
	    if( gothic_syotai < 0 ) {
		fprintf( stderr, "%s: negative Gothic face number %d\n",
			 progname, gothic_syotai );
		exit( 1 );
	    }
	}
	else if( strcmpn( OPTFONT, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    fontname = arg;
	    fontattr = strtok( NULL, " \t" );
	    if( (fontname == NULL) || (fontattr == NULL) )
		goto NoArg;
	    if( setjfontinfo( fontname, fontattr ) )
		goto NoArg;
	}
	else if( strcmpn( OPTFORCEDOWN, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    if( strcmpn( "yes", arg ) == 0 )
		forcedownload = TRUE;
	    else if( strcmpn( "no", arg ) == 0 )
		forcedownload = FALSE;
	    else
		goto NoArg;	/* cuase syntax error */
	}
	else if( strcmpn( OPTSIDE, token ) == 0 ) {
	    if( arg == NULL )	goto NoArg;
	    if( strcmpn( "both", arg ) == 0 )
		prpage = BOTHPAGE;
	    else if( strcmpn( "even", arg ) == 0 )
		prpage = EVENPAGE;
	    else if( strcmpn( "odd", arg ) == 0 )
		prpage = ODDPAGE;
	    else
		goto NoArg;
	}
	else
	    goto NoArg;
    }

    /* check */
    if( landscape ) {
	h_base = land_h_base;
	v_base = land_v_base;
    }
    else {
	h_base = port_h_base;
	v_base = port_v_base;
    }
    h_base += (int4)( mm2dot( h_offset ) + 0.5 );
    v_base += (int4)( mm2dot( v_offset ) + 0.5 );
    if( v_base < 0 ) {
	fprintf( stderr, "%s: vertical base out of paper\n", progname );
	exit( 1 );
    }
    if( h_base < 0 ) {
	fprintf( stderr, "%s: horizontal base out of paper\n", progname );
	exit( 1 );
    }

    /* set font search path */
    setpath( fontpath, fontdir, MAXPATH );
    setpath( tfmpath,  tfmdir,  MAXPATH );

    return;

  NoArg:
    fprintf( stderr, "%s: command syntax error near %s\n", progname, token );
    exit( 1 );
}

/* main routine: analyze configuratin file and option */
int
main( argc, argv )
int	argc;
char	*argv[];
{
    char	*cnf;		/* configuration file name */
    FILE	*optfile;	/* conf file descripter */
    FILE	*dfont;		/* download information file */
#if MSDOS
    char	*efd, *etd;	/* environment variable value */
    char	lbuf[512];
#endif

#ifdef	DEBUG
    malloc_debug( 2 );
#endif

    /* set global */
    progname = argv[0];

    /* get configurations */
    cnf = getenv( CNFENV );	/* get configuration file name */
    if( cnf != NULL )
	conffile = cnf;
#if    MSDOS
    optfile = fopen( conffile, "rt" );
#else
    optfile = fopen( conffile, "r" );
#endif
    if( optfile != NULL ) {
	set_options( optfile );	/* if no conf file, use default */
	fclose( optfile );
    }
#if MSDOS	/* use environment variable on DOS */
    efd = getenv( "FONTDIR" );	/* get font directory name */
    if( efd != NULL )
    	fontdir = efd;
    etd = getenv( "TFMDIR" );	/* get tfm directory name */
    if( etd != NULL )
	tfmdir = etd;
#endif
    set_optarg( argc, argv );	/* next eval command through pipe */

    /* show copyright notice */
    if( verbose ) {
	fprintf( stderr, "TeX Printer driver" );
#if	ESCP
	fprintf( stderr, "for ESC/Page\n" );
#else
	fprintf( stderr, "for new Laser Shot (LIPS3)\n" );
#endif
	fprintf( stderr, "Copyright (C) 1990-1992 ASCII Corporation.\n" );
	fprintf( stderr, "Version %s\n", VERSION );
    }

#if	USEEMS
    open_EMS();
#endif
    
    /* check & do my work */
    if( infile == NULL ) {
	fprintf( stderr, "%s: no inputfile\n", progname );
	exit( 1 );
    }
    dvifile = fopen( infile, "r" );
#if	MSDOS		/* on UNIX dvipr will did it */
    if( dvifile == NULL ) {
	sprintf( lbuf, "%s.dvi", infile );
	dvifile = fopen( lbuf, "r" );
    }
#endif
    if( dvifile != NULL ) {
	if( makefontlist( dvifile ) )
	    exit( 1 );
	if( usejxlfont )
	    if( makecache() )
		exit( 1 );
#if MSDOS
	lopen();
#endif
	if( fontlist != NULL ) {	/* use any fonts */
	    if( verbose )
		fprintf( stderr, "Expanding:" );
	    if( download() )
		goto Error;
	    if( verbose )
		fprintf( stderr, "...Done.\n" );
	}
	else {				/* use no font */
	    if( forcedownload ) {	/* clear download info. file */
#if MSDOS
		dfont = fopen( downinfo, "wt" );
#else
		dfont = fopen( downinfo, "w" );
#endif
		if( dfont != NULL )
		    fclose( dfont );
	    }
	}

	if( verbose )
	    fprintf( stderr, "Printing:" );
	if( doprint( dvifile ) )
	    goto Error;
	if( verbose )
	    fprintf( stderr, "...Done.\n" );

	fclose( dvifile );
    }
    else
	fprintf( stderr, "%s: cannot open %s as input\n", progname, infile );
    exit( 0 );
  Error:
    exit( 1 );
}

/* search and open file */
FILE *
tryopen( path, fn )
char	*(path[]);		/* sarch path pool */
char	*fn;			/* basename */
{
    char	lbuf[BUFLEN];
    char	*p;
    FILE	*fp;

    for( p = *path; p != NULL; p = *(++path) ) {
	sprintf( lbuf, "%s/%s", p, fn );
	fp = fopen( lbuf, "r" );
	if( fp != NULL )
	    return fp;
    }
    return NULL;
}
