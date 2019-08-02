/*
 *	doprint.c -- dvi interpriter routine
 *
 *	Written by H. Nagahara on 2/10/92 08:57:54.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: doprint.c 10.1
 */
#include	<stdio.h>
#if    MSDOS
# include	<malloc.h>
#else
  extern char	*malloc(), *calloc();
#endif
#include	"config.h"
#include	"font.h"
#include	"drive.h"
#include	"lbp.h"
#include	"dvi.h"

extern int		NOPFUNC();
extern struct fdir	*getfont();	/* expand a font image */
extern FILE		*tryopen();
void		set_print_func();

/* status report to user */
void
showregister( c0, c1, c2, c3, c4, c5, c6, c7, c8, c9 )
int4	c0, c1, c2, c3, c4, c5, c6, c7, c8, c9;
{
    register int	n;

    if( verbose ) {
	n = 0;
	if( c9 > 0 ) 
	    n = 9;
	else if( c8 > 0 )
	    n = 8;
	else if( c7 > 0 )
	    n = 7;
	else if( c6 > 0 )
	    n = 6;
	else if( c5 > 0 )
	    n = 5;
	else if( c4 > 0 )
	    n = 4;
	else if( c3 > 0 )
	    n = 3;
	else if( c2 > 0 )
	    n = 2;
	else if( c1 > 0 )
	    n = 1;

	fprintf( stderr, "[%d", c0 );
	if( n >= 1 )
	    fprintf( stderr, ",%d", c1 );
	if( n >= 2 )
	    fprintf( stderr, ",%d", c2 );
	if( n >= 3 )
	    fprintf( stderr, ",%d", c3 );
	if( n >= 4 )
	    fprintf( stderr, ",%d", c4 );
	if( n >= 5 )
	    fprintf( stderr, ",%d", c5 );
	if( n >= 6 )
	    fprintf( stderr, ",%d", c6 );
	if( n >= 7 )
	    fprintf( stderr, ",%d", c7 );
	if( n >= 8 )
	    fprintf( stderr, ",%d", c8 );
	if( n >= 9 )
	    fprintf( stderr, ",%d", c9 );
	fprintf( stderr, "]" );
    }
}

/* illegal instrcution found */
int
illegal_abort( cmd )
int	cmd;
{
    fprintf( stderr, "illegal instruction %d is found in dvi. abort.\n", cmd );
    return TRUE;
}

/* FNT_DEF command check routine */
int
fnt_def_check( font, sum, scale, size, dirlen, fnamelen, s )
uint4	font;		/* font # */
uint4	sum;		/* font file checksum */
int4	scale;		/* font design size scale */
int4	size;		/* font disign size */
uint1	dirlen;		/* length of directory name */
uint1	fnamelen;	/* length of font file name */
char	*s;		/* font name */	
{
    register struct font	*f;

    /* search font table */
    f = fontlist;
    while( f != NULL ) {
	if( font == (uint4)f->fno )
	    break;
	else if( font < (uint4)f->fno )
	    f = f->prev;
	else
	    f = f->next;
    }
    if( f == NULL ) {
	fprintf( stderr, "%s: font %s is not defined in postamble. abort\n",
		 progname, s );
	return TRUE;
    }

    /* check font info */
    if( sum != f->sum ) {
	fprintf( stderr, "%s: font %s is different from postamble. abort\n",
		 progname, s );
	return TRUE;
    }
    return FALSE;
}

/* PRE command check routine */
int
pre_check( ver, lnum, lden, lmag, len, s )
uint1	ver;		/* version # of dvi */
uint4	lnum, lden;	/* dimension unit */
uint4	lmag;		/* magnification */
uint1	len;		/* comment length */
char	*s;		/* comment string */
{
    if( ver != 2 ) {
	fprintf( stderr, "%s: cannot print version %d DVI\n", progname, ver );
	return TRUE;
    }
    if( (num != lnum) || (den != lden) || (mag != lmag) ) {
	fprintf( stderr, "%s: not match PRE & POST parameters\n", progname );
	return TRUE;
    }
    return FALSE;
}

/* POST command check routine for skipping */
int
post_check( lastp, num, den, mag, l, u, stack, page )
int4	lastp;		/* pointer to the last page */
uint4	num, den;	/* dimension unit */
uint4	mag;		/* magnification */
int4	l;		/* max of hight + depth */
int4	u;		/* max of width */
uint2	stack;		/* stack size */
uint2	page;		/* # of pages */
{
    return TRUE;
}

