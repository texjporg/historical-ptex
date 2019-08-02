/*
 *	lp3000.c -- printer dependent routines
 *		 This file is for Epson Page printer LP-3000
 *
 *	Written by H. Nagahara on 8/23/91 22:31:05. (base Rev. 8.2)
 *	Modified by A. Iwamoto on 9/12/91 for ESC/Page.
 *	Modified by H. Nagahara on 2/10/92 08:58:56
 *	Copyright (C) 1990,1991 by ASCII Corporation and A. Iwamoto.
 *						All rights reserved.
 *		SCCS memo: lp3000.c 10.1
 */

#include	<stdio.h>
#include	<string.h>
#include	"config.h"
#include	"font.h"
#include	"drive.h"
#include	"convert.h"
#define		_LBP_
#include	"lbp.h"

#if	MSDOS
# include	<stdlib.h>
int	lprintf( char *, ... );
int	lputchar( int );
static void LBP_locate( int4, int4 );
#else
#define	lprintf		printf
#define	lputchar	putchar
#endif

#if	ESCP

#define USER_SYMBOLBASE	16384	/* user symbol-set minimam number */
#define USER_TYPEFACEBASE 128	/* user type face minimam number */

#define U2(c)		((uint2)(c))
#define U4(c)		((uint4)(c))

#define lputint2(c)	lputchar((int)(U2(c) >> 8));\
			lputchar((int)(U2(c) & U2(0xff)))


/*
#define lputint4(c)	lputchar((int)((U4(c)>>24) & U2(0xff)));\
			lputchar((int)((U4(c)>>16) & U2(0xff)));\
			lputchar((int)((U4(c)>> 8) & U2(0xff)));\
			lputchar((int)(U4(c)&U2(0xff)))

*/

#define lputint4(c)	lputchar((int)((U4(c)>> 8) & U2(0xff)));\
			lputchar((int)(U4(c)&U2(0xff)));\
			lputchar((int)((U4(c)>>24) & U2(0xff)));\
			lputchar((int)((U4(c)>>16) & U2(0xff)))

/*
 * 	flags
 */

int inkanji = 0;		/* printing kanji now */
int intate = 0;			/* printing tategaki-kanji now */

int lastkanji = 0;		/* latest kanji font number */
int lastascii = 0;		/* latest 1-byte set font number */

/*
 *	CAP position 
 */

#define NEVERAPPEARPOS	-3000000L
static int4 current_x = NEVERAPPEARPOS;
static int4 current_y = NEVERAPPEARPOS;


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

/* send StartOfJob seq. to printer */
void
LBP_sendSOJ( print )
int	print;		/* true if printing job */
{
    int		base;

    if( !print ) {	/* only once */

	lprintf( "\035%dcoO", copypage );	/* copy pages */
	lprintf( "\0351pmP" );	/* unit size mode for moving CAP */
	lprintf( "\0350;0.24muE" );	/* select unit (dot = 1/300 inch) */
	lprintf( "\0350;300;300drE" );	/* 300dpi */
	lprintf( "\0351;300;300drE" );	/* 300dpi */
	lprintf( "\0352;300;300drE" );	/* 300dpi */
	lprintf( "\0351mmE" );		/* page memory mode */
 	lprintf( "\0350;0;100;spE" );	 /*screen pattern 100% black */
 	lprintf( "\0351;0;100;spE" );	 /*screen pattern 100% black */
	lprintf( "\035%difF", MAXASSIGN+1 ); /* max download font number */
	lprintf( "\035%didF", MAXASSIGN+1 ); /* max download font number */
	lprintf( "\035%diaF", MAXASSIGN+1 ); /* max font attribute number */
	lprintf( "\035%dissF", MAXASSIGN+1 ); /* max symbol set number */

	/* page format selection */
	switch( papersize ) {
	  case A4:
	    base = 14;
	    break;
	  case A5:
	    base = 15;
	    break;
	  case HAGAKI:
	    base = 38;
	    break;
	  case B4:
	    base = 24;
	    break;
	  case B5:
	    base = 25;
	    break;
	  case LETTER:
	    base = 30;
	    break;
	  case LIGAL:
	    base = 32;
	    break;
	  case USER:
	    base = -1;
	    break;
	  case UNKNOWN:
	    base = 0;
	}
	if( landscape )
	    lprintf( "\0351poE" );
	else
	    lprintf( "\0350poE" );

	if( papersize != USER )
	    lprintf( "\035%dpsE", base );
	else
	    lprintf( "\035%d;%d;%dpsE", base, (int)(mm2dot(paperwidth) + 0.5),
		    (int)(mm2dot(paperlength) + 0.5) );
	
	lprintf( "\0350alfP" );	/* no auto-wrap */
	lprintf( "\0350affP" );	/* no auto-new-page */
	lprintf( "\0350;0lpP" ); /* no cr */
	lprintf( "\0350;0pP" );	/* don't move */
	lprintf( "\0350abP" ); 	/* don't move after writing image */
    }
}

