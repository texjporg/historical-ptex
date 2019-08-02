/*
 * Main include file for tangle etc. in C
 *
 * Tim Morgan  2/11/88
 */

#include <stdio.h>
#include "site.h"

/*
 * Global routines implemented as macros, plus some type coercion stuff.
 */

#define	toint(x)	((integer) (x))
#define	odd(x)		((x) % 2)
#define	putbyte(x,f)	putc(((x)&255), f)
#define	round(x)	zround((double)(x))
#define incr(x)		++x
#define	decr(x)		--x
#define	trunc(x)	( (integer) (x) )
#define	readln(f)	{register c; while((c=getc(f)) != '\n' && c != EOF);}
#define	read(f, b)	b = getc(f)
#define	input3ints(a,b,c)	zinput_3ints(&a, &b, &c)
#define zfseek(f,n,w)	(void) fseek(f, (long) n, (int) w)
#define eof(f)		test_eof(f)
#define	abs(x)		((x>=0)?(x):(-(x)))
#define	fabs(x)		((x>=0.0)?(x):(-(x)))
#define	Fputs(stream, s)	(void) fputs(s, stream)
#define rewrite(f,n)	f = openf(n+1, "w")
#define	reset(f,n)	f = openf(n+1, "r")
#define flush(f)	(void) fflush(f)
#define	true		1
#define	false		0
#define	chr(x)		(x)
#define	ord(x)		(x)
#define	vgetc(f)	(void) getc(f)
#define	uexit(x)	exit(x)
#define even
#define evenoverlap	evenoverlapbody	/* for forward */
#define oddoverlap	oddoverlapbody	/* for forward */
#define oddnldot	oddnldotbody	/* for forward */
#define evennldot	evennldotbody	/* for forward */

FILE *openf();

/*
 * Global Types
 */
typedef FILE	*text, *file_ptr;
typedef char	*ccharpointer;

/*
 * Global Data and Datastructures
 */

extern integer argc;
extern integer zround();
extern integer inputint();


/*
 * One global routine: test_eof
 * Return true if we're at the end of the file, else false.
 */
test_eof(file)
FILE *file;
{
    int ch;

    if (feof(file))
	return(true);
    if ((ch = getc(file)) == EOF)
	return(true);
    (void) ungetc(ch,file); /* yuck */
    return(false);
}

/*
 * Next, efficiency for Unix systems: use write() instead of stdio
 * to write to the dvi file.
 */
#ifndef	unix
#define	writedvi(a,b)	(void) fwrite((char *) &dvibuf[a], sizeof(dvibuf[a]), (int)(b-a+1), dvifile)
#define	dumpcore()	exit(1)
#else	/* unix */
#define	writedvi(a,b)	(void) write(fileno(dvifile), (char *) &dvibuf[a], (int)(b-a+1))
#define	dumpcore	abort
#endif	/* unix */
