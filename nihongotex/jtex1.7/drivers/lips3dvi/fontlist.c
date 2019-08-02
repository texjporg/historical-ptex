/*
 *	fontlist.c -- read postample and return font list structure.
 *
 *	Written by H. Nagahara on 2/10/92 08:58:29.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: fontlist.c 10.1
 */
#include	<stdio.h>
#include	"config.h"
#include	<string.h>
#include	"dvi.h"
#include	"font.h"
#include	"drive.h"

# define	MAXTAIL	16

#if	MSDOS	/* ANSI prototype declaration */
# include	<malloc.h>
void	swap2( uint1 * );
void	swap4( uint1 * );
int	get_1byte( FILE *, uint1 * );
int	get_2bytes( FILE *, uint2 * );
int	get_3bytes( FILE *, uint4 * );
int	get_4bytes( FILE *, uint4 * );
int	readstring( FILE *, uint4, char * );
#else
  extern char	*malloc(), *calloc();
#endif

extern FILE	*tryopen();

/* read tfm file and set variables in font-list element */
int
readtfm( f, m )
FILE	*f;		/* tfm file descripter */
struct metric	*m;	/* pointer to the metric info */
{
    uint2	fword,	/* words of the file */
		hword,	/* words of the header */
		wt_size,/* words of the width table */
		begin,	/* least char code in file */
		end;	/* beggest char code in file */
    uint4	dummy,	/* dummy data */
		csum,	/* checksum in TFM */
		dsize;	/* design size in TFM */
    uint4	*wt;	/* width table buffer */
    register unsigned int c;	/* loop index */
    uint4	idx;	/* index of width table */

    /* rewind tfm file */
    rewind( f );

    /* get parameters */
    if( get_2bytes( f, &fword ) )	goto Error;	/* words of the file */
    if( get_2bytes( f, &hword ) )	goto Error;	/* words of the header */
    if( get_2bytes( f, &begin ) )	goto Error;	/* begin of char code */
    if( get_2bytes( f, &end ) )		goto Error;	/* end of char code */
    if( get_2bytes( f, &wt_size ) )	goto Error;	/* size of witdh table */
    if( get_2bytes( f, (uint2 *)&dummy ) )	goto Error;	/* skip 14 bytes */
    if( get_4bytes( f, &dummy ) )	goto Error;
    if( get_4bytes( f, &dummy ) )	goto Error;
    if( get_4bytes( f, &dummy ) )	goto Error;

    /* get header info .... */
    if( get_4bytes( f, &csum ) )	goto Error;	/* check sum */
    if( get_4bytes( f, &dsize ) )	goto Error;	/* design size */

    /* allocate width table buffer */
    wt = (uint4 *)malloc( 4 * wt_size );
    if( wt == NULL ) {
	fprintf( stderr, "%s: cannot allocate TFM width work table\n", progname );
	return TRUE;
    }

    /* set metric data */
    m->begin = begin;
    m->end = end;
    m->sum = csum;
    m->design = dsize;
    m->cnum = end - begin + 1;
    m->type = TFM;
    m->cc.width = (uint4 *)malloc( sizeof(uint4) * (end - begin + 1) );
    if( m->cc.width == NULL ) {
	fprintf( stderr, "%s: cannot allocate TFM width table\n", progname );
	return TRUE;
    }

    /* readin width table */
    if( fseek( f, (long)( (hword + end - begin + 1) * 4 + 24 ), 0 ) != 0 )
	goto Error;
    if( fread( wt, sizeof(uint4), (int)wt_size, f ) <= 0 )
	goto Error;
#if	SWAP
    for( c = 0; c < wt_size; c++ )
	swap4( (uint1 *)(wt + c) );
#endif
    
