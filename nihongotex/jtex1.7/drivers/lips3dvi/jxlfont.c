/*
 *	jxlfont.c -- expand jxl font grif
 *
 *	Written by H. Nagahara on 2/10/92 08:58:38.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: jxlfont.c 10.1
 */
#include	<stdio.h>
#include	"config.h"
#include	<memory.h>
#include	<string.h>
#include	"font.h"
#include	"drive.h"
#include	"ktype.h"
#include	"lbp.h"

#if MSDOS	/* ANSI prototype declaration */
# include	<stdlib.h>
# include	<malloc.h>
#else
  extern char	*malloc(), *calloc();
#endif

#define	PXLID	101157

/* pattern expansion work variables : defined in pkfont.c */
extern uint1	pat[8];
extern int	hnybble;
extern int	nyb_count;
extern uint1	*cdatap;
static int	cdatalen;
static int	beforerep;

#ifdef JXLDEBUG
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
static char	*(bitpp[]) = {
    "        ",
    "       @",
    "      @ ",
    "      @@",
    "     @  ",
    "     @ @",
    "     @@ ",
    "     @@@",
    "    @   ",
    "    @  @",
    "    @ @ ",
    "    @ @@",
    "    @@  ",
    "    @@ @",
    "    @@@ ",
    "    @@@@",
    "   @    ",
    "   @   @",
    "   @  @ ",
    "   @  @@",
    "   @ @  ",
    "   @ @ @",
    "   @ @@ ",
    "   @ @@@",
    "   @@   ",
    "   @@  @",
    "   @@ @ ",
    "   @@ @@",
    "   @@@  ",
    "   @@@ @",
    "   @@@@ ",
    "   @@@@@",
    "  @     ",
    "  @    @",
    "  @   @ ",
    "  @   @@",
    "  @  @  ",
    "  @  @ @",
    "  @  @@ ",
    "  @  @@@",
    "  @ @   ",
    "  @ @  @",
    "  @ @ @ ",
    "  @ @ @@",
    "  @ @@  ",
    "  @ @@ @",
    "  @ @@@ ",
    "  @ @@@@",
    "  @@    ",
    "  @@   @",
    "  @@  @ ",
    "  @@  @@",
    "  @@ @  ",
    "  @@ @ @",
    "  @@ @@ ",
    "  @@ @@@",
    "  @@@   ",
    "  @@@  @",
    "  @@@ @ ",
    "  @@@ @@",
    "  @@@@  ",
    "  @@@@ @",
    "  @@@@@ ",
    "  @@@@@@",
    " @      ",
    " @     @",
    " @    @ ",
    " @    @@",
    " @   @  ",
    " @   @ @",
    " @   @@ ",
    " @   @@@",
    " @  @   ",
    " @  @  @",
    " @  @ @ ",
    " @  @ @@",
    " @  @@  ",
    " @  @@ @",
    " @  @@@ ",
    " @  @@@@",
    " @ @    ",
    " @ @   @",
    " @ @  @ ",
    " @ @  @@",
    " @ @ @  ",
    " @ @ @ @",
    " @ @ @@ ",
    " @ @ @@@",
    " @ @@   ",
    " @ @@  @",
    " @ @@ @ ",
    " @ @@ @@",
    " @ @@@  ",
    " @ @@@ @",
    " @ @@@@ ",
    " @ @@@@@",
    " @@     ",
    " @@    @",
    " @@   @ ",
    " @@   @@",
    " @@  @  ",
    " @@  @ @",
    " @@  @@ ",
    " @@  @@@",
    " @@ @   ",
    " @@ @  @",
    " @@ @ @ ",
    " @@ @ @@",
    " @@ @@  ",
    " @@ @@ @",
    " @@ @@@ ",
    " @@ @@@@",
    " @@@    ",
    " @@@   @",
    " @@@  @ ",
    " @@@  @@",
    " @@@ @  ",
    " @@@ @ @",
    " @@@ @@ ",
    " @@@ @@@",
    " @@@@   ",
    " @@@@  @",
    " @@@@ @ ",
    " @@@@ @@",
    " @@@@@  ",
    " @@@@@ @",
    " @@@@@@ ",
    " @@@@@@@",
    "@       ",
    "@      @",
    "@     @ ",
    "@     @@",
    "@    @  ",
    "@    @ @",
    "@    @@ ",
    "@    @@@",
    "@   @   ",
    "@   @  @",
    "@   @ @ ",
    "@   @ @@",
    "@   @@  ",
    "@   @@ @",
    "@   @@@ ",
    "@   @@@@",
    "@  @    ",
    "@  @   @",
    "@  @  @ ",
    "@  @  @@",
    "@  @ @  ",
    "@  @ @ @",
    "@  @ @@ ",
    "@  @ @@@",
    "@  @@   ",
    "@  @@  @",
    "@  @@ @ ",
    "@  @@ @@",
    "@  @@@  ",
    "@  @@@ @",
    "@  @@@@ ",
    "@  @@@@@",
    "@ @     ",
    "@ @    @",
    "@ @   @ ",
    "@ @   @@",
    "@ @  @  ",
    "@ @  @ @",
    "@ @  @@ ",
    "@ @  @@@",
    "@ @ @   ",
    "@ @ @  @",
    "@ @ @ @ ",
    "@ @ @ @@",
    "@ @ @@  ",
    "@ @ @@ @",
    "@ @ @@@ ",
    "@ @ @@@@",
    "@ @@    ",
    "@ @@   @",
    "@ @@  @ ",
    "@ @@  @@",
    "@ @@ @  ",
    "@ @@ @ @",
    "@ @@ @@ ",
    "@ @@ @@@",
    "@ @@@   ",
    "@ @@@  @",
    "@ @@@ @ ",
    "@ @@@ @@",
    "@ @@@@  ",
    "@ @@@@ @",
    "@ @@@@@ ",
    "@ @@@@@@",
    "@@      ",
    "@@     @",
    "@@    @ ",
    "@@    @@",
    "@@   @  ",
    "@@   @ @",
    "@@   @@ ",
    "@@   @@@",
    "@@  @   ",
    "@@  @  @",
    "@@  @ @ ",
    "@@  @ @@",
    "@@  @@  ",
    "@@  @@ @",
    "@@  @@@ ",
    "@@  @@@@",
    "@@ @    ",
    "@@ @   @",
    "@@ @  @ ",
    "@@ @  @@",
    "@@ @ @  ",
    "@@ @ @ @",
    "@@ @ @@ ",
    "@@ @ @@@",
    "@@ @@   ",
    "@@ @@  @",
    "@@ @@ @ ",
    "@@ @@ @@",
    "@@ @@@  ",
    "@@ @@@ @",
    "@@ @@@@ ",
    "@@ @@@@@",
    "@@@     ",
    "@@@    @",
    "@@@   @ ",
    "@@@   @@",
    "@@@  @  ",
    "@@@  @ @",
    "@@@  @@ ",
    "@@@  @@@",
    "@@@ @   ",
    "@@@ @  @",
    "@@@ @ @ ",
    "@@@ @ @@",
    "@@@ @@  ",
    "@@@ @@ @",
    "@@@ @@@ ",
    "@@@ @@@@",
    "@@@@    ",
    "@@@@   @",
    "@@@@  @ ",
    "@@@@  @@",
    "@@@@ @  ",
    "@@@@ @ @",
    "@@@@ @@ ",
    "@@@@ @@@",
    "@@@@@   ",
    "@@@@@  @",
    "@@@@@ @ ",
    "@@@@@ @@",
    "@@@@@@  ",
    "@@@@@@ @",
    "@@@@@@@ ",
    "@@@@@@@@"
    };

