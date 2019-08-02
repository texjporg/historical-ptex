#include	<stdio.h>
#include	"defs.h"
#include	"global.h"

struct openfont_list {
    struct font_entry *openfont_entry;	/* font entry	*/
    int use_count;			/* count of "opens"	*/
    int last_use;
} openfont_files[MAXOPEN];	/* list of open FNT file identifiers */

int nopen = 0;			/* number of open FNT files */
int openfont_time = 0;

void
openfontfile(fe)
register struct font_entry *fe;
/***********************************************************************
    The original version of this dvi driver reopened the font file  each
    time the font changed, resulting in an enormous number of relatively
    expensive file  openings.   This version  keeps  a cache  of  up  to
    MAXOPEN open files,  so that when  a font change  is made, the  file
    pointer, fntfp, can  usually be  updated from the  cache.  When  the
    file is not found in  the cache, it must  be opened.  In this  case,
    the next empty slot  in the cache  is assigned, or  if the cache  is
    full, the least used font file is closed and its slot reassigned for
    the new file.  Identification of the least used file is based on the
    counts of the number  of times each file  has been "opened" by  this
    routine.  On return, the file pointer is always repositioned to  the
    beginning of the file.

***********************************************************************/
{
    register int i, least_used, current;
    struct openfont_list *curopen;
    int prio;
    struct font_entry *lu_fe;
    FILE *fntfp;

#ifdef DEBUG
    if (Debug) printf("Open Font file\n");
#endif
    openfont_time++;
    if ((fntfp = fe->openfile) != NO_FILE) {	/* file already open */
	fseek(fntfp, 0, 0);		/* reposition to start of file */
	curopen = fe->openlist;
#ifdef STATS
	if (Stats)
	    fprintf(stderr, "FNT file %s already open.\n", fe->name);
#endif
    } else {
	if (nopen < MAXOPEN)	/* just add it to list */
	    current = nopen++;
	else {			/* list full -- find least used file, */
				/* close it, and reuse slot for new file */
	    least_used = 0;
	    prio = priority(least_used);
	    for (i = 1; i < MAXOPEN; ++i)
		if (prio > priority(i))
		    prio = priority(least_used = i);
	    lu_fe = openfont_files[least_used].openfont_entry;
#ifdef STATS
	    if (Stats)
		fprintf(stderr, "FNT file %s closed.\n", lu_fe->name);
#endif
	    fclose(lu_fe->openfile);
	    lu_fe->openfile = NO_FILE;
	    current = least_used;
	}
	if ((fe->openfile = BINARYOPEN(fe->name, "r")) == NULL) {
	    Fatal("FNT file %s could not be opened", fe->name);
	} else {
#ifdef STATS
	    if (Stats)
		fprintf(stderr, "FNT file %s opened.\n", fe->name);
#endif
	}
	fe->openlist = curopen = &openfont_files[current];
	curopen->openfont_entry = fe;
	curopen->use_count = 0;
    }
    curopen->use_count++;	/* update reference count */
    curopen->last_use = openfont_time;
}

priority(i)
int i;
{
    return (openfont_files[i].use_count -
	    (openfont_time-openfont_files[i].last_use));
}
