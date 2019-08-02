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
 * abspath.c - Make absolute path name
 */

/*
 * $Id: abspath.c,v 1.6 90/03/27 10:51:19 void Exp $
 */

#include <stdio.h>
#include "config.h"

#define READ_OK		4

#if MSDOS
#define DELIMITER	';'			/* Don't specify ':' */
#define IsPathChar(c)	((c) == '/' || (c) == '\\')
#else
#define DELIMITER	':'
#define IsPathChar(c)	((c) == '/')
#endif

hidden char	    **path = (char **) NULL;

extern char	    *StrDup();
extern char	    *getenv(), *malloc(), *realloc();
extern char	    *strcpy(), *strcat();
extern char	    *rindex(), *index();

/*
 * Make absolute path name
 */

visible char *
AbsPath(name)
char	*name;
{
    register int    i;
    char	    buf[MAXPATHLEN];

    if (path == (char **) NULL)
	InitPath();

    for (i = 0; path[i] != NULL; i++) {
	(void) strcat(strcpy(buf, path[i]), name);
	if (access(buf, READ_OK) == SUCCESS)
	    return (StrDup(buf));
    }
    return (NULL);
}

/*
 * Initialize path table
 */

hidden
InitPath()
{
    register char   *env = getenv(FONTDIRENV);
    register char   *p, *lp;

    if (env == NULL)		/* Use default path */
	StorePath(DEFAULTFONTDIR);
    else {
	env = StrDup(env);
	for (lp = env; (p = index(lp, DELIMITER)) != NULL; lp = p) {
	    *p++ = EOS;
	    StorePath(lp);
	}
	StorePath(lp);
	(void) free(env);
    }
}

/*
 * Store path to table
 */

hidden
StorePath(p)
char	*p;
{
    char	buf[MAXPATHLEN];
    static int	n = 0;
    char	**Malloc();

    if (*p != EOS) {
	AddSlash(strcpy(buf, p));
	path = Malloc(path, (unsigned int) (n + 2));
	path[n++] = StrDup(buf);
	path[n] = NULL;
    }
}

/*
 * Add slash to the end of path
 */

hidden
AddSlash(p)
register char	*p;
{
    register int    len = strlen(p);

    if (len != 0 && !IsPathChar(p[len - 1])) {
#if MSDOS
	if (len == 2 && p[1] == ':')	/* In case of "A:" */
	    return;
#endif
	p[len++] = '/';
	p[len] = EOS;
    }
}

hidden char **
Malloc(p, size)
char		**p;
unsigned int	size;
{
    size *= sizeof(char *);
    if (p == (char **) NULL)
	return ((char **) malloc(size));
    else
	return ((char **) realloc((char *) p, size));
}
