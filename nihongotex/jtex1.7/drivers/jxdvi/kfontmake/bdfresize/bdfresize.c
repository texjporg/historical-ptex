#include	<stdio.h>
#include	<ctype.h>	/* isdigit(), isxdigit macro */
#include	<strings.h>	/* bzero() function */
#include	<sys/param.h>	/* howmany() macro */

#define		BUFSIZE		1000

extern char	*optarg;
extern int	optind, opterr;

char	*cmdname;

int	line;
int	numerator_x = 1, denominator_x = 1;
int	numerator_y = 1, denominator_y = 1;
int	level = 30;

main( argc, argv )
int	argc;
char	*argv[];
{
	int	c, err;
	int	charcount;
	cmdname = argv[0];
	while ( (c = getopt( argc, argv, "w:h:f:" )) != EOF ) {
		switch ( c )  {
		case 'w':
			numerator_x = atoi( optarg );
			denominator_x = index(optarg,'/')
					? atoi( index(optarg,'/')+1 ) : 1;
			if ( numerator_x <= 0 || denominator_x <= 0 ) err ++;
			break;
		case 'h':
			numerator_y = atoi( optarg );
			denominator_y = index(optarg,'/')
					? atoi( index(optarg,'/')+1 ) : 1;
			if ( numerator_y <= 0 || denominator_y <= 0 ) err ++;
			break;
		case 'f':
			numerator_x = numerator_y = atoi( optarg );
			denominator_x = denominator_y = index(optarg,'/')
					? atoi( index(optarg,'/')+1 ) : 1;
			if ( numerator_x <= 0 || denominator_x <= 0 ) err ++;
			break;
		case '?':
			err ++;
			break;
		}
	}
	if ( err ) {
		usage( argv[0] );
	}
	if ( optind < argc && freopen( argv[optind], "r", stdin ) == NULL ) {
		perror( argv[optind] );
		exit( 1 );
	}
	charcount = processHeader();
	for ( ; charcount > 0; charcount -- ) {
		processChar();
	}
	processFooter();
}


int
processHeader()
{
	char	*processProperties();
	startfont();
	font();
	size();
	fontboundingbox();
	return chars( processProperties() );
}


processFooter()
{
	endfont();
}


char	*
processProperties()
{
	char	linebuf[BUFSIZE];
	int	arg;

	get_line( linebuf );
	if ( beginwith( linebuf, "STARTPROPERTIES" ) ) {
		if ( sscanf( linebuf, "STARTPROPERTIES%d", &arg ) != 1
		  || arg < 0 ) {
			error( "STARTPROPERTIES has illegal or no argument\n" );
		}
		put_line( linebuf );
		for ( ; arg > 0; arg -- ) {
			get_line( linebuf );
			if ( beginwith( linebuf, "FONT_ASCENT" ) ) {
				font_ascent( linebuf );
			} else if ( beginwith( linebuf, "FONT_DESCENT" ) ) {
				font_descent( linebuf );
			}
			put_line( linebuf );
		}
		get_line( linebuf );
		if ( !beginwith( linebuf, "ENDPROPERTIES" ) ) {
			error( "ENDPROPERTIES expected\n" );
		}
		put_line( linebuf );
		get_line( linebuf );
	}
	return linebuf;
}


font_ascent( linebuf )
char	*linebuf;
{
	int	arg;
	int	new;
	if ( sscanf( linebuf, "FONT_ASCENT%d", &arg ) != 1 || arg < 0 ) {
		error( "FONT_ASCENT has illegal arguments\n" );
	}
	new = howmany( arg * numerator_y, denominator_y );
	sprintf( linebuf, "FONT_ASCENT %d", new );
}


font_descent( linebuf )
char	*linebuf;
{
	int	arg;
	int	new;
	if ( sscanf( linebuf, "FONT_DESCENT%d", &arg ) != 1 || arg < 0 ) {
		error( "FONT_DESCENT has illegal arguments\n" );
	}
	new = arg * numerator_y / denominator_y;
	sprintf( linebuf, "FONT_DESCENT %d", new );
}


char	*
processAttributes()
{
	char	linebuf[BUFSIZE];

	get_line( linebuf );
	if ( beginwith( linebuf, "ATTRIBUTES" ) ) {
		put_line( linebuf );
		get_line( linebuf );
	}
	return linebuf;
}


