/*
 *	lbp.c -- printer dependent routines
 *		 This file is for Canon New LaserShot (LIPS3)
 *
 *	Written by H. Nagahara on 2/10/92 08:58:45.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: lbp.c 10.1
 */
#include	<stdio.h>
#include	<string.h>
#include	"config.h"
#include	"font.h"
#include	"drive.h"
#include	"convert.h"
#define		_LBP_
#include	"lbp.h"

#if MSDOS
# include	<stdlib.h>
int	lprintf( char *, ... );
int	lputchar( int );
#else
#define	lprintf		printf
#define	lputchar	putchar
#endif

#if	!ESCP

#ifdef LBPDEBUG
static char	*(bitp[]) = {
    "        ",
    "       #",
    "      # ",
    "      ##",
    "     #  ",
    "     # #",
    "     ## ",
    "     ###",
    "    #   ",
    "    #  #",
    "    # # ",
    "    # ##",
    "    ##  ",
    "    ## #",
    "    ### ",
    "    ####",
    "   #    ",
    "   #   #",
    "   #  # ",
    "   #  ##",
    "   # #  ",
    "   # # #",
    "   # ## ",
    "   # ###",
    "   ##   ",
    "   ##  #",
    "   ## # ",
    "   ## ##",
    "   ###  ",
    "   ### #",
    "   #### ",
    "   #####",
    "  #     ",
    "  #    #",
    "  #   # ",
    "  #   ##",
    "  #  #  ",
    "  #  # #",
    "  #  ## ",
    "  #  ###",
    "  # #   ",
    "  # #  #",
    "  # # # ",
    "  # # ##",
    "  # ##  ",
    "  # ## #",
    "  # ### ",
    "  # ####",
    "  ##    ",
    "  ##   #",
    "  ##  # ",
    "  ##  ##",
    "  ## #  ",
    "  ## # #",
    "  ## ## ",
    "  ## ###",
    "  ###   ",
    "  ###  #",
    "  ### # ",
    "  ### ##",
    "  ####  ",
    "  #### #",
    "  ##### ",
    "  ######",
    " #      ",
    " #     #",
    " #    # ",
    " #    ##",
    " #   #  ",
    " #   # #",
    " #   ## ",
    " #   ###",
    " #  #   ",
    " #  #  #",
    " #  # # ",
    " #  # ##",
    " #  ##  ",
    " #  ## #",
    " #  ### ",
    " #  ####",
    " # #    ",
    " # #   #",
    " # #  # ",
    " # #  ##",
    " # # #  ",
    " # # # #",
    " # # ## ",
    " # # ###",
    " # ##   ",
    " # ##  #",
    " # ## # ",
    " # ## ##",
    " # ###  ",
    " # ### #",
    " # #### ",
    " # #####",
    " ##     ",
    " ##    #",
    " ##   # ",
    " ##   ##",
    " ##  #  ",
    " ##  # #",
    " ##  ## ",
    " ##  ###",
    " ## #   ",
    " ## #  #",
    " ## # # ",
    " ## # ##",
    " ## ##  ",
    " ## ## #",
    " ## ### ",
    " ## ####",
    " ###    ",
    " ###   #",
    " ###  # ",
    " ###  ##",
    " ### #  ",
    " ### # #",
    " ### ## ",
    " ### ###",
    " ####   ",
    " ####  #",
    " #### # ",
    " #### ##",
    " #####  ",
    " ##### #",
    " ###### ",
    " #######",
    "#       ",
    "#      #",
    "#     # ",
    "#     ##",
    "#    #  ",
    "#    # #",
    "#    ## ",
    "#    ###",
    "#   #   ",
    "#   #  #",
    "#   # # ",
    "#   # ##",
    "#   ##  ",
    "#   ## #",
    "#   ### ",
    "#   ####",
    "#  #    ",
    "#  #   #",
    "#  #  # ",
    "#  #  ##",
    "#  # #  ",
    "#  # # #",
    "#  # ## ",
    "#  # ###",
    "#  ##   ",
    "#  ##  #",
    "#  ## # ",
    "#  ## ##",
    "#  ###  ",
    "#  ### #",
    "#  #### ",
    "#  #####",
    "# #     ",
    "# #    #",
    "# #   # ",
    "# #   ##",
    "# #  #  ",
    "# #  # #",
    "# #  ## ",
    "# #  ###",
    "# # #   ",
    "# # #  #",
    "# # # # ",
    "# # # ##",
    "# # ##  ",
    "# # ## #",
    "# # ### ",
    "# # ####",
    "# ##    ",
    "# ##   #",
    "# ##  # ",
    "# ##  ##",
    "# ## #  ",
    "# ## # #",
    "# ## ## ",
    "# ## ###",
    "# ###   ",
    "# ###  #",
    "# ### # ",
    "# ### ##",
    "# ####  ",
    "# #### #",
    "# ##### ",
    "# ######",
    "##      ",
    "##     #",
    "##    # ",
    "##    ##",
    "##   #  ",
    "##   # #",
    "##   ## ",
    "##   ###",
    "##  #   ",
    "##  #  #",
    "##  # # ",
    "##  # ##",
    "##  ##  ",
    "##  ## #",
    "##  ### ",
    "##  ####",
    "## #    ",
    "## #   #",
    "## #  # ",
    "## #  ##",
    "## # #  ",
    "## # # #",
    "## # ## ",
    "## # ###",
    "## ##   ",
    "## ##  #",
    "## ## # ",
    "## ## ##",
    "## ###  ",
    "## ### #",
    "## #### ",
    "## #####",
    "###     ",
    "###    #",
    "###   # ",
    "###   ##",
    "###  #  ",
    "###  # #",
    "###  ## ",
    "###  ###",
    "### #   ",
    "### #  #",
    "### # # ",
    "### # ##",
    "### ##  ",
    "### ## #",
    "### ### ",
    "### ####",
    "####    ",
    "####   #",
    "####  # ",
    "####  ##",
    "#### #  ",
    "#### # #",
    "#### ## ",
    "#### ###",
    "#####   ",
    "#####  #",
    "##### # ",
    "##### ##",
    "######  ",
    "###### #",
    "####### ",
    "########"
    };
