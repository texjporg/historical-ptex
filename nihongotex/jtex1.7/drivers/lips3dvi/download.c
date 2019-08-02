/*
 *	download.c -- download cms fonts to printer
 *
 *	Written by H. Nagahara on 2/10/92 08:58:02.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: download.c 10.1
 */
#include	<stdio.h>
#include	"config.h"
#include	<string.h>
#if	MSDOS
# include	<malloc.h>
# include	<stdlib.h>
#else
  extern char	*malloc(), *calloc();
#endif

#include	"font.h"
#include	"drive.h"
#include	"lbp.h"

#ifdef	toupeer
# undef	toupper
#endif
#define	toupper(c)	((((c) >= 'a') && ((c) <= 'z'))?((c) - 0x20):(c))
#define	getbyte(p)	(getnib(p) * 16 + getnib((p+1)))

extern struct grif *farload PP((struct font *));
extern FILE	*tryopen();

/* font information in the printer */
struct prfont {
    struct prfont	*next;
    char		*name;
    int2		graphset;
    int4		datacount;
    int1		incore;
    struct grif		*pattern20;	/* bitmap pattern for 0x20 */
};

static struct prfont *InPrinter;
static int	flag = FALSE;		/* flag for do download */

/* external function declaration */
#if	PKFONT
extern struct grif	*pkload();	/* PK file load routine in pkload.c */
#endif
extern void		jxlready();
#if	PXLFONT
extern struct grif	*pxlload();
#endif

/* Set downloaded info into fontlist */
int
setdowninfo( f )
struct font	*f;
{
    int		fontsize;
    char	fontname[128];
    struct prfont	*df;
    struct jfatr	*af;

    /* smaller font */
    if( f->prev != NULL )
	if( setdowninfo( f->prev ) )
	    return TRUE;

    /* set myself */
    f->prn = (struct prinfo *)malloc( sizeof(struct prinfo) );
    if( f->prn == NULL )
	return TRUE;
    f->prn->download = FALSE;
    f->prn->prn_no = -1;
    f->prn->graphset = -1;
    f->prn->datacount = 0;
    f->prn->pattern20 = NULL;
    f->prn->attrib = NULL;
    fontsize = (int)(PRINTER_RES * (double)f->scale / (double)(f->design)
		     * magnification + 0.5);
    sprintf( fontname, "%s-%d.%s", f->info->name, fontsize, "TeX" );
    f->prn->name = malloc( strlen(fontname) + 1 );
    if( f->prn->name == NULL ) {
	free( f->prn );
	f->prn = NULL;
	return TRUE;
    }
    strcpy( f->prn->name, fontname );

    if( f->type == TFM ) {	/* only do with TFM */
	/* search downloaded list */
	for( df = InPrinter; df != NULL; df = df->next ) {
	    if( strcmp( df->name, fontname ) == 0 ) {
		f->prn->download = TRUE;
		f->prn->graphset = df->graphset;
		f->prn->datacount = df->datacount;
		f->prn->pattern20 = df->pattern20;
		df->incore = TRUE;
		break;
	    }
	}
    }
    else {			/* only do with JFM or TFM */
	/* search font attribute */
	for( af = fattr; af != NULL; af = af->next ) {
	    if( strncmp( af->name, fontname, strlen(af->name) ) == 0 )
		f->prn->attrib = af;
	}
    }

    /* larger font */
    if( f->next != NULL )
	if( setdowninfo( f->next ) )
	    return TRUE;

    return FALSE;
}

/* get byte pattern data from string */
uint2
getnib( p )
char	*p;		/* hex data */
{
    if( (*p >= '0') && (*p <= '9') )
	return (*p - '0');
    else
	return (toupper(*p) - 'A' + 10);
}

#define		BUFLEN		2048
/* get downloaded infomation */
int
getdinfo( f )
struct font	*f;
{
    FILE	*dfontfp;	/* downloaded font infomation file */
    struct prfont *fp;		/* printer font info */
    char	*lbuf;
    char	*fontname;
    char	*pdata;		/* pattern data string for 0x20 char */
    int		i;		/* byte counter for 0x20 char's pattern */
    int		plen;		/* byte length for 0x20 char's pattern */
    char	*widthvalue;	/* value string for width of 0x20 */
	
    /* allocate work buffer */
    lbuf = malloc( BUFLEN );
    if( lbuf == NULL )
	return TRUE;

