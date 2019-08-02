/*
 *	jxdvi.h	include file for jxdvi/dvi2nwp.
 *	$Header: jxdvi.h,v 3.2 89/03/14 13:29:49 atsuo Exp $
 *          written by Atsuo, K.
 *          on 88/02/09
 */

#include "atsuo-copyright.h"

#define pixel_round(x)      ((long) (conv * (double) (x) + 0.5))
#define dvi_round(x)        ((long) ((double) (x) / conv + 0.5))

#define one(fp)  ( ((unsigned)getc((fp))) )
/* #define two(fp)  (getc((fp))+(getc((fp))*0x100))
 * #define four(fp) (getc(fp)+(getc(fp)+(getc(fp)+(getc(fp)<<8)<<8)<<8))
 */

#define stwo(fp)    snum(fp, 2)
#define sfour(fp)   snum(fp, 4)

                      
/* num((fp), 4)*/
/*( ((unsigned)getc((fp))<<24) + ((unsigned)getc((fp))<<16) + ((unsigned)getc((fp))<<8) + ((unsigned)getc((fp))) )
*/

extern long numerator, denominator, magnification;
extern int debug;
extern int pixels_per_inch;
extern double fraction, conv;
extern shrink_factor;

#define PIXELS_PER_INCH pixels_per_inch

extern char *malloc(), *calloc(), *index();

/* Paper size */
#define A4_PAPER 1
#define B4_PAPER 2
#define LETTER_PAPER 3

#define A4_WIDTH 8.27		/* (inches) */
#define A4_HEIGHT 11.69
#define B4_WIDTH 10.12
#define B4_HEIGHT 14.33
#define LETTER_WIDTH 8.5
#define LETTER_HEIGHT 11
