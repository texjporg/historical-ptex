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
 * code.c - Handling the PDL independent DVI code.
 */

/*
 * $Id: code.c,v 1.8 90/03/27 10:51:32 void Exp $
 */

/*
 * This is the PDL independent DVI Code Routine.
 *
 * 1. Page Initialize Routine.
 *    Initialize the Stack and other things.
 * 2. Stacking Routine.
 *    Handling the Stacking Area. It's independent.
 * 3. No Operation Routine.
 *    Handling the NOP Operation.
 * 4. Illegal DVI Code Routine.
 *    When Scanning the DVI Code after read the Preamble and Postabmle,
 *    there is no chance to see those code once more.  And also, we have
 *    that function for undefined DVI Code at 250-255.
 */

#include <stdio.h>
#include "dvicom.h"
#include "config.h"
#include "fio.h"
#include "stack.h"

extern double   conv;
#if 0
extern FILE	*out;
#endif
extern FILE	*dvi;

extern int	use_silent;

visible
InitPage()
{
    Sig4Byte    count;
    register	int i;

    /*
     * Get Page Number
     */
    count = Get4Byte(dvi);
    for (i = 0; i < 10; ++i)
	(void) Get4Byte(dvi);

    /*
     * Clear the Current System Position.
     */
    ResetStack();

    /*
     * Initialize the new page.
     */
    SetPage();

    if (!use_silent)
	(void) fprintf(stderr, "[%d", count);
}

/* ARGSUSED */
visible
nop(c)
int	c;
{
    /*
     * This routine is dummy.
     */
#if 0
    (void) fprintf(out, "! Nooperation.\n");
#endif
}

/* ARGSUSED */
visible
illegal(c)
int	c;
{
    (void) fprintf(stderr, "! Illegal command (%d).\n", c);
    exit(1);
}

/* ARGSUSED */
visible
undefined(c)
int  c;
{
    (void) fprintf(stderr, "! Undefined command (%d).\n", c);
    exit(1);
}