#endif

extern FILE	*tryopen();

#define	getnyb()	( (hnybble = !hnybble),\
			  (nyb_count++),\
			  ( (hnybble) ? ((*(cdatap++)) & 0xf) : ((*cdatap) >> 4) ))

/* get a nibble from compressed data and covnert it */
void
init_jxlgetrun( buf, len )
uint1	*buf;	/* compressed data */
uint4	len;	/* data length */
{
    hnybble = TRUE;
    cdatap = buf;
    cdatalen = (int)len;
    nyb_count = 0;
    beforerep = 0;
}

int
jxlgetrun( dyn_f, val, re )
uint1	dyn_f;
int	*val;
int	*re;
{
    register int	nyb;
    register int	run;
    register int	rep;

    rep = 0;
    run = 0;
    for( ;; ) {
	if( nyb_count >= (cdatalen * 2) )
	    return TRUE;
	if( (nyb = getnyb()) == 14 ) {
	    rep = -1;
	    continue;
	}
	else if( nyb == 15 ) {
	    rep = beforerep;
	    continue;
	}
	else if( nyb > (int)dyn_f )
	    run = (nyb - dyn_f - 1) * 16 + getnyb() + dyn_f + 1;
	else if( nyb > 0 )
	    run = nyb;
	else {	/* nyb == 0 */
	    if( nyb_count >= (cdatalen * 2) )
		return TRUE;
	    do {
		nyb++;
	    } while( (run = getnyb()) == 0 );
	    while( --nyb >= 0 )
		run = run * 16 + getnyb();
	    run += (13 - dyn_f) * 16 + dyn_f - 15;
	}
	if( rep >= 0 )
	    break;
	rep = run;
    }
    *val = run;
    *re  = rep;
    if( rep != 0 )
	beforerep = rep;
    return FALSE;
}

