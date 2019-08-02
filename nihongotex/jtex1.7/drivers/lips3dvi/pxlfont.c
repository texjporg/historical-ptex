/*
 *	pxlfont.c -- expand pxl font grif
 *
 *	Written by H. Nagahara on 2/10/92 08:59:14.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: pxlfont.c 10.1
 */
#include	<stdio.h>
#include	"config.h"
#if	PXLFONT
#include	<memory.h>
#include	"drive.h"
#include	"font.h"

#if MSDOS	/* ANSI prototype declaration */
# include	<malloc.h>
int	get_1byte( FILE *, uint1 * );
int	get_2bytes( FILE *, uint2 * );
int	get_3bytes( FILE *, uint4 * );
int	get_4bytes( FILE *, uint4 * );
int	readstring( FILE *, uint4, char * );
#else
  extern char	*malloc(), *calloc();
#endif

#define	WORDPXL		1001
#define	BYTEPXL		1002
#define	PKPXL		1003

extern uint1	pat[8];		/* work for unpack PK data: in pkfont.c */
extern int	hnybble;
extern int	nyb_count;
extern uint1	*cdatap;

/* read byte raster */
#define	pxl_byteload(f,x,y,b)	(fread((b),sizeof(uint1),(unsigned int)(((x+7)/8)*y),f) != (unsigned int)(((x+7)/8)*y))

/* read word raster */
int
pxl_wordload( f, x, y, buf )
FILE		*f;	/* raster data file */
uint2		x, y;	/* bitmap size */
uint1		*buf;	/* bitmap buffer */
{
    register uint2	xx, yy;
    uint2		wsize;
    register int	counter;
    uint1		dmy;

    wsize = ((x + 7) / 8);
    counter = 0;
    for( yy = 0; yy < y; yy++ ) {
	for( xx = 0; xx < wsize; xx++ ) {
	    if( get_1byte( f, buf++ ) )		return TRUE;
	    counter++;
	}
	while( (counter % 4) != 0 ) {
	    if( get_1byte( f, &dmy ) )		return TRUE;
	    counter++;
	}
    }
    return FALSE;
}

/* take a nybble form font file */
int
pxl_getnyb( f )
FILE	*f;
{
    static uint1    c;
    register int    nyb;

    nyb_count++;
    if( hnybble ) {
	(void)fread( &c, sizeof(uint1), 1, f );
	nyb = c >> 4;
    }
    else
	nyb = c & 0xf;
    hnybble = !hnybble;
    return nyb;
}			 

/* take a run of PK data */
void
pxl_getrun( f, dyn_f, val, re )
FILE	*f;
uint1	dyn_f;
int	*val;
int	*re;
{
    register int	nyb;
    register int	run;
    register int	rep;

    rep = *re;
    run = 0;
    for( ;; ) {
	if( (nyb = pxl_getnyb( f )) >= 14 ) {
	    rep = nyb * 2 - 29;
	    continue;
	}
	else if( nyb > (int)dyn_f )
	    run = (nyb - (int)dyn_f - 1) * 16 + pxl_getnyb( f ) + dyn_f + 1;
	else if( nyb > 0 )
	    run = nyb;
	else {	/* nyb == 0 */
	    do {
		nyb++;
	    } while( (run = pxl_getnyb( f )) == 0 );
	    while( --nyb >= 0 )
		run = run * 16 + pxl_getnyb( f );
	    run += (13 - (int)dyn_f) * 16 + (int)dyn_f - 15;
	}
		if( rep >= 0 )
	    break;
	rep = run;
    }
    *val = run;
    *re  = rep;
    return;
}

