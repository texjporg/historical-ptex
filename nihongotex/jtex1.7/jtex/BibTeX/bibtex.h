/*
 * Main include file for BibTeX in C.
 *
 * Tim Morgan  2/15/88
 * Eduardo Krell 4/21/88
 */

/*
 * JBibTeX Version by Shouichi Matsui
 *  matsui@denken.or.jp (matsui%denken.or.jp@uunet.uu.net)
 *  1989-05-22
 */

#include <stdio.h>
#include <setjmp.h>
#include "site.h"

#undef	STAT
#undef	DEBUG

extern char *strncpy();

typedef FILE *file_ptr;
typedef FILE *palphafile;
#define	incr(x)	++(x)
#define	decr(x)	--(x)
extern int testaccess();
#define	true	1
#define	false	0
#define rewrite(f,n)	f = openf(n, "w")
#define	reset(f,n)	f = openf(n, "r")
#define	aclose(f)	if (f) (void) fclose(f)
#define	chr(x)		(x)
extern void lineread(), setpaths();
#define readln(f)	{register int c; while ((c=getc(f))!='\n' && c!=EOF); }
extern FILE *openf();
#define	uexit(x)	exit((int) (x))
#define	eof(f)		feof(f)

#ifdef KANJI
extern void PRINTSTR();
#define printstr(s,c)	PRINTSTR(logfile,s,c);PRINTSTR(stdout,s,c)
#else
#define	Fputs(stream, s)	(void) fputs(s, stream)
#define printstr(s,c)	fprintf(logfile,"%s%c",s,c);fprintf(stdout,"%s%c",s,c)
#endif

extern char **gargv;
extern int gargc;
