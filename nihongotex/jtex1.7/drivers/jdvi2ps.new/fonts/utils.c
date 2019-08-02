/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: utils.c,v 1.5 89/12/28 21:05:50 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/12/28 21:05:50 $
 */

/*
 * some utilities
 * ==============
 */

#include <stdio.h>
#include "font.h"
#include "common.h"

/*
 * utilities for raster operations
 */

void
FillRectangle(dp, dwidth, dheight, bytes_per_line, x, y, width, height)
register uchar *dp;
register int dwidth, dheight, bytes_per_line, x, y, width, height;
{
  if (x >= dwidth || y >= dheight) return;
  if (x < 0) {
    width += x;
    x = 0;
  }
  if (y < 0) {
    height += y;
    y = 0;
  }
  if (x + width > dwidth) width = dwidth - x;
  if (y + height > dheight) height = dheight - y;

  {
    register int i, j;
    register int endx = x + width - 1,
                 start_bytes = x >> 3,
                 end_bytes = endx >> 3,
                 bwidth = end_bytes - start_bytes - 1;
    register uchar start_pattern = 0xff >> (x & 0x7),
                   end_pattern = 0xff << ((endx & 0x7) ^ 0x7),
                   *d0 = (uchar *) dp + (y * bytes_per_line + start_bytes),
                   *d = d0;

    if (bwidth < 0)
      start_pattern &= end_pattern;
    for (j = 0; j++ < height; ) {
      *(d++) |= start_pattern;
      for (i = 0; i++ < bwidth; ) *(d++) |= 0xff;
      if (start_bytes != end_bytes) *d |= end_pattern;
      d = d0 += bytes_per_line;
    }
  }
}

void
DrawRectangle(dp, dwidth, dheight, bytes_per_line, x, y, width, height, border)
register uchar *dp;
register int dwidth, dheight, bytes_per_line, x, y, width, height, border;
{
  FillRectangle(dp, dwidth, dheight, bytes_per_line, x, y,
		width, border);
  FillRectangle(dp, dwidth, dheight, bytes_per_line, x, y + height - border,
		width, border);
  FillRectangle(dp, dwidth, dheight, bytes_per_line, x, y,
	        border, height);
  FillRectangle(dp, dwidth, dheight, bytes_per_line, x + width - border, y,
		border, height);
}

/*
 * get_unsigned:
 *
 */

unsigned int
get_unsigned(fp, n)	/* return n byte quantity from file fd */
register FILE *fp;	/* file pointer	*/
register int n;		/* number of bytes */
{
  register int x;	/* number being constructed */

  for (x = 0; n-- > 0; ) x = (x << 8) | getc(fp);
  return(x);
}

/*
 * get_signed:
 *
 */

int
get_signed(fp, n)   /* return n byte quantity from file fd */
register FILE *fp;  /* file pointer	*/
register int n;	 /* number of bytes */
{
  register int x;

  if ((x = getc(fp)) >= 128) x -= 256;
  while (--n > 0) x = (x << 8) | getc(fp);
  return(x);
}

void
prerror(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
char *fmt;
long arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7;
{
  fprintf(stderr, fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