/* BOP command for skipping */
int
bop_skip( c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, lastp )
int4	c0, c1, c2, c3, c4, c5, c6, c7, c8, c9;	/* counter value */
int4	lastp;		/* pointer of last page */
{
    /* ignore page ? */
    if( (c0 < startpage) || (c0 > endpage) ) {
	showregister( c0, c1, c2, c3, c4, c5, c6, c7, c8, c9 );
	return FALSE;
    }
    if( ((prpage == EVENPAGE) && ((c0 % 2) != 0))
      ||((prpage == ODDPAGE)  && ((c0 % 2) != 1)) ) {
	showregister( c0, c1, c2, c3, c4, c5, c6, c7, c8, c9 );
	return FALSE;
    }

    /* start printing */
    set_print_func();
    return bop_print( c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, lastp );
}

/* set readdvi() procedures to skip pages */
void
set_skip_func()
{
    set_char_func	= NOPFUNC;
    fnt_num_func	= NOPFUNC;
    pre_func		= pre_check;
    post_func		= post_check;
    post_post_func	= NOPFUNC;
    fnt_def_func	= fnt_def_check;
    bop_func		= bop_skip;
    eop_func		= NOPFUNC;
    fnt_func		= NOPFUNC;
    set_func		= NOPFUNC;
    put_func		= NOPFUNC;
    set_rule_func	= NOPFUNC;
    put_rule_func	= NOPFUNC;
    right_func		= NOPFUNC;
    down_func		= NOPFUNC;
    w0_func		= NOPFUNC;
    w_func		= NOPFUNC;
    x0_func		= NOPFUNC;
    x_func		= NOPFUNC;
    y0_func		= NOPFUNC;
    y_func		= NOPFUNC;
    z0_func		= NOPFUNC;
    z_func		= NOPFUNC;
    push_func		= NOPFUNC;
    pop_func		= NOPFUNC;
    xxx1_func		= NOPFUNC;
    xxx2_func		= NOPFUNC;
    xxx3_func		= NOPFUNC;
    xxx4_func		= NOPFUNC;
    nop_func		= NOPFUNC;
    dir_func		= NOPFUNC;
    illegal_func	= illegal_abort;
}

/* SET & SET_CHAR command: print a character */
int
set_print( c )
uint4	c;	/* character code: 0-127 */
{
    register struct jfm	*base;
    register int	idx;
    register int	top, bottom;
    int			found;
    uint4		charwidth;
    struct fdir		*jimage;

    /* print character */
    if( (fontp->type == JFM) && (fontp->prn->download) ) {
	jimage = getfont( fontp, c );
	LBP_raster( h, v, jimage );
    }
    else if( (fontp->type == TFM) && !(fontp->prn->download) )
	LBP_rasterAchar( h, v, fontp, c );
    else {
	if( c != 0x20 )
	    LBP_putchar( h, v, fontp, c );
	else		/* send raster image <- LIPS3 bug(?) */
	    LBP_raster20( h, v, fontp->prn->pattern20 );
    }

    /* update point */
    if( fontp->type == TFM ) {
	charwidth = fontp->info->cc.width[c - fontp->info->begin];
    }
    else {
	base = fontp->info->cc.klist;
	charwidth = base->cinfo->width;	/* default width */
	/* search width table */
	top = 0;
	bottom = fontp->info->cnum - 1;
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
	    if( found )
		charwidth = (base + idx)->cinfo->width;
	}
    }
    if( dir == YOKO )
	h += (int4)( ((double)charwidth / 1048576.0) * (double)fontp->scale + 0.5);
    else	/* tategaki */
	v += (int4)( ((double)charwidth / 1048576.0) * (double)fontp->scale + 0.5);

    return FALSE;
}

/* FNT & FNT_NUM command: select font */
int
fnt_print( c )
int	c;	/* font */
{
    register struct font	*f;
	
    /* search font from fontlist */
    f = fontlist;
    while( f != NULL ) {
	if( f->fno == c )
	    break;
	else if( f->fno > c )
	    f = f->prev;
	else
	    f = f->next;
    }
    if( f == NULL ) {
	fprintf( stderr, "%s: undefined font %d. abort.\n", progname, c );
	return TRUE;
    }

    /* delete font attribute */
    if( (fontp != NULL) && (fontp->type != TFM) && (fontp->prn->attrib != NULL) )
	LBP_clearattr( fontp );

    /* set font pointers */
    fontp = f;
    if( f->type == TFM ) {
	if( f != g0font ) {
	    if( f->prn->prn_no > 0 )
		LBP_numAfont( f->prn->prn_no );
	    else if( f->prn->download )
		LBP_selectAfont( f );
	    g0font = f;
	}
    }
    else {
	if( f != g1font ) {
	    if( (g1font != NULL) && (g1font->prn->download) )
		fclose( g1font->gdata.finfo->jxlfp );
	    if( /* usejxlfont && */ f->prn->download ) {
		/* open JXL font file for later use */
		f->gdata.finfo->jxlfp = tryopen( fontpath, f->gdata.finfo->jxlname );
		if( f->gdata.finfo->jxlfp == NULL ) {
		    fprintf( stderr, "%s: waooo! %s disappear!\n",
			    progname, f->gdata.finfo->jxlname );
		    return TRUE;
		}
	    }
	    else {
		if( f->prn->prn_no > 0 )
		    LBP_numJfont( f->prn->prn_no );
		else
		    LBP_selectJfont( f );
	    }
	    g1font = f;
	}
	/* set font attribute */
	if( f->prn->attrib != NULL )
	    LBP_attrib( (int)f->prn->attrib->fillpattern,
		        (int)f->prn->attrib->shadowpattern,
		        (int)f->prn->attrib->outline );
    }

    return FALSE;
}

