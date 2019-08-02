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
 * stack.h - h, v, w, x, y, z, hh, vv value stack.
 */

/*
 * $Id: stack.h,v 1.6 90/03/27 11:03:42 void Exp $
 */

typedef struct Stack_Info {
    Sig4Byte	Horizontal;	/* Current Position of Horizontal */
    Sig4Byte    Vertical;	/* Current Position of Vertical   */
    Sig4Byte    Woffset;	/* Current Spacing for Horizontal */
    Sig4Byte    Xoffset;	/* Current Spacing for Horizontal */
    Sig4Byte    Yoffset;	/* Current Spacing for Vertical   */
    Sig4Byte    Zoffset;	/* Current Spacing for Vertical   */
#if TATE
    Sig1Byte    Direction;      /* Current Direciton */
#endif
} STACK;
