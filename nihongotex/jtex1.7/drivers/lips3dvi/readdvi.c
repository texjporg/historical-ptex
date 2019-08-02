/*
 *	readdvi.c -- read dvi file and call command handling function
 *		     for every command.	
 *
 *	Written by H. Nagahara on 2/10/92 08:59:17.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: readdvi.c 10.1
 */
#include	<stdio.h>
#include	"config.h"
#define		DVIBODY
#include	"dvi.h"

/* dummy command handler */
int
NOPFUNC()
{
    return FALSE;
}

/* command handling functions */
int		(*set_char_func)()	= NOPFUNC,
		(*fnt_num_func)()	= NOPFUNC,
		(*pre_func)()		= NOPFUNC,
		(*post_func)()		= NOPFUNC,
		(*post_post_func)()	= NOPFUNC,
		(*fnt_def_func)()	= NOPFUNC,
		(*bop_func)()		= NOPFUNC,
		(*eop_func)()		= NOPFUNC,
		(*fnt_func)()		= NOPFUNC,
		(*set_func)()		= NOPFUNC,
		(*put_func)()		= NOPFUNC,
		(*set_rule_func)()	= NOPFUNC,
		(*put_rule_func)()	= NOPFUNC,
		(*right_func)()		= NOPFUNC,
		(*down_func)()		= NOPFUNC,
		(*w0_func)()		= NOPFUNC,
		(*w_func)()		= NOPFUNC,
		(*x0_func)()		= NOPFUNC,
		(*x_func)()		= NOPFUNC,
		(*y0_func)()		= NOPFUNC,
		(*y_func)()		= NOPFUNC,
		(*z0_func)()		= NOPFUNC,
		(*z_func)()		= NOPFUNC,
		(*push_func)()		= NOPFUNC,
		(*pop_func)()		= NOPFUNC,
		(*xxx1_func)()		= NOPFUNC,
		(*xxx2_func)()		= NOPFUNC,
		(*xxx3_func)()		= NOPFUNC,
		(*xxx4_func)()		= NOPFUNC,
		(*nop_func)()		= NOPFUNC,
		(*dir_func)()		= NOPFUNC,
		(*illegal_func)()	= NOPFUNC;
static	char	lbuf[512];

#if	SWAP
/* swap byte order for 2byte numeric */
void
swap2( p )
uint1	p[];
{
    register uint1	tmp;

    tmp = p[0];
    p[0] = p[1];
    p[1] = tmp;
}

/* swap byte order for 4byte numeric */
void
swap4( p )
uint1	p[];
{
    register uint1	tmp;

    tmp = p[0];
    p[0] = p[3];
    p[3] = tmp;
    tmp = p[1];
    p[1] = p[2];
    p[2] = tmp;
}
#endif	/* SWAP */

/* read argument as a byte */
int
get_1byte( f, p )
FILE	*f;	/* file descripter */
uint1	*p;	/* data buffer */
{
    if( fread( p, 1, 1, f ) <= 0 )
	return TRUE;
    else
	return FALSE;
}

/* read argument as 2 bytes */
int
get_2bytes( f, p )
FILE	*f;	/* file to read */
uint2	*p;	/* address of data holder */
{
    if( fread( p, 2, 1, f ) <= 0 )
	return TRUE;
#if	SWAP
    swap2( (uint1 *)p );
#endif
    return FALSE;
}

/* read argument as 3 bytes */
#define	signextension(a)	if((a)&0x800000)(a)=((a)|0xff000000)
int
get_3bytes( f, p )
FILE	*f;	/* file to read */
uint4	*p;	/* address of data holder */
{
    uint1	tmp[3];

    if( fread( tmp, 3, 1, f ) <= 0 )
	return TRUE;
    *p = ((((uint4)tmp[0] << 8) | (uint4)tmp[1] ) << 8) | (uint4)tmp[2];
    return FALSE;
}

/* read argumetn as 4 bytes */
int
get_4bytes( f, p )
FILE	*f;	/* file to read */
uint4	*p;	/* address of data holder */
{
    if( fread( p, 4, 1, f ) <= 0 )
	return TRUE;
#if	SWAP
    swap4( (uint1 *)p );
#endif
    return FALSE;
}

/* read extra argument as a string */
int
readstring( fp, len, buf )
FILE	*fp;
uint4	len;
char	buf[];
{
    if( len > 0 ) {
	if( fread( buf, (int)len, 1, fp ) <= 0 )
	    return TRUE;
    }
    buf[len] = '\0';
    return FALSE;
}

