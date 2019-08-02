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
 * fio.c - Non machine dependent file I/O routine
 */

/*
 * $Id: fio.c,v 1.4 90/03/27 11:00:17 void Exp $
 */

#include <stdio.h>
#include "config.h"
#include "fio.h"

visible Sig2Byte
Get2Byte(fp)
register FILE	*fp;
{
    register Sig2Byte	n;

    n = Get1Byte(fp);
    return ((n << 8) | Get1Byte(fp));
}

visible Sig3Byte
Get3Byte(fp)
register FILE	*fp;
{
    register Sig3Byte	n;

    /* We must cast to 'Signed Char' to set the MSB bit */
    n = (Sig1Byte) Get1Byte(fp);
    n = (n << 8) | Get1Byte(fp);
    return ((n << 8) | Get1Byte(fp));
}

visible Sig4Byte
Get4Byte(fp)
register FILE	*fp;
{
    register Sig4Byte	n;

    n = Get1Byte(fp);
    n = (n << 8) | Get1Byte(fp);
    n = (n << 8) | Get1Byte(fp);
    return ((n << 8) | Get1Byte(fp));
}

visible int
Put2Byte(n, fp)
register Sig2Byte   n;
register FILE	    *fp;
{
    (void) Put1Byte(n >> 8, fp);
    return (Put1Byte(n, fp));
}

visible int
Put3Byte(n, fp)
register Sig3Byte   n;
register FILE	    *fp;
{
    (void) Put1Byte(n >> 16, fp);
    (void) Put1Byte(n >> 8, fp);
    return (Put1Byte(n, fp));
}

visible int
Put4Byte(n, fp)
register Sig4Byte   n;
register FILE	    *fp;
{
    (void) Put1Byte(n >> 24, fp);
    (void) Put1Byte(n >> 16, fp);
    (void) Put1Byte(n >> 8, fp);
    return (Put1Byte(n, fp));
}