#endif	/* LBPDEBUG */

/* rotate bitmap table */
static uint1 tbl[8] = { 0x80, 0x40, 0x20, 0x10,
			0x08, 0x04, 0x02, 0x01 };

#define	bittest(x,y,w,h,s)	(*(s + y * ((w + 7) / 8) + (x / 8)) & tbl[x % 8])
	/* int	x, y;	testing point */
	/* int	w, h;	raster size */
	/* uint1 *s;	raster buffer */

#define	bitset(x,y,w,h,s)	(*(s + y * ((w + 7) / 8) + (x / 8)) |= tbl[x % 8])

void
rotate_raster( w, h, s, d )
int	w, h;	/* rater size */
uint1	*s;	/* original raster */
uint1	*d;	/* distination raster */
{
    register int	x, y;
    register int	oy;

    for( y = 0; y < w; y++ ) {
	for( x = 0; x < h; x++ ) {
	    oy = h - x - 1;
	    if( bittest( y, oy, w, h, s ) )
		bitset( x, y, h, w, d );
	}
    }
}

/* convert Yokogaki code to Tategaki code */
#define		CVTABLE		69
struct kconv {
    uint2	yoko;
    uint2	tate;
};
struct kconv	conv[CVTABLE+1] = {
    { 0x2122, 0x2846 },		/* Canon no LIPS3 PROGRAMME'S MANUAL */
    { 0x2123, 0x2847 },		/* COMMAND REFERENCE no */
    { 0x2124, 0x2e21 },		/* P.225 ni aru, kono henkan TABLE ha */    
    { 0x2125, 0x2e22 },		/* machigai ga aru. */
    { 0x2127, 0x2e2d },		/* majimeni MANUAL tsukure yona!! > Canon */
    { 0x2128, 0x2e2e },
    { 0x2131, 0x2848 },
    { 0x2132, 0x2849 },
    { 0x213c, 0x284a },
    { 0x213d, 0x284b },
    { 0x213e, 0x284c },
    { 0x2141, 0x284d },
    { 0x2142, 0x284e },
    { 0x2143, 0x284f },
    { 0x2144, 0x2850 },
    { 0x2145, 0x2851 },
    { 0x2146, 0x2e28 },
    { 0x2147, 0x2e29 },
    { 0x2148, 0x2e2a },
    { 0x2149, 0x2e2b },
    { 0x214a, 0x2852 },
    { 0x214b, 0x2853 },
    { 0x214c, 0x2854 },
    { 0x214d, 0x2855 },
    { 0x214e, 0x2856 },
    { 0x214f, 0x2857 },
    { 0x2150, 0x2858 },
    { 0x2151, 0x2859 },
    { 0x2152, 0x285a },
    { 0x2153, 0x285b },
    { 0x2154, 0x285c },
    { 0x2155, 0x285d },
    { 0x2156, 0x285e },
    { 0x2157, 0x285f },
    { 0x2158, 0x2860 },
    { 0x2159, 0x2861 },
    { 0x215a, 0x2862 },
    { 0x215b, 0x2863 },
    { 0x215d, 0x2e2f },
    { 0x2161, 0x2864 },
    { 0x2162, 0x2e30 },
    { 0x2163, 0x2e31 },
    { 0x2164, 0x2e32 },
    { 0x2165, 0x2e33 },
    { 0x2166, 0x2e34 },
    { 0x2421, 0x2865 },
    { 0x2423, 0x2866 },
    { 0x2425, 0x2867 },
    { 0x2427, 0x2868 },
    { 0x2429, 0x2869 },
    { 0x2443, 0x286e },
    { 0x2463, 0x286a },
    { 0x2465, 0x286b },
    { 0x2467, 0x286c },
    { 0x246e, 0x286d },
    { 0x2521, 0x286f },
    { 0x2523, 0x2870 },
    { 0x2525, 0x2871 },
    { 0x2527, 0x2872 },
    { 0x2529, 0x2873 },
    { 0x2543, 0x2878 },
    { 0x2563, 0x2874 },
    { 0x2565, 0x2875 },
    { 0x2567, 0x2875 },
    { 0x256e, 0x2877 },
    { 0x2575, 0x2879 },
    { 0x2576, 0x287a },
    { 0, 0 }
};
	

