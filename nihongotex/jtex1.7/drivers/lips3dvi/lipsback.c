/*
 *	lipsback.c --- backend program for lips3dvi
 *
 *	Written by H. Nagahara on 2/10/92 08:58:53.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: lipsback.c 10.1
 */
#include	<stdio.h>
#include	<signal.h>
#include	"config.h"

extern	char	*strtok();

#define		BLKSIZE		1024
#define		MAXOPT		20

char	fname[BLKSIZE];

void
die()
{
    unlink( fname );
    exit( 1 );
}

int
main( argc, argv )
int	argc;
char	**argv;
{
    int		high, low;
    int		id, len;
    char	optbuf[BLKSIZE], buf[BLKSIZE];
    FILE	*tmp;
    int		bytes;
    char	*(barg[MAXOPT]);
    char	*op;
    int		n;
    int		child;

    /* get ID */
    high = getchar();
    low = getchar();
    if( (high == EOF) || (low == EOF) )
	goto Error;
    id = high * 256 + low;
    if( id != PROGID ) {
	fprintf( stderr, "%s: invaild ID %d\n", argv[0], id );
	exit( 2 );
    }

    /* get options */
    high = getchar();
    low = getchar();
    if( (high == EOF) || (low == EOF) )
	goto Error;
    len = high * 256 + low;
    if( len > 0 )
	if( fread( optbuf, 1, len, stdin ) < len )
	    goto Error;
    optbuf[len] = '\0';

    /* copy dvi */
    sprintf( fname, "%s.%d", TMPFNAME, getpid() );
    tmp = fopen( fname, "w" );
    if( tmp == NULL ) {
	fprintf( stderr, "%s: cannot open %s\n", argv[0], fname );
	exit( 2 );
    }

    /* set signal handler for 'lprm' */
    signal( SIGINT, die );

    /* copy body of dvi */
    while( (bytes = fread(buf, 1, BLKSIZE, stdin)) > 0 ) {
	if( fwrite( buf, 1, bytes, tmp ) <= 0 ) {
	    fprintf( stderr, "%s: write error to %s\n", argv[0], fname );
	    die();
	}
    }
    fclose( tmp );

    /* exec body program */
    barg[0] = MAINPROG;
    op = strtok( optbuf, " " );
    n = 1;
    while( op != NULL ) {
	barg[n++] = op;
	op = strtok( NULL, " " );
    }
    barg[n++] = fname;
    barg[n] = NULL;
    if( (child = fork()) == 0 ) {
	execv( MAINPROG, barg );
	fprintf( stderr, "%s: cannot exec %s\n", argv[0], MAINPROG );
	exit( 2 );
    }
    else if( child < 0 ) {
	fprintf( stderr, "%s: cannot fork\n", argv[0] );
	die();
    }
    else
	wait( NULL );

    /* dispose temp file */
    unlink( fname );
    exit( 0 );

  Error:
    fprintf( stderr, "%s: read error on stdin\n", argv[0] );
    exit( 1 );
}