/* BOP command: printing control & initialize variables */
int
bop_print( c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, lastp )
int4	c0, c1, c2, c3, c4, c5, c6, c7, c8, c9;	/* counter value */
int4	lastp;		/* pointer of last page */
{
#ifdef	PAGEMSG
    char	msgbuf[32];
#endif

    /* print this page ? */
    if( c0 > endpage ) {
	set_skip_func();
	return bop_skip( c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, lastp );
    }
    if( ((prpage == EVENPAGE) && ((c0 % 2) != 0))
      ||((prpage == ODDPAGE)  && ((c0 % 2) != 1)) ) {
	set_skip_func();
	return bop_skip( c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, lastp );
    }

    /* status report to user */
    showregister( c0, c1, c2, c3, c4, c5, c6, c7, c8, c9 );
#ifdef	PAGEMSG
    sprintf( msgbuf, "Page %ld", c0 );
    LBP_sendMSG( msgbuf );
#endif

    /* clear registers */
    h = v = w = x = y = z = 0;
    fno = -1;

    /* clear stack */
    stackp = stack_top;
    return FALSE;
}

/* EOP command: flush page */
int
eop_print()
{
    LBP_flushpage();
    return FALSE;
}

/* PUT command: print a character without moving position */
int
put_print( c )
uint4	c;	/* character code */
{
    struct fdir		*jimage;

    if( (fontp->type == JFM) && (fontp->prn->download) ) {
	jimage = getfont( fontp, c );
	LBP_raster( h, v, jimage );
    }
    else if( (fontp->type == TFM) && !(fontp->prn->download) )
	LBP_rasterAchar( h, v, fontp, c );
    else {
	if( c != 0x20 )
	    LBP_putchar( h, v, fontp, c );
	else		/* send raster image <- LIPS3 bug(?) */
	    LBP_raster20( h, v, fontp->prn->pattern20 );
    }

    return FALSE;
}

/* SET_RULE command: draw rectangle */
int
set_rule_print( hight, width )
int4	hight;		/* ruler hight */
int4	width;		/* ruler width */
{
    if( dir == YOKO ) {
	LBP_rectangle( h, v, width, hight );    /* write rectangle */
	h += width;    /* update point */
    }
    else {
	LBP_rectangle( h, v, hight, -width );
	v += width;
    }
    return FALSE;
}

/* PUT_RULE command: draw rectangle without moving point */
int
put_rule_print( hight, width )
int4	hight;		/* ruler hight */
int4	width;		/* ruler width */
{
    /* write rectangle */
    if( dir == YOKO )
	LBP_rectangle( h, v, width, hight );
    else
	LBP_rectangle( h, v, hight, -width );
    return FALSE;
}

/* RIGHT command: move point */
int
right_print( value )
int4	value;	/* right value */
{
    if( dir == YOKO )
	h += value;
    else
	v += value;
    return FALSE;
}

/* DOWN command: move point */
int
down_print( value )
int4	value;	/* down value */
{
    if( dir == YOKO )
	v += value;
    else
	h -= value;
    return FALSE;
}

/* W0 command: move point */
int
w0_print()
{
    if( dir == YOKO )
	h += w;
    else
	v += w;
    return FALSE;
}

/* W command: move point */
int
w_print( value )
int4	value;
{
    w = value;
    if( dir == YOKO )
	h += w;
    else
	v += w;
    return FALSE;
}

/* X0 command: move point */
int
x0_print()
{
    if( dir == YOKO )
	h += x;
    else
	v += x;
    return FALSE;
}

/* X command: move point */
int
x_print( value )
int4	value;
{
    x = value;
    if( dir == YOKO )
	h += x;
    else
	v += x;
    return FALSE;
}

/* Y0 command: move point */
int
y0_print()
{
    if( dir == YOKO )
	v += y;
    else
	h -= y;
    return FALSE;
}

