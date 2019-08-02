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
 * fontsuffix.c - get target font's suffix.
 *	Thus, ... PXL, PK, GF, or FAR ... and more(?).
 *	NOTE: Suffix of the GF font file is same as PK's one.
 */

/*
 * $Id: fontsuffix.c,v 1.11 90/03/27 11:01:56 void Exp $
 */

#include <stdio.h>
#include "config.h"
#include "fio.h"

#define fix(x)	(int)((x) + 0.5)

extern int	use_resolution;
extern double   use_magnification;

/* usagi */
extern double	Magnification;

#if !MSDOS
extern char	*sprintf();
#endif

hidden char	realname[128];

hidden double
ratio(i, j)	/* due to TeX's bad design */
Sig4Byte i, j;
{
    double f;

    f = (double) i / (double) j;
    switch(fix(f * 1000.0)) {
	case 1095: case 1096:
	    f = 1.0954451; break;
	case 1314: case 1315:
	    f = 1.3145341; break;
	case 1643: case 1644:
	    f = 1.6431676; break;
	case 2073: case 2074:
	    f = 2.0736; break;
	case 2488: case 2489:
	    f = 2.48832; break;
	case 2985: case 2986:
	    f = 2.985984; break;
	case 3110: case 3111:
	    f = 3.1104; break;
	case 3732: case 3733:
	    f = 3.73248; break;
	case 4478: case 4479:
	    f = 4.478976; break;
	default:	/* O.K. */;
    }
    return (f);
}

visible char *
PXLSuffix(font_mag, font_dsz)
Sig4Byte  font_mag; /* font's magnification */
Sig4Byte  font_dsz; /* font's desizn size */
{
    register int    num;
    double	    fmag;

    fmag = ratio(font_mag, font_dsz);	/* font mag */
    num  = fix((use_resolution * 5.0) * fmag * use_magnification *
		Magnification);
    (void) sprintf(realname, ".%d%s", num, "pxl");

    return (realname);
}

#if	0
visible char *
GFSuffix(font_mag, font_dsz)
Sig4Byte  font_mag; /* font's magnification */
Sig4Byte  font_dsz; /* font's desizn size */
{
    register int    num;
    double	    fmag;

    fmag = ratio(font_mag, font_dsz);	/* font mag */
    num =  fix(use_resolution * fmag * use_magnification *
		Magnification);
    (void) sprintf(realname, ".%d%s", num, "gf");

    return (realname);
}
#endif

#if	0
visible char *
PKSuffix(font_mag, font_dsz)
Sig4Byte  font_mag; /* font's magnification */
Sig4Byte  font_dsz; /* font's desizn size */
{
    register int    num;
    double	    fmag;

    fmag = ratio(font_mag, font_dsz);	/* font mag */
    num  = fix(use_resolution * fmag * use_magnification *
		Magnification);
    (void) sprintf(realname, ".%d%s", num, "pk");

    return (realname);
}
#endif
