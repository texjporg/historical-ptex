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
 * fntdef.c - FontDef code handling.
 */

/*
 * $Id: fntdef.c,v 1.11 90/03/27 11:00:23 void Exp $
 */

#include <stdio.h>
#include "dvicom.h"
#include "config.h"
#include "fio.h"
#include "font.h"

extern NewFontFileInfo font_node[];

extern FILE	*dvi;

extern char	*AbsPath();
extern int	(*GetFontIndex)();
extern char	*(*GetFontSuffix)();
extern char	*strcpy();

visible char *
GetFontByName(i)
int i;
{
    return (font_node[i].fontfilename);
}

/* ARGSUSED */
visible
GetFontDef1(c)
int c;
{
    FntDef_Parse(Get1Byte(dvi));
}

/* ARGSUSED */
visible
GetFontDef2(c)
int c;
{
    FntDef_Parse(Get2Byte(dvi));
}

/* ARGSUSED */
visible
GetFontDef3(c)
int c;
{
    FntDef_Parse(Get3Byte(dvi));
}

/* ARGSUSED */
visible
GetFontDef4(c)
int c;
{
    FntDef_Parse(Get4Byte(dvi));
}

hidden
FntDef_Parse(def_byte)
Sig4Byte    def_byte;
{
    (*GetFontIndex)(def_byte);
}

/* ARGSUSED */
visible
CheckFontDef1(c)
int c;
{
    CheckDef_Parse(Get1Byte(dvi));
}

/* ARGSUSED */
visible
CheckFontDef2(c)
int c;
{
    CheckDef_Parse(Get2Byte(dvi));
}

/* ARGSUSED */
visible
CheckFontDef3(c)
int c;
{
    CheckDef_Parse(Get3Byte(dvi));
}

/* ARGSUSED */
visible
CheckFontDef4(c)
int c;
{
    CheckDef_Parse(Get4Byte(dvi));
}

/* ARGSUSED */
hidden
CheckDef_Parse(def_byte)
Sig4Byte    def_byte;
{
    char junk[128];
    char *targetfont;
    Sig4Byte fontmag;
    Sig4Byte fontdsz;

    char *strcat();
    char *GetString();

    targetfont = NULL;
    (void)Get4Byte(dvi);	/* skip (long)checksum */
    fontmag = Get4Byte(dvi);
    fontdsz = Get4Byte(dvi);

    (void) strcpy(junk, strcat(GetString(dvi, (int) (Get1Byte(dvi) +
	    Get1Byte(dvi))), (*GetFontSuffix)(fontmag, fontdsz)));

    targetfont = AbsPath(junk);

    if (targetfont != NULL)
	(void) fprintf(stderr, "[exist] : ");
    else
	(void) fprintf(stderr, "[absent]: ");

    (void) fprintf(stderr, "%s\n", junk);
}

/* ARGSUSED */
visible
SkipFontDef1(c)
int c;
{
    SkipFntDefMain(Get1Byte(dvi));
}

/* ARGSUSED */
visible
SkipFontDef2(c)
int c;
{
    SkipFntDefMain(Get2Byte(dvi));
}

/* ARGSUSED */
visible
SkipFontDef3(c)
int c;
{
    SkipFntDefMain(Get3Byte(dvi));
}

/* ARGSUSED */
visible
SkipFontDef4(c)
int c;
{
    SkipFntDefMain(Get4Byte(dvi));
}

/* ARGSUSED */
hidden
SkipFntDefMain(def_byte)
Sig4Byte    def_byte;
{
    register int i;

    for (i = 0; i < 3; ++i)
	(void) Get4Byte(dvi);

    (void) GetString(dvi, (int) (Get1Byte(dvi) + Get1Byte(dvi)));
}