/* create bitmap form compressed data */
int
unpack_jxlrun( len, w, h, f, p, dyn_f )
uint4	len;	/* raster length */
uint2	w;	/* bitmap width */
uint2	h;	/* bitmap hight */
FILE	*f;	/* JXL file descripter */
uint1	*p;	/* bitmap buffer */
uint1	dyn_f;	/* grif data flag */
{
    uint1		*raster;
    register int	x, y;
    int			repc;	/* repeat count */
    int			runl;	/* run length */
    register int	bits;	/* how many bits to set */
    register int	dmy;	/* dummy variable */
    int			bwid;	/* width in bytes */
    register uint1	*curline, *prevline;
    int			start;
    register uint1	*pp;

    /* readin raster data */
    raster = (uint1 *)malloc( (int)len );
    if( raster == NULL ) {
	fprintf( stderr, "%s: cannot allocate JXL raster buffer\n", progname );
	exit( 2 );
    }
    if( fread( raster, sizeof(uint1), (int)len, f ) <=0 )
	return TRUE;
#ifdef	JXLDEBUG
    fprintf( stderr, "data dump:\n" );
    for( x = 0; x < len; x++ )
	fprintf( stderr, "%02.2X ", raster[x] );
    fprintf( stderr, "\ndata dump end\n" );
#endif
    /* create differential bitmap */
    init_jxlgetrun( raster, len );
    runl = 0;
    repc = 0;
    start = -1;
    pp = p;
    for( y = 0; y < (int)h; y++ ) {
	for( x = start; x < (int)w; x += bits ) {
	    if( (runl + repc) == 0 ) {
		if( jxlgetrun( dyn_f, &runl, &repc ) ) {
		    y = (int)h;	/* no more data --> exit loop */
		    break;
		}
#ifdef	JXLDEBUG
		fprintf( stderr, "(%d,%d) ", runl, repc );
#endif
		repc++;
	    }
	    if( (x + runl) >= (int)w ) {
		runl -= (int)w - x;
		break;
	    }
	    x += runl;
	    runl = 0;
	    bits = repc;
	    if( (x + bits) >= (int)w )
		bits = (int)w -x;
	    if( (dmy = (8 - (x & 7))) < bits )
		bits = dmy;
	    pp[x / 8] |= pat[x & 7];
	    if( (x + bits) & 7 )
		pp[(x + bits)/8] &= ~pat[(x + bits) & 7];
	    repc -= bits;
	}
	start = 0;
	pp += (w + 7) / 8;
    }

#ifdef	JXLDEBUG
{
    register int	xx,yy;
    register uint1	*bp;

    fprintf( stderr, "\n" );
    bp = p;
    for( yy = 0; yy < h; yy++ ) {
	for( xx = 0; xx < (w + 7) / 8; xx++ ) {
	    fprintf( stderr, "%s", bitpp[*bp] );
	    bp++;
	}
	fprintf( stderr, "\n" );
    }
}
#endif	/* JXLDEBUG */

    /* convert to normal bitmap */
    bwid = ((int)w + 7) / 8;
    for( y = 1; y < (int)h; y++ ) {
	curline = &(p[y * bwid]);
	prevline = curline - bwid;
	for( x = 0; x < bwid; x++ )
	    *(curline + x) ^= *(prevline + x);
    }

    free( (char *)raster );
    return FALSE;
}

