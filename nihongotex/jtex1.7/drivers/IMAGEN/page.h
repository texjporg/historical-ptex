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
 * page.h - page linkage structure for dviimp
 */

/*
 * $Id: page.h,v 1.7 90/03/27 11:02:55 void Exp $
 */

typedef struct Link {
    Sig4Byte	Bop;	/* Pointer to BOP (Beginning Of Page) */
#if 0
    Sig4Byte	Eop;	/* Pointer to EOP (End Of Page) */
#endif
    struct Link *Next;	/* Pointer to Next Page */
    struct Link *Prev;	/* Pointer to Prev Page */
} PAGE_LINK;

#if 0
typedef struct UserPage {
    int id_counter;
    struct UserPage *Next;
} USER_PAGE;
#endif

#define STRAIGHT    0
#define ODD	    1
#define EVEN	    2
#define REVERSE	    3
