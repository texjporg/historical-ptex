/*
 * Copyright 1988 ASCII Corporation.   
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of ASCII Corporation not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * ASCII Corporation makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * ASCII Corporation DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL DIT CO., LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
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
 * jxl4.c - expand compress bigmap data.
 */

/*
 * $Id: jxl4.c,v 1.20 90/03/23 17:28:10 void Exp $
 */

#include <stdio.h>
#include "config.h"

#if JXL

extern char	*malloc();

static	int  rep;
static	int  peek;
static  int  lastrep;
static	char bitset[] = { 128, 64, 32, 16, 8, 4, 2, 1};
static	char expf;
unsigned char *bufp, *maxbuf;


visible
jxlexp(fp, raster, hbytes, width, height)
int  fp;
char *raster;
int  hbytes;
int  width, height;
{
    unsigned char	*buf, x;
    unsigned int	size;
    int	flag;
    int	t;

    (void)read(fp, (char *)&x, 1);
    flag = x;

    if ((flag & 0x0f) == 14) {
	size = ((width * height) + 7) / 8;

	if ((buf = bufp = (unsigned char *)malloc(size)) == NULL)
	    cant_alloc("expand");	/* exit */
	(void)read(fp, (char *)buf, size);
	expand0(raster, hbytes, width, height);
    }
    else {
	t = ((flag >> 4) & 3) + 1;

	size = 0;
	while (t--) {
	    (void)read(fp, (char *)&x, 1);
	    size = (size << 8) + x;
	}

	if ((buf = bufp = (unsigned char *)malloc(size)) == NULL)
	    cant_alloc("expand");	/* exit */
	maxbuf = buf + size;
	(void)read(fp, (char *)buf, size);

	if ((flag & 0x40) == 0)
	    expand1(flag, raster, hbytes, width, height);
	else
	    expand2(flag, raster, hbytes, width, height);
    }
    free((char *)buf);
}

hidden
expand0(raster, hbytes, width, height)
unsigned char *raster;
int hbytes;
int width, height;
{
    int	i;
    int	shift;
    int	mod;

    if ((mod = width % 8) == 0) {
	for (i = 0; i < hbytes * height; i++)
	    *raster++ = *bufp++;
    }
    else {
	for (shift = 0; height-- > 0; shift = (shift + mod) % 8) {
	    for (i = 0; i < hbytes - 1; i++) {
		*raster = *bufp << shift;
		*raster |= *(++bufp) >> (8 - shift);
		++raster;
	    }
	    *raster = *bufp << shift;
	    if (mod + shift >= 8)
		*raster |= *(++bufp) >> (8 - shift);
	    *raster &= 0xff << (8 - mod);
	    ++raster;
	}
    }
}

hidden
expand1(flag, raster, hbytes, width, height)
int flag;
register char *raster;
int hbytes;
int width, height;
{
    register int    n;
    register int    hpos, bit;
    register char  *lastrow;
    int             black;
    int             dynf;

    expf = 0;
    black = (flag & 0x80) != 0;
    dynf = flag & 0x0f;

    rep = bit = peek = hpos = 0;
    lastrow = raster;

    while (height > 0) {
	n = pcknum(dynf);
	while (n--) {
	    if (black)
		bit |= bitset[hpos & 7];

	    hpos++;

	    if (hpos == width) {
		*raster++ = bit;

		if (--height == 0)
		    return;

		while (rep > 0) {
		    for (hpos = 0; hpos < hbytes; hpos++)
			*raster++ = *lastrow++;

		    if (--height == 0)
			break;

		    rep--;
		}
		lastrow += hbytes;
		bit = hpos = 0;
	    }
	    else if ((hpos & 7) == 0) {
		*raster++ = bit;
		bit = 0;
	    }
	}
	black = !black;
    }
}

hidden
expand2(flag, raster, hbytes, width, height)
register char *raster;
{
    register int    n, i;
    register int    hpos;
    register char  *lastrow;

    expf = 1;
    rep = peek = 0;
    hpos = -1;
    lastrow = raster;

    for (i = 0; i < hbytes; ++i)
	lastrow[i] = 0;

    while (height > 0) {
	n = pcknum(flag & 0xf);

	if (n < 0) {
	    while (--height) {
		raster = lastrow + hbytes;

		for (i = 0; i < hbytes; i++)
		    *raster++ = *lastrow++;

	    }
	    return;
	}

	while (n--) {
	    hpos++;

	    if (!(hpos & 7) && hpos)
		raster++;

	    if (hpos == width) {
		if (!(--height))
		    return;
		if (hpos & 7)
		    raster++;

		for (i = 0; i < hbytes; i++)
		    *raster++ = *lastrow++;

		raster = lastrow;
		hpos = 0;
	    }
	}

	rep++;

	while (rep) {
	    *raster ^= bitset[hpos & 7];
	    hpos++;

	    if (!(hpos & 7) && hpos)
		raster++;

	    if (hpos == width) {
		if (!(--height))
		    return;
		if (hpos & 7)
		    raster++;

		for (i = 0; i < hbytes; i++)
		    *raster++ = *lastrow++;

		raster = lastrow;
		hpos = 0;
	    }
	    rep--;
	}
    }
}

hidden
getnyb()
{
    if (bufp >= maxbuf)
	return (-1);

    if (peek) {
	peek = 0;
	return (*bufp++ & 15);
    }
    else {
	peek = 1;
	return (*bufp >> 4);
    }
}

hidden
pcknum(dynf)
int dynf;
{
    register int    i, j;

    if ((i = getnyb()) < 0)
	return (-1);

    if (i == 0) {
	do {
	    j = getnyb();
	    i++;
	} while (j == 0);

	while (i > 0) {
	    j = j * 16 + getnyb();
	    i--;
	}
	return (j - 15 + (13 - dynf) * 16 + dynf);
    }
    else if (i <= dynf)
	return (i);
    else if (i < 14)
	return ((i - dynf - 1) * 16 + getnyb() + dynf + 1);
    else {
	if (rep != 0)
	    error("Extra repeat count");	/* exit */
	if (i == 14)
	    lastrep = rep = pcknum(dynf);
	else {
	    if (expf)
		rep = lastrep;
	    else
		rep = 1;
	}
	return (pcknum(dynf));
    }
}
#endif