    /* setup InPrinter list */
    InPrinter = NULL;
    using_gset = MINUSERGSET - 1;
    if( ! forcedownload ) {
	/* load downloaded font infomation */
#if MSDOS
	dfontfp = fopen( downinfo, "rt" );
#else
	dfontfp = fopen( downinfo, "r" );
#endif
	if( dfontfp != NULL ) {
	    while( fgets( lbuf, BUFLEN, dfontfp ) != NULL ) {
		if( (lbuf[0] == '\0') || (lbuf[0] == '#') )
		    continue;

		/* font name */
		fontname = strtok( lbuf, " \t" );
		fp = (struct prfont *)malloc( sizeof(struct prfont) );
		if( fp == NULL )
		    goto MemError;
		fp->name = malloc( strlen( fontname ) + 1 );
		if( fp->name == NULL ) {
		    free( fp );
		    goto MemError;
		}
		strcpy( fp->name, lbuf );

#if	ESCP
		/* font number */
		set_fontname( atoi(strtok(NULL, " \t")) , fp->name );
#endif

		/* graphic set # */
		fp->graphset = atoi( strtok(NULL, " \t") );
		if( using_gset < fp->graphset )
		    using_gset = fp->graphset;

		/* data count */
		fp->datacount = (int4)atol( strtok(NULL, " \t") );
		downloadcount += fp->datacount;

		/* font info for 0x20 pattern */
		widthvalue = strtok( NULL, " \t\n" );
		if( widthvalue != NULL ) {
		    fp->pattern20 = (struct grif *)malloc( sizeof( struct grif ) );
		    if( fp->pattern20 == NULL ) {
			free( fp );
			goto MemError;
		    }
		    fp->pattern20->code = 0x20;
		    fp->pattern20->width = atoi( widthvalue );
		    fp->pattern20->hight = atoi( strtok(NULL, " \t") );
		    fp->pattern20->hrefp = atoi( strtok(NULL, " \t") );
		    fp->pattern20->vrefp = atoi( strtok(NULL, " \t") );
		    plen = (fp->pattern20->width + 7) / 8 * fp->pattern20->hight ;
#if	USEEMS
		    alloc_grif_bitmap(fp->pattern20, plen);
#else
		    fp->pattern20->bitmap = (uint1 *)malloc( plen );
#endif
		    if( fp->pattern20->bitmap == NULL ) {
			free( fp->pattern20 );
			free( fp );
			goto MemError;
		    }
		    pdata = strtok( NULL, " \t" );
		    for( i = 0; i < plen; i++ ) {
			*(fp->pattern20->bitmap + i) = (uint1)getbyte( pdata );
			pdata += 2;
		    }
		}
		else
		    fp->pattern20 = NULL;

		/* setup InPrinter font list */
		fp->next = InPrinter;
		fp->incore = FALSE;
		InPrinter = fp;
	    }
	    fclose( dfontfp );
	}
    }
    
    /* set downloaded info to fontlist */
    free( lbuf );
    return( setdowninfo( f ) );

  MemError:
    free( lbuf );	/* dispose line buffer */
    return TRUE;
}

/* put a font named downloaded to printer */
int
putfontd( fp, f )
FILE		*fp;
struct font	*f;
{
    unsigned int i;	/* byte counter */
    uint1	*p;	/* image data */

    if( f->prev != NULL )
	if( putfontd( fp, f->prev ) )
	    return TRUE;

    if( f->prn->download && (f->type == TFM) ) {
#if	ESCP
	if( fprintf( fp, "%s\t%d\t%d\t%ld\t", f->prn->name, get_fontname(f->prn->name),
		     (int)(f->prn->graphset), (int4)(f->prn->datacount) ) == EOF )
#else
	if( fprintf( fp, "%s\t%d\t%ld\t", f->prn->name,
		     (int)(f->prn->graphset), (int4)(f->prn->datacount) ) == EOF )
#endif
	    return TRUE;
	if( f->prn->pattern20 != NULL ) {
	    if( fprintf( fp, "%d\t%d\t%d\t%d\t", f->prn->pattern20->width,
			f->prn->pattern20->hight, f->prn->pattern20->hrefp,
			f->prn->pattern20->vrefp ) == EOF )
		return TRUE;
	    for( i = 0, p = f->prn->pattern20->bitmap;
		i < ((f->prn->pattern20->width + 7) / 8 * f->prn->pattern20->hight );
		i++, p++ ) {
		if( fprintf( fp, "%02.2X", *p ) == EOF )
		    return TRUE;
	    }
	}
	if( fprintf( fp, "\n" ) == EOF )
	    return TRUE;
    }

    if( f->next != NULL )
	if( putfontd( fp, f->next ) )
	    return TRUE;

    return FALSE;
}
/* write back downloaded font info file */
void
putdinfo( f )
struct font	*f;
{
    FILE	*dfontfp;	/* downloaded font infomation */
    struct prfont	*p, *next;
    unsigned int i;
    uint1	*ptn;

#if MSDOS
    dfontfp = fopen( downinfo, "wt" );
#else
    dfontfp = fopen( downinfo, "w" );
#endif
    if( dfontfp == NULL ) {
	fprintf( stderr, "%s: warning: cannot open %s\n", progname, downinfo );
 	return;
    }

#if	ESCP
    if( fprintf( dfontfp, "# font name\tnum\tgset\tbytes\twidth\theight\thref\tvref\timage\n" ) == EOF ) 
#else
    if( fprintf( dfontfp, "# font name\tgset\tbytes\twidth\theight\thref\tvref\timage\n" ) == EOF ) 
#endif
	goto Error;

