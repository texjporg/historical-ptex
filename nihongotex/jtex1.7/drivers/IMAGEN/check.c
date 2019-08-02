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
 * check.c - Scan the Preamble and Postamble
 */

/*
 * $Id: check.c,v 1.19 90/03/27 10:51:27 void Exp $
 */

#include <stdio.h>
#include "config.h"
#include "fio.h"
#include "dvicom.h"
#include "amble.h"

visible DIMENSION   Post;
visible DIMENSION   Pre;
visible Sig2Byte    TotalPage;
visible Sig2Byte    MaxOfStack;
visible Sig4Byte    SizeOfMaxTallPage;
visible Sig4Byte    SizeOfMaxWidePage;

visible double	    GlobalMag;
visible double	    Magnification;

Sig4Byte GlobalDen;
Sig4Byte GlobalNum;

double	conv;
double	UserMag;

extern	FILE	*dvi;
extern  int	use_fsck;
extern  int	use_resolution;
extern  double	use_magnification;

extern	GetFontDef1();
extern	GetFontDef2();
extern	GetFontDef3();
extern	GetFontDef4();

extern	CheckFontDef1();
extern	CheckFontDef2();
extern	CheckFontDef3();
extern	CheckFontDef4();

extern  double	DMagFactor();

visible Sig4Byte
GetPostamble()
{
    int		byte;
    Sig4Byte    startchain; /* Link Page Pointer */

    char    *GetString();

    if (Get1Byte(dvi) != POST)
	error("Can't find postamble");	/* exit */

    startchain = Get4Byte(dvi);	/* pointer to Last Page's BOP */

    if ((Post.Num = Get4Byte(dvi)) == Pre.Num)
	GlobalNum = Post.Num;
    else
	PPError("Numerator");

    if ((Post.Den = Get4Byte(dvi)) == Pre.Den)
	GlobalDen = Post.Den;
    else
	PPError("Denominator");

    if ((Post.Mag = Get4Byte(dvi)) == Pre.Mag)
	GlobalMag = Post.Mag;
    else
	PPError("Magnification");

    SizeOfMaxTallPage = Get4Byte(dvi);
    SizeOfMaxWidePage = Get4Byte(dvi);
    MaxOfStack = Get2Byte(dvi);
    TotalPage = Get2Byte(dvi);

    /*
     * calculate conv value.
     */

    Magnification = (double) GlobalMag / 1000.0;
    UserMag = use_magnification * ((double) (use_resolution) / 200.0);
    GlobalMag = DMagFactor(Post.Mag) * UserMag;
    conv = (Post.Num / 254000.0) * (200.0 / Post.Den) * GlobalMag;

    while ((byte = Get1Byte(dvi))) {
	if (use_fsck == 1) {
	    switch (byte) {
	    case FNT_DEF1:
		CheckFontDef1(byte);
		break;
	    case FNT_DEF2:
		CheckFontDef2(byte);
		break;
	    case FNT_DEF3:
		CheckFontDef3(byte);
		break;
	    case FNT_DEF4:
		CheckFontDef4(byte);
		break;
	    default:
		exit(1); /* check only, not producing the pdl */
	    /*  goto getaway; */
	    }
	}
	else if (use_fsck == 0) {
	    switch (byte) {
	    case FNT_DEF1:
		(void) Get1Byte(dvi);
		break;
	    case FNT_DEF2:
		(void) Get2Byte(dvi);
		break;
	    case FNT_DEF3:
		(void) Get3Byte(dvi);
		break;
	    case FNT_DEF4:
		(void) Get4Byte(dvi);
		break;
	    default:
		goto getaway;
	    }
	}
	else if (use_fsck == 2) {
	    switch (byte) {
	    case FNT_DEF1:
		GetFontDef1(byte);
		break;
	    case FNT_DEF2:
		GetFontDef2(byte);
		break;
	    case FNT_DEF3:
		GetFontDef3(byte);
		break;
	    case FNT_DEF4:
		GetFontDef4(byte);
		break;
	    default:
		goto getaway;
	    }
	}
    }

getaway:
    return (startchain);
}

visible
GetPreamble()
{
    char    *GetString();

    if (Get1Byte(dvi) != PRE)
	error("It's not start with a preamble");	/* exit */
    (void)Get1Byte(dvi);	/* dvi file ID number, TeX82 is "2" */
    Pre.Num = Get4Byte(dvi);
    Pre.Den = Get4Byte(dvi);
    Pre.Mag = Get4Byte(dvi);
    (void)GetString(dvi, Get1Byte(dvi));
}

visible Sig4Byte
GetPostPtr()
{
    register	int i;
    Sig4Byte    Post_Ptr;
#if TATE
    Sig1Byte	dvi_id;
#endif

/*
 * DVI File Format is contain the PosPostamble in last Part.
 * DVI File's last bytes is as follows.
 *
 *-------------------------------------------------------------
 * ... POST_POST + POST_PTR + DVI_ID + DVI_PAD + DVI_PAD + ...
 *
 * POST_POST is a starting command for PostPostamble.
 * POST_PTR  is a starting pointer to Postamble.
 * DVI_ID    is a DVI File Format ID. (Must be 2, this time.)
 * DVI_PAD   is a stabb for setting the file bytes on boundary.
 *-------------------------------------------------------------
 *
 * So, I'll see the file from last part, and then check & get
 * Postamble Part.
 */

    i = 0;
    do {
	(void) fseek(dvi, (long) --i, 2);
    } while (Get1Byte(dvi) == DVI_PAD);

    /*
     * Skip Back 5L.
     * 5L = POST_POST(1) + POST_ptr(4).
     */

    (void) fseek(dvi, (long) i - 5, 2);

    if (Get1Byte(dvi) != POST_POST)
	error("Can't find postpostamble");	/* exit */

    Post_Ptr = Get4Byte(dvi);

#if TATE
    if ((dvi_id = Get1Byte(dvi)) != DVI_ID && dvi_id != DVI_ID_TATE) {
#else
    if (Get1Byte(dvi) != DVI_ID) {
#endif
	error("dvi id is not much in postpostamble");	/* exit */
    }

    return (Post_Ptr);
}

hidden
PPError(msg)
char	*msg;
{
    (void) fprintf(stderr, "! %s in preambe and postamble is not much.\n", msg);    exit(1);
}