uint2
yoko2tate( y )
uint2	y;	/* yokogaki code */
{
    register int	idx;
    register int	top, bottom;

    top = 0;
    bottom = CVTABLE - 1;
    while( (bottom - top) > 1 ) {
	idx = (top + bottom) / 2;
	if( y == conv[idx].yoko )
	    return( conv[idx].tate );
	if( conv[idx].yoko > y )
	    bottom = idx;
	else
	    top = idx;
    }
    if( y == conv[top].yoko )
	return( conv[top].tate );
    else
	return( y );
}

    
/* display a message to printer console */
void
LBP_sendMSG( s )
char	*s;
{
    lprintf( "\033P1y%s\033\\", s );
}

/* send StartOfJob seq. to printer */
void
LBP_sendSOJ( print )
int	print;		/* true if printing job */
{
    int		base;

    lprintf( "\033%%@" );		/* start of text-mode */
    lprintf( "\033P31;300;2;JTeX Out\033\\" );	/* start of JOB */
    lprintf( "\033<" );		/* soft reset */

    if( print ) {
	LBP_sendMSG( "DVI printing" );
	if( copypage > 1 )
	    lprintf( "\033[%dv", copypage );	/* copy pages */
	lprintf( "\033[11h" );	/* unit size mode for moving CAP */
	lprintf( "\033[7 I" );	/* select unit (dot = 1/300 inch) */
	/* page format selection */
	switch( papersize ) {
	  case A4:
	    base = 14;
	    break;
	  case A5:
	    base = 16;
	    break;
	  case HAGAKI:
	    base = 18;
	    break;
	  case B4:
	    base = 24;
	    break;
	  case B5:
	    base = 26;
	    break;
	  case LETTER:
	    base = 30;
	    break;
	  case LIGAL:
	    base = 32;
	    break;
	  case USER:
	    base = 80;
	    break;
	  case UNKNOWN:
	    base = 0;
	}
	if( landscape )
	    base++;
	if( papersize != USER )
	    lprintf( "\033[%d;;p", base );
	else
	    lprintf( "\033[%d;%d;%dp", base, (int)(mm2dot(paperwidth) + 0.5),
		    (int)(mm2dot(paperlength) + 0.5) );
	
	lprintf( "\033[?1l" );	/* no auto-wrap */
	lprintf( "\033[?2h" );	/* no auto-new-page */
	lprintf( "\033[?3h" );	/* no auto-move of CAP */
    }
}

/* send EndOfJob seq. to printer */
void
LBP_sendEOJ()
{
    LBP_sendMSG( "" );
    lprintf( "\033P0J\033\\" );	/* end of JOB */
}

/* reset printer */
void
LBP_hardreset()
{
    lprintf( "\033c" );
}

/* select Japanese font */
void
LBP_selectJfont( f )
struct font	*f;
{
    uint4	default_width;
    double	pitch_in_meter;
    int		pitch;
    int		point;
    int		face;
    int		weight;