/* Y command: move point */
int
y_print( value )
int4	value;
{
    y = value;
    if( dir == YOKO )
	v += y;
    else
	h -= y;
    return FALSE;
}

/* Z0 command: move point */
int
z0_print()
{
    if( dir == YOKO )
	v += z;
    else
	h -= z;
    return FALSE;
}

/* Z commadn: move point */
int
z_print( value )
int4	value;
{
    z = value;
    if( dir == YOKO )
	v += z;
    else
	h -= z;
    return FALSE;
}

/* PUSH command: push registers to stack */
int
push_print()
{
    /* save current value */
    stackp->h = h;
    stackp->v = v;
    stackp->w = w;
    stackp->x = x;
    stackp->y = y;
    stackp->z = z;
    stackp->d = dir;
    stackp++;
    return FALSE;
}

/* POP command: pop registers from stack */
int
pop_print()
{
    --stackp;
    h = stackp->h;
    v = stackp->v;
    w = stackp->w;
    x = stackp->x;
    y = stackp->y;
    z = stackp->z;
    dir = stackp->d;
    return FALSE;
}

/* DIR command: change direction */
int
dir_print( d )
int	d;	/* direction */
{
    dir = d;
    return FALSE;
}

/* set readdvi() procedures to print pages */
void
set_print_func()
{
    set_char_func	= set_print;
    fnt_num_func	= fnt_print;
    pre_func		= pre_check;
    post_func		= post_check;
    post_post_func	= NOPFUNC;
    fnt_def_func	= fnt_def_check;
    bop_func		= bop_print;
    eop_func		= eop_print;
    fnt_func		= fnt_print;
    set_func		= set_print;
    put_func		= put_print;
    set_rule_func	= set_rule_print;
    put_rule_func	= put_rule_print;
    right_func		= right_print;
    down_func		= down_print;
    w0_func		= w0_print;
    w_func		= w_print;
    x0_func		= x0_print;
    x_func		= x_print;
    y0_func		= y0_print;
    y_func		= y_print;
    z0_func		= z0_print;
    z_func		= z_print;
    push_func		= push_print;
    pop_func		= pop_print;
    xxx1_func		= NOPFUNC;
    xxx2_func		= NOPFUNC;
    xxx3_func		= NOPFUNC;
    xxx4_func		= NOPFUNC;
    nop_func		= NOPFUNC;
    dir_func		= dir_print;
    illegal_func	= illegal_abort;
}

/* assign font # for Japanese font */
void
jfontassign( f )
struct font	*f;
{
    /* smaller font */
    if( f->prev != NULL )
	jfontassign( f->prev );

    /* assign myself */
    if( f->type == JFM ) {
	if( !f->prn->download ) {
	    if( prn_max_font <= MAXASSIGN ) {
#if	ESCP
		f->prn->prn_no = LBP_assignJfont( f );
#else
		LBP_assignJfont( prn_max_font, f );
		f->prn->prn_no = prn_max_font++;
#endif
	    }
	}
    }

    /* larger font */
    if( f->next != NULL )
	jfontassign( f->next );
}

/* assign font # for alphabet font */
void
afontassign( f )
struct font	*f;
{
    /* smaller font */
    if( f->prev != NULL )
	afontassign( f->prev );

    /* assign myself */
    if( (f->type == TFM) && f->prn->download ) {
	if( prn_max_font <= MAXASSIGN ) {
#if	ESCP
	    f->prn->prn_no = LBP_assignAfont( f );
#else
	    LBP_assignAfont( prn_max_font, f );
	    f->prn->prn_no = prn_max_font++;
#endif
	}
    }

    /* larger font */
    if( f->next != NULL )
	afontassign( f->next );
}

/* doprinting */
int
doprint( dvi )
FILE	*dvi;		/* dvi file descripter */
{
    int		status;

    /* select font # and so on */
    rewind( dvi );
    LBP_sendSOJ( TRUE );
    prn_max_font = 1;
    if( fontlist != NULL ) {	/* skip assign when no font to print */
	jfontassign( fontlist );
	afontassign( fontlist );
    }
    stack_top = (STACK *)malloc( sizeof(STACK) * stack_size );
    if( stack_top == NULL ) {
	fprintf( stderr, "%s: cannot allocate stack space\n", progname );
	return TRUE;
    }
    stackp = stack_top;
    g0font = g1font = NULL;
    current_page = 0;
    if( current_page < startpage )
	set_skip_func();
    else
	set_print_func();

    /* do skipping or printing */
    status = readdvi( dvi );

    /* end of priting */
    LBP_sendEOJ();

    return status;
}
    
