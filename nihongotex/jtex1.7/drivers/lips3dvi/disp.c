/*
 *	disp.c -- echo any arguments
 *
 *	Written by H. Nagahara on 17 Aug. 1990
 *	Copyright (C) 1990 by H. Nagahara.  All rights reserved.
 *
 * Syntax:
 *	echo [options] <message>
 *	<message> may include below escape characters.
 *	`"'(double quote) is string seperater.
 *	\b	bell
 *	\r	carrige return
 *	\n	line feed
 *	\t	tab
 *	\"	double quote
 *	\\	backslash (yen mark)
 *	\$	doller mark
 *	\%	parcent
 *	$xx	hex code (1 byte)
 *	%xxxx	JIS code (2 bytes)
 * options:
 *	-n	suppress newline
 */
#include	<stdio.h>
#include	<fcntl.h>
#include	<io.h>
#include	<jstring.h>

#define		TRUE	1
#define		FALSE	0

void	putstring( char * );
int	ishex( int );
int	hex( int );

int
main( argc, argv )
int	argc;
char	*argv[];
{
	int	i;
	int	supress = FALSE;

	(void)setmode( fileno(stdout), O_BINARY );
	for( i = 1; i < argc; i++ ) {
		if( (*argv[i] == '-') && (*(argv[i] + 1) == 'n') ) {
			supress = TRUE;
			continue;
		}
		putstring( argv[i] );
	}
	if( !supress ) {
		putchar( '\r' );
		putchar( '\n' );
	}
	return 0;
}

void
putstring( s )
char	*s;
{
	unsigned int	v;

	while( *s ) {
		if( *s == '\\' ) {
			switch( *(++s) ) {
			    case 'b':
				putchar( '\b' );
				break;
			    case 'r':
				putchar( '\r' );
				break;
			    case 'n':
				putchar( '\n' );
				break;
			    case 't':
				putchar( '\t' );
				break;
			    case '"':
				putchar( '"' );
				break;
			    case '\\':
				putchar( '\\' );
				break;
			    case '$':
				putchar( '$' );
				break;
			    case '%':
				putchar( '%' );
				break;
			    default:
				putchar( *s );
				break;
			}
			s++;
		}
		else if( *s == '$' ) {
			s++; v = 0;
			if( ishex( *s ) ) {
				v = hex( *s );
				s++;
				if( ishex( *s ) ) {
					v = v * 16 + hex( *s );
					s++;
				}
			}
			putchar( v );
		}
		else if( *s == '%' ) {
			s++; v = 0;
			if( ishex( *s ) ) {
				v = hex( *s );
				s++;
				if( ishex( *s ) ) {
					v = v * 16 + hex( *s );
					s++;
					if( ishex( *s ) ) {
						v = v * 16 + hex( *s );
						s++;
						if( ishex( *s ) ) {
							v = v * 16 + hex( *s );
							s++;
						}
					}
				}
			}
			v = jistojms( v );
			putchar( v >> 8 );
			putchar( v & 0xff );
		}
		else
			putchar( *(s++) );
	}
}

int
ishex( c )
int	c;
{
	if( ((c >= '0') && (c <= '9'))
	 || ((c >= 'a') && (c <= 'f'))
	 || ((c >= 'A') && (c <= 'F')) )
		return TRUE;
	else
		return FALSE;
}

int
hex( c )
int	c;
{
	if( ((c >= '0') && (c <= '9')) )
		return( c - '0' );
	if( ((c >= 'a') && (c <= 'f')) )
		return( c - 'a' + 10 );
	if( ((c >= 'A') && (c <= 'F')) )
		return( c - 'A' + 10 );
	else
		return 0;
}