/* create bitmap form compressed data */
int
pxl_pkload( f, w, h, p, dyn_f, bw )
FILE	*f;	/* PK file descripter */
uint2	w;	/* bitmap width */
uint2	h;	/* bitmap hight */
uint1	*p;	/* bitmap buffer */
uint1	dyn_f;	/* grif data flag */
uint1	bw;	/* black or white flag */
{
    register uint2	x, y;
    unsigned int	repc;	/* repeat count */
    unsigned int	runl;	/* run length */
    register int	bits;	/* how many bits to set */
    register int	dmy;	/* dummy variable */
    uint1		*curline;	/* current line */
    register unsigned int i;	/* loop counter */

    /* initialize pxl_getrun variables */
    hnybble = TRUE;
    cdatap = p;
    nyb_count = 0;

    /* create bitmap */
    bw = (uint1)!bw;
    runl = 0;
    for( y = 0; y < h; y += repc + 1 ) {
	repc = 0;
	for( x = 0; x < w; x += bits ) {
	    if( runl == 0 ) {
		pxl_getrun( f, dyn_f, &runl, &repc );
#ifdef PXLDEBUG
		fprintf( stderr, "(%d,%d)", runl, repc );
#endif
		bw = (uint1)!bw;
	    }
	    if( (x + runl) > w )
		bits = w - x;
	    else
		bits = runl;
	    if( bw ) {
		if( (dmy = (8 - (x & 7))) < bits )
		    bits = dmy;
		p[x / 8] |= pat[x & 7];
		if( (x + bits) & 7 )
		    p[(x + bits)/8] &= ~pat[(x + bits) & 7];
	    }
	    runl -= bits;
	}
	curline = p;
	p += (w + 7)/8;
	for( i = 0; i < repc; i++ ) {
	    (void)memcpy( p, curline, (int)((w + 7)/8) );
	    p += (w + 7)/8;
	}
    }
#ifdef PXLDEBUG
	fprintf( stderr, "\n\n" );
#endif
    return FALSE;
}