/* send EndOfJob seq. to printer */
void
LBP_sendEOJ()
{
    return; /* noting to do! */
}

/* reset printer */
void
LBP_hardreset()
{
    lprintf( "\035rhE" );
}

/* select Japanese font as its # */
void
LBP_numJfont( fno )
int	fno;	/* assigned font # */
{
    lprintf( "\0351;%dmcF", fno );
    lastkanji = fno;
    inkanji = 1;
    intate = 0;
}

/* select alphabet font as its # */
void
LBP_numAfont( fno )
int	fno;	/* assigned font # */
{
    lprintf( "\0351;%dmcF", fno );
    lastascii = fno;
    inkanji = 0;
    intate = 0;
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

    lprintf( "\0351;2ssF" );			/* kanji YOKO*/
    intate = 0;

    if( f->prn->attrib != NULL )		/* stroke weight */
	weight = f->prn->attrib->weight;
    else
	weight = DEFAULT_WEIGHT;

    if( (f->prn->attrib != NULL) &&
        (strncmp(f->prn->attrib->name, f->info->name, strlen(f->prn->attrib->name)) == 0 ) )
	face = f->prn->attrib->face;
    else if( strncmp( f->info->name, "min", 3 ) == 0 )
	face = mincho_syotai;
    else if( strncmp( f->info->name, "goth", 4 ) == 0 )
	face = gothic_syotai;
    else {
	fprintf( stderr, "%s: unknown Japanese font %s\n", progname, f->info->name );
	exit( 2 );
    }
    lprintf( "\035%dtfF", face );		/* select syotai */

    default_width = f->info->cc.klist->cinfo->width;
					/* default width is in element 0 */
    pitch_in_meter = ((double)default_width / 1048576.0)
	* (f->scale * scalepoint) * WIDTH_ADJ;
    pitch = (int)(INCH_IN_METER / pitch_in_meter * 100.0 + 0.5);
    point = (int)(m2dot(f->scale * scalepoint) * HIGHT_ADJ + 0.5);
    lprintf( "\035%dwmF\035%dhmF", point, point );	/* select char size */
    lprintf( "\035%dweF", weight );			/* select stroke weight */
    lprintf( "\0350;%dmcF", 0 );
    LBP_numJfont( 0 );
}
	
/* select alphabet font */
void
LBP_selectAfont( f )
struct font	*f;
{
    int	no;

    no = get_fontname( f->prn->name );
    if ( no <= 0 ) {
	fprintf(stderr,"ERROR: can't select font %s. abort.\n",f->prn->name );
	exit(5);
    }
    LBP_numAfont( no );
}

