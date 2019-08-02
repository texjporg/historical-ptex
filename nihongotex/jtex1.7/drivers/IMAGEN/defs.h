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
 * defs.h - General definition for dvi2
 */

/*
 * $Id: defs.h,v 1.7 90/03/27 10:51:41 void Exp $
 */

#define MASK	    0377

#define CharMask(c) ((c) & MASK)

#ifdef	NULL
#undef	NULL
#endif

#ifndef	EOF
#define EOF	    (-1)
#endif

#define NULL	    0
#define ERR	    (-1)
#define TRUE	    1
#define FALSE	    0
#define ON	    1
#define OFF	    0
#define YES	    1
#define OK	    1
#define NO	    0
#define ZERO	    0
#define SUCCESS	    0

#define EOS	    '\0'
#define TAB	    '\t'
#define CR	    '\r'
#define LF	    '\n'
#define FF	    '\f'
#define BS	    '\b'
#define DEL	    '\177'
#define ESC	    '\033'
#define BLANK	    '\040'
#define BELL	    '\007'

#define UNLOAD      -1

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define abs(a)	    (((a) < 0)  ? -(a) : (a))

#define RoundUp(n,r)	(((n) + ((r) - 1)) & ~((r) - 1))

#define visible
#define hidden	    static