    lputchar( SHIFTOUT );			/* select G1 */
    lprintf( "\033$)%c", JIS_GSET - 0x80 );		/* select JIS83/78 */
    default_width = f->info->cc.klist->cinfo->width;
					/* default width is in element 0 */
    pitch_in_meter = ((double)default_width / 1048576.0)
	* (f->scale * scalepoint) * WIDTH_ADJ;
    pitch = (int)(INCH_IN_METER / pitch_in_meter * 100.0 + 0.5);
    lprintf( "\033[?%d K", pitch );		/* select char pitch */	
    point = (int)(m2dot(f->scale * scalepoint) * HIGHT_ADJ + 0.5);
    lprintf( "\033[%d C", point );		/* select char size */
    lprintf( "\033[23m" );			/* select char style */

    if( f->prn->attrib != NULL )		/* stroke weight */
	weight = f->prn->attrib->weight;
    else
	weight = DEFAULT_WEIGHT;

    if( (f->prn->attrib != NULL) &&		/* face # */
        (strncmp(f->prn->attrib->name, f->info->name, strlen(f->prn->attrib->name)) == 0 ) )
	face = f->prn->attrib->face;
    else if( strncmp( f->info->name, "min", 3 ) == 0 )
	face = mincho_syotai;
    else if( strncmp( f->info->name, "goth", 4 ) == 0 ) {
	face = gothic_syotai;
	if( face == MARU_GOTHIC )
	    weight = LIGHT_WEIGHT;
    }
    else {
	fprintf( stderr, "%s: unknown Japanese font %s\n", progname, f->info->name );
	exit( 2 );
    }
    lprintf( "\033[%dm", weight );		/* select stroke weight */
    lprintf( "\033[%d;y", face );		/* select syotai */
    lputchar( SHIFTIN );			/* return to G0 */
}
	
/* assign font # to Japanese font for later selection */
void
LBP_assignJfont( no, f )
int		no;	/* font # to assign */  
struct font	*f;	/* font to assign */
{
    /* select Japanese font from scalable font */
    LBP_selectJfont( f );
    lprintf( "\033[%d;2 D", no );
}

/* select alphabet font */
void
LBP_selectAfont( f )
struct font	*f;
{
    lprintf( "\033Pz%s\033\\", f->prn->name );
}

/* assign font # to alphabet font for later selection */
void
LBP_assignAfont( no, f )
int		no;	/* font # to assign */  
struct font	*f;	/* font to assign */
{
    /* select Japanese font from scalable font */
    LBP_selectAfont( f );
    lprintf( "\033[%d;1 D", no );
}

#define	dotpitch(c)	m2dot(((f->info->cc.width[(int)(c) - f->info->begin] / 1048576.0) * (f->scale * scalepoint)))

/* download a font set */
int
LBP_aFont( f, map )
struct font	*f;	/* font info */
struct grif	*map;	/* font bitmap data */
{
    register struct grif	*cur;
    register int		nchar, width, hight, depth;
    int4			bytes;
    int				direction;
    int				pitch;
    int				csize;
    register int		cc;
    register int		dmy;
    register int		i;
    register uint1		*p;
    int				gset;
    char			msgbuf[64];
    char			*cp;

    /* gather download info. */
    nchar = 0;
    bytes = 0;
    width = 0;
    hight = 0;
    depth = 0;
    for( cc = 0; cc < (f->info->end - f->info->begin + 1); cc++ ) {
	cur = map + cc;
	if( (cur->code == 0x20) || (cur->bitmap == NULL) )
	    continue;
	nchar++;
	dmy = cur->width;
	if( cur->hrefp < 0 )
	    dmy -= cur->hrefp;
	if( width < dmy )
	    width = dmy;
	if( hight < cur->vrefp )
	    hight = cur->vrefp;
	if( depth < (dmy = (int)cur->hight - (int)cur->vrefp) )
	    depth = dmy;
	bytes += (cur->width + 7)/8 * cur->hight + 11;
		/* prolog for every char has 11 bytes */
    }
    f->prn->datacount = bytes;
    if( (width > MAXDOWNWIDTH) || ((hight + depth) > MAXDOWNHIGHT) )
	return FALSE;	/* cannot download! */
    if( (maxdownsize >= 0) && ((downloadcount + bytes) > maxdownsize) )
	return FALSE;	/* download size over! */

    /* setup parameters */
    downloadcount += bytes;
    direction = landscape ? 1 : 0;
    gset = ++using_gset;
    if( (f->info->begin <= 0x20) && (f->info->end >= 0x20 ) )
	pitch = (int)(dotpitch( 0x20 ) + 0.5);
    else
	pitch = (int)m2dot(((f->info->cc.width[0] / 1048576.0) * (f->scale * scalepoint)));
    if( pitch < 8 )	/* LIPS3 ignores font data when pitch is 0-7 */
	pitch = 1000;	/* So, set default value. (lips3dvi doesn't use this value) */
    csize = (int)(m2point( f->scale * scalepoint ) * 100.0 + 0.5);

