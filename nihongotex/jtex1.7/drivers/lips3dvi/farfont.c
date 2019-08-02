/*
 *	farfont.c -- handle ASCII far font archive
 *
 *	This routine is only for DOS machine.
 *
 *	Written by H. Nagahara on 2/10/92 08:58:21.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: farfont.c 10.1
 */
#include	<stdio.h>
#include	"config.h"

#if	MSDOS && FARFONT
#include	<stdlib.h>
#include	<string.h>
#include	"drive.h"
#include	"font.h"
#include	"lbp.h"

#define	FARID	('f'+'o'+'n'+'t'+'a'+'r'+'c'+'h'+'i'+'v'+'e')

extern FILE	*tryopen();

/* font directory entry */
struct fardir {
    char	name[10];
    long	timev;
    long	fontsize;
    long	fontpos;
};

struct grif *
farload( f )
struct font	*f;
{
    FILE	*farfp;		/* far file */
    int		res;		/* fotn resolution */
    char	farname[128];	/* far file name */
    struct grif	*fbuf;		/* font buffer */
    int4	top, last;	/* pxlfont position in far file */
    uint2	fontnum;	/* number of fonts in far file */
    int4	dirpos;		/* font directory position */
    uint2	farid;		/* far file ID */
    struct fardir dirent;	/* font directory */
    int		found;		/* flag */
    int		i;		/* loop counter */
    int		stat;		/* compare result */
    
    /* open far file */
    res = (int)(PRINTER_RES * (double)f->scale / (double)f->design
		* magnification * 5.0 + 0.51);
    sprintf( farname, "%d.far", res );
    farfp = tryopen( fontpath, farname );
    if( farfp == NULL ) {
	sprintf( farname, "%d.far", res - 1);
	farfp = tryopen( fontpath, farname );
	if( farfp == NULL ) {
	    sprintf( farname, "%d.far", res + 1 );
	    farfp = tryopen( fontpath, farname );
	    if( farfp == NULL )
		return NULL;
	}
    }

    /* check far ID */
    if( fseek( farfp, -8L, 2 ) < 0 )	goto Error;
    if( get_2bytes( farfp, &fontnum ) )	goto Error;
    if( get_4bytes( farfp, &dirpos ) )	goto Error;
    if( get_2bytes( farfp, &farid ) )	goto Error;
    if( farid != (uint2)FARID ) {
	fprintf( stderr, "%s: invalid FAR file %s\n", progname, farname );
	fclose( farfp );
	return NULL;
    }

    /* search font entry */
    if( fseek( farfp, dirpos, 0 ) )	goto Error;
    found = FALSE;
    for( i = 0; i < (int)fontnum; i++ ) {
	if( fread( &dirent, sizeof(struct fardir), 1, farfp ) <= 0 )
	    goto Error;
	if( (stat = strncmp( dirent.name, f->info->name, 8 )) == 0 ) {
	    found = TRUE;
	    break;
	}
	else if( stat > 0 )
	    break;
    }
    if( !found ) {
	fclose( farfp );
	return NULL;
    }

    /* found and read it */
    top = dirent.fontpos;
    last = dirent.fontpos + dirent.fontsize;
    fbuf = pxlload( f, farfp, farname, top, last );
    fclose( farfp );
    return fbuf;

  Error:
    fprintf( stderr, "%s: read or seek error on %s\n", progname, farname );
    fclose( farfp );
    return NULL;
}
#endif	/* MSDOS && FARFONT */