/* assign font # to Japanese font for later selection */
int
LBP_assignJfont( f )
struct font	*f;	/* font to assign */
{
    int		no;	/* font # to assign */  
    /* select Japanese font from scalable font */
    LBP_selectJfont( f );
    no = add_fontname( f->prn->name );
    if ( no > 0 )
	lprintf( "\0350;%dmcF", no );
    return no;
}
/* assign font # to alphabet font for later selection */
int
LBP_assignAfont( f )
struct font	*f;	/* font to assign */
{
    int		no;	/* font # to assign */  
    /* select ASCII font from scalable font */
    no = add_fontname( f->prn->name );
    return no;
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
    register int		no;
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
	if( cur->bitmap == NULL )
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
	bytes += (cur->width + 7)/8 * cur->hight + 16;
		/* prolog for every char has 16 bytes */
    }
    f->prn->datacount = bytes;
    if( (width > MAXDOWNWIDTH) || ((hight + depth) > MAXDOWNHIGHT) )
	return FALSE;	/* cannot download! */
    if( (maxdownsize >= 0) && ((downloadcount + bytes) > maxdownsize) )
	return FALSE;	/* download size over! */
    if ( prn_max_font >= MAXASSIGN )
	return FALSE;	/* no more download please! */

    no = add_fontname(f->prn->name);
    if ( no < 1 )
	return FALSE;

    /* setup parameters */
    downloadcount += bytes;
    direction = landscape ? 1 : 0;
    gset = ++using_gset;
    if( (f->info->begin <= 0x20) && (f->info->end >= 0x20 ) )
	pitch = (int)(dotpitch( 0x20 ) + 0.5);
    else
	pitch = (int) (m2dot(((f->info->cc.width[0] / 1048576.0) * (f->scale * scalepoint))));
    csize = (int)(m2point( f->scale * scalepoint ) * 100.0 + 0.5);

    /* download prolog */
    strcpy( msgbuf, f->prn->name );
    cp = strchr( msgbuf, '.' );
    if( cp != NULL )
	*cp = '\0';

    lprintf( "\035134;%ddh{F",no); /* download font number */

    /* download header */

    lputint2(2);			/* format  (must 2)*/
    lputint2(0x86);			/* data length (must 0x86) */
    lputint2(USER_SYMBOLBASE+no);	/* symbol set number */
    lputint2(0);			/* char spacing 0=fix pitch*/
    lputint4(width);			/* width */
    lputint4(hight+depth);		/* character hight */
    lputint2(USER_TYPEFACEBASE+no); /*	 style set */
    lputint2(0);			/* char line width */
    lputint2(0);			/* rotation */
    lputint2(0);			/* slide */
    lputint2(0xff);			/* max char code */
    lputint2(0x00);			/* min char code */
    lputint2(0);			/* underline offset */
    lputint2(3);			/* underline width */
    lputint2(0);			/* overline offset */
    lputint2(3);			/* overline width */
    lputint2(0);			/* strike out offset */
    lputint2(3);			/* strike out line width */
    lputint2(width);			/* char cell width */
    lputint2(hight+depth);		/* char cell hight */
    lputint2(0);			/* char cell left offset */
    lputint2(hight);			/* char cell assender */
    lputint4(width);			/* fix pitch length */
    lprintf("%-54.54s","");		/* font name */
    lprintf("%-28.28s","No name");	/* symbol set name */

    lprintf("\035%ddcF",no);	/* select download font for later use*/

    /* data download loop */

    for( cc = 0; cc < (f->info->end - f->info->begin + 1); cc++ ) {
	cur = map + cc;
	if( cur->bitmap == NULL )
	    continue;

	dmy = 16+((cur->width + 7)/8 * cur->hight);
	lprintf( "\035%d;%dsc{F", dmy, cur->code ); /* download a char */
	lputchar( 1 );			/* data format (must 1   )*/
	lputchar( 0x10 );			/* data length (must 0x10)*/
	lputint2( USER_SYMBOLBASE+no );	/* symbol set number */
	lputint2( cur->width );		/* bitmap width */
	lputint2( cur->hight );		/* bitmap hight */

	dmy =  -cur->hrefp;
	lputint2( dmy );		/* bitmap left offset */

	dmy = cur->vrefp;
	lputint2( dmy );		/* bitmap assender */

	lputint4( cur->width );

#ifdef	LBPDEBUG2
	fprintf( stderr, "\n[%X] (%d,%d):(%d,%d)", cur->code, cur->width, cur->hight,cur->hrefp, cur->vrefp );
#endif
#if	USEEMS
	map_grif_bitmap(cur);
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

    lprintf( "\035%d;0ssF",USER_SYMBOLBASE+no); /*symbol set number*/
    lprintf( "\0350weF" );		/* stroke weight */
    lprintf( "\0350roF" );		/* rotation */
    lprintf( "\0350slF" );		/* slide */
    lprintf( "\035%dwmF", width);	/* character width */
    lprintf( "\035%dhmF", hight+depth );	/* character hight */
    lprintf( "\035%dtfF", USER_TYPEFACEBASE+no ); /*style set */

    lprintf( "\0350;%dmcF", no );	/* record this font attribute*/

    /* set downloaded font info into fontlist */
    f->prn->graphset = gset;
    f->prn->download = TRUE;
    return TRUE;
}