    if( putfontd( dfontfp, f ) )
	goto Error;

    for( p = InPrinter; p != NULL; p = next ) {
	if( !p->incore ) {
#if	ESCP
	    if( fprintf( dfontfp, "%s\t%d\t%d\t%ld\t", p->name, get_fontname(p->name),
		     (int)(p->graphset), (int4)(p->datacount) ) < 0 )
#else
	    if( fprintf( dfontfp, "%s\t%d\t%ld\t", p->name,
		      (int)(p->graphset), (int4)(p->datacount) ) < 0 )
#endif
		goto Error;
	    if( p->pattern20 != NULL ) {
		if( fprintf( dfontfp, "%d\t%d\t%d\t%d\t", p->pattern20->width,
			    p->pattern20->hight, p->pattern20->hrefp,
			    p->pattern20->vrefp ) == EOF )
		    goto Error;
		for( i = 0, ptn = p->pattern20->bitmap;
		    i < ((p->pattern20->width + 7) / 8 * p->pattern20->hight );
		    i++, ptn++ ) {
		    if( fprintf( dfontfp, "%02.2X", *ptn ) == EOF )
			goto Error;
		}
	    }
	    if( fprintf( dfontfp, "\n" ) == EOF )
		goto Error;
	}
	next = p->next;
	free( p->name );
	free( p );
    }

    fclose( dfontfp );
    return;

  Error:
    fprintf( stderr, "%s: warning: cannot write %s\n", progname, downinfo );
    fclose( dfontfp );
    (void)unlink( downinfo );
}

/* open font grif data */
struct grif *
loadfont( f )
struct font	*f;
{
    int		res, res5;	/* font resolution */
    FILE	*grif;		/* grif file */
    char	grifname[128];	/* grif file name */
    struct grif	*fbuf;		/* font buffer */
#if	PXLFONT
    int4	last;		/* pxlfont size */
#endif