    /* download prolog */
    strcpy( msgbuf, f->prn->name );
    cp = strchr( msgbuf, '.' );
    if( cp != NULL )
	*cp = '\0';
    LBP_sendMSG( msgbuf );
    lprintf( "\033P;;;;;;x%s\033\\", f->prn->name );
    lprintf( "\033[%ld;%d;0;", bytes, nchar );	/* bytes, nchar, default-mark */
    lprintf( "%d;%d;9;", direction, gset );	/* orientation, gr-set, kerning */
    lprintf( "%d;%d;0;0;", pitch, csize );	/* pitch, size, style, stroke */
    lprintf( "200;%d;%d;%d;", width, hight + depth, depth );
						/* typeface, cel(w), cel(h), base */
    lprintf( "2;0;1;0;1" );	/* u-line, code len, level, base flag, control */
    lprintf( ".p" );

    /* data download loop */
    for( cc = 0; cc < (f->info->end - f->info->begin + 1); cc++ ) {
	cur = map + cc;
	if( (cur->code == 0x20) || (cur->bitmap == NULL) )
	    continue;
	lputchar( (int)cur->code );		/* char code */
	dmy = (int)(m2dot(((f->info->cc.width[cc] / 1048576.0) * (f->scale * scalepoint))) + 0.5);
	lputchar( dmy >> 8 );
	lputchar( dmy & 0xff );
	lputchar( (int)(cur->width >> 8) );	/* width */
	lputchar( (int)(cur->width & 0xff) );
	dmy =  -cur->hrefp;		/* horizontal offset */
	lputchar( dmy >> 8 );
	lputchar( dmy & 0xff );
	lputchar( (int)(cur->hight >> 8) );	/* hight */
	lputchar( (int)(cur->hight & 0xff) );
	dmy = cur->hight - cur->vrefp;	/* virtical offset */
	lputchar( (dmy >> 8) & 0xff );
	lputchar( dmy & 0xff );
#ifdef	LBPDEBUG2
	fprintf( stderr, "\n[%X] (%d,%d):(%d,%d)", cur->code, cur->width, cur->hight,cur->hrefp, cur->vrefp );
#endif
#if	USEEMS
	map_grif_bitmap( cur );
#endif
	for( i = 0, p = cur->bitmap;
	     i < (int)((cur->width + 7)/8 * cur->hight);
	     i++, p++ ) {
	    lputchar( (int)*p );
#ifdef	LBPDEBUG2
	    if( (i % ((cur->width + 7)/8)) == 0 )
		fprintf( stderr, "\n" );
	    fprintf( stderr, "%s", bitp[*p] );
#endif
	}
    }

    /* set downloaded font info into fontlist */
    f->prn->graphset = gset;
    f->prn->download = TRUE;

    return TRUE;
}

/* move CAP */
void
LBP_move( x, y )
int4	x, y;
{
    register long	v, h;

    h = (long)(m2dot( (double)x * scalepoint ) + h_base + 0.5);
    v = (long)(m2dot( (double)y * scalepoint ) + v_base + 0.5);
    lprintf( "\033[%ld;%ldf", v, h );
}

#ifdef	SQUARE
/* write frame */
void
LBP_square( h0, v0, h1, v1, pattern )
int4	h0, v0;		/* start point */
int4	h1, v1;		/* end point */
int	pattern;	/* pattern # */
{
    LBP_move( h0, v0 );		/* move to start point */
    lprintf( "\033[%d;9;s", pattern );	/* start of overlay */
    LBP_move( h0 + h1, v0 + v1 );	/* move to end point */
    lprintf( "\033[9r" );	/* end of overlay */
}
#endif	/* SQUARE */

/* write rectangle */
void
LBP_rectangle( h0, v0, h1, v1 )
int4	h0, v0;		/* start point */
int4	h1, v1;		/* end point */
{
    register int4	move_x, move_y;

    LBP_move( h0, v0 );		/* move to start point */
    lprintf( "\033[3;9;s" );	/* start of overlay */
    move_x = (int4)(m2dot( (double)h1 * scalepoint ) + 0.5);
    if( move_x > 0 )
	lprintf( "\033[%lda", move_x );		/* move right */
    else if( move_x < 0 )
	lprintf( "\033[%ldj", (int4)(- move_x) );	/* move left */
    move_y = (int4)(m2dot( (double)v1 * scalepoint ) + 0.5);
    if( move_y > 0 )
	lprintf( "\033[%ldk", move_y );		/* move down */
    else if( move_y < 0 )
	lprintf( "\033[%lde", (int4)(- move_y) );
    lprintf( "\033[9r" );	/* end of overlay */
}
    