    /* skip to char_info table and readin */
    if( fseek( f, (long)(hword * 4 + 24), 0 ) != 0 )
	goto Error;
    for( c = begin; c <= end; c++ ) {
	if( get_4bytes( f, &dummy ) )	goto Error;
	idx = dummy >> 24;
	*(m->cc.width + (c - begin)) = *(wt + idx);
    }

    free( (char *)wt );
    return FALSE;

Error:
    fprintf( stderr, "%s: read error on tfm file\n", progname );
    return TRUE;
}

/* read jfm file and set variables in font-list element */
int
readjfm( f, m )
FILE	*f;		/* tfm file descripter */
struct metric	*m;	/* pointer to the metric info */
{
    uint2	type,	/* JFM file type (JFM or JFMT) */
		cword,	/* words of the char_type table */
		hword,	/* words of the header */
		begin,	/* least char code in file */
		end,	/* beggest char code in file */
		wt_size,/* words of the width table */
		ht_size,/* words of the hight table */
		dt_size,/* words of the depth table */    
		it_size,/* words of the italic table */
		gk_size,/* words of the glue/kern table */
		kt_size,/* words of the kern table */
		gt_size;/* words of the glue table */    
    uint2	dummy2;	/* dummy data */
    uint4	dummy,	/* dummy data */
		csum,	/* checksum in TFM */
		dsize;	/* design size in TFM */
    uint4	*ci,	/* char_infor table buffer */
		*wt,	/* width table buffer */
		*ht,	/* hight table buffer */
		*dt,	/* depth table buffer */
		*gk,	/* glue/kern table buffer */
		*gt;	/* glue table buffer */
    register unsigned int c;	/* loop index */
    int		idx;	/* offset of charinfo */
    struct chartype	*cinfotop,	/* char info top */
			*cinfop;	/* to cinfo */
    uint4	char_type;	/* char_type content */
    uint4	char_info;	/* char_type content */
    long	gk_offset;	/* glue/kern table offset in file */
    uint4	cmd, *cmdp;	/* glue/kern table command */
    int		glueidx;	/* index of glue table */

    /* rewind tfm file */
    rewind( f );

    /* get parameters */
    if( get_2bytes( f, &type ) )	goto Error;	/* JFM ID */
    if( get_2bytes( f, &cword ) )	goto Error;	/* words of the char_type */
    if( get_2bytes( f, &dummy2 ) )	goto Error;	/* words of the file */
    if( get_2bytes( f, &hword ) )	goto Error;	/* words of the header */
    if( get_2bytes( f, &begin ) )	goto Error;	/* begin of char_info */
    if( get_2bytes( f, &end ) )		goto Error;	/* end of char_info */
    if( get_2bytes( f, &wt_size ) )	goto Error;	/* size of witdh table */
    if( get_2bytes( f, &ht_size ) )	goto Error;	/* size of hight table */
    if( get_2bytes( f, &dt_size ) )	goto Error;	/* size of depth table */
    if( get_2bytes( f, &it_size ) )	goto Error;	/* skip italic table size */
    if( get_2bytes( f, &gk_size ) )	goto Error;	/* size of glue/kern table */
    if( get_2bytes( f, &kt_size ) )	goto Error;	/* skip kern table size */
    if( get_2bytes( f, &gt_size ) )	goto Error;	/* size of glue table */
    if( get_2bytes( f, &dummy2 ) )	goto Error;	/* skip param table size */

    /* get header info .... */
    if( get_4bytes( f, &csum ) )	goto Error;	/* check sum */
    if( get_4bytes( f, &dsize ) )	goto Error;	/* design size */

