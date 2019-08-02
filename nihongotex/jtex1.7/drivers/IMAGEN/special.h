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
 * special.h - speacial
 */

/*
 * $Id: special.h,v 1.3 90/03/27 11:03:36 void Exp $
 */

/*
#define TAB	'\t'
#define EOS	'\0'
*/

#define	OR	7
#define WHITE	0
#define BLACK	15
#define SHADE	3

#define DEFAULT_PEN_SIZE    2
#define MAX_PEN_SIZE	    20
#define	MAX_POINTS	    300
#define	SPLINE_POINTS	    900
#define	RADTOPXL	    2607.435436
#define	TWOPI		    (3.14157926536*2.0)

typedef struct {
    int (*pn_com)();
    int (*fp_com)();
    int (*da_com)();
    int (*dt_com)();
    int (*pa_com)();
    int (*ar_com)();
    int (*sp_com)();
    int (*wh_com)();
    int (*bk_com)();
    int (*sh_com)();
    int (*tx_com)();
    int (*fl_com)(); /* dit */
} NEW_SPECIAL;
