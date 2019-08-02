/*
 *	jxdvi.h	include file for jxdvi/dvi2nwp.
 *	$Header: jxdvi.h,v 1.3 88/02/27 17:41:20 atsuo Exp $
 *          written by Atsuo, K.
 *          on 88/02/09
 */

#include "atsuo-copyright.h"

#define one(fp)  ( ((unsigned)getc((fp))) )
#define two(fp)  (getc((fp))+(getc((fp))*0x100))
/*
 * #define two(fp)  ( ((unsigned)getc((fp))*0x100) + ((unsigned)getc((fp))) )
 */
#define four(fp) (getc(fp)+(getc(fp)+(getc(fp)+(getc(fp)<<8)<<8)<<8))

#define stwo(fp)    snum(fp, 2)
#define sfour(fp)   snum(fp, 4)

                      
/* num((fp), 4)*/
/*( ((unsigned)getc((fp))<<24) + ((unsigned)getc((fp))<<16) + ((unsigned)getc((fp))<<8) + ((unsigned)getc((fp))) )
*/
