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
 * link.c - Make Linkage for Page Pointer
 */

/*
 * $Id: link.c,v 1.13 90/03/27 11:02:45 void Exp $
 */

#include <stdio.h>
#include <ctype.h>
#include "dvicom.h"
#include "config.h"
#include "fio.h"
#include "page.h"

extern FILE	*dvi;
extern char	*malloc();
extern Sig2Byte	TotalPage;

visible PAGE_LINK *LastPage;
visible PAGE_LINK *FirstPage;

/*
 * PAGE_LINK Page is the real body of linkage-chains.
 * PAGE_LINK dummy is next pointer of first/last link.
 */

hidden  PAGE_LINK Page;
hidden  PAGE_LINK dummy;

#if 0
hidden  USER_PAGE *UserPageList;
#endif

/*
 * set BOP-linkage for every page.
 */

visible
SetPageLink(flag, startchain, PageString)
int	    flag;
Sig4Byte    startchain;
char	    *PageString;
{
    register	int i;
    int		count;
    PAGE_LINK	*junk, *new;

#if 0
    ParseUserPage(PageString);
#endif

    Page.Bop = startchain;
    Page.Next = Page.Prev = NULL;
    LastPage = FirstPage = junk = &Page;

    while (junk->Bop != -1L) {
	(void) fseek(dvi, (long)junk->Bop, 0);

	if (Get1Byte(dvi) != BOP)
	    error("Can't find the BOP in SetPageLink");	/* exit */

	count = Get4Byte(dvi); /* count = Get4Byte(dvi) % 2; */

	for (i = 0; i < 9; ++i)
	    (void) Get4Byte(dvi);

	if (flag != STRAIGHT) {
	    count %= 2;
	    if ((flag == EVEN && count != 0) || (flag == ODD && count == 0)) {
		junk->Bop = Get4Byte(dvi);
		continue;
	    }
#if	0
	else if (!PageMatch((int) count)) {
		junk->Bop = Get4Byte(dvi);
		continue;
	    }
	    else if (flag == PIECES) {
		if (!is_this_page(count, page_list)) {
			junk->Bop = Get4Byte(dvi);
			countinue;
		}
	    }
#endif /* mixing */
	}

	if ((new = (struct Link *) malloc(sizeof(struct Link))) == NULL)
	    cant_alloc("Page Link");	/* exit */

	new->Bop = Get4Byte(dvi);
	new->Prev = NULL;
	new->Next = junk;
	junk->Prev = new;
	junk = new;
    }

    dummy.Next = dummy.Prev = NULL;

    if (junk->Next == NULL)
	FirstPage = &dummy;
    else
	FirstPage = junk->Next;

    LastPage->Next  = &dummy;
    FirstPage->Prev = &dummy;
}

#if	0
hidden
PageMatch(dvi_page)
register int dvi_page;
{
    USER_PAGE *temp;

    temp = UserPageList;

    while (temp->Next != NULL) {
	if (dvi_page == temp->id_counter)
	    return (YES);
	temp = temp->Next;
    }
    return (NO);
}

hidden
ParseUserPage(s)
char	*s;
{
    register int n;
    register USER_PAGE *temp;

    char *eatblk();

    if ((UserPageList = (struct UserPage *)
        malloc((unsigned int) (TotalPage * sizeof(struct UserPage)))) == NULL)
	cant_alloc("user specified page list");	/* exit */

    temp = UserPageList;
    temp->Next = NULL;

    while (*s != EOS) {
	s = eatblk(s);

	for (n = 0; isdigit(*s); s++) {
	    n *= 10;
	    n += *s - '0';
	}
	temp->id_counter = n;
	temp->Next = (temp + 1);
	++temp;
	s++;
    }
    temp->Next = NULL;
}
#endif
