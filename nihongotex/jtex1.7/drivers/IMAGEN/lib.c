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
 * lib.c - miscellanious funcitons & subroutines library.
 */

/*
 * $Id: lib.c,v 1.11 90/03/27 11:02:41 void Exp $
 */

#include    <stdio.h>
#include    "config.h"

extern char *malloc();
extern char *strcpy();

visible char *
GetString(fp, n)
register FILE	*fp;
register int	n;
{
    register char   *point;

    if ((point = malloc((unsigned int)(n + 1))) == NULL)
	cant_alloc("string");	/* exit */

    (void) fread(point, 1, n, fp);
    point[n] = EOS;
    return (point);
}

visible
PutMsg(s)
char **s;
{
    while (*s)
	(void) fputs(*s++, stderr);

#ifdef lint
    (void) fputc('\n', stderr);
#else
    (void) putc('\n', stderr);
#endif
}

visible int
CheckSuffix(filename, suffix)
char *filename;
char *suffix;
{
    register char   *point;
    char     *rindex();

    if ((point = rindex(filename, '.')) == NULL)
	return (NO);
    else if (strncmp(point, suffix, strlen(suffix) + 1))
	return (NO);
	
    return (YES);
}

visible char *
StrDup(string)
register char	*string;
{
    register int    length;
    register char   *point;

    /*
     * We must check NULL to prevent core dump.
     */
    if (string == NULL)
	return (NULL);

    length = strlen(string) + 1;

    if ((point = malloc((unsigned int) length)) == NULL)
	return (NULL);

    return (strcpy(point, string));
}

visible char *
index(p, c)
register char	*p;
register int	c;
{
    do {
	if (CharMask(*p) == c)
	    return (p);
    } while (*p++);
    return (NULL);
}

visible char *
rindex(p, c)
register char	*p;
register int	c;
{
    register char *rvalue;

    rvalue = NULL;

    do {
	if (CharMask(*p) == c)
	    rvalue = p;
    } while (*p++);
    return (rvalue);
}


visible void
error(msg)
char	*msg;
{
	(void)fprintf(stderr, "! %s.\n", msg);
	exit(1);
}


visible void
cant_alloc(name)
char	*name;
{
	(void)fprintf(stderr, "! Can't allocate memory for %s.\n", name);
	exit(1);
}


visible void
cant_open(name)
char	*name;
{
	(void)fprintf(stderr, "! Can't open file ``%s''.\n", name);
	exit(1);
}

visible char *
eatblk(s)
char *s;
{
    while (*s == ' ' || *s == '\t')
	++s;
    return (s);
}


#ifndef	BYTESWAP
visible void
read_4byte(fd, p)
int	fd;
Sig4Byte	*p;
{
	char	buf[1];

	(void)read(fd, buf, 1);
	*p = (buf[1] & 0xff);
	(void)read(fd, buf, 1);
	*p <<= 8;
	*p |= (buf[1] & 0xff);
	(void)read(fd, buf, 1);
	*p <<= 8;
	*p |= (buf[1] & 0xff);
	(void)read(fd, buf, 1);
	*p <<= 8;
	*p |= (buf[1] & 0xff);
}
#endif


#ifndef	BYTESWAP
visible void
read_2byte(fd, p)
int	fd;
Sig4Byte	*p;
{
	char	buf[1];

	(void)read(fd, buf, 1);
	*p = (buf[1] & 0xff);
	(void)read(fd, buf, 1);
	*p <<= 8;
	*p |= (buf[1] & 0xff);
}
#endif
