/*
 * Fopenp function.
 *
 * Neil Hunt (Neil%Teleos.com@ai.sri.com)
 *
 * Copyright (c) 1989 Teleos Research, Inc 1989.
 * Copyright (c) 1988 Schlumberger Technologies, Inc 1988.
 *
 * Anyone can use this software in any manner they choose,
 * including modification and redistribution, provided they make
 * no charge for it, and these conditions remain unchanged.
 *
 * This program is distributed as is, with all faults (if any), and
 * without any warranty. No author or distributor accepts responsibility
 * to anyone for the consequences of using it, or for whether it serves any
 * particular purpose at all, or any other reason.
 *
 * $Log:	fopenp.c,v $
 * Revision 1.1  89/02/10  18:40:39  neil
 * Initial revision
 * 
 * Copied from newlib.
 * Revision 1.2  88/09/19  18:29:53  hunt
 * Fixed typo.
 * 
 * Revision 1.1  88/09/19  15:52:47  hunt
 * Initial revision
 */

static char rcsid[] = "$Revision: 1.1 $";

#include <stdio.h>
#include "std.h"

FILE *
fopenp(path, name, fullname, mode)
char *path;
char *name;
char *fullname;
char *mode;
{
	register char *p;
	register FILE *f;

	if(*name == '/')
	{
		strcpy(fullname, name);
		return fopen(fullname, mode);
	}

	while(*path)
	{
		/*
		 * Copy first/next path prefix to fullname.
		 * Skip over the ':'.
		 * Add the '/'.
		 * Concat the filename.
		 */
		for(p = fullname; *path != '\0'; )
		{
			if(*path == ':' || *path == ';')
			{
				path++;
				break;
			}
			else
				*p++ = *path++;
		}
		*p++ = '/';
		strcpy(p, name);

		/*
		 * Try to open the file.
		 */
		if((f = fopen(fullname, mode)) != (FILE *)NULL)
			return f;
	}

	return (FILE *)NULL;
}
