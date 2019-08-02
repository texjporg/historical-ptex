/*
 *	JIS code to NWF index
 *	$Header: c2i.c,v 3.1 89/02/25 22:58:56 atsuo Exp $
 *		written by Atsuo, K.
 *		on 87/12/26
 */

#include "atsuo-copyright.h"  

#include <stdio.h>

struct jis_to_index {
	unsigned short	start;		/* start of block */
	unsigned short	end;		/* end of block */
	unsigned short	index_base;
};

struct jis_to_index jis_code_table[] = {
	{ 0x2121, 0x217e, 0 },
	{ 0x2221, 0x222e, 94 },
	{ 0x223a, 0x2241, 108 },
	{ 0x224a, 0x2251, 116 },
	{ 0x225c, 0x226a, 123 },
	{ 0x2272, 0x2279, 138 },
	{ 0x227e, 0x227e, 146 },
	{ 0x2330, 0x2339, 147 },		/* 0..9 */
	{ 0x2341, 0x235a, 157 },		/* A..Z */
	{ 0x2361, 0x237a, 183 },		/* a..z */
	{ 0x2421, 0x2473, 209 },		/* 'a'..'nn' (hiragana) */
	{ 0x2521, 0x257e, 292 },		/* 'A'..'NN' (katakana) */
	{ 0x2621, 0x2638, 378 },
	{ 0x2641, 0x2658, 402 },
	{ 0x2721, 0x2742, 426 },
	{ 0x2751, 0x2771, 459 },
	{ 0x2821, 0x2840, 492 }, 
	{ 0, 0, 0 }
};

jis_to_index(jis)
register unsigned int jis;
{
	register int	i;
        register struct jis_to_index *j;
        

	if ( jis <= 0x2840 ) {       
	  j = jis_code_table;
	  for (i=0; ; i++) {				/* 1st block */
		if ((j+i)->start == 0) break;

		if (jis <= (j+i)->end) {
			return (jis - (j+i)->start + 
					(j+i)->index_base);
		}
	  }
	}

	if (jis <= 0x4f53) {			/* 2nd block */
	  return ( (((jis-0x3021)>>8)&0x7f)*94 + ((jis-0x3021)&0x7f) + 524 );
	}
	if (jis <= 0x7424) {			/* 3rd block */
	  return ( (((jis-0x5021)>>8)&0x7f)*94 + ((jis-0x5021)&0x7f) + 3489 );
	}

	return (-1);
}

jis_to_dnf_index(jis)
register unsigned int jis;
{
	register int	kanji;

	kanji = jis - 0x2121;
	return ( ((kanji >> 8) & 0x7f ) * 94 + ( kanji & 0x7f ) );
}