startfont()
{
	char	linebuf[BUFSIZE];
	char	arg[BUFSIZE];

	get_line( linebuf );
	if ( !beginwith( linebuf, "STARTFONT" ) ) {
		error( "STARTFONT expected\n" );
	}
	if ( sscanf( linebuf, "STARTFONT%s", arg ) != 1 ) {
		error( "STARTFONT has illegal or no argument\n" );
	}
	if ( strcmp( arg, "2.1" ) ) {
		error( "STARTFONT version error\n" );
	}
	put_line( linebuf );
}


font()
{
	char	linebuf[BUFSIZE];

	get_line( linebuf );
	if ( !beginwith( linebuf, "FONT" ) ) {
		error( "FONT expected\n" );
	}
	put_line( linebuf );
	sprintf( linebuf, "COMMENT This font was automatically magnified" );
	put_line( linebuf );
	sprintf( linebuf, "COMMENT mag_x = %d/%d", numerator_x, denominator_x );
	put_line( linebuf );
	sprintf( linebuf, "COMMENT mag_y = %d/%d", numerator_y, denominator_y );
	put_line( linebuf );
	sprintf( linebuf, "COMMENT threshold = %d%%", level );
	put_line( linebuf );
}


size()
{
	char	linebuf[BUFSIZE];
	int	arg1, arg2, arg3;
	int	newsize;

	get_line( linebuf );
	if ( !beginwith( linebuf, "SIZE" ) ) {
		error( "SIZE expected\n" );
	}
	if ( sscanf( linebuf, "SIZE%d%d%d", &arg1, &arg2, &arg3 ) != 3
	  || arg1 <= 0 || arg2 <= 0 || arg3 <= 0 ) {
		error( "SIZE has illegal arguments\n" );
	}
	newsize = howmany( arg1 * numerator_y, denominator_y );
	sprintf( linebuf, "SIZE %d %d %d", newsize, arg2, arg3 );
	put_line( linebuf );
}


fontboundingbox()
{
	char	linebuf[BUFSIZE];
	int	arg1, arg2, arg3, arg4;
	int	new1, new2, new3, new4;

	get_line( linebuf );
	if ( !beginwith( linebuf, "FONTBOUNDINGBOX" ) ) {
		error( "FONTBOUNDINGBOX expected\n" );
	}
	if ( sscanf( linebuf, "FONTBOUNDINGBOX%d%d%d%d",
				&arg1, &arg2, &arg3, &arg4 ) != 4
	  || arg1 <= 0 || arg2 <= 0 ) {
		error( "FONTBOUNDINGBOX has illegal arguments\n" );
	}
	new1 = howmany( arg1 * numerator_x, denominator_x );
	new2 = howmany( arg2 * numerator_y, denominator_y );
	new3 = arg3 * numerator_x / denominator_x;
	new4 = arg4 * numerator_y / denominator_y;
	sprintf( linebuf, "FONTBOUNDINGBOX %d %d %d %d", new1,new2,new3,new4 );
	put_line( linebuf );
}


chars( linebuf )
char	*linebuf;
{
	int	arg;

	if ( !beginwith( linebuf, "CHARS" ) ) {
		error( "CHARS expected\n" );
	}
	if ( sscanf( linebuf, "CHARS%d", &arg ) != 1 || arg < 0 ) {
		error( "CHARS has illegal or no argument\n" );
	}
	put_line( linebuf );
	return arg;
}


endfont()
{
	char	linebuf[BUFSIZE];

	get_line( linebuf );
	if ( !beginwith( linebuf, "ENDFONT" ) ) {
		error( "ENDFONT expected\n" );
	}
	put_line( linebuf );
}


get_line( s )
char	*s;
{
	while ( gets( s ) != NULL ) {
		line ++;
		if ( s[0] == '\0' || beginwith( s, "COMMENT" ) ) {
			put_line( s );
		} else {
			return;
		}
	}
	error( "EOF detected\n" );
}


put_line( s )
char	*s;
{
	puts( s );
}


beginwith( line, s )
char	*line;
char	*s;
{
	return !strncmp( line, s, strlen(s) );
}


error( fmt, a, b, c, d, e, f, g )
char	*fmt;
{
	char	buf[100];
	strcat( buf, "%s: line %d: " );
	strcat( buf, fmt );
	fprintf( stderr, buf, cmdname, line, a, b, c, d, e, f, g );
	exit( 1 );
}


usage()
{
	fprintf( stderr, "usage: %s [-options] [bdffile]\n", cmdname );
	fprintf( stderr, "\t-f <factor> : \n" );
	fprintf( stderr, "\t-w <factor> : (width only)\n" );
	fprintf( stderr, "\t-h <factor> : (height only)\n" );
	fprintf( stderr, "\t        <factor> ::= digits | digits/digits)\n" );
	exit( 1 );
}