    /* allocate working buffer */
    ci = (uint4 *)malloc( 4 * (end - begin + 1) );
    wt = (uint4 *)malloc( 4 * wt_size );	/* allocate width table buffer */
    ht = (uint4 *)malloc( 4 * ht_size );	/* allocate hight table buffer */
    dt = (uint4 *)malloc( 4 * dt_size );	/* allocate depth table buffer */
    gk = (uint4 *)malloc( 4 * gk_size );	/* allocate glue/kern table buf */
    gt = (uint4 *)malloc( 4 * gt_size );	/* allocate glue table buffer */
    if( (ci == NULL) || (wt == NULL) || (ht == NULL)
     || (dt == NULL) || (gk == NULL) || (gt == NULL) ) {
	fprintf( stderr, "%s: cannot allocate JFM work table\n", progname );
	return TRUE;
    }
    /* allocate char_info table */
    cinfotop = cinfop
	= (struct chartype *)malloc( sizeof(struct chartype) * (end - begin + 1) );
    if( cinfotop == NULL ) {
	fprintf( stderr, "%s: cannot allocate JFM char_info table\n", progname );
	return TRUE;
    }

    /* set metric data */
    m->begin = begin;
    m->end = end;
    m->sum = csum;
    m->design = dsize;
    m->cnum = cword;
    if( type == JFMTID )
	m->type = JFMT;
    else
	m->type = JFM;
    m->cc.klist = (struct  jfm *)malloc( sizeof(struct jfm) * cword );
    if( m->cc.width == NULL ) {
	fprintf( stderr, "%s: cannot allocate JFM width table\n", progname );
	return TRUE;
    }

    /* readin char_info table */
    if( fseek( f, (long)( 28 + (hword + cword) * sizeof(uint4) ), 0 ) != 0 )
	goto Error;
    if( fread( ci, sizeof(uint4), (int)(end - begin + 1), f ) <= 0 )
	goto Error;
#if	SWAP
    for( c = 0; c < (end - begin + 1); c++ )
	swap4( (uint1 *)(ci + c) );
#endif

    /* readin width table */
    if( fread( wt, sizeof(uint4), (int)wt_size, f ) <= 0 )
	goto Error;
#if	SWAP
    for( c = 0; c < wt_size; c++ )
	swap4( (uint1 *)(wt + c) );
#endif

    /* readin hight table */
    if( fread( ht, sizeof(uint4), (int)ht_size, f ) <= 0 )
	goto Error;
#if	SWAP
    for( c = 0; c < ht_size; c++ )
	swap4( (uint1 *)(ht + c) );
#endif

    /* readin depth table */
    if( fread( dt, sizeof(uint4), (int)dt_size, f ) <= 0 )
	goto Error;
#if	SWAP
    for( c = 0; c < dt_size; c++ )
	swap4( (uint1 *)(dt + c) );
#endif
    
    /* readin gkue/kern table */
    gk_offset = (long)((hword + cword + end - begin + 1 + wt_size + ht_size
			+ dt_size + it_size) * 4 + 28);
    if( fseek( f, gk_offset, 0 ) != 0 )
	goto Error;
    if( fread( gk, sizeof(uint4), (int)gk_size, f ) <= 0 )
	goto Error;
#if	SWAP
    for( c = 0; c < gk_size; c++ )
	swap4( (uint1 *)(gk + c) );
#endif

    /* readin glue table */
    if( fseek( f, gk_offset + (long)((gk_size + kt_size) * 4), 0 ) != 0 )
	goto Error;
    if( fread( gt, sizeof(uint4), (int)gt_size, f ) <= 0 )
	goto Error;
#if	SWAP
    for( c = 0; c < gt_size; c++ )
	swap4( (uint1 *)(gt + c) );
#endif
    
    /* set width/hight/depth into my table */
    for( c = 0, cinfop = cinfotop; c < (end - begin + 1); c++, cinfop++ ) {
	char_info = *(ci + c);
	cinfop->width = *(wt + (char_info >> 24));
	cinfop->hight = *(ht + ((char_info >> 20) & 0xf));
	cinfop->depth = *(dt + ((char_info >> 16) & 0xf));
	cinfop->nglue = 0;
#ifdef	DEBUG
	fprintf( stderr, "char_type:%d width->%d hight->%d depth->%d\n", c - begin,
		cinfop->width, cinfop->hight, cinfop->depth );
#endif	
    }
    