/* expand JXL grif data */
uint1	*
jxlexpand( fp, w, h )
FILE	*fp;	/* JXL file descripter */
uint2	w;	/* width */
uint2	h;	/* height */
{
    uint1	*bmap;
    uint1	cmd, data;
    uint1	dyn_f, s, ep, bw;
    uint4	datalen;

    /* allocate bitmap area */
    bmap = (uint1 *)calloc( (w + 7)/8 * h, sizeof(uint1) );
    if( bmap == NULL )
	goto MemError;

    /* get raster flag */
    if( get_1byte (fp, &cmd ) )		goto ReadError;
    dyn_f = (uint1)(cmd & 0xf);
    s = (uint1)((cmd >> 4) & 3);
    ep = (uint1)(cmd & 0x40);
    bw = (uint1)(cmd & 0x80);

    /* expand bitmap */
    if( dyn_f == 14 ) {		/* raster data */
#ifdef	JXLDEBUG
	fprintf( stderr, "Raster\n" );
#endif
	datalen = (h * w + 7) / 8;
	if( unpack_raster( datalen, (uint4)w, (uint4)h, fp, bmap ) )	/* same as pkfont.c */
	    goto ReadError;
    }
    else {
	/* take data length */
	datalen = 0;
	do {
	    if( get_1byte(fp, &data) )	goto ReadError;
	    datalen = datalen * 256 + data;
	} while( s-- > 0 );

	if( !ep ) {		/* standard compress data */
#ifdef	JXLDEBUG
	    fprintf( stderr, "PK raster\n" );
#endif
	    if( unpack_run( datalen, (uint4)w, (uint4)h, fp, bmap, dyn_f, bw ) )
		goto ReadError;
	}
	else {			/* expanded compress data */
#ifdef	JXLDEBUG
	    fprintf( stderr, "JXL raster\n" );
#endif
	    if( unpack_jxlrun( datalen, w, h, fp, bmap, dyn_f ) )
		goto ReadError;
	}
    }

    return bmap;

  ReadError:
    fprintf( stderr, "%s: read error on JXL font\n", progname );
    exit( 2 );
    /* NOTREACHED */

  MemError:
    fprintf( stderr, "%s: cannot allocate bitmap buffer\n", progname );
    exit( 2 );
    /* NOTREACHED */
}

