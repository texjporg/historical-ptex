#include	<stdio.h>
#include	<ctype.h>	/* isdigit(), isxdigit() macro */
#include	<strings.h>	/* bzero() function */
#include	<sys/param.h>	/* howmany(), MAX(), MIN() macro */

#define		BUFSIZE		1000
#define		xdigit(c)	((c)>='a' ? (c)-'a'+10\
					  : (c)>='A' ? (c)-'A'+10\
						     : (c)-'0')

extern int	numerator_x, denominator_x;
extern int	numerator_y, denominator_y;
extern int	level;

static int	bbw;
static int	newbbw;
static int	bbh;
static int	newbbh;
static int	graylevel;

static int	bit[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };


processChar()
{
	char	*processAttributes();
	char	*malloc();
	char	*image;
	int	*gray;

	startchar();
	encoding();
	swidth();
	dwidth();
	bbx();
	bitmap( processAttributes() );

	image = (char*)malloc( roundup(bbw,8) * bbh * sizeof(char) );
	bzero( image, roundup(bbw,8) * bbh * sizeof(char) );
	gray = (int*)malloc( roundup(newbbw,8) * newbbh * sizeof(int) );
	bzero( gray, roundup(newbbw,8) * newbbh * sizeof(int) );

	read_image( image );
	magnify( image, gray );
	write_image( gray );

	free( gray );
	free( image );

	endchar();
}


startchar()
{
	char	linebuf[BUFSIZE];

	get_line( linebuf );
	if ( !beginwith( linebuf, "STARTCHAR" ) ) {
		error( "STARTCHAR expected\n" );
	}
	put_line( linebuf );
}


encoding()
{
	char	linebuf[BUFSIZE];

	get_line( linebuf );
	if ( !beginwith( linebuf, "ENCODING" ) ) {
		error( "ENCODING expected\n" );
	}
	put_line( linebuf );
}


swidth()
{
	char	linebuf[BUFSIZE];
	int	arg1, arg2;
	int	newwx, newwy;

	get_line( linebuf );
	if ( !beginwith( linebuf, "SWIDTH" ) ) {
		error( "SWIDTH expected\n" );
	}
	if ( sscanf( linebuf, "SWIDTH%d%d", &arg1, &arg2 ) != 2 ) {
		error( "SWIDTH has illegal arguments\n" );
	}
	newwx = howmany( arg1 * numerator_x, denominator_x );
	newwy = howmany( arg2 * numerator_y, denominator_y );
	sprintf( linebuf, "SWIDTH %d %d", newwx, newwy );
	put_line( linebuf );
}


dwidth()
{
	char	linebuf[BUFSIZE];
	int	arg1, arg2;
	int	newwx, newwy;

	get_line( linebuf );
	if ( !beginwith( linebuf, "DWIDTH" ) ) {
		error( "DWIDTH expected\n" );
	}
	if ( sscanf( linebuf, "DWIDTH%d%d", &arg1, &arg2 ) != 2 ) {
		error( "DWIDTH has illegal arguments\n" );
	}
	newwx = howmany( arg1 * numerator_x, denominator_y );
	newwy = howmany( arg2 * numerator_y, denominator_y );
	sprintf( linebuf, "DWIDTH %d %d", newwx, newwy );
	put_line( linebuf );
}


bbx()
{
	char	linebuf[BUFSIZE];
	int	arg1, arg2, arg3, arg4;
	int	newbbox, newbboy;

	get_line( linebuf );
	if ( !beginwith( linebuf, "BBX" ) ) {
		error( "BBX expected\n" );
	}
	if ( sscanf( linebuf, "BBX%d%d%d%d", &arg1, &arg2, &arg3, &arg4 ) != 4
	  || arg1 < 0 || arg2 < 0 ) {
		error( "BBX has illegal arguments\n" );
	}
	bbw = arg1;
	newbbw = howmany( arg1 * numerator_x, denominator_x );
	bbh = arg2;
	newbbh = howmany( arg2 * numerator_y, denominator_y );
	graylevel = bbw * bbh * level / 100;

	newbbox = arg3 * numerator_x / denominator_x;
	newbboy = arg4 * numerator_y / denominator_y;
	sprintf( linebuf, "BBX %d %d %d %d", newbbw, newbbh, newbbox, newbboy );
	put_line( linebuf );
}