    /* set natural size of glue into my table */
    if( (*ci & 0x300) == 0x100 ) {	/* char_type->0 has glue/kern remainder */
	cmdp = gk + (*ci & 0xff);	/* ptr to glue/kern command */
	do {
	    cmd = *cmdp++;
	    if( ((cmd >> 8) & 0xff) < 128 ) {	/* glue step ?*/
		char_type = (cmd >> 16) & 0xff;
		glueidx = (int)(cmd & 0xff) * 3;
		(cinfotop + (char_type - begin))->nglue = *(gt + glueidx);
#ifdef	DEBUG
		fprintf( stderr, "char_type:%d nglue->%d\n", char_type, *(gt + glueidx) );
#endif
	    }
	} while( (cmd >> 24) < 128 );
    }
	
    /* return to char_type table and readin */
    if( fseek( f, (long)(hword * 4 + 28), 0 ) != 0 )
	goto Error;
    for( c = 0; c < cword; c++ ) {
	if( get_4bytes( f, &dummy ) )	goto Error;
	idx = (int)dummy & 0xffff;
	(m->cc.klist + c)->kcode = (uint2)(dummy >> 16);
	(m->cc.klist + c)->cinfo = cinfotop + (idx - begin);
#ifdef	DEBUG
	fprintf( stderr, "[%04.4X]@%x w:%x h:%x d:%x g:%x\n",
		(m->cc.klist + c)->kcode, (m->cc.klist + c)->cinfo,
		(m->cc.klist + c)->cinfo->width, (m->cc.klist + c)->cinfo->hight,
		(m->cc.klist + c)->cinfo->depth, (m->cc.klist + c)->cinfo->nglue );
#endif
    }

    free( (char *)ci );
    free( (char *)wt );
    free( (char *)ht );
    free( (char *)dt );
    free( (char *)gk );
    free( (char *)gt );
    return FALSE;

Error:
    fprintf( stderr, "%s: read error on jfm file\n", progname );
    return TRUE;
}

/* read font metric file */
int
read_metric( p, name )
struct font	*p;	/* font data buffer */
char		*name;	/* font file basename */
{
    char	fname[256];	/* current font path name */
    struct metric *m;		/* current metric info */
    FILE	*ff;		/* tfm/jfm file descripter */
    uint2	type;		/* JFM id */

    /* search font metric list */
    m = metriclist;
    while( m != NULL ) {
	if( strcmp( m->name, name ) == 0 ) {
	    /* found same one! -> connect it to the font-list */
	    p->info = m;
	    p->type = m->type;
	    return FALSE;
	}
	m = m->next;
    }

    /* initialize for new metric element */
    m = (struct metric *)malloc( sizeof(struct metric) );
    if( m == NULL ) {
	fprintf( stderr, "%s: cannot allocate memory for metric info\n", progname );
	return TRUE;
    }
    m->name = malloc( strlen(name) + 1 );
    if( m->name == NULL ) {
	free( m );
	fprintf( stderr, "%s: cannot allocate memory for metric info\n", progname );
	return TRUE;
    }
    strcpy( m->name, name );
    m->next = NULL;

    /* open tfm(jfm) file */
    sprintf( fname, "%s.tfm", name );
    ff = tryopen( tfmpath, fname );
    if( ff == NULL ) {
	fprintf( stderr, "%s: cannot open tfm file %s\n", progname, fname );
	free( m->name );
	free( m );
	return TRUE;
    }

    /* identify tfm type */
    if( fread( &type, sizeof(uint2), 1, ff ) == 0 )
	goto TFMError;
#if SWAP
    swap2( (uint1 *)&type );
#endif

    /* read tfm/jfm file */
    if( (type == JFMID) || (type == JFMTID) ) {
	p->type = (int1)((type == JFMID) ? JFM : JFMT);
	if( readjfm( ff, m ) )
	    goto Error;
    }
    else {
	p->type = TFM;
	if( readtfm( ff, m ) )
	    goto Error;
    }

    /* connect metric info to the list */
    m->next = metriclist;
    metriclist = m;
    p->info = m;

    fclose( ff );
    return FALSE;
    
TFMError:
    fprintf( stderr, "%s: read error on tfm(jfm) file [%s]\n", progname, fname );
Error:
    fclose( ff );
    free( m->name );
    free( m );
    return TRUE;
}