/* get a bitmap from font file */
void
jxlloadchar( info, f, c )
struct fdir	*info;		/* font cache element */
struct font	*f;		/* font */
uint4		c;		/* char code */
{
    int2	ku, ten;
    FILE	*fp;		/* JXL file descripter */
    int4	raster_off,	/* offset of raster data in file */
		x_off,		/* offset of Xinfo data in file */
		y_off;		/* offset of Yinfo data in file */
    uint2	xidx,		/* Xinfo offset */
		yidx;		/* Yinfo offset */    
    int2	href, vref;	/* refernece point */
    uint2	width, hight;	/* bitmap size */
    struct fdir	*node, *prevnode;	/* point for work */
    int4	diroff;		/* PXLdir offset of file */
    int		ctype;		/* character code type */
    uint4	dir1, dir2;	/* PXLdir entry data */
    uint1	*bmap;		/* bitmap data */

    /* get PXLdir info */
    fp = f->gdata.finfo->jxlfp;
    ku  = ((int2)c >> 8) - 0x20;
    ten = ((int2)c & 0xff) - 0x20;
    if( iskanji0( c ) ) {
	diroff = (int4)((ku -  1) * 94 + (ten - 1)) * 8 + f->gdata.finfo->dirptr0;
	ctype = 0;
    }
    else if( iskanji1( c ) ) {
	diroff = (int4)((ku - 16) * 94 + (ten - 1)) * 8 + f->gdata.finfo->dirptr1;
	ctype = 1;
    }
    else if( iskanji2( c ) ) {
	diroff = (int4)((ku - 48) * 94 + (ten - 1)) * 8 + f->gdata.finfo->dirptr2;
	ctype = 2;
    }
    else
	goto InvalidCode;
    
    if( fseek( fp, (long)diroff, 0 ) != 0 )	goto SeekError;
    if( get_4bytes( fp, &dir1 ) )	goto Error;
    if( get_4bytes( fp, &dir2 ) )	goto Error;
    raster_off = dir1 & 0xffffff;
    xidx = (uint2)(dir2 >> 16);
    yidx = (uint2)(dir2 & 0xffff);

    /* get Xinfo */
    x_off = f->gdata.finfo->xinfo + (xidx * 6);
    if( fseek( fp, (long)x_off, 0 ) != 0 )	goto SeekError;
    if( get_2bytes( fp, &href ) )	goto Error;
    if( get_2bytes( fp, &width ) )	goto Error;

    /* get Yinfo */
    y_off = f->gdata.finfo->yinfo + (yidx * 6);
    if( fseek( fp, (long)y_off, 0 ) != 0 )	goto SeekError;
    if( get_2bytes( fp, &vref ) )	goto Error;
    if( get_2bytes( fp, &hight ) )	goto Error;
    
#ifdef	JXLDEBUG
    /* print info */
    fprintf( stderr, "#%d [%s:%s]\n\t sum:%lu desing:%lu scale:%lu\n",
	   f->fno, f->info->name, (f->type == JFM)?"JFM":"TFM",
	   f->sum, f->design, f->scale );
    fprintf( stderr, "[%04.4X] %d x %d : ref @ (%d,%d)\n",
	   c, width, hight, href, vref );
    if( c == 0x2142 )
	c = c;
#endif

    /* readin image */
    if( fseek( fp, (long)raster_off, 0 ) != 0 )	goto SeekError;
    bmap = jxlexpand( fp, width, hight );

    /* set data into element */
    info->code = (int2)c;
    info->width = width;
    info->hight = hight;
    info->h_offset = href;
    info->v_offset = vref;
    info->bitmap = bmap;
    info->fontnode = f;

#ifdef	JXLDEBUG
{
    register int	x,y;
    register uint1	*bp;

    bp = bmap;
    for( y = 0; y < hight; y++ ) {
	for( x = 0; x < (width + 7) / 8; x++ ) {
	    fprintf( stderr, "%s", bitp[*bp] );
	    bp++;
	}
	fprintf( stderr, "\n" );
    }
}
#endif	/* JXLDEBUG */

    /* select search list */
    switch( ctype ) {
      case 0:
	node = f->gdata.finfo->jgrif0;
	break;
      case 1:
	node = f->gdata.finfo->jgrif1;
	break;
      case 2:
	node = f->gdata.finfo->jgrif2;
    }

    /* search insersion point */
    prevnode = NULL;
    while( node != NULL ) {
	if( node->code > (int2)c )
	    break;
	prevnode = node;
	node = node->samenext;
    }

    /* do insersion */
    if( prevnode == NULL ) {
	/* insert at top */
	switch( ctype ) {
	  case 0:
	    f->gdata.finfo->jgrif0 = info;
	    break;
	  case 1:
	    f->gdata.finfo->jgrif1 = info;
	    break;
	  case 2:
	    f->gdata.finfo->jgrif2 = info;
	}
	info->sameprev = NULL;
	info->samenext = NULL;
    }
    else {
	info->sameprev = prevnode;
	info->samenext = prevnode->samenext;
	prevnode->samenext = info;
    }
    if( node != NULL ) {
	node->sameprev = info;
	info->samenext = node;
    }

    return;

  InvalidCode:
    fprintf( stderr, "%s: unknown char type %x\n", progname, c );
    abort();
  SeekError:
    fprintf( stderr, "%s: internal error. cannot seek %s\n",
	    progname, f->gdata.finfo->jxlname );
    abort();
  Error:
    fprintf( stderr, "%s: read error on %s\n", progname, f->gdata.finfo->jxlname );
    exit( 2 );
}

/* set basic information of JXL file */
void
jxlready( f )
struct font	*f;
{
    int		res;
    FILE	*jxlfp;
    uint4	dummy,
		xoffset,	/* Xinfo offset */
		yoffset,	/* Yinfo offset */
		ptr0,		/* Hi-Kanji PXLdir offset */
		ptr1,		/* Level 1 Kanji PXLdir offset */
		ptr2;		/* Level 2 Kanji PXLdir offset */
    struct jxlinfo *jxlp;	/* JXL access information */
    char	fname[128];	/* font file name */

