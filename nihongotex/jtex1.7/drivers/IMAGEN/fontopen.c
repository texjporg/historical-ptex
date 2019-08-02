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
 * fontopen.c - Open fontfile
 */

/*
 * $Id: fontopen.c,v 1.5 90/03/27 11:01:52 void Exp $
 */

#include <stdio.h>
#include "config.h"
#include "fio.h"

typedef struct {
    int	    fd;
    int     fntnum;
} FILEINFO;

hidden FILEINFO	    FileInfo[MAXFILES];
hidden int	    top = 0;
hidden int	    count = 0;

extern char	    *GetFontByName();

/*
 * FontOpen: Open font file. Open mode is always `read only'
 * because we never write to font file
 */

visible int
FontOpen(fntnum)
int	fntnum;
{
    int    fd;

    /* Is file already opened? */
    if ((fd = CheckFileEntry(fntnum)) != ERR)
	return (fd);
    return (NewFileEntry(fntnum));
}

/*
 * CheckFileEntry: Returns file handle if it has already opened
 */

hidden int
CheckFileEntry(fntnum)
int	fntnum;
{
    register int    i, n;

    for (n = count, i = top; n--;) {
	if (FileInfo[i].fntnum == fntnum)
	    return (FileInfo[i].fd);
	i = (i == MAXFILES - 1)? 0 : i++;
    }
    return (ERR);
}

/*
 * NewFileEntry: Open file and stores file info. to FileInfo[]
 */

hidden int
NewFileEntry(fntnum)
int	fntnum;
{
    register int    fd;
    register int    n;

    if ((fd = open(GetFontByName(fntnum), READMODE)) == ERR)
	return (ERR);
    if (count == MAXFILES) {
	(void) close(FileInfo[top].fd);
	n = top;
	top = (top == MAXFILES - 1)? 0 : top++;
    } else
	n = count++;
    FileInfo[n].fntnum = fntnum;
    FileInfo[n].fd = fd;
    return (fd);
}