/* make element of font-list */
int
set_font_list( fp, font )
FILE	*fp;		/* dvi file descripter */
uint4	font;		/* font # */
{
    uint4	sum;		/* font checksum */
    uint4	scale;		/* scaled size */
    uint4	design;		/* designe size */
    uint1	dirlen, namelen;/* font file name length */
    char	fname[128];
    struct font	*fpool;		/* pointer to the current font data pool */
    struct font	*p, *pp;	/* work variable for search insertion point */

    /* readin arguments */
    if( get_4bytes( fp, &sum ) )	goto Error;	/* sum */
    if( get_4bytes( fp, &scale ) )	goto Error;	/* scale */
    if( get_4bytes( fp, &design) )	goto Error;	/* design */
    if( get_1byte ( fp, &dirlen) )	goto Error;	/* dirlen */
    if( get_1byte ( fp, &namelen) )	goto Error;	/* namelen */
    if( readstring( fp, (uint4)dirlen + (uint4)namelen, fname ) )
	goto Error;

    fpool = (struct font *)malloc( sizeof( struct font ) );
    if( fpool == NULL ) {
	goto MemError;
    }
    fpool->fno = (int)font;
    fpool->prev = fpool->next = NULL;
    fpool->info = NULL;
    fpool->prn = NULL;
    fpool->gdata.pattern = NULL;
    fpool->sum = sum;
    fpool->design = design;
    fpool->scale = scale;

    if( read_metric( fpool, fname ) ) {
	free( fpool );
	return TRUE;
    }

    /* add it to the font list */
    pp = fontlist;
    if( pp == NULL )
	p = NULL;
    else
	if( pp->fno > (int)font )
	    p = pp->prev;
	else
	    p = pp->next;
    while( p != NULL ) {
	if( p->fno == (int)font ) {
	    break;
	}
	else if( p->fno > (int)font ) {
	    pp = p;
	    p = pp->prev;
	}
	else {
	    pp = p;
	    p = pp->next;
	}
    }
    if( p != NULL )
	fprintf( stderr, "%s: warning! font #%d is defined twice\n", progname, font );
    else {
	if( pp == NULL )	/* top of font list */
	    fontlist = fpool;
	else {
	    if( pp->fno > (int)font )
		pp->prev = fpool;
	    else
		pp->next = fpool;
	}
    }

    return FALSE;

MemError:
    fprintf( stderr, "%s: cannot allocate memory for font-list\n", progname );
    return TRUE;

Error:
    fprintf( stderr, "%s: read error on reading %s\n", progname, infile );
    return TRUE;
}

