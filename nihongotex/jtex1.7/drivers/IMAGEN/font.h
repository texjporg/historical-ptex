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
 * font.h - font structure definition.
 */

/*
 * $Id: font.h,v 1.9 90/03/27 11:00:31 void Exp $
 */

typedef struct {
    Sig2Byte	width;
    Sig2Byte	height;
    Sig2Byte	xoffset;
    Sig2Byte	yoffset;
    Sig4Byte	dataoffset;
    Sig4Byte	tfmwidth;
} GLYPH_DATA;

typedef struct {
    Sig4Byte	pxl_sum;
    Sig4Byte	pxl_mag;
    Sig4Byte	pxl_dsn;
    Sig4Byte	pxl_dir;
    Sig4Byte	pxl_id;
} PXL;

typedef struct {
    Sig4Byte    CheckSum;
    Sig4Byte    RelMag;
    Sig4Byte    font_mag;
    Sig4Byte    font_dsz;
    char	*fontfilename;
    PXL		pxl_data;
    GLYPH_DATA  *GlyphData;
    short	*DownLoad;
    Sig4Byte	*TFMwidth;
} NewFontFileInfo;

#define MAXFONTFILE	256

#define PXLID		1001	/* Enlgish version PXL font id */
#define DITID		1004	/* dit version PXL font id */

#define SIZEof_GLYPH_DATA	16
#define SIZEof_PXL			20