/* print Japanese char as raster image */
void
LBP_raster( x0, y0, cdata )
int4		x0, y0;		/* write positon */
struct fdir	*cdata;		/* font image data */
{
    register int	i;
    int			len;
    int4		rh, rv, doth, dotv;

    /* move to start position */
    rh = (int4)(m2dot( (double)x0 * scalepoint ) + 0.5) + h_base;
    rv = (int4)(m2dot( (double)y0 * scalepoint ) + 0.5) + v_base;
    doth = rh - (int4)cdata->h_offset;
    dotv = rv - (int4)cdata->v_offset;
    lprintf( "\033[%ld;%ldf", dotv, doth );

    /* send laster image */
    len = ((cdata->width + 7) / 8) * cdata->hight;
    lprintf( "\033[%d;%d;300;0;.r", len, (int)((cdata->width +7) / 8) );
#if	USEEMS
    map_fdir_bitmap(cdata);
#endif
    for( i = 0; i < len; i++ )
	lputchar( (int)(*(cdata->bitmap + i)) );
}

/* print char as raster image for non-downloaded font */
void
LBP_rasterAchar( x0, y0, f, c )
int4		x0, y0;
struct font	*f;	/* font */
uint4		c;	/* character code */
{
    register struct grif	*grifdata;
    register int		i;
    int				len;
    register int4		rh, rv;
    int4			doth, dotv;
    uint1			*dist;
#ifdef	LBPDEBUG
    uint1			*p;
#endif

#ifdef	DEBUG
    if( f->gdata.pattern == NULL ) {
	fprintf( stderr, "%s: internal error.  no pattern of %s.\n",
		 progname, f->info->name );
	return;
    }
#endif

    /* move to start position */
    rh = (int4)(m2dot( (double)x0 * scalepoint ) + 0.5 + h_base);
    rv = (int4)(m2dot( (double)y0 * scalepoint ) + 0.5 + v_base);
    grifdata = f->gdata.pattern + (c - f->info->begin);
    if( dir == YOKO ) {
	doth = rh - (int4)grifdata->hrefp;
	dotv = rv - (int4)grifdata->vrefp;
    }
    else {
	doth = rh + (int4)grifdata->vrefp - grifdata->hight;
	dotv = rv - (int4)grifdata->hrefp;
    }	
    lprintf( "\033[%ld;%ldf", dotv, doth );

    /* send laster image */
    if( dir == YOKO ) {
	len = ((grifdata->width + 7) / 8) * grifdata->hight;
	lprintf( "\033[%d;%d;300;0;.r", len, (int)((grifdata->width +7) / 8) );
#if	USEEMS
	map_grif_bitmap(grifdata);
#endif
	for( i = 0; i < len; i++ )
	    lputchar( (int)(*(grifdata->bitmap + i)) );
    }
    else {	/* tategaki -> rotate raster */
#ifdef	LBPDEBUG
	len = ((grifdata->width + 7) / 8) * grifdata->hight;
	fprintf( stderr, "\n[%X] (%d,%d):(%d,%d)", grifdata->code, grifdata->width, grifdata->hight,grifdata->hrefp, grifdata->vrefp );
#if	USEEMS
	map_grif_bitmap(grifdata);
#endif	/* USEEMS */
	for( i = 0, p = grifdata->bitmap; i < len; i++, p++ ) {
	    if( (i % ((grifdata->width + 7)/8)) == 0 )
		fprintf( stderr, "\n" );
	    fprintf( stderr, "%s", bitp[*p] );
	}
#endif	/* LBPDEBUG */
	len = ((grifdata->hight + 7) / 8) * grifdata->width;
	dist = (uint1 *)calloc( len, sizeof(uint1) );
	if( dist == NULL )
	    goto MemError;
#if	USEEMS
	map_grif_bitmap( grifdata );
#endif
	rotate_raster( grifdata->width, grifdata->hight, grifdata->bitmap, dist );
#ifdef	LBPDEBUG
	fprintf( stderr, "\n[%X] (%d,%d):(%d,%d)", grifdata->code, grifdata->hight, grifdata->width,grifdata->vrefp, grifdata->hrefp );
	for( i = 0, p = dist; i < len; i++, p++ ) {
	    if( (i % ((grifdata->hight + 7)/8)) == 0 )
		fprintf( stderr, "\n" );
	    fprintf( stderr, "%s", bitp[*p] );
	}
#endif
	lprintf( "\033[%d;%d;300;0;.r", len, (int)((grifdata->hight +7) / 8) );
	for( i = 0; i < len; i++ )
	    lputchar( (int)(*(dist + i)) );
	free( dist );
    }
    return;

  MemError:
    fprintf( stderr, "%s: no memory\n", progname );
    exit( 1 );
}

