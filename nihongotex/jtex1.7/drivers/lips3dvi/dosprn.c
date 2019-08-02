/*
 *	dosprn.c -- printer handling routine for PC9801
 *
 *	Written by H. Nagahara on 2/10/92 08:57:58.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: dosprn.c 10.1
 */
#include    <stdio.h>
#include    "config.h"
#include    <dos.h>
#include    <varargs.h>

#ifdef	FILTER
#  include	<fcntl.h>
#  include	<io.h>
#endif

#if	PC98
# define	PRNBIOS		0x1a	/* printer BIOS int */
#endif
#if	J3100
# define	PRNBIOS		0x17	/* printer BIOS int */
#endif
#if	DOSV
# define	BIOSWORK	0x40	/* BIOS work area segment */
# define	LPT1_PORT	8	/* BIOS work area LPT1 address */
#endif

#define	BUFSIZE		2048

static char		buf[BUFSIZE];
int	lopen(void);
int	lputchar(int);
int	lprintf(char*,...);
#if	DOSV
static	unsigned int	ioport;		/* LPT1 I/O port address */
#endif

/* initialize printer BIOS */
int
lopen()
{
#if	PC98
    union REGS	in, out;
#endif

#ifdef	FILTER
	setmode( fileno(stdout), O_BINARY );
	return 0;
#endif

#if	PC98
    in.h.ah = 0x10;
    int86( PRNBIOS, &in, &out );
    return out.h.al;
#endif

#if	J3100
    return 0;
#endif

#if	DOSV
    _asm {
	push	es			// keep current ES
	mov	ax, BIOSWORK		// load BIOS work segment to ES
	mov	es, ax
	mov	ax, es:[LPT1_PORT]	// get IO port addr. of LPT1:
	pop	es
	mov	ioport, ax		// set IO addr to ioport    
	xor	ax, 0xff		// invert retun value: 0 -> error
					//  because 0 means no printer port	    
    }
    return 0;
#endif
}

/* send a byte to printer */
int
lputchar( c )
int	c;
{
#if	!DOSV
    union REGS	in, out;
#endif

#ifdef	FILTER
	putchar( c );
	return c;
#endif

#if	PC98
    do {
	in.h.ah = 0x11;
	in.h.al = (unsigned char)c;
	int86( PRNBIOS, &in, &out );
    } while( out.h.ah != 0x01 );
    return c;		/* always return success */
#endif

#if	J3100
    do {
	in.h.ah = 0x84;	/* print without Kanji code conversion */
	in.h.al = (unsigned char)c;
	in.x.dx = 0;	/* device # */
	int86( PRNBIOS, &in, &out );
    } while( out.h.ah & 0x29 );
    return c;
#endif

#if	DOSV
    _asm {
	mov	dx, ioport		// get IO addr.
	inc	dx			// DX -> status port
wait_busy:
	in	al, dx	    		// get status
	nop
	nop
	nop
	and	al, 0x80		// busy?
	jz	wait_busy
	dec	dx			// DX -> data port
	mov	ax, c			// set output data
	out	dx, al
	inc	dx			// DX -> cntrol port
	inc	dx
	in	al, dx			// get current cmd
	nop
	nop
	nop
	nop
	nop
	or	al, 1			// set strobe
	out	dx, al			// out strobe
	jmp	complete0
complete0:
	jmp	complete1
complete1:
	jmp	complete2
complete2:    
	and	al, 0x0fe	    	// reset strobe
	out	dx, al
    }
    return c;
#endif
}

/* formatted print out to printer */
int
lprintf( va_alist )
va_dcl
{
    va_list	args;
    char	*fmt;
    int		bytes;
    register int i;

    /* format paramters */
    va_start( args );
    fmt = va_arg( args, char * );
    bytes = vsprintf( buf, fmt, args );

    /* printout string */
    for( i = 0; i < bytes; i++ )
	lputchar( buf[i] );

    return bytes;
}	

#ifdef	STANDALONE
int
main( argc, argv )
int	argc;
char	*argv[];
{
	FILE	*prfile;
	int	gotten;
	int	i;

	if( argc == 1 )
		prfile = stdin;
	else if( argc == 2 ) {
		prfile = fopen( argv[1], "rb" );
		if( prfile == NULL ) {
			fprintf( stderr, "cannot open %s\n", argv[1] );
			exit( 1 );
		}
	}
	else {
		fprintf( stderr, "Usagge: %s <input_file>\n", argv[0] );
		exit( 1 );
	}
	
	(void)lopen();

	while( (gotten = fread( buf, 1, BUFSIZE, prfile )) > 0 ) {
		for( i = 0; i < gotten; i++ )
			(void)lputchar( buf[i] );
	}
	
	return 0;
}
#endif	/* STANDALONE */
