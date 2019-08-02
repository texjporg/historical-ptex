#include	<stdio.h>
#include	"dvi2ricoh.h"
#include	"font.h"

int
pxl_fontdef(fptr, pxlfp)
	register struct fontinfo	*fptr;
	register FILE			*pxlfp;
{
	double			mag;
	register struct chinfo	*chptr;
	register int		i;

	fptr->flags = PXLFMT;
	fseek(pxlfp, -20, 2);
	i = getuint(pxlfp, 4);
	if ((fptr->c != 0) && (i != 0) && (fptr->c != i))
		warning("font(%s): font checksum = %d, dvi checksum = %d",
						fptr->name, fptr->c, i);
	fptr->mag = getuint(pxlfp, 4);
	fptr->design = getuint(pxlfp, 4);

	fseek(pxlfp, getuint(pxlfp, 4) * 4, 0);

	mag = (double)fptr->s / (double)(1<<20);
	fptr->chdir = (struct chinfo *)malloc(sizeof(struct chinfo) * MAXCHAR);
	if (fptr->chdir == NULL)
		fatal("can not allocate memory");
	bzero((char *)fptr->chdir, sizeof(struct chinfo) * MAXCHAR);
	for (i = 0; i < MAXCHAR; i++) {
		chptr = &(fptr->chdir[i]);
		chptr->width = getuint(pxlfp, 2);
		chptr->height = getuint(pxlfp, 2);
		chptr->xoffset= getsint(pxlfp, 2);
		chptr->yoffset = getsint(pxlfp, 2);
		/*
		chptr->flags = 0;
		*/
		chptr->image = (char *)(getuint(pxlfp, 4) * 4);
		chptr->tfmw = getuint(pxlfp, 4) * mag;
	}
}