/* move CAP to phisical position */
static void
LBP_locate( x, y )
int4	x, y;
{
    if ( current_x != x ) {
	lprintf( "\035%ldX", x );
	current_x = x;
    }

    if ( current_y != y ) {
	lprintf( "\035%ldY", y );
	current_y = y;
    }
}

/* move CAP */
void
LBP_move( x, y )
int4	x, y;
{
    register int4	v, h;

    h = (int4)(m2dot( (double)x * scalepoint )) + h_base;
    v = (int4)(m2dot( (double)y * scalepoint )) + v_base;
    LBP_locate( h, v );
}

#ifdef	SQUARE
/* write frame */
void
LBP_square( h0, v0, h1, v1, pattern )
int4	h0, v0;		/* start point */
int4	h1, v1;		/* end point */
int	pattern;	/* pattern # */
{
    LBP_locate( h0, v0 );
    lprintf( "\035%ld;%ldbgI", h0 + h1 , v0 + v1 );
}
#endif	/* SQUARE */

/* write rectangle */
void
LBP_rectangle( h0, v0, h1, v1 )
int4	h0, v0;		/* start point */
int4	h1, v1;		/* end point */
{
    register int4	move_x, move_y, h, v;

    h = (int4)(m2dot( (double)h0 * scalepoint )) + h_base;
    v = (int4)(m2dot( (double)v0 * scalepoint )) + v_base;

    move_x = (int4)(m2dot( (double)h1 * scalepoint ) );

    if( move_x < 0L ) {
	move_x = 0L - move_x;
	h -= move_x;
    }

    move_y = (int4)(m2dot( (double)v1 * scalepoint ) );

    if( move_y > 0L ) {
	v -= move_y;
    }

    LBP_locate( h, v );
    lprintf( "\035%ld;%ldbgI", move_x, move_y );

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
    LBP_locate( doth, dotv );

    /* send laster image */
    len = ((cdata->width + 7) / 8) * cdata->hight;
    lprintf( "\035%d;%d;%d;0bi{I", len, cdata->width, cdata->hight );
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

    LBP_locate( doth, dotv );

    /* send laster image */
    if( dir == YOKO ) {
	len = ((grifdata->width + 7) / 8) * grifdata->hight;
	lprintf( "\035%d;%d;%d;0bi{I", len, grifdata->width, grifdata->hight );
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
#endif
	for( i = 0, p = grifdata->bitmap; i < len; i++, p++ ) {
	    if( (i % ((grifdata->width + 7)/8)) == 0 )
		fprintf( stderr, "\n" );
	    fprintf( stderr, "%s", bitp[*p] );
	}
#endif
	len = ((grifdata->hight + 7) / 8) * grifdata->width;
	dist = (uint1 *)calloc( len, sizeof(uint1) );
	if( dist == NULL )
	    goto MemError;
#if	USEEMS
	map_grif_bitmap(grifdata);
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
	lprintf( "\035%d;%d;%d;0bi{I", len, grifdata->hight, grifdata->width /*, grifdata->hight*/ );
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

    LBP_locate( doth, dotv );

    /* send laster image */
    if( dir == YOKO ) {
	len = ((image->width + 7) / 8) * image->hight;
	lprintf( "\035%d;%d;%d;0bi{I", len, image->width, image->hight );
#if	USEEMS
	map_grif_bitmap(image);
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
	lprintf( "\035%d;%d;%d;0bi{I", len, image->hight, image->width /*image->hight*/ );
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

    if( c > 255 ) {		/* Japanese font */
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
        if ( !inkanji ) {
		LBP_numJfont( lastkanji );
        }

	if( dir == YOKO ) {
	    cd = (int4)((double)chardepth / 1048576.0
			* DEPTH_ADJ *(double)f->scale + 0.5);
	    LBP_move( x0 - cg, y0 + cd );
	    if ( intate ) {
		lprintf("\0351;2ssF");
		intate = 0;
	    }
	    lputchar( (int)(c >> 8) );
	    lputchar( (int)(c & 0xff) );
	}
	else {
	    if( f->type == JFM ) {	/* yokogaki font */
		cd = (int4)((double)chardepth / 1048576.0
			    * DEPTH_ADJ *(double)f->scale + 0.5);
		LBP_move( x0 - cd, y0 - cg );
		lprintf( "\035270roF" );	/* rotate char */
		lputchar( (int)(c >> 8) );
		lputchar( (int)(c & 0xff) );
		lprintf( "\0350roF" );		/* original direction */
	    }
	    else {			/* tategaki font */
		cd = (int4)((double)chardepth / 1048576.0
			    * (double)f->scale + 0.5);
		LBP_move( x0 - cd, y0 + dw - cg );
		if ( !intate ) {
			lprintf("\0352;2ssF");
			intate = 1;
		}
		lputchar( (int)(c >> 8) );
		lputchar( (int)(c & 0xff) );
	    }
	}
    }
    else {				/* alphabet font */

        if ( inkanji ) {
		LBP_numAfont( lastascii );
        }

	LBP_move( x0, y0 );

	if( dir == TATE )
	    lprintf( "\035270roF" );	/* rotate char */

        if ( (int)(c) < 0x20 )
		lprintf( "\0351cp{C" );	/* control character */

	lputchar( (int)c );
    }
}

/* flush page */
void
LBP_flushpage()
{
    lputchar( 0x0c );
    current_x = NEVERAPPEARPOS;
    current_y = NEVERAPPEARPOS;
}

/* set font attribute */
void
LBP_attrib( fill, shadow, outline )
int	fill;		/* fill pattern # */
int	shadow;		/* shadow pattern # */
int	outline;	/* outline dot */
{
    if( fill >= 0 ) {
	return;	/* select fill pattern */
    }
    if( shadow >= 0 ) {
	return;					/* select shadow pattern */
    }
    if( outline >= 0 ) {
	return;	/* select outline dot */
    }
}
     
/* clear font attribute */
void
LBP_clearattr( f )
struct font	*f;		/* font for clear */
{
    if( f->prn->attrib->fillpattern >= 0 )
	return;				/* end of fill */
    if( f->prn->attrib->shadowpattern >= 0 )
	return;				/* end of shadow */
    if( f->prn->attrib->outline >= 0 )
	return;				/* end of outline */
}


/*
 *	font name discripter for LP-3000(ESC/Page)
 */

static char *fnt_d[MAXASSIGN+1];

void
set_fontname( num, name )
int	num;
char *name;
{
    if ( num <= 0 || num > MAXASSIGN ) {
	fprintf( stderr, "INTERNAL ERROR: font number %d out-of-range. abort.\n", num);
	exit( 5 );
    }
    if ( fnt_d[num] != NULL ) {
	fprintf( stderr, "INTERNAL ERROR: font number %d already in use. abort.\n", num );
	exit( 6 );
    }

    fnt_d[num] = strdup( name );
}

int
get_fontname( name )
char *name;
{
    int i;

    for ( i = 1 ; i <= MAXASSIGN ; i++ ) {
	if ( fnt_d[i] != NULL && strcmp( name, fnt_d[i] ) == 0 )
	    return i;
    }
    return 0;
}

int
add_fontname( name )
char *name;
{
    int i, no;

    if ( ( no = get_fontname( name ) ) > 0 )
	return no;

    for ( i = 1 ; i < MAXASSIGN ; i++ ) {
	if ( fnt_d[i] == NULL ) {
	    set_fontname( i, name );
	    return i;
	}
    }
    return 0;
}

#else	/* ESCP */
#ifndef	MAKEDECL
 ERROR: your 'config.h' and 'Makefile' is not match for LP3000 version.
#endif
#endif	/* ESCP */