/* print char as raster image for 0x20 code (not-downloaded) */
void
LBP_raster20( x0, y0, image )
int4		x0, y0;
struct grif	*image;
{
    register int		i;
    int				len;
    register int4		rh, rv;
    int4			doth, dotv;
    uint1			*dist;

    /* move to start position */
    rh = (int4)(m2dot( (double)x0 * scalepoint ) + 0.5) + h_base;
    rv = (int4)(m2dot( (double)y0 * scalepoint ) + 0.5) + v_base;
    if( dir == YOKO ) {
	doth = rh - (int4)image->hrefp;
	dotv = rv - (int4)image->vrefp;
    }
    else {
	doth = rh + (int4)image->vrefp - (int4)image->hight;
	dotv = rv - (int4)image->hrefp;
    }	
    lprintf( "\033[%ld;%ldf", dotv, doth );

    /* send laster image */
    if( dir == YOKO ) {
	len = ((image->width + 7) / 8) * image->hight;
	lprintf( "\033[%d;%d;300;0;.r", len, (int)((image->width +7) / 8) );
#if	USEEMS
	map_grif_bitmap( image );
#endif
	for( i = 0; i < len; i++ )
	    lputchar( (int)(*(image->bitmap + i)) );
    }
    else {	/* tategaki -> rotate raster */
	len = ((image->hight + 7) / 8) * image->width;
	dist = (uint1 *)calloc( len, sizeof(uint1) );
	if( dist == NULL )
	    goto MemError;
#if	USEEMS
	map_grif_bitmap(image);
#endif
	rotate_raster( image->width, image->hight, image->bitmap, dist );
	lprintf( "\033[%d;%d;300;0;.r", len, (int)((image->hight +7) / 8) );
	for( i = 0; i < len; i++ )
	    lputchar( (int)(*(dist + i)) );
	free( dist );
    }
    return;

  MemError:
    fprintf( stderr, "%s: no memory\n", progname );
    exit( 1 );
}