/* readin PXL grif in memory */
struct grif *
pxlload( f, fp, fname, top, last )
struct font	*f;	/* font data */
FILE		*fp;	/* PK file descripter */
char		*fname;	/* file name */
int4		top, last;
{
    uint4		pxlid;		/* file format */
    uint4		dummy;
    uint4		dirpos;		/* PXLdir position in file */
    uint4		dir_pos;	/* PXLdir byte offset */
    uint4		*pos;		/* raster data byte offset array */
    struct grif		*info;		/* grif data */
    register struct grif *myroom;	/* current girf data */
    register int	c;		/* char code */
    uint2		ww;		/* grif width */
    uint2		hh;		/* grif height */
    int2		ho;		/* horizontal ref. point */
    int2		vo;		/* vertical ref. point */
    uint4		rp;		/* raster offset in file */
    uint1		cmd;		/* PK raster commadn */
    uint1		dyn_f;
    uint1		bw;
    uint1		ep;

    /* check ID */
    if( fseek( fp, last - 4L, 0 ) != 0 )	goto Error;
    if( get_4bytes( fp, &pxlid ) )	goto Error;
    if( (pxlid != WORDPXL) && (pxlid != BYTEPXL) && (pxlid != PKPXL) ) {
	fprintf( stderr, "%s: %s not valid PXL format\n", progname, fname );
	return NULL;
    }

    /* get BASIC data */
    if( fseek( fp, last - 20L, 0 ) != 0 )	goto Error;
    if( get_4bytes( fp, &dummy ) )	goto Error;	/* check sum */

    if( (f->sum != 0) && (dummy != f->sum) )
	fprintf( stderr, "%s: warning %s check-sum is not match to TFM\n", progname, fname );

    if( get_4bytes( fp, &dummy ) )	goto Error;	/* magnification */
    if( get_4bytes( fp, &dummy ) )	goto Error;	/* disign size */
    if( get_4bytes( fp, &dirpos ) )	goto Error;	/* PXLdir position */

    /* allocate PXLdir buffer */
    pos = (uint4 *)malloc( sizeof(uint4) * 128 );
    if( pos == NULL ) {
	fprintf( stderr, "%s: cannot allocate PXL work\n", progname );
	return NULL;
    }

    /* allocate grif buffer */
    info = (struct grif *)calloc( (f->info->end) - (f->info->begin) + 1,
				  sizeof(struct grif) );
    if( info == NULL ) {
	fprintf( stderr, "%s: cannot allocate grif buffer\n", progname );
	return NULL;
    }
    
    /* read PXLdir */
    dir_pos = (pxlid == WORDPXL) ? (dirpos * 4) : dirpos;
    if( fseek( fp, (long)dir_pos + top, 0 ) != 0 )	goto Error;
    for( c = 0; c <= f->info->end; c++ ) {
	/* read PXLdir entry */
	if( get_2bytes( fp, &ww ) )	goto Error;	/* width */
	if( get_2bytes( fp, &hh ) )	goto Error;	/* height */
	if( get_2bytes( fp, &ho ) )	goto Error;	/* horizontal offset */
	if( get_2bytes( fp, &vo ) )	goto Error;	/* vertical offset */
	if( get_4bytes( fp, &rp ) )	goto Error;	/* raster offset */
	if( get_4bytes( fp, &dummy ) )	goto Error;	/* skip TFM width */
	if( c < f->info->begin )
	    continue;
	
	/* set directory entry data */
	myroom = info + (c - f->info->begin);
	myroom->code = c;
	myroom->width = ww;
	myroom->hight = hh;
	myroom->hrefp = ho;
	myroom->vrefp = vo;
	if( (ww == 0) || (hh == 0) ) {
	    myroom->bitmap = NULL;
	    continue;
	}
#if	USEEMS
	calloc_grif_bitmap( myroom, ((ww + 7) / 8 * hh), sizeof(uint1) );
#else
	myroom->bitmap = (uint1 *)calloc( (ww + 7) / 8 * hh, sizeof(uint1) );
#endif
	if( myroom->bitmap == NULL ) {
	    fprintf( stderr, "%s: cannot allocate memory for bitmap\n", progname );
	    goto Release;
	}
	pos[c] = (pxlid == WORDPXL) ? (rp * 4) : rp;
    }

    /* font reading loop */
    switch( (int)pxlid ) {
      case WORDPXL:
	for( c = f->info->begin; c <= f->info->end; c++ ) {
	    myroom = info + (c - f->info->begin);
	    if( myroom->bitmap == NULL )
		continue;
	    if( fseek( fp, (long)pos[c] + top, 0 ) != 0 )	goto Error;
	    if( pxl_wordload( fp, myroom->width, myroom->hight, myroom->bitmap ) )
		goto Release;
	}
	break;
      case BYTEPXL:
	for( c = f->info->begin; c <= f->info->end; c++ ) {
	    myroom = info + (c - f->info->begin);
	    if( myroom->bitmap == NULL )
		continue;
	    if( fseek( fp, (long)pos[c] + top, 0 ) != 0 )	goto Error;
	    if( pxl_byteload( fp, (int)myroom->width,
			     (int)myroom->hight, myroom->bitmap ) )
		goto Release;
	}
	break;
      case PKPXL:
	for( c = f->info->begin; c <= f->info->end; c++ ) {
	    myroom = info + (c - f->info->begin);
	    if( myroom->bitmap == NULL )
		continue;
	    if( fseek( fp, (long)pos[c] + top, 0 ) != 0 )	goto Error;
	    if( get_1byte( fp, &cmd ) )		goto Error;
	    dyn_f = (uint1)(cmd & 0xf);
	    bw = (uint1)(cmd & 0x80);
	    ep = (uint1)(cmd & 0x40);
	    if( ep ) {
		if( pxl_byteload( fp, (int)myroom->width,
				 (int)myroom->hight, myroom->bitmap ) )
		    goto Release;
	    }
	    else {
#ifdef	PXLDEBUG
		fprintf( stderr, "%x(%d,%d)\n",myroom->code,myroom->width,myroom->hight );
#endif
		if( pxl_pkload( fp, myroom->width, myroom->hight,
			       myroom->bitmap, dyn_f, bw ) )
		    goto Release;
	    }
	}
	break;
    }

    free( (char *)pos );
    return info;

  Error:
    fprintf( stderr, "%s: read error on %s\n", progname, fname );
  Release:
    for( c = f->info->begin; c <= f->info->end; c++ ) {
	if( (info + c)->bitmap != NULL )
#if	USEEMS
	    free_grif_bitmap( (info + c) );
#else
	    free( (info + c)->bitmap );
#endif
	else break;
    }
    free( info );
    return NULL;
}

#endif	/* PXLFONT */
