/*
 *	dvipr.c --- front end program for lips3dvi
 *
 *	Written by H. Nagahara on 2/10/92 08:58:13.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: dvipr.c 10.1
 */
#include	<stdio.h>
#include	"config.h"
#include	"lbp.h"

#ifdef	toupeer
# undef	toupper
#endif
#define	toupper(c)	((((c) >= 'a') && ((c) <= 'z'))?((c) - 0x20):(c))
#define	BLKSIZE		1048

/* library and other file functions declaration */
extern char		*malloc();
extern char		*getenv();
extern long		atol();
extern int		getopt();
extern char		*optarg;
extern int		optind, opterr;

static char	*infile = NULL;
static char	*progname;
static char	lpropt[512];
static int	filearg;

/* usage display */
static void
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
	    
    fprintf( stderr, "%s [options] dvi-file [lpr options]\n", progname );
    p = umsg;
    while( *p != NULL )
	fprintf( stderr, "%s\n", *(p++) );
    exit( 1 );
}

/* string compare without case */
static int
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

/* setup option variables from arguments */
static int
set_optarg( argc, argv )
int	argc;
char	**argv;
{
    int		cmd;
    static char	*(paper[]) = {
	"a4", "a5", "hagaki", "b4", "b5", "letter", "legal", "user", NULL
	};
    char	**p;
    int		okey;
    double	paperlength, paperwidth, magnification;

    while( (cmd = getopt(argc, argv, "c:s:e:t:h:w:lpx:y:m:jid:fFSVEO")) != -1 ) {
	switch( cmd ) {
	  case 'c':	/* copy page */
	    if( atoi( optarg ) <= 1 )
		return TRUE;
	    break;
	  case 's':	/* start page */
	  case 'e':	/* end page */
	    if( atoi( optarg ) < 0 )
		return TRUE;
	    break;
	  case 't':	/* paper size */
	    p = paper;
	    okey = FALSE;
	    while( *p != NULL ) {
		if( strcmpn( *(p++), optarg ) == 0 ) {
		    okey = TRUE;
		    break;
		}
	    }
	    if( !okey )
		return TRUE;
	    break;
	  case 'h':	/* paper height */
	    sscanf( optarg, "%f", &paperlength );
	    if( (paperlength < MINPAPERLEN) || (paperlength > MAXPAPERLEN) )
		return TRUE;
	    break;
	  case 'w':	/* paper width */
	    sscanf( optarg, "%f", &paperwidth );
	    if( (paperwidth < MINPAPERWID) || (paperwidth > MAXPAPERWID) )
		return TRUE;
	    break;
	  case 'm':	/* maginifications */
	    sscanf( optarg, "%f", &magnification );
	    if( magnification < 0.0 )
		return TRUE;
	    break;
	  case 'j':	/* use JXL font */
	  case 'i':	/* not use JXL font */
	  case 'd':	/* download size */
	  case 'f':	/* forcedownload */
	  case 'F':	/* forcedownload no */
	  case 'S':	/* silent */
	  case 'V':	/* verbose */
	  case 'p':	/* portrait */
	  case 'l':	/* landscape */
	  case 'x':	/* horizontal offset */
	  case 'y':	/* vertical offset */
	  case 'E':	/* even page print */
	  case 'O':	/* odd page print */
	    break;
	  default:
	    return TRUE;
	}
    }
    if( optind >= argc )
	usage();
    infile = malloc( strlen(argv[optind]) + 1 );
    if( infile == NULL ) {
	fprintf( stderr, "%s: no memory\n", progname );
	exit( 1 );
    }
    strcpy( infile, argv[optind] );
    filearg = optind;

    /* split options for lpr */
    lpropt[0] = '\0';
    if( ++optind < argc ) {
	while( optind < argc ) {
	    strcat( lpropt, argv[optind++] );
	    strcat( lpropt, " " );
	}
    }
    strcat( lpropt, LPRDEFOPT );
    return FALSE;
}

/* send options to pipe */
void
putoptions( f, argc, argv )
FILE	*f;	/* output pipe */
int	argc;
char	**argv;
{
    char	buf[512];
    int		i;
    int		len;

    /* set command buf */
    buf[0] = 0;
    for( i = 1; i < filearg; i++ ) {
	if( i > 1 )
	    strcat( buf, " " );
	strcat( buf, argv[i] );
    }

    if( fputc( (PROGID >> 8), f ) == EOF )	/* send ID */
	goto Error;
    if( fputc( (PROGID & 0xff), f ) == EOF )
	goto Error;
    len = strlen( buf );
    if( fputc( (len >> 8), f ) == EOF )		/* send cmd length */
	goto Error;
    if( fputc( (len & 0xff), f ) == EOF )
	goto Error;
    if( len > 0 )
	if( fwrite( buf, len, 1, f ) <= 0 )
	    goto Error;

    return;

  Error:
    fprintf( stderr, "%s: write erro to lpr\n", progname );
    exit( 2 );
}

/* cat dvi file */
void
catfile( out, in )
FILE	*out;
FILE	*in;
{
    char	buf[BLKSIZE];
    int		bytes;

    while( (bytes = fread(buf, 1, BLKSIZE, in)) > 0 ) {
	if( fwrite( buf, 1, bytes, out ) <= 0 )
	    goto Error;
    }

    fclose( in );
    return;

  Error:
    fprintf( stderr, "%s: write error to lpr\n", progname );
    exit( 2 );
}
    
/* main procedure */
int
main( argc, argv )
int	argc;
char	**argv;
{
    FILE	*lpr;
    FILE	*in;
    char	dvifile[1024];
    char	lprcmd[1024];

    /* set grobal */
    progname = malloc( strlen(argv[0]) + 1 );
    if( progname == NULL ) {
	fprintf( stderr, "???? no memory!!\n" );
	exit( 1 );
    }
    strcpy( progname, argv[0] );
    
    /* check options */
    if( set_optarg( argc, argv ) ) {
	usage();
	exit( 1 );
    }

    /* open dvi file */
    in = fopen( infile, "r" );
    if( in == NULL ) {
	sprintf( dvifile, "%s.dvi", infile );
	in = fopen( dvifile, "r" );
    }
    if( in == NULL ) {
	fprintf( stderr, "%s: cannot open %s\n", progname, infile );
	exit( 2 );
    }

    /* send options and dvi file to lpr command */
    sprintf( lprcmd, "%s %s", LPRCMD, lpropt );
    lpr = popen( lprcmd, "w" );
    if( lpr == NULL ) {
	fprintf( stderr, "%s: cannot exec %s\n", progname, LPRCMD );
	exit( 1 );
    }
    putoptions( lpr, argc, argv );
    catfile( lpr, in );
    pclose( lpr );
    exit( 0 );
}