/* print a char */
void
LBP_putchar( x0, y0, f, c )
int4		x0, y0;
struct font	*f;	/* font */
uint4		c;	/* char code */
{
    register struct jfm	*base;
    register int	idx;
    register int	top, bottom;
    int			found;
    int4		chardepth, charnglue;
    int4		charwidth, defwidth;
    int4		cd, cg, cw, dw;
#ifdef	SQUARE
    uint4		charhight;
    int			ch;
#endif

    if( (c < ' ') || (c == 0x7f) ) {	/* alphabet font */
	LBP_move( x0, y0 );
	if( dir == TATE ) {
	    lprintf( "\033[270&t" );	/* rotate char */
	    lprintf( "\033[1.v%c", (int)c );
	    lprintf( "\033[0&t" );	/* original direction */
	}
	else
	    lprintf( "\033[1.v%c", (int)c );
    }
    else if( c > 255 ) {		/* Japanese font */
	/* get character's metric data */
	base = f->info->cc.klist;
#ifdef	SQUARE
	charhight = base->cinfo->hight;
#endif
	charwidth = defwidth = base->cinfo->width;	/* default metrics */
	chardepth = base->cinfo->depth;
	charnglue = base->cinfo->nglue;
	top = 0;
	bottom = f->info->cnum - 1;
	if( (base + bottom)->kcode > (uint2)c ) {
	    /* code is in range of the table --> do search */
	    found = FALSE;
	    while( (bottom - top) > 1 ) {
		idx = (top + bottom) / 2;
		if( (base + idx)->kcode == (uint2)c ) {
		    found = TRUE;
		    break;
		}
		else if( (base + idx)->kcode < (uint2)c )
		    top = idx;
		else
		    bottom = idx;
	    }
	    if( found ) {
#ifdef	SQUARE
		charhight = (base + idx)->cinfo->hight;
#endif
		charwidth = (base + idx)->cinfo->width;
		chardepth = (base + idx)->cinfo->depth;
		charnglue = (base + idx)->cinfo->nglue;
	    }
	}

	cg = (int4)((double)charnglue / 1048576.0 * (double)f->scale + 0.5);
	cw = (int4)((double)charwidth / 1048576.0 * (double)f->scale + 0.5);
	dw = (int4)((double)defwidth / 1048576.0 * HIGHT_ADJ * (double)f->scale + 0.5);
#ifdef	SQUARE
	ch = (int4)((double)charhight / 1048576.0 * (double)f->scale + 0.5);
	cd = (int4)((double)chardepth / 1048576.0 * (double)f->scale + 0.5);
	if( dir == YOKO ) {
	    LBP_square( x0, y0, (int4)cw, (int4)-ch, 1 );
	    LBP_square( x0, y0, (int4)cw, (int4)cd, 2 );
	}
	else {
	    LBP_square( x0, y0, (int4)ch, (int4)cw, 1 );
	    LBP_square( x0, y0, (int4)-cd, (int4)cw, 2 );
	}
#endif	/* SQUARE */
	if( dir == YOKO ) {
	    cd = (int4)((double)chardepth / 1048576.0
			* DEPTH_ADJ *(double)f->scale + 0.5);
	    LBP_move( x0 - cg, y0 + cd );
	    lputchar( (int)((c >> 8) | 0x80) );
	    lputchar( (int)((c & 0xff) | 0x80) );
	}
	else {
	    if( f->type == JFM ) {	/* yokogaki font */
		cd = (int4)((double)chardepth / 1048576.0
			    * DEPTH_ADJ *(double)f->scale + 0.5);
		LBP_move( x0 - cd, y0 - cg );
		lprintf( "\033[270&t" );	/* rotate char */
		lputchar( (int)((c >> 8) | 0x80) );
		lputchar( (int)((c & 0xff) | 0x80) );
		lprintf( "\033[0&t" );		/* original direction */
	    }
	    else {			/* tategaki font */
		cd = (int4)((double)chardepth / 1048576.0
			    * (double)f->scale + 0.5);
		LBP_move( x0 - cd, y0 + dw - cg );
		c = (uint4)yoko2tate( (uint2)c );
		lputchar( (int)((c >> 8) | 0x80) );
		lputchar( (int)((c & 0xff) | 0x80) );
	    }
	}
    }
    else {				/* alphabet font */
	LBP_move( x0, y0 );
	if( dir == TATE ) {
	    lprintf( "\033[270&t" );	/* rotate char */
	    lputchar( (int)c );
	    lprintf( "\033[0&t" );	/* original direction */
	}
	else
	    lputchar( (int)c );
    }
}

/* select Japanese font as its # */
void
LBP_numJfont( fno )
int	fno;	/* assigned font # */
{
    lprintf( "\033[%d;2;1%%v", fno );
}

/* select alphabet font as its # */
void
LBP_numAfont( fno )
int	fno;	/* assigned font # */
{
    lprintf( "\033[%d;1;1%%v", fno );
}

/* flush page */
void
LBP_flushpage()
{
    lputchar( 0x0c );
}

/* set font attribute */
void
LBP_attrib( fill, shadow, outline )
int	fill;		/* fill pattern # */
int	shadow;		/* shadow pattern # */
int	outline;	/* outline dot */
{
    if( fill >= 0 ) {
	lprintf( "\033[%d;;;;;;%%}", fill );	/* select fill pattern */
	lprintf( "\033[?5m" );			/* start fill */
    }
    if( shadow >= 0 ) {
	lprintf( "\033[;;%d;;;;%%}", shadow );	/* select shadow pattern */
	lprintf( "\033[?6m" );			/* start shadow */
    }
    if( outline >= 0 ) {
	lprintf( "\033[;;;;;;%d%%}", outline );	/* select outline dot */
	lprintf( "\033[?7m" );			/* start outline */
    }
}
     
/* clear font attribute */
void
LBP_clearattr( f )
struct font	*f;		/* font for clear */
{
    if( f->prn->attrib->fillpattern >= 0 )
	lprintf( "\033[?25m" );		/* end of fill */
    if( f->prn->attrib->shadowpattern >= 0 )
	lprintf( "\033[?26m" );		/* end of shadow */
    if( f->prn->attrib->outline >= 0 )
	lprintf( "\033[?27m" );		/* end of outline */
}

#else	/* !ESCP */
#ifndef	MAKEDECL
 ERROR: your 'config.h' and 'Makefile' not match to LIPS3 version.
#endif
#endif	/* !ESCP */