    /* open JXL font */
    res = (int)(PRINTER_RES * (double)f->scale / (double)f->design
		* magnification * 5.0 + 0.5);
#if MSDOS
    sprintf( fname, "%d/%s.pxl", res, f->info->name );
#else
    sprintf( fname, "%s.%dpxl", f->info->name, res );
#endif
    jxlfp = tryopen( fontpath, fname );
    if( jxlfp == NULL ) {
#if MSDOS
	sprintf( fname, "%d/%s.pxl", res - 1, f->info->name );
#else
	sprintf( fname, "%s.%dpxl", f->info->name, res - 1 );
#endif
	jxlfp = tryopen( fontpath, fname );
	if( jxlfp == NULL ) {
#if MSDOS
	    sprintf( fname, "%d/%s.pxl", res + 1, f->info->name );
#else
	    sprintf( fname, "%s.%dpxl", f->info->name, res + 1 );
#endif
	    jxlfp = tryopen( fontpath, fname );
	    if( jxlfp == NULL ) {
#if MSDOS
		sprintf( fname, "%dpxl/%s.pxl", res, f->info->name );
		jxlfp = tryopen( fontpath, fname );
		if( jxlfp == NULL ) {
		    sprintf( fname, "%dpxl/%s.pxl", res + 1, f->info->name );
		    jxlfp = tryopen( fontpath, fname );
		    if( jxlfp == NULL ) {
			sprintf( fname, "%dpxl/%s.pxl", res - 1, f->info->name );
			jxlfp = tryopen( fontpath, fname );
			if( jxlfp == NULL ) {
#endif
			    fprintf( stderr, "%s: warning! %s(-2) is not exist\n",
				    progname, fname );
			    return;
#if MSDOS
			}	
		    }
		}
#endif
	    }
	}
    }

    /* check ID */
    if( fseek( jxlfp, -4L, 2 ) != 0 )	goto Error;
    if( get_4bytes( jxlfp, &dummy ) )	goto Error;
    if( dummy != PXLID ) {
	fprintf( stderr, "%s: warning! %s has invalid ID. ignore\n",
		 progname, fname );
	goto Finish;
    }

    /* get basic data */
    if( fseek( jxlfp, -64L, 2 ) != 0 )	goto Error;
    if( get_4bytes( jxlfp, &dummy ) )	goto Error;	/* skip check-sum */
    if( get_4bytes( jxlfp, &dummy ) )	goto Error;	/* magnification */
    if( get_4bytes( jxlfp, &dummy ) )	goto Error;	/* magnification */
    if( dummy != f->info->design ) {
	fprintf( stderr, "%s: warning! %s has invalid design size. ignore\n",
		 progname, fname );
	goto Finish;
    }
    if( get_4bytes( jxlfp, &dummy ) )	goto Error;	/* fixed size */
    if( get_4bytes( jxlfp, &dummy ) )	goto Error;	/* fixed width */
    if( get_4bytes( jxlfp, &dummy ) )	goto Error;	/* tfm ptr */
    if( get_4bytes( jxlfp, &dummy ) )	goto Error;	/* # of tfm entry */
    if( get_4bytes( jxlfp, &xoffset ) )	goto Error;	/* Xinfo offset */
    if( get_4bytes( jxlfp, &dummy ) )	goto Error;	/* # of Xinfo entry */
    if( get_4bytes( jxlfp, &yoffset ) )	goto Error;	/* Yinfo offset */
    if( get_4bytes( jxlfp, &dummy ) )	goto Error;	/* # of Yinfo entry */
    if( get_4bytes( jxlfp, &dummy ) )	goto Error;	/* # of PXLdir entry */
    if( get_4bytes( jxlfp, &ptr0 ) )	goto Error;	/* PXLdir-0 offset */
    if( get_4bytes( jxlfp, &ptr1 ) )	goto Error;	/* PXLdir-1 offset */
    if( get_4bytes( jxlfp, &ptr2 ) )	goto Error;	/* PXLdir-2 offset */

    /* allocate information table and set value */
    jxlp = (struct jxlinfo *)malloc( sizeof(struct jxlinfo) );
    if( jxlp == NULL ) {
	fprintf( stderr, "%s: warning! cannot allocate JXL font table\n", progname );
	goto Finish;
    }
    jxlp->jxlfp = NULL;
    jxlp->jxlname = malloc( strlen(fname) + 1 );
    if( jxlp->jxlname == NULL ) {
	fprintf( stderr, "%s: warning! cannot allocate JXL font table\n", progname );
	goto Finish;
    }
    (void)strcpy( jxlp->jxlname, fname );
    jxlp->xinfo = xoffset;
    jxlp->yinfo = yoffset;
    jxlp->dirptr0 = ptr0;
    jxlp->dirptr1 = ptr1;
    jxlp->dirptr2 = ptr2;
    jxlp->jgrif0 = jxlp->jgrif1 = jxlp->jgrif2 = NULL;
    f->gdata.finfo = jxlp;
    
    f->prn->download = TRUE;	/* using JXL file flag */
    fclose( jxlfp );
    return;

  Error:
    fprintf( stderr, "%s: read error %s. use internal font\n", progname, fname );
  Finish:
    fclose( jxlfp );
    return;
}