bitmap( linebuf )
char	*linebuf;
{
	if ( !beginwith( linebuf, "BITMAP" ) ) {
		error( "BITMAP expected\n" );
	}
	put_line( linebuf );
}


endchar()
{
	char	linebuf[BUFSIZE];

	get_line( linebuf );
	if ( !beginwith( linebuf, "ENDCHAR" ) ) {
		error( "ENDCHAR expected\n" );
	}
	put_line( linebuf );
}


read_image( image )
register char	*image;
{
	register int	x, y;
	char	linebuf[BUFSIZE];
	int	bytebuf[BUFSIZE];

	for ( y = 0; y < bbh; y ++ ) {
		get_line( linebuf );
		for ( x = 0; x < bbw; x += 8 ) {
			if ( !isxdigit(linebuf[x/4])
			  || !isxdigit(linebuf[x/4+1]) ) {
				error( "illegal raster data\n" );
			}
			bytebuf[x/8] = get_byte( linebuf+x/4 );
		}
		for ( x = 0; x < bbw; x ++ ) {
			*image++ = bytebuf[x/8] & bit[x%8];
		}
	}
}


get_byte( s )
register char	*s;
{
	int	byte = 0;
	byte |= xdigit(*s);
	byte <<= 4;
	s ++;
	byte |= xdigit(*s);
	return byte;
}


magnify( image, gray )
register char	*image;
register int	*gray;
{
	register int	x, y;

	for ( y = 0; y < bbh; y ++ ) {
		for ( x = 0; x < bbw; x ++ ) {
			if ( image[y*bbw + x] ) {
				countup_score( gray, x*newbbw, y*newbbh );
			}
		}
	}
}


countup_score( gray, ox, oy )
register int	*gray;
register int	ox;
register int	oy;
{
	register int	newx, newy;
	int	xlim = howmany( ox+newbbw, bbw );
	int	ylim = howmany( oy+newbbh, bbh );
/*
printf("ox=%2d, oy=%2d\n", ox/newbbw, oy/newbbh);
*/
	for ( newy = oy/bbh; newy < ylim; newy++ ) {
		for ( newx = ox/bbw; newx < xlim; newx++ ) {
			gray[newy*newbbw+newx] +=
				duplicate(ox,oy,ox+newbbw,oy+newbbh,
				newx*bbw,newy*bbh,(newx+1)*bbw,(newy+1)*bbh);
/*
printf("newx=%2d, newy=%2d, gray+=%2d\n",newx,newy,
				duplicate(ox,oy,ox+newbbw,oy+newbbh,
				newx*bbw,newy*bbh,(newx+1)*bbw,(newy+1)*bbh));
*/
		}
	}
}


duplicate( x1, y1, x2, y2, w1, h1, w2, h2 )
register int	x1;
register int	y1;
register int	x2;
register int	y2;
register int	w1;
register int	h1;
register int	w2;
register int	h2;
{
	return (MIN(x2,w2)-MAX(x1,w1)) * (MIN(y2,h2)-MAX(y1,h1));
}


write_image( gray )
register int	*gray;
{
	register int	x, y, i;
	for ( y = 0; y < newbbh; y ++ ) {
		register int	*row = gray + y*newbbw;
		for ( x = 0; x < newbbw; x += 8 ) {
			register int	byte = 0;
			for ( i = 0; i < 8 && x+i < newbbw; i ++ ) {
				if ( row[x + i] > graylevel ) {
					byte |= bit[i];
				}
			}
			printf( "%02X", byte );
		}
		printf( "\n" );
	}
}