/* read postamble and make font list in memory */
int
makefontlist( fp )
FILE	*fp;		/* dvi file descripter */
{
    static unsigned char lbuf[MAXTAIL];
    register int	i;
    long		fptr;
    uint1		cmd;
    uint1		ui1_0;
    uint2		ui2_0;
    uint4		ui4_0;

    /* seek to the POST command */
    if( fseek( fp, -16L, 2 ) != 0 ) {
	fprintf( stderr, "%s: can't seek DVI %s\n", progname, infile );
	return TRUE;
    }
  ReadAgain:
    if( fread( lbuf, 16, 1, fp ) == 0 )
	goto Error;

    /* search POST_POST command */
    for( i = 15; i >= 8; i-- ) {
	if( lbuf[i] == 223 )
	    continue;
	if( (lbuf[i] != 2) && (lbuf[i] != 3) ) {	/* DVI version # */
	    fprintf( stderr, "%s: DVI's version is not 2 but %d on %s\n",
		     progname, (int)lbuf[i], infile );
	    return TRUE;
	}
	if( lbuf[i] == 3 ) {
	    if( verbose )
		fprintf( stderr, "pTeX extension.\n" );
	    if( usejxlfont ) {
		fprintf( stderr,
			 "%s: warning cannot use JXL4 font on pTeX extension\n",
			 progname );
		usejxlfont = FALSE;
	    }
	}
	fptr = (((int4)lbuf[i-4] * (int4)256 + (int4)lbuf[i-3]) * 256
		 + (int4)lbuf[i-2]) * (int4)256 + (int4)lbuf[i-1];
	break;
    }
#if	MICROTEX	/* microTeX has too many 0xDFs in postamble */
    if( i < 8 ) {
	if( fseek( fp, -24L, 1 ) != 0 ) {
	    fprintf( stderr, "%s: can't seek DVI %s\n", progname, infile );
	    return TRUE;
	}
	goto ReadAgain;
    }
#endif
    if( (i < 8) || (lbuf[i-5] != POST_POST) ) {
	fprintf( stderr, "%s: %s not seemed as a DVI file\n", progname, infile );
	return TRUE;
    }

    /* read POST command */
    if( fseek( fp, fptr, 0 ) != 0 ) {
	fprintf( stderr, "%s: can't seek DVI %s\n", progname, infile );
	return TRUE;
    }
    if( get_1byte( fp, &cmd   ) )	goto Error;	/* POST */
    if( cmd != POST ) {
	fprintf( stderr, "%s: %s is not seemed as a DVI file\n",
		 progname, infile );
	return TRUE;
    }
    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* lastp */
    if( get_4bytes( fp, &num   ) )	goto Error;	/* num */
    if( get_4bytes( fp, &den   ) )	goto Error;	/* den */
    if( get_4bytes( fp, &mag   ) )	goto Error;	/* mag */
    if( get_4bytes( fp, &max_h ) )	goto Error;	/* hight/depth */
    if( get_4bytes( fp, &max_w ) )	goto Error;	/* width */
    if( get_2bytes( fp, &stack_size ) )	goto Error;	/* stack */
    if( get_2bytes( fp, &pages ) )	goto Error;	/* pages */
    scalepoint = (double)num * (double)mag * 1.0e-10 / (double)den * magnification;

    /* create font list */
    do {
	if( get_1byte( fp, &cmd ) )
	    goto Error;
	switch( cmd ) {
	  case FNT_DEF1:
	    if( get_1byte ( fp, &ui1_0 ) )	goto Error;	/* font # */
	    if( set_font_list( fp, (uint4)ui1_0 ) )
		return TRUE;
	    break;
	    
	  case FNT_DEF2:
	    if( get_2bytes( fp, &ui2_0 ) )	goto Error;	/* font # */
	    if( set_font_list( fp, (uint4)ui2_0 ) )
		return TRUE;
	    break;
	    
	  case FNT_DEF3:
	    if( get_3bytes( fp, &ui4_0 ) )	goto Error;	/* font # */
	    if( set_font_list( fp, (uint4)ui4_0 ) )
		return TRUE;
	    break;
	    
	  case FNT_DEF4:
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* font # */
	    if( set_font_list( fp, (uint4)ui4_0 ) )
		return TRUE;
	    break;

	  case POST_POST:
	    break;

	  default:
	    fprintf( stderr, "%s: %s has invalid DVI format\n", progname, infile );
	    return TRUE;
	}
    } while( cmd != POST_POST );

    return FALSE;

Error:
    fprintf( stderr, "%s: read error on reading DVI %s\n", progname, infile );
    return TRUE;
}
