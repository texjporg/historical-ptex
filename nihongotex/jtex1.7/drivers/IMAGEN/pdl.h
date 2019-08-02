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
 * pdl.h - Page Description Language of device.
 */

/*
 * $Id: pdl.h,v 1.5 90/03/27 11:03:22 void Exp $
 */

typedef struct {
    int	(*SetChar)();
    int	(*Set1)();
    int	(*Set2)();
    int	(*Set3)();
    int	(*Set4)();
    int	(*SetRule)();
    int	(*Put1)();
    int	(*Put2)();
    int	(*Put3)();
    int	(*Put4)();
    int	(*PutRule)();
    int	(*Nop)();
    int	(*Bop)();
    int	(*Eop)();
    int	(*Push)();
    int (*Pop)();
    int	(*Right1)();
    int	(*Right2)();
    int	(*Right3)();
    int	(*Right4)();
    int	(*W0)();
    int	(*W1)();
    int	(*W2)();
    int	(*W3)();
    int	(*W4)();
    int	(*X0)();
    int	(*X1)();
    int	(*X2)();
    int	(*X3)();
    int	(*X4)();
    int	(*Y0)();
    int	(*Y1)();
    int	(*Y2)();
    int	(*Y3)();
    int	(*Y4)();
    int	(*Z0)();
    int	(*Z1)();
    int	(*Z2)();
    int	(*Z3)();
    int	(*Z4)();
    int	(*Down1)();
    int	(*Down2)();
    int	(*Down3)();
    int	(*Down4)();
    int	(*FntNum)();
    int	(*Fnt1)();
    int	(*Fnt2)();
    int	(*Fnt3)();
    int	(*Fnt4)();
    int	(*XXX1)();
    int	(*XXX2)();
    int	(*XXX3)();
    int	(*XXX4)();
    int	(*FntDef1)();
    int	(*FntDef2)();
    int	(*FntDef3)();
    int	(*FntDef4)();
    int	(*Preamble)();
    int	(*Postamble)();
    int	(*PostPostamble)();
    int	(*Undefined)();
    int	(*Illegal)();

#if TATE
    int (*Dir)();
#endif

    int (*SetUpJob)();
    int (*EndUpJob)();
    int (*SetUpPage)();
    int (*EndUpPage)();
} NEW_PDL;
