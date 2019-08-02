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
 * fontinit.c - initialize target font routine, depending on suffix.
 */

/*
 * $Id: fontinit.c,v 1.10 90/03/27 11:01:07 void Exp $
 */

#include <stdio.h>
#include "config.h"
#include "fio.h"

visible char *(*GetFontSuffix)();   /* Getting font suffix. */
visible int  (*GetFontIndex)();     /* Getting font index dir. */
visible char *(*GetFontGlyph)();    /* Getting font's glyph data. */

typedef struct fonttype {
    char *SuffixName;
    char *(*SuffixRoutine)();
    int  (*IndexRoutine)();
    char *(*GlyphRoutine)();
} FONTTYPE;

/*
 * Define the available font formats routines, as follows.
 */

#if	0
extern char *PKSuffix();
extern char *PKGlyph();
extern int  PKIndex();
#endif

#if	0
extern char *GFSuffix();
extern char *GFGlyph();
extern int  GFIndex();
#endif

#if	0
extern char *RSTSuffix();
extern char *RSTGlyph();
extern int  RSTIndex();
#endif

#if	0
extern char *FARSuffix();
extern char *FARGlyph();
extern int  FARIndex();
#endif

extern char *PXLSuffix();
extern char *PXLGlyph();
extern int  PXLIndex();

FONTTYPE SuffixTable[] = {
#if	0
    {"PK",	PKSuffix,	PKIndex,    PKGlyph},
    {"GF",	GFSuffix,	GFIndex,    GFGlyph},
    {"RST",	RSTSuffix,	RSTIndex,   RSTGlyph},
    {"FAR",	FARSuffix,	FARIndex,   FARGlyph},
#endif
    {"PXL",	PXLSuffix,	PXLIndex,   PXLGlyph},
    {NULL,	NULL,		NULL,	    NULL}
};

visible
InitFontRoutine(suffix)
register char *suffix;
{
    register FONTTYPE *p;

    for (p = SuffixTable; p->SuffixName != NULL; ++p) {
	if (strcmp(suffix, p->SuffixName) == 0) {
	    GetFontSuffix = p->SuffixRoutine;
	    GetFontIndex  = p->IndexRoutine;
	    GetFontGlyph  = p->GlyphRoutine;
	    return;
	}
    }
    error("Unknown Font file information");	/* exit */
}
