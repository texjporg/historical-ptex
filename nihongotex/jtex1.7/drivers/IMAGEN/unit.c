/*
 * Copyright 1988 dit Co., Ltd.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of dit Co., Ltd. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * Dit Co., Ltd. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * DIT CO., LTD. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL DIT CO., LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/*
 * unit.c - calculate routines for device units.
 */

/*
 * $Id: unit.c,v 1.7 90/03/27 11:03:45 void Exp $
 */

#include <stdio.h>
#include "config.h"
#include "fio.h"

extern double conv;

visible Sig4Byte
SPtoDEV(sp)
Sig4Byte	sp;
{
    double temp;

    temp = (double) sp * conv;

    if (temp < (double) 0.0)
	temp -= (double) 0.5; 
    else
	temp += (double) 0.5;

    return ((Sig4Byte) temp);
}

/*
 * Convert a magnification factor to floating point.  This is used in
 * conjunction with the FONT_SLOP stuff to try to get the file names
 * right, and may also be used by DVI reading programs to get slightly
 * more accurate values for (mag/1000.0).
 */

visible double
DMagFactor(mag)
int mag;
{
    switch (mag) {
    case 1095:
	return (1.095445);	/* stephalf */
    case 1315:
	return (1.314534);	/* stepihalf */
    case 2074:
	return (2.0736);		/* stepiv */
    case 2488:
	return (2.48832);	/* stepv */
    case 2986:
	return (2.985984);	/* stepiv */
    default:			/* remaining mags have been ok */
	return ((double) mag / 1000.0);
    }
}

visible Sig4Byte
AdvanceWidth (pxlw, S)
Sig4Byte	pxlw, S;
{
    Sig4Byte    alpha = 16 * S, 
		log2beta = 4,
		t;

#define a (CharMask (t >> 24))
#define b (CharMask (t >> 16))
#define c (CharMask (t >> 8))
#define d (CharMask (t))

    while (S >= ((Sig4Byte) 1 << 23)) {
	S >>= 1;
	log2beta--;
    }
    t = pxlw;
    t = (((((d * S) >> 8) + c * S) >> 8) + b * S) >> log2beta;
    if (a) {
	if (a != 255)
	    error("Bad tfm_width");	/* exit */
	t -= alpha;
    }

    return (t);

/*  return (SPtoDEV(t)); */
}
