/*
 *	pkfont.c -- expand pk font grif
 *
 *	Written by H. Nagahara on 2/10/92 08:59:10.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: pkfont.c 10.1
 */
#include	<stdio.h>
#include	"config.h"
#include	<memory.h>
#include	"drive.h"
#include	"font.h"

#if	MSDOS
# include	<malloc.h>
#else
  extern	char	*malloc(), *calloc();
#endif

/* PK file command definition */
#define	PK_PRE		247
#define	PK_POST		245
#define	PK_XXX1		240
#define	PK_XXX2		241
#define	PK_XXX3		242
#define	PK_XXX4		243
#define	PK_YYY		244
#define	PK_NO_OP	246
#define	PKID		89

uint1	pat[8] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
int	hnybble;
int	nyb_count;
uint1	*cdatap;

#if	PKFONT || PXLFONT
#define	getnyb()	( (hnybble = !hnybble),\
			  (nyb_count++),\
			  ( (hnybble) ? ((*(cdatap++)) & 0xf) : ((*cdatap) >> 4) ))

#ifdef	BYTELOAD
FILE	*pkfile;

int
getnyb()
{
    static uint1 c;
    register int nyb;

    nyb_count++;
    if( hnybble ) {
	(void)fread( &c, sizeof(uint1), 1, pkfile );
	nyb = c >> 4;
    }
    else
	nyb = c & 0xf;
    hnybble = !hnybble;
    return nyb;
}			 
#endif

/* get a nibble from compressed data and covnert it */
void
init_getrun( buf )
uint1	*buf;	/* compressed data */
{
    hnybble = TRUE;
    cdatap = buf;
    nyb_count = 0;
}

void
getrun( dyn_f, val, re )
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
	if( (nyb = getnyb()) >= 14 ) {
	    rep = nyb * 2 - 29;
	    continue;
	}
	else if( nyb > (int)dyn_f )
	    run = (nyb - dyn_f - 1) * 16 + getnyb() + dyn_f + 1;
	else if( nyb > 0 )
	    run = nyb;
	else {	/* nyb == 0 */
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
    return;
}