/* read dvi file and branch to command functions */
int
readdvi( fp )
FILE	*fp;
{
    uint1	cmd;	/* command */
    uint1	ui1_0, ui1_1, ui1_2;
    uint2	ui2_0, ui2_1;
    uint4	ui4_0, ui4_1, ui4_2, ui4_3, ui4_4, ui4_5;
    int1	i1_0;
    int2	i2_0;
    int4	i4_0, i4_1, i4_2, i4_3, i4_4,
		i4_5, i4_6, i4_7, i4_8, i4_9, i4_10;

    while( fread( (char *)&cmd, sizeof(char), 1, fp ) > 0 ) {
	if( cmd < 128 ) {
	    if( (*set_char_func)( (uint4)cmd ) )
		return FALSE;
	    continue;
	}
	if( (cmd > 170) && (cmd < 235) ) {
	    if( (*fnt_num_func)( (uint4)cmd - 171 ) )
		return FALSE;
	    continue;
	}
	switch( cmd ) {
	  case PRE:
	    if( get_1byte ( fp, &ui1_0 ) )	goto Error;	/* ver */
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* num */
	    if( get_4bytes( fp, &ui4_1 ) )	goto Error;	/* den */
	    if( get_4bytes( fp, &ui4_2 ) )	goto Error;	/* mag */
	    if( get_1byte ( fp, &ui1_1 ) )	goto Error;	/* len */
	    if( readstring( fp, (uint4)ui1_1, lbuf ) )
		goto Error;	/* comment */
	    if( (*pre_func)( ui1_0, ui4_0, ui4_1, ui4_2, ui1_1, lbuf ) )
		return FALSE;
	    break;

	  case POST:
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* lastp */
	    if( get_4bytes( fp, &ui4_1 ) )	goto Error;	/* num */
	    if( get_4bytes( fp, &ui4_2 ) )	goto Error;	/* den */
	    if( get_4bytes( fp, &ui4_3 ) )	goto Error;	/* mag */
	    if( get_4bytes( fp, &ui4_4 ) )	goto Error;	/* hight/depth */
	    if( get_4bytes( fp, &ui4_5 ) )	goto Error;	/* width */
	    if( get_2bytes( fp, &ui2_0 ) )	goto Error;	/* stack */
	    if( get_2bytes( fp, &ui2_1 ) )	goto Error;	/* pages */
	    if( (*post_func)( ui4_0, ui4_1, ui4_2, ui4_3, ui4_4, ui4_5, ui2_0, ui2_1 ) )
		return FALSE;
	    break;

	  case POST_POST:
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* postp */
	    if( get_1byte ( fp, &ui1_0 ) )	goto Error;	/* ver */
	    if( (*post_post_func)( ui4_0, ui1_0 ) )
		return FALSE;
	    break;
	    
	  case FNT_DEF1:
	    if( get_1byte ( fp, &ui1_0 ) )	goto Error;	/* font # */
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* sum */
	    if( get_4bytes( fp, &i4_0  ) )	goto Error;	/* scale */
	    if( get_4bytes( fp, &i4_1  ) )	goto Error;	/* design */
	    if( get_1byte ( fp, &ui1_1 ) )	goto Error;	/* dirlen */
	    if( get_1byte ( fp, &ui1_2 ) )	goto Error;	/* dirlen */
	    if( readstring( fp, (uint4)ui1_1 + (uint4)ui1_2, lbuf ) )
		goto Error;
	    if( (*fnt_def_func)( (uint4)ui1_0, ui4_0, i4_0, i4_1, ui1_1, ui1_2, lbuf ) )
		return FALSE;
	    break;
	    
	  case FNT_DEF2:
	    if( get_2bytes( fp, &ui2_0 ) )	goto Error;	/* font # */
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* sum */
	    if( get_4bytes( fp, &i4_0  ) )	goto Error;	/* scale */
	    if( get_4bytes( fp, &i4_1  ) )	goto Error;	/* design */
	    if( get_1byte ( fp, &ui1_1 ) )	goto Error;	/* dirlen */
	    if( get_1byte ( fp, &ui1_2 ) )	goto Error;	/* dirlen */
	    if( readstring( fp, (uint4)ui1_1 + (uint4)ui1_2, lbuf ) )
		goto Error;
	    if( (*fnt_def_func)( (uint4)ui2_0, ui4_0, i4_0, i4_1, ui1_1, ui1_2, lbuf ) )
		return FALSE;
	    break;
	    
	  case FNT_DEF3:
	    if( get_3bytes( fp, &ui4_1 ) )	goto Error;	/* font # */
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* sum */
	    if( get_4bytes( fp, &i4_0  ) )	goto Error;	/* scale */
	    if( get_4bytes( fp, &i4_1  ) )	goto Error;	/* design */
	    if( get_1byte ( fp, &ui1_1 ) )	goto Error;	/* dirlen */
	    if( get_1byte ( fp, &ui1_2 ) )	goto Error;	/* dirlen */
	    if( readstring( fp, (uint4)ui1_1 + (uint4)ui1_2, lbuf ) )
		goto Error;
	    if( (*fnt_def_func)( (uint4)ui4_1, ui4_0, i4_0, i4_1, ui1_1, ui1_2, lbuf ) )
		return FALSE;
	    break;
	    
	  case FNT_DEF4:
	    if( get_4bytes( fp, &ui4_1 ) )	goto Error;	/* font # */
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* sum */
	    if( get_4bytes( fp, &i4_0  ) )	goto Error;	/* scale */
	    if( get_4bytes( fp, &i4_1  ) )	goto Error;	/* design */
	    if( get_1byte ( fp, &ui1_1 ) )	goto Error;	/* dirlen */
	    if( get_1byte ( fp, &ui1_2 ) )	goto Error;	/* dirlen */
	    if( readstring( fp, (uint4)ui1_1 + (uint4)ui1_2, lbuf ) )
		goto Error;
	    if( (*fnt_def_func)( (uint4)ui4_1, ui4_0, i4_0, i4_1, ui1_1, ui1_2, lbuf ) )
		return FALSE;
	    break;
	    
	  case BOP:
	    if( get_4bytes( fp, &i4_0  ) )	goto Error;	/* counter0 */
	    if( get_4bytes( fp, &i4_1  ) )	goto Error;	/* counter1 */
	    if( get_4bytes( fp, &i4_2  ) )	goto Error;	/* counter2 */
	    if( get_4bytes( fp, &i4_3  ) )	goto Error;	/* counter3 */
	    if( get_4bytes( fp, &i4_4  ) )	goto Error;	/* counter4 */
	    if( get_4bytes( fp, &i4_5  ) )	goto Error;	/* counter5 */
	    if( get_4bytes( fp, &i4_6  ) )	goto Error;	/* counter6 */
	    if( get_4bytes( fp, &i4_7  ) )	goto Error;	/* counter7 */
	    if( get_4bytes( fp, &i4_8  ) )	goto Error;	/* counter8 */
	    if( get_4bytes( fp, &i4_9  ) )	goto Error;	/* counter9 */
	    if( get_4bytes( fp, &i4_10  ) )	goto Error;	/* previous page */
	    if((*bop_func)( i4_0, i4_1, i4_2, i4_3, i4_4, i4_5, i4_6, i4_7, i4_8, i4_9,
		     i4_10 ) )
		return FALSE;
	    break;

	  case EOP:
	    if( (*eop_func)() )
		return FALSE;
	    break;

	  case FNT1:
	    if( get_1byte ( fp, &ui1_0 ) )	goto Error;	/* font */
	    if( (*fnt_func)( (uint4)ui1_0 ) )
		return FALSE;
	    break;

	  case FNT2:
	    if( get_2bytes( fp, &ui2_0 ) )	goto Error;	/* font */
	    if( (*fnt_func)( (uint4)ui2_0 ) )
		return FALSE;
	    break;

	  case FNT3:
	    if( get_3bytes( fp, &ui4_0 ) )	goto Error;	/* font */
	    if( (*fnt_func)( (uint4)ui4_0 ) )
		return FALSE;
	    break;

	  case FNT4:
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* font */
	    if( (*fnt_func)( (uint4)ui4_0 ) )
		return FALSE;
	    break;

	  case SET1:
	    if( get_1byte ( fp, &ui1_0 ) )	goto Error;	/* char code */
	    if( (*set_func)( (uint4)ui1_0 ) )
		return FALSE;
	    break;
	    
	  case SET2:
	    if( get_2bytes( fp, &ui2_0 ) )	goto Error;	/* char code */
	    if( (*set_func)( (uint4)ui2_0 ) )
		return FALSE;
	    break;
	    
	  case SET3:
	    if( get_3bytes( fp, &ui4_0 ) )	goto Error;	/* char code */
	    if( (*set_func)( (uint4)ui4_0 ) )
		return FALSE;
	    break;
	    
	  case SET4:
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* char code */
	    if( (*set_func)( (uint4)ui4_0 ) )
		return FALSE;
	    break;
	    
	  case PUT1:
	    if( get_1byte ( fp, &ui1_0 ) )	goto Error;	/* char code */
	    if( (*put_func)( (uint4)ui1_0 ) )
		return FALSE;
	    break;

	  case PUT2:
	    if( get_2bytes( fp, &ui2_0 ) )	goto Error;	/* char code */
	    if( (*put_func)( (uint4)ui2_0 ) )
		return FALSE;
	    break;

	  case PUT3:
	    if( get_3bytes( fp, &ui4_0 ) )	goto Error;	/* char code */
	    if( (*put_func)( (uint4)ui4_0 ) )
		return FALSE;
	    break;

	  case PUT4:
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* char code */
	    if( (*put_func)( (uint4)ui4_0 ) )
		return FALSE;
	    break;

	  case SET_RULE:
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* hight */
	    if( get_4bytes( fp, &ui4_1 ) )	goto Error;	/* width */
	    if( (*set_rule_func)( ui4_0, ui4_1 ) )
		return FALSE;
	    break;

	  case PUT_RULE:
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* hight */
	    if( get_4bytes( fp, &ui4_1 ) )	goto Error;	/* width */
	    if( (*put_rule_func)( ui4_0, ui4_1 ) )
		return FALSE;
	    break;

	  case RIGHT1:
	    if( get_1byte ( fp, &i1_0 ) )	goto Error;	/* step */
	    if( (*right_func)( (int4)i1_0 ) )
		return FALSE;
	    break;

	  case RIGHT2:
	    if( get_2bytes( fp, &i2_0 ) )	goto Error;	/* step */
	    if( (*right_func)( (int4)i2_0 ) )
		return FALSE;
	    break;

	  case RIGHT3:
	    if( get_3bytes( fp, &i4_0 ) )	goto Error;	/* step */
	    signextension( i4_0 );
	    if( (*right_func)( (int4)i4_0 ) )
		return FALSE;
	    break;

	  case RIGHT4:
	    if( get_4bytes( fp, &i4_0 ) )	goto Error;	/* step */
	    if( (*right_func)( (int4)i4_0 ) )
		return FALSE;
	    break;

	  case DOWN1:
	    if( get_1byte ( fp, &i1_0 ) )	goto Error;	/* step */
	    if( (*down_func)( (int4)i1_0 ) )
		return FALSE;
	    break;

	  case DOWN2:
	    if( get_2bytes( fp, &i2_0 ) )	goto Error;	/* step */
	    if( (*down_func)( (int4)i2_0 ) )
		return FALSE;
	    break;

	  case DOWN3:
	    if( get_3bytes( fp, &i4_0 ) )	goto Error;	/* step */
	    signextension( i4_0 );
	    if( (*down_func)( (int4)i4_0 ) )
		return FALSE;
	    break;

	  case DOWN4:
	    if( get_4bytes( fp, &i4_0 ) )	goto Error;	/* step */
	    if( (*down_func)( (int4)i4_0 ) )
		return FALSE;
	    break;

	  case W0:
	    (*w0_func)();
	    break;
	    
	  case W1:
	    if( get_1byte ( fp, &i1_0 ) )	goto Error;	/* step */
	    if( (*w_func)( (int4)i1_0 ) )
		return FALSE;
	    break;

	  case W2:
	    if( get_2bytes( fp, &i2_0 ) )	goto Error;	/* step */
	    if( (*w_func)( (int4)i2_0 ) )
		return FALSE;
	    break;

	  case W3:
	    if( get_3bytes( fp, &i4_0 ) )	goto Error;	/* step */
	    signextension( i4_0 );
	    if( (*w_func)( (int4)i4_0 ) )
		return FALSE;
	    break;

	  case W4:
	    if( get_4bytes( fp, &i4_0 ) )	goto Error;	/* step */
	    if( (*w_func)( (int4)i4_0 ) )
		return FALSE;
	    break;

	  case X0:
	    if( (*x0_func)() )
		return FALSE;
	    break;
	    
	  case X1:
	    if( get_1byte ( fp, &i1_0 ) )	goto Error;	/* step */
	    if( (*x_func)( (int4)i1_0 ) )
		return FALSE;
	    break;

	  case X2:
	    if( get_2bytes( fp, &i2_0 ) )	goto Error;	/* step */
	    if( (*x_func)( (int4)i2_0 ) )
		return FALSE;
	    break;

	  case X3:
	    if( get_3bytes( fp, &i4_0 ) )	goto Error;	/* step */
	    signextension( i4_0 );
	    if( (*x_func)( (int4)i4_0 ) )
		return FALSE;
	    break;

	  case X4:
	    if( get_4bytes( fp, &i4_0 ) )	goto Error;	/* step */
	    if( (*x_func)( (int4)i4_0 ) )
		return FALSE;
	    break;

	  case Y0:
	    if( (*y0_func)() )
		return FALSE;
	    break;
	    
	  case Y1:
	    if( get_1byte ( fp, &i1_0 ) )	goto Error;	/* step */
	    if( (*y_func)( (int4)i1_0 ) )
		return FALSE;
	    break;

	  case Y2:
	    if( get_2bytes( fp, &i2_0 ) )	goto Error;	/* step */
	    if( (*y_func)( (int4)i2_0 ) )
		return FALSE;
	    break;

	  case Y3:
	    if( get_3bytes( fp, &i4_0 ) )	goto Error;	/* step */
	    signextension( i4_0 );
	    if( (*y_func)( (int4)i4_0 ) )
		return FALSE;
	    break;

	  case Y4:
	    if( get_4bytes( fp, &i4_0 ) )	goto Error;	/* step */
	    if( (*y_func)( (int4)i4_0 ) )
		return FALSE;
	    break;

	  case Z0:
	    if( (*z0_func)() )
		return FALSE;
	    break;
	    
	  case Z1:
	    if( get_1byte ( fp, &i1_0 ) )	goto Error;	/* step */
	    if( (*z_func)( (int4)i1_0 ) )
		return FALSE;
	    break;

	  case Z2:
	    if( get_2bytes( fp, &i2_0 ) )	goto Error;	/* step */
	    if( (*z_func)( (int4)i2_0 ) )
		return FALSE;
	    break;

	  case Z3:
	    if( get_3bytes( fp, &i4_0 ) )	goto Error;	/* step */
	    signextension( i4_0 );
	    if( (*z_func)( (int4)i4_0 ) )
		return FALSE;
	    break;

	  case Z4:
	    if( get_4bytes( fp, &i4_0 ) )	goto Error;	/* step */
	    if( (*z_func)( (int4)i4_0 ) )
		return FALSE;
	    break;

	  case PUSH:
	    if( (*push_func)() )
		return FALSE;
	    break;

	  case POP:
	    if( (*pop_func)() )
		return FALSE;
	    break;

	  case DIR:
	    if( get_1byte ( fp, &ui1_0 ) )	goto Error;
	    if( (*dir_func)( (uint4)ui1_0 ) )
		return FALSE;
	    break;

	  case XXX1:
	    if( get_1byte ( fp, &ui1_0 ) )	goto Error;	/* str len */
	    if( readstring( fp, (uint4)ui1_0, lbuf ))
		goto Error;
	    if( (*xxx1_func)( ui1_0, lbuf ) )
		return FALSE;
	    break;

	  case XXX2:
	    if( get_2bytes( fp, &ui2_0 ) )	goto Error;	/* str len */
	    if( readstring( fp, (uint4)ui2_0, lbuf ))
		goto Error;
	    if( (*xxx2_func)( ui2_0, lbuf ) )
		return FALSE;
	    break;

	  case XXX3:
	    if( get_3bytes( fp, &ui4_0 ) )	goto Error;	/* str len */
	    if( readstring( fp, ui4_0, lbuf ) )
		goto Error;
	    if( (*xxx3_func)( ui4_0, lbuf ) )
		return FALSE;
	    break;

	  case XXX4:
	    if( get_4bytes( fp, &ui4_0 ) )	goto Error;	/* str len */
	    if( readstring( fp, ui4_0, lbuf ))
		goto Error;
	    if( (*xxx4_func)( ui4_0, lbuf ) )
		return FALSE;
	    break;

	  case NOP:
	    if( (*nop_func)() )
		return FALSE;
	    break;
	    
	  default:
	    if( (*illegal_func)( cmd ) )
		return FALSE;
	}
    } /* reach to EOF */
    return FALSE;
Error:
    return TRUE;
}
