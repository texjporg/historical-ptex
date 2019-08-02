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
 * page.c - Handling the one page
 */

/*
 * $Id: page.c,v 1.10 90/03/27 11:02:53 void Exp $
 */

#include <stdio.h>
#include "dvicom.h"
#include "config.h"
#include "fio.h"

extern FILE	*dvi;
extern int	use_silent;
extern int	((**DviCode[])());

visible
ScanOnePage(from)
Sig4Byte	from;
{
    register int    c;
    register int    (***func)() = DviCode;

    (void) fseek(dvi, (long)from, 0);

    if (Get1Byte(dvi) == BOP)
	InitPage();
    else
	error("Can't find BOP in ScanOnePage");	/* exit */

#if 1
    do {
	c = Get1Byte(dvi);
	c = (**func[c])(c);
    } while (c != EOP);

    if (!use_silent)
	(void) fprintf(stderr, "] ");

#else
    while ((c = Get1Byte(dvi)) != EOP)
	(**func[c])(c);

    if (!use_silent)
	(void) fprintf(stderr, "] ");

    EndPage();
#endif
}