/* create bitmap form compressed data */
int
unpack_run( len, w, h, f, p, dyn_f, bw )
uint4	len;	/* raster length */
uint4	w;	/* bitmap width */
uint4	h;	/* bitmap hight */
FILE	*f;	/* PK file descripter */
uint1	*p;	/* bitmap buffer */
uint1	dyn_f;	/* grif data flag */
uint1	bw;	/* black or white flag */
{
    uint1		*raster;
    register int	x, y;
    int			repc;	/* repeat count */
    int			runl;	/* run length */
    register int	bits;	/* how many bits to set */
    register int	dmy;	/* dummy variable */
    uint1		*curline;	/* current line */
    register int	i;	/* loop counter */

    /* readin raster data */
    raster = (uint1 *)malloc( (int)len );
    if( raster == NULL ) {
	fprintf( stderr, "%s: cannot allocate raster buffer\n", progname );
	return TRUE;
    }
    if( fread( raster, sizeof(uint1), (int)len, f ) <=0 ) {
	fprintf( stderr, "%s: PK file read error\n", progname );
	return TRUE;
    }

#ifdef	BYTELOAD
    pkfile = f;
#endif

    /* create bitmap */
    init_getrun( raster );
    bw = (uint1)!bw;
    runl = 0;
    for( y = 0; y < (int)h; y += repc + 1 ) {
	repc = 0;
	for( x = 0; x < (int)w; x += bits ) {
	    if( runl == 0 ) {
		getrun( dyn_f, &runl, &repc );
		bw = (uint1)!bw;
	    }
	    if( (x + runl) > (int)w )
		bits = (int)w - x;
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
    free( (char *)raster );
    return FALSE;
}

/* create bitmap from raster data */
int
unpack_raster( len, w, h, f, p )
uint4	len;	/* raster length */
uint4	w;	/* bitmap width */
uint4	h;	/* bitmap hight */
FILE	*f;	/* PK file descripter */
uint1	*p;	/* bitmap buffer */
{
    uint1		*raster;
    register uint1	*d;
    register int	x, y, bits;
    register int	n;

    /* readin raster data */
    raster = (uint1 *)malloc( (int)len );
    if( raster == NULL ) {
	fprintf( stderr, "%s: cannot allocate raster buffer\n", progname );
	return TRUE;
    }
    if( fread( raster, sizeof(uint1), (int)len, f ) <=0 ) {
	fprintf( stderr, "%s: PK file read error\n", progname );
	return TRUE;
    }

    /* create bitmap */
    d = raster;
    n = 8;
    for( y = 0; y < (int)h; y++ ) {
	for( x = 0; x < (int)w; x += bits ) {
	    if( n == 0 ) {
		n = 8;
		d++;
	    }
	    bits = 8 - (x & 7);
	    if( bits > n )
		bits = n;
	    if( bits > ((int)w - x) )
		bits = (int)w - x;
	    p[x/8] |= *d >> (x & 7);
	    *d <<= bits;
	    n -= bits;
	}
	if( w & 7 )
	    p[w / 8] &= ~pat[w & 7];
	p += (w + 7) / 8;
    }

    free( (char *)raster );
    return FALSE;
}
    
#if	PKFONT
/* get bitmap data */
int
expandbit( cmd, fp, p, f )
uint1		cmd;
struct font	*fp;
struct grif	*p;
FILE		*f;
{
    int		base;		/* character code base */
    uint1	dyn_f,		/* greif type */
		bw,		/* black or white */
		es,		/* expanded or not */
		size;		/* grif data size */
    uint4	pl,		/* grif data size */
		cc,		/* character code */
		tfm,		/* design size */
		dx,		/* next reference point (horizontal) */
		dy,		/*			(virtical) */
		w,		/* bitmap horizontal size */
		h;		/* bitmap virtical size */
    int4	hoff,		/* reference point (horizontal) */
		voff;		/* 		   (virtical) */
    uint4	datalen;	/* raster data length */
    uint1	*bmap;		/* pointer to bitmap data */
    struct grif	*myroom;	/* pointer to my font info */
    uint1	dmy1;		/* dummy variable */
    uint2	dmy2;
    int1	dmy1s;
    int2	dmy2s;

    /* analyze command */
    dyn_f = (uint1)(cmd >> 4);
    bw = (uint1)(cmd & 0x8);
    es = (uint1)(cmd & 0x4);
    size = (uint1)(cmd & 0x3);
    base = fp->info->begin;

    /* get parameters */
    if( es ) {
	if( size == 3  ) {
	    /* standard format */
	    if( get_4bytes( f, &pl ) )	goto Error;
	    if( get_4bytes( f, &cc ) )	goto Error;
	    if( get_4bytes( f, &tfm ) )	goto Error;
	    if( get_4bytes( f, &dx ) )	goto Error;
	    if( get_4bytes( f, &dy ) )	goto Error;
	    if( get_4bytes( f, &w ) )	goto Error;
	    if( get_4bytes( f, &h ) )	goto Error;
	    if( get_4bytes( f, &hoff ))	goto Error;
	    if( get_4bytes( f, &voff ))	goto Error;
	    datalen = pl - 28;
	}
	else {
	    /* expanded and compressed format */
	    if( get_2bytes( f, &dmy2 ))	goto Error;
	    pl = (uint4)dmy2;
	    if( get_1byte( f, &dmy1 ) )	goto Error;
	    cc = (uint4)dmy1;
	    if( get_3bytes( f, &tfm ))	goto Error;
	    if( get_2bytes( f, &dmy2 ))	goto Error;
	    dx = (uint4)dmy2;
	    dy = (uint4)0;
	    if( get_2bytes( f, &dmy2) )	goto Error;
	    w = (uint4)dmy2;
	    if( get_2bytes( f, &dmy2) )	goto Error;
	    h = (uint4)dmy2;
	    if( get_2bytes( f, &dmy2s))	goto Error;
	    hoff = (int4)dmy2s;
	    if( get_2bytes( f, &dmy2s))	goto Error;
	    voff = (int4)dmy2s;
	    datalen = size * 65536 + pl - 13;
	}
    }
    else {
	/* compressed format */
	if( get_1byte( f, &dmy1 ))	goto Error;
	pl = (uint4)dmy1;
	if( get_1byte( f, &dmy1 ) )	goto Error;
	cc = (uint4)dmy1;
	if( get_3bytes( f, &tfm ))	goto Error;
	if( get_1byte( f, &dmy1 ))	goto Error;
	dx = (uint4)dmy1;
	dy = (uint4)0;
	if( get_1byte( f, &dmy1) )	goto Error;
	w = (uint4)dmy1;
	if( get_1byte( f, &dmy1) )	goto Error;
	h = (uint4)dmy1;
	if( get_1byte( f, &dmy1s))	goto Error;
	hoff = (int4)dmy1s;
	if( get_1byte( f, &dmy1s))	goto Error;
	voff = (int4)dmy1s;
	datalen = size * 256 + pl - 8;
    }
	
    /* set character info */
    myroom = p + (cc - base);
    myroom->code = cc;
    myroom->width = (uint2)w;
    myroom->hight = (uint2)h;
    myroom->hrefp = (int2)hoff;
    myroom->vrefp = (int2)voff;
    if( (w == 0) || (h == 0 ) ) {
	myroom->bitmap = NULL;
	return FALSE;
    }
#if	USEEMS
    calloc_grif_bitmap( myroom, ((int)w + 7)/8 * (int)h, sizeof(uint1) );
    bmap = myroom->bitmap;
#else
    bmap = (uint1 *)calloc( ((int)w + 7)/8 * (int)h, sizeof(uint1) );
    myroom->bitmap = bmap;
#endif
    if( bmap == NULL ) {
	fprintf( stderr, "%s: cannot allocate memory for bitmap\n", progname );
	return TRUE;
    }

    /* expand bitmap */
    if( dyn_f == 14 ) {
	if( unpack_raster( datalen, w, h, f, bmap ) )
	    return TRUE;
    }
    else {
	if( unpack_run( datalen, w, h, f, bmap, dyn_f, bw ) )
	    return TRUE;
    }

    return FALSE;

  Error:
    fprintf( stderr, "%s: PK file read error\n", progname );
    free( bmap );
    return TRUE;
}

/* readin PK grif in memory */
struct grif *
pkload( f, fp, fname )
struct font	*f;	/* font data */
FILE		*fp;	/* PK file descripter */
char		*fname;	/* file name */
{
    uint1	cmd;	/* command */
    uint1	dmy1;
    uint2	dmy2;
    uint4	dmy4;
    uint1	slen;	/* string length */
    char	lbuf[128];
    uint4	design,	/* design size */
		csum;	/* check sum */
    uint4	hppp,	/* horizontal pixcels / point */
		vppp;	/* virtical pixcels / point */
    struct grif	*info;	/* top of font info */
    int		cont;	/* loop flag */
    
    /* get preample */
    if( get_1byte ( fp, &cmd ) )	goto Error;
    if( get_1byte ( fp, &dmy1 ) )	goto Error;
    if( (cmd != PK_PRE) || (dmy1 != PKID) ) {
	fprintf( stderr, "%s: %s not valid PK format\n", progname, fname );
	return NULL;
    }
    if( get_1byte ( fp, &slen ) )	goto Error;
    if( slen != 0 )
	if( readstring( fp, (uint4)slen, lbuf ) )	goto Error;
    if( get_4bytes( fp, &design ) )	goto Error;
    if( get_4bytes( fp, &csum ) )	goto Error;
    if( get_4bytes( fp, &hppp ) )	goto Error;
    if( get_4bytes( fp, &vppp ) )	goto Error;

    if( (f->sum != 0) && (f->sum != csum) )
	fprintf( stderr, "%s: warning! %s check-sum is not match to TFM\n",
		progname, fname );

    /* allocate grif buffer */
    info = (struct grif *)calloc( (f->info->end) - (f->info->begin) + 1,
				  sizeof(struct grif) );
    if( info == NULL ) {
	fprintf( stderr, "%s: cannot allocate grif buffer\n", progname );
	return NULL;
    }
    
    /* font reading loop */
    cont = TRUE;
    while( cont ) {
	if( fread( (char *)&cmd, sizeof(char), 1, fp ) < 1 )
	    goto PKError;
	if( (cmd >> 4) != 0xf ) {
	    if( expandbit( cmd, f, info, fp ) )
		goto PKError;
	    continue;
	}
	
	switch( cmd ) {
	  case PK_POST:
	    cont = FALSE;
	    break;
	  case PK_XXX1:
	    if( get_1byte( fp, &slen ) )	goto PKError;
	    if( readstring( fp, (uint4)slen, lbuf ) )	goto PKError;
	    break;
	  case PK_XXX2:
	    if( get_2bytes(fp, &dmy2) )	goto PKError;
	    if( readstring( fp, (uint4)dmy2, lbuf ) )	goto PKError;
	    break;
	  case PK_XXX3:
	    if( get_3bytes(fp, &dmy4) )	goto PKError;
	    if( readstring( fp, (uint4)dmy4, lbuf ) )	goto PKError;
	    break;
	  case PK_XXX4:
	    if( get_4bytes(fp, &dmy4) )	goto PKError;
	    if( readstring( fp, (uint4)dmy4, lbuf ) )	goto PKError;
	    break;
	  case PK_YYY:
	    if( get_4bytes(fp, &dmy4) )	goto PKError;
	    break;
	  case PK_NO_OP:
	    break;
	  default:
	    fprintf( stderr, "%s: invalid command in PK file %s\n",
		    progname, fname );
	    goto Error;
	}
    }
    
    return info;

  PKError:
    fprintf( stderr, "%s: read error on %s\n", progname, fname );
  Error:
    free( info );
    return NULL;
}
#endif	/* PKFONT */
#endif	/* PKFONT || PXLFONT */
