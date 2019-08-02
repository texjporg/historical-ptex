/*
 *	cache.c -- JXL font caching routine.
 *
 *	Written by H. Nagahara on 2/10/92 08:57:36.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: cache.c 10.1
 */
#include	<stdio.h>
#include	"config.h"
#include	"font.h"
#include	"drive.h"
#include	"ktype.h"
#if    MSDOS
#  include	<malloc.h>
#  include	<stdlib.h>
#else
  extern char	*malloc(), *calloc();
#endif

/* load JXL image */
extern	void	jxlloadchar PP((struct fdir *, struct font *, uint4));

#ifdef	CACHEDEBUG
void
cache_status()
{
    struct fdir	*element;

    for( element = cachetop; element != NULL; element = element->next ) {
	fprintf( stderr, "%04.4X in %s\n", element->code, (element->fontnode != NULL)?(element->fontnode->info->name):"Not belong" );
    }
    if( cachebottom == NULL )
	fprintf( stderr, "No Bottom char!\n" );
    else
	fprintf( stderr, "%04.4X in %s <--Bottom!\n", cachebottom->code, (cachebottom->fontnode != NULL)?(cachebottom->fontnode->info->name):"Not belong" );
}
#endif

/* allocate & initialize cache area */
int
makecache()
{
    register struct fdir	*area, *prev;
    register int		c;

    /* allocate memory for cache */
    area = (struct fdir *)calloc( cache_size, sizeof(struct fdir) );
    if( area == NULL ) {
	fprintf( stderr, "%s: cannot allocate cache space\n", progname );
	return TRUE;
    }

    /* initialize cache area */
    prev = NULL;
    for( c = 0; c < cache_size; c++ ) {
	(area + c)->prev = prev;
	prev = area + c;
	(area + c)->next = prev + 1;
    }
    (area + cache_size - 1)->next = NULL;

    cachefree = area;
    return FALSE;
}

/* touch a node: move node to the top */
void
touchnode( element )
struct fdir	*element;
{
    /* cut chain */
    if( element->prev == NULL )
	return;		/* already top of list */
    else
	(element->prev)->next = element->next;
    if( element->next == NULL )
	cachebottom = element->prev;
    else
	(element->next)->prev = element->prev;

    /* reconnect as top */
    element->next = cachetop;
    element->prev = NULL;
    if( cachetop != NULL )
	cachetop->prev = element;
    cachetop = element;
    if( cachebottom == NULL )
	cachebottom = element;

    return;
}

/* make new font node */
struct fdir *
getnode()
{
    struct fdir	*element;

    /* takeout a element */
    if( cachefree != NULL ) {
	/* take a node from free node list */
	element = cachefree;
	cachefree = element->next;
    }
    else {
	/* disconnect a element */
	element = cachebottom;
	cachebottom = element->prev;
	cachebottom->next = NULL;
	if( element->sameprev == NULL ) {
	    if( iskanji0( element->code ) )
		element->fontnode->gdata.finfo->jgrif0 = element->samenext;
	    else if( iskanji1( element->code ) )
		element->fontnode->gdata.finfo->jgrif1 = element->samenext;
	    else if( iskanji2( element->code ) )
		element->fontnode->gdata.finfo->jgrif2 = element->samenext;
	    else
		goto Error;
	}
	else
	    (element->sameprev)->samenext = element->samenext;
        if( element->samenext != NULL )
	    (element->samenext)->sameprev = element->sameprev;
	free( element->bitmap );
    }

    /* initialize element and add it to the top */
    element->code = 0;
    element->width = element->hight
	= element->h_offset = element->v_offset = 0;
    element->bitmap = NULL;
    element->samenext = element->sameprev = NULL;
    element->next = cachetop;
    element->prev = NULL;
    if( cachetop != NULL )
	cachetop->prev = element;
    cachetop = element;
    if( cachebottom == NULL )
	cachebottom = element;

    return element;

  Error:
    fprintf( stderr, "%s: unknown type of code %x\n", progname, element->code );
    exit( 2 );
    /* NOTREACHED */
}

/* search and return a font */
struct fdir *
getfont( f, c )
struct font	*f;
uint4		c;	/* char code */
{
    register struct fdir	*chp;

    /* search in chace */
    if( iskanji0( c ) )
	chp = f->gdata.finfo->jgrif0;
    else if( iskanji1( c ) )
	chp = f->gdata.finfo->jgrif1;
    else if( iskanji2( c ) )
	chp = f->gdata.finfo->jgrif2;
    else {
	fprintf( stderr, "%s: invalid Kanji code %x\n", progname, c );
	abort();
    }
    while( chp != NULL ) {
	if( (uint4)chp->code == c ) {
	    /* found in chace */
	    touchnode( chp );
#ifdef	CACHEDEBUG
	    fprintf( stderr, "touch element!\n" );
	    cache_status();
#endif
	    return chp;
	}
	if( (uint4)chp->code > c )
	    break;		/* not found in samefont chain */
	chp = chp->samenext;
    }

    /* take new node and set its value */
    chp = getnode();
    jxlloadchar( chp, f, c );
#ifdef	CACHEDEBUG
    fprintf( stderr, "changed element!\n" );
    cache_status();
#endif
    return chp;
}