    res = (int)(PRINTER_RES * (double)f->scale / (double)f->design
		* magnification * (mag / 1000.0) + 0.51);
    res5 = (int)(PRINTER_RES * (double)f->scale / (double)f->design
		* magnification * (mag / 1000.0) * 5.0 + 0.51);
    if( verbose )
	fprintf( stderr, "(%s-%d)", f->info->name, res );

#if	MSDOS && FARFONT
    /* check & load far font */
    fbuf = farload( f );
    if( fbuf != NULL )
	return fbuf;
#endif	/* MSDOS */

#if	PXLFONT
    /* try to pxl file */
#if MSDOS
    sprintf( grifname, "%d/%.8s.pxl", res5, f->info->name );
    grif = tryopen( fontpath, grifname );
    if( grif == NULL ) {
	sprintf( grifname, "%dpxl/%.8s", res5, f->info->name );
	grif = tryopen( fontpath, grifname );
	if( grif == NULL ) {
	    sprintf( grifname, "%d/%.8s.pxl", res5 - 1, f->info->name );
	    grif = tryopen( fontpath, grifname );
	    if( grif == NULL ) {
		sprintf( grifname, "%dpxl/%.8s.pxl", res5 - 1, f->info->name );
		grif = tryopen( fontpath, grifname );
		if( grif == NULL ) {
		    sprintf( grifname, "%d/%.8s.pxl", res5 + 1, f->info->name );
		    grif = tryopen( fontpath, grifname );
		    if( grif == NULL ) {
			sprintf( grifname, "%dpxl/%.8s.pxl", res5 + 1, f->info->name );
			grif = tryopen( fontpath, grifname );
			if( grif == NULL ) {
#else	/* !DOS */
    sprintf( grifname, "%s.%dpxl", f->info->name, res * 5 );
    grif = tryopen( fontpath, grifname );
    if( grif == NULL ) {
	sprintf( grifname, "%s.%dpxl", f->info->name, res5 );
	grif = tryopen( fontpath, grifname );
	if( grif == NULL ) {
	    sprintf( grifname, "%s.%dpxl", f->info->name, res5 - 1 );
	    grif = tryopen( fontpath, grifname );
	    if( grif == NULL ) {
		sprintf( grifname, "%s.%dpxl", f->info->name, res5 + 1 );
		grif = tryopen( fontpath, grifname );
		if( grif == NULL ) {
#endif	/* !DOS */
#endif	/* PXLFONT */

#if	PKFONT
    /* try to PK file */
#if MSDOS
    sprintf( grifname, "%dpk/%.8s.pk", res, f->info->name );
    grif = tryopen( fontpath, grifname );
    if( grif == NULL ) {
	sprintf( grifname, "%d/%.8s.pk", res5, f->info->name );
	grif = tryopen( fontpath, grifname );
	if( grif == NULL ) {
	    sprintf( grifname, "%d/%.8s.pk", res5 + 1, f->info->name );
	    grif = tryopen( fontpath, grifname );
	    if( grif == NULL ) {
		sprintf( grifname, "%d/%.8s.pk", res5 - 1, f->info->name );
		grif = tryopen( fontpath, grifname );
		if( grif == NULL ) {
#else
    sprintf( grifname, "%s.%dpk", f->info->name, res );
    grif = tryopen( fontpath, grifname );
    if( grif == NULL ) {
#endif
	fprintf( stderr, "%s: No font file %s\n", progname, grifname );
	return NULL;
#if	MSDOS
		}
	    }
	}
#endif
    }
    fbuf = pkload( f, grif, grifname );
    goto LOAD_okey;
#else
	fprintf( stderr, "%s: No font file %s\n", progname, grifname );
	return NULL;
#endif	/* PKFONT */

#if	PXLFONT
#if	MSDOS
			}
                    }
#endif
	        }
	    }
	}
    }
    /* found PXL file */
    if( fseek( grif, 0L, 2 ) < 0 )	goto Error;
    last = ftell( grif );
    fbuf = pxlload( f, grif, grifname, (int4)0, last );
#endif	/* PXLFONT */

    /* finish of read grif file */
#if	PKFONT
  LOAD_okey:
    fclose( grif );
    return fbuf;
#endif

#if	PXLFONT
  Error:
    fprintf( stderr, "%s: cannot seek %s\n", progname, grifname );
    return NULL;
#endif
}
	
/* release font grif area */
void
freefont( f, p )
struct font	*f;
struct grif	*p;
{
    register int	n;

    for( n = f->info->end - f->info->begin; n >= 0; n-- ) {
#if	USEEMS
	free_grif_bitmap( (p + n) );
#else
	free( (p + n)->bitmap );
#endif
    }
    free( p );
}

/* download a font set */
int
downfont( f )
struct font	*f;
{
    struct grif		*fbuf;
    struct grif		*grif20;
    int			datalen;

    /* download smaller fonts */
    if( f->prev != NULL )
	if( downfont( f->prev ) )
	    return TRUE;

    /* download myself */
    if( !(f->prn->download) && (f->type == TFM) ) {
	fbuf = loadfont( f );	/* expand grif file and make bitmap */
	if( fbuf == NULL )
	    return TRUE;
	if( !flag ) {
	    if( forcedownload )
		LBP_hardreset();
	    LBP_sendSOJ( FALSE );
	    flag = TRUE;
	}
	if( LBP_aFont( f, fbuf ) ) {
	    if( (f->info->begin <= 0x20) && (f->info->end >= 0x20) ) {
		f->prn->pattern20 = (struct grif *)malloc( sizeof(struct grif) );
		if( f->prn->pattern20 == NULL )
		    return FALSE;
		grif20 = fbuf + 0x20 - f->info->begin;
		f->prn->pattern20->code  = 0x20;
		f->prn->pattern20->width = grif20->width;
		f->prn->pattern20->hight = grif20->hight;
		f->prn->pattern20->hrefp = grif20->hrefp;
		f->prn->pattern20->vrefp = grif20->vrefp;
		datalen = (grif20->width + 7) / 8 * grif20->hight;
#if	USEEMS
		alloc_grif_bitmap(f->prn->pattern20, datalen);
#else
		f->prn->pattern20->bitmap = (uint1 *)malloc( datalen );
#endif
		if( f->prn->pattern20->bitmap == NULL ) {
		    free( f->prn->pattern20 );
		    return FALSE;
		}
		(void)memcpy( f->prn->pattern20->bitmap, grif20->bitmap, datalen );
	    }
	    freefont( f, fbuf );
	}
	else
	    f->gdata.pattern = fbuf;
    }
    else if( (f->type == JFM) && usejxlfont )
	jxlready( f );

    /* download larger fonts */
    if( f->next != NULL )
	if( downfont( f->next ) )
	    return TRUE;
    
    return FALSE;
}

/* download fonts as a printer job */
int
download()
{
    flag = FALSE;
    /* load downinfo from status file */
    if( getdinfo( fontlist ) )
	return TRUE;

    /* download fonts */
    if( downfont( fontlist ) )
	return TRUE;

    /* finish download */
    if( flag ) {
	LBP_sendEOJ();		/* send EndOfJob seq. to printer */
	putdinfo( fontlist );	/* update downinfo file */
    }

    return FALSE;
}
