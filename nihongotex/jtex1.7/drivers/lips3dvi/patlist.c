/*
 *	patlist.c -- print LIPS3 pattern # list
 *
 *	Written by H. Nagahara on 8/19/91 13:45:40
 *	Copyright (C) 1991-1992 by ASCII Corporaton.
 *		SCCS memo: patlist.c 7.1
 */
#include	<stdio.h>
#include	"config.h"

#if	MSDOS
extern int	lopen(void);
extern int	lputchar(int);
extern int	lprintf(char*,...);
#else
# define	lprintf		printf
# define	lputchar	putchar
#endif

int main( argc, argv )
int	argc;
char	*argv[];
{
    int		x, y;
    int		pat;

#if	MSDOS
    lopen();
#endif
    
    lprintf( "\033%@" );		/* start of text-mode */
    lprintf( "\033P31;300;2;JPattern\033\\" );	/* start of JOB */
    lprintf( "\033<" );		/* soft reset */
    lprintf( "\033[14;;p" );	/* page format -> a4 portrait */
    lprintf( "\033[11l" );	/* line-column mode */

    x = y = 1;			/* start postition */
    for( pat = 0; pat <= 68; pat++ ) {
	lprintf( "\033[%d;%df", y, x );	/* move to start pos */
	lprintf( "\033[%d;0;0s", pat );	/* start filling */
	lprintf( "\033[%d;%df", y + 2, x + 10 );	/* move to end point */
	lprintf( "\033[8r" );		/* end filling */
	lprintf( "\033[%d;%df", y + 1, x + 12 );
	lprintf( "%d", pat );		/* print pattern number */
	x += 15;
	if( (pat % 6) == 5 ) {
	    x = 1;
	    y += 4;
	}
    }
    lputchar( 12 );			/* form feed */
    lprintf( "\033P0J\033\\" );		/* end of job */
}
