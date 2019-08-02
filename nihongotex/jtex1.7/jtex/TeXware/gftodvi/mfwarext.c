/*
 * External procedures for all mfware WEB programs
 *
 * Pierre A. MacKay
 * The various routines consolidated into this package 
 * were produced over the past six years, by a number
 * of contributors.  Howard Trickey and Tim Morgan
 * are the principal authors.
 */

#include <stdio.h>
#include "site.h"
#ifdef	SYSV
#include <string.h>
extern int sprintf();
#else
#include <strings.h>
extern char *sprintf();
#endif

#define	TRUE	1
#define	FALSE	0

integer argc;
char *texfontpath;
char *gffontpath;
char *pkfontpath;
extern char *getenv();

static char **gargv;
argv(n, buf)
int n;
char buf[];
{
    (void) strcpy(buf+1, gargv[n]);
    (void) strcat(buf+1, " ");
}

main(ac, av)
char *av[];
{
    argc = ac;
    gargv = av;
    main_body();
}

/* Same routine as index() aka strchr() */
#ifndef	SYSV
char *char_index(cp, c)
char *cp, c;
{
    while (*cp != c && *cp) ++cp;
    if (*cp) return(cp);
    return(0);
}
#else	/* not SYSV */
#define	char_index	strchr
#endif

/* Open a file; don't return if error */
FILE *openf(name, mode)
char *name, *mode;
{
    FILE *result;
    char *cp;

    cp = char_index(name, ' ');
    if (cp) *cp = '\0';
    result = fopen(name, mode);
    if (result) return(result);
    perror(name);
    exit(1);
    /*NOTREACHED*/
}

/* Print real number r in format n:m */
printreal(r, n, m)
double r;
int n,m;
{
    char fmt[50];

    (void) sprintf(fmt, "%%%d.%df", n, m);
    (void) printf(fmt, r);
}

/* Return true on end of line or eof of file, else false */
eoln(f)
FILE *f;
{
    register int c;

    if (feof(f)) exit(1);
    c = getc(f);
    if (c != EOF)
	(void) ungetc(c, f);
    return (c == '\n' || c == EOF);
}

integer zround(f)
double f;
{
    if (f >= 0.0) return(f + 0.5);
    return(f - 0.5);
}

/* Read an integer in from file f; read past the subsequent end of line */
integer inputint(f)
FILE *f;
{
    char buffer[20];

    if (fgets(buffer, sizeof(buffer), f)) return(atoi(buffer));
    return(0);
}

zinput_3ints(a,b,c)
integer *a, *b, *c;
{
    while (scanf("%ld %ld %ld\n", a, b, c) != 3)
	(void) fprintf(stderr, "Please enter three integers\n");
}

/*
 * setpaths is called to set up the pointer fontpath
 * as follows:  if the user's environment has a value for 
 * one of: TEXFONTS, PKFONTS, GFFONTS
 * then use it;  otherwise, use defaultfontpath.
 * TEXFONTS is the traditional place for TFM files, and
 * used to be used indiscriminately for raster files as well
 * It now seems desirable to separate these
 */
setpaths()
{
	register char *envpath;
	
	if ((envpath = getenv("TEXFONTS")) != NULL)
	    texfontpath = envpath;
	else
	    texfontpath = TEXFONTS;
	if ((envpath = getenv("GFFONTS")) != NULL)
	    gffontpath = envpath;
	else
	    gffontpath = TEXFONTS;
	if ((envpath = getenv("PKFONTS")) != NULL)
	    pkfontpath = envpath;
	else
	    pkfontpath = TEXFONTS;
}

extern char nameoffile[],realnameoffile[]; 
	/* should have length FILENAMESIZE as goven in site.h */

/*
 *	testaccess(amode,filepath)
 *
 *  Test whether or not the file whose name is in the global nameoffile
 *  can be opened for reading (if mode=READACCESS)
 *  or writing (if mode=WRITEACCESS).
 *
 *  The filepath argument is one of the ...FILEPATH constants defined below.
 *  If the filename given in nameoffile does not begin with '/', we try 
 *  prepending all the ':'-separated areanames in the appropriate path to the
 *  filename until access can be made, if it ever can.
 *
 *  The realnameoffile global array will contain the name that yielded an
 *  access success.
 */

#define READACCESS 4
#define WRITEACCESS 2

#define NOFILEPATH 0
#define TEXFONTFILEPATH 3
#define GFFONTFILEPATH 4
#define PKFONTFILEPATH 5

testaccess(amode,filepath)
int amode,filepath;
{
    register boolean ok;
    register char *p;
    char *curpathplace;
    int f;
    
    switch(filepath) {
	case NOFILEPATH: curpathplace = NULL; break;
	case TEXFONTFILEPATH: curpathplace = texfontpath; break;
	case GFFONTFILEPATH: curpathplace = gffontpath; break;
	case PKFONTFILEPATH: curpathplace = pkfontpath; break;
	}
    if (nameoffile[1]=='/')	/* file name has absolute path */
	curpathplace = NULL;
    do {
	packrealnameoffile(&curpathplace);
	if (amode==READACCESS)
	    /* use system call "access" to see if we could read it */
	    if (access(realnameoffile,READACCESS)==0) ok = TRUE;
	    else ok = FALSE;
	else {
	    /* WRITEACCESS: use creat to see if we could create it, but close
	    the file again if we're OK, to let pc open it for real */
	    f = creat(realnameoffile,0666);
	    ok = (f >= 0);
	    if (ok)
		(void) close(f);
	    }
    } while (!ok && curpathplace != NULL);
    if (ok) {  /* pad realnameoffile with blanks, as Pascal wants */
	for (p = realnameoffile; *p != '\0'; p++)
	    /* nothing: find end of string */ ;
	while (p < &(realnameoffile[FILENAMESIZE]))
	    *p++ = ' ';
    }
    return (ok);
}

/*
 * packrealnameoffile(cpp) makes realnameoffile contain the directory at *cpp,
 * followed by '/', followed by the characters in nameoffile up until the
 * first blank there, and finally a '\0'.  The cpp pointer is left pointing
 * at the next directory in the path.
 * But: if *cpp == NULL, then we are supposed to use nameoffile as is.
 */
static packrealnameoffile(cpp)
    char **cpp;
{
    register char *p,*realname;
    
    realname = realnameoffile;
    if ((p = *cpp)!=NULL) {
	while ((*p != ':') && (*p != '\0')) {
	    *realname++ = *p++;
	    if (realname == &(realnameoffile[FILENAMESIZE-1]))
		break;
	}
	if (*p == '\0') *cpp = NULL; /* at end of path now */
	else *cpp = p+1; /* else get past ':' */
	*realname++ = '/';  /* separate the area from the name to follow */
    }
    /* now append nameoffile to realname... */
    p = nameoffile + 1;
    while (*p != ' ') {
	if (realname >= &(realnameoffile[FILENAMESIZE-1])) {
	    (void) fprintf(stderr,"! Full file name is too long\n");
	    break;
	}
	*realname++ = *p++;
    }
    *realname = '\0';
}
