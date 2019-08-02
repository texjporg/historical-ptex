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
 * stack.c - handling the dvi stack codes.
 */

/*
 * $Id: stack.c,v 1.10 90/03/27 11:03:39 void Exp $
 */

#include <stdio.h>
#include "config.h"
#include "fio.h"
#include "dvicom.h"
#include "stack.h"

visible	STACK	Curr;
hidden	STACK	*StackBuf;
hidden	int	IndexOfStack;

extern Sig2Byte	MaxOfStack;
extern char *malloc();

visible
ResetStack()
{
    Curr.Horizontal = (Sig4Byte) 0; Curr.Vertical = (Sig4Byte) 0;
    Curr.Woffset    = (Sig4Byte) 0; Curr.Xoffset  = (Sig4Byte) 0;
    Curr.Yoffset    = (Sig4Byte) 0; Curr.Zoffset  = (Sig4Byte) 0;
#if TATE
    Curr.Direction  = (Sig1Byte) YOKO_DIR;
#endif
}

visible
InitStack()
{
    if ((StackBuf = (struct Stack_Info *) malloc((unsigned int) (MaxOfStack * sizeof(struct Stack_Info)))) == NULL)
	cant_alloc("stack");	/* exit */

    /*
     * Initialize the value
     */
    IndexOfStack = 0;
    ResetStack();
}

visible
Push()
{
    register STACK *NewStack = &StackBuf[IndexOfStack];

    NewStack->Horizontal = Curr.Horizontal;
    NewStack->Vertical   = Curr.Vertical;
    NewStack->Woffset    = Curr.Woffset;
    NewStack->Xoffset    = Curr.Xoffset;
    NewStack->Yoffset    = Curr.Yoffset;
    NewStack->Zoffset    = Curr.Zoffset;
#if TATE
    NewStack->Direction  = Curr.Direction;
#endif

    if (IndexOfStack++ > MaxOfStack) {
	(void) fprintf(stderr, "! Stack overflow. limit %d, current %d.\n",
		MaxOfStack, IndexOfStack);
	exit(1);
    }
}

visible
Pop()
{
    register STACK *NewStack;

    if (IndexOfStack-- < 0) {
	(void) fprintf(stderr, "! Stack underflow, current %d.\n", IndexOfStack);
	exit(1);
    }

    NewStack = &StackBuf[IndexOfStack];
    Curr.Horizontal = NewStack->Horizontal;
    Curr.Vertical   = NewStack->Vertical;
    Curr.Woffset    = NewStack->Woffset;
    Curr.Xoffset    = NewStack->Xoffset;
    Curr.Yoffset    = NewStack->Yoffset;
    Curr.Zoffset    = NewStack->Zoffset;
#if TATE
    Curr.Direction  = NewStack->Direction;
#endif
}
