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
 * dvicom.h - dvi code nemonic command nemonic.
 */

/*
 * $Id: dvicom.h,v 1.6 90/03/27 10:51:45 void Exp $
 */

#define	DVI_ID		2
#if TATE
#define	DVI_ID_TATE	3
#define TATE_DIR        1
#define YOKO_DIR        0
#endif

#define	DVI_PAD		223

#define	SET_RULE	132
#define	PUT_RULE	137
#define	NOP		138
#define	BOP		139
#define	EOP		140
#define	PUSH		141
#define	POP		142
#define FNT_NUM_0	171
#define	FNT_DEF1	243
#define	FNT_DEF2	244
#define	FNT_DEF3	245
#define	FNT_DEF4	246
#define	PRE		247
#define	POST		248
#define	POST_POST	249
#define BEGIN_REFLECT	250
#define END_REFLECT	251
