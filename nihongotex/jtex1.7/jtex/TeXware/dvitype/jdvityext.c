/* External procedures for dvitype				*/
/*   Written by: H. Trickey, 2/19/83 (adapted from TeX's ext.c) */

#include <stdio.h>
#include "site.h"	/* defines default TEXFONTS path */

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
char *fontpath;
extern char *getenv();
extern FILE *dvityout;

/*
 * setpaths is called to set up the pointer fontpath
 * as follows:  if the user's environment has a value for TEXFONTS
 * then use it;  otherwise, use defaultfontpath.
 */
setpaths()
{
	register char *envpath;
	
	if ((envpath = getenv("TEXFONTS")) != NULL)
	    fontpath = envpath;
	else
	    fontpath = TEXFONTS;
}

#define namelength 100   /* should agree with dvitype.ch */
extern char curname[],realnameoffile[]; /* these have size namelength */

/*
 *	testaccess(amode,filepath)
 *
 *  Test whether or not the file whose name is in the global curname
 *  can be opened for reading (if mode=READACCESS)
 *  or writing (if mode=WRITEACCESS).
 *
 *  The filepath argument is one of the ...FILEPATH constants defined below.
 *  If the filename given in curname does not begin with '/', we try 
 *  prepending all the ':'-separated areanames in the appropriate path to the
 *  filename until access can be made, if it ever can.
 *
 *  The realnameoffile global array will contain the name that yielded an
 *  access success.
 */

#define READACCESS 4
#define WRITEACCESS 2

#define NOFILEPATH 0
#define FONTFILEPATH 3

testaccess(amode,filepath)
int amode,filepath;
{
    register boolean ok;
    register char *p;
    char *curpathplace;
    int f;
    
    switch(filepath) {
	case NOFILEPATH: curpathplace = NULL; break;
	case FONTFILEPATH: curpathplace = fontpath; break;
	}
    if (curname[0]=='/')	/* file name has absolute path */
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
	while (p < &(realnameoffile[namelength]))
	    *p++ = ' ';
	}
    return (ok);
}

/*
 * packrealnameoffile(cpp) makes realnameoffile contain the directory at *cpp,
 * followed by '/', followed by the characters in curname up until the
 * first blank there, and finally a '\0'.  The cpp pointer is left pointing
 * at the next directory in the path.
 * But: if *cpp == NULL, then we are supposed to use curname as is.
 */
static packrealnameoffile(cpp)
    char **cpp;
{
    register char *p,*realname;
    
    realname = realnameoffile;
    if ((p = *cpp)!=NULL) {
	while ((*p != ':') && (*p != '\0')) {
	    *realname++ = *p++;
	    if (realname == &(realnameoffile[namelength-1]))
		break;
	    }
	if (*p == '\0') *cpp = NULL; /* at end of path now */
	else *cpp = p+1; /* else get past ':' */
	*realname++ = '/';  /* separate the area from the name to follow */
	}
    /* now append curname to realname... */
    p = curname + 1;
    while (*p != ' ') {
	if (realname >= &(realnameoffile[namelength-1])) {
	    (void) fprintf(stderr,"! Full file name is too long\n");
	    break;
	    }
	*realname++ = *p++;
	}
    *realname = '\0';
}

static char **gargv;
argv(n, buf)
int n;
char buf[];
{
    (void) sprintf(buf+1, "%s ", gargv[n]);
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

    --name;	/* All names are indexed starting at 1 */
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
    (void) fprintf(dvityout, fmt, r);
}

/* Return true on end of line or eof of file, else false */
eoln(f)
FILE *f;
{
    register int c;

    if (feof(f)) return(1);
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
