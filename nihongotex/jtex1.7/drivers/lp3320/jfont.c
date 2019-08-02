#include	<stdio.h>
#include	"dvi2ricoh.h"
#include	"font.h"

int
jfmt_fontdef(fptr, pxlfp)
	register struct fontinfo	*fptr;
	register FILE			*pxlfp;
{
	register struct chinfo	*chptr;
	register int		i;
	double			mag;
	struct chinfo		kanji;

	fseek(pxlfp, -20, 2);
	i = getuint(pxlfp, 4);
	if ((fptr->c != 0) && (i != 0) && (fptr->c != i))
		warning("font(%s): font checksum = %d, dvi checksum = %d",
						fptr->name, fptr->c, i);
	fptr->mag = getuint(pxlfp, 4);
	fptr->design = getuint(pxlfp, 4);

	fseek(pxlfp, getuint(pxlfp, 4), 0);

	mag = (double)fptr->s / (double)(1<<20);
	fptr->chdir = (struct chinfo *)malloc(sizeof(struct chinfo) * MAXKHAR);
	if (fptr->chdir == NULL)
		fatal("can not allocate memory");
	bzero((char *)fptr->chdir, sizeof(struct chinfo) * MAXKHAR);
	for (i = 0, chptr = &fptr->chdir[0];
				i < MAXSYMKU * MAXTEN; i++, chptr++) {
		chptr->width = getuint(pxlfp, 2);
		chptr->height = getuint(pxlfp, 2);
		chptr->xoffset= getsint(pxlfp, 2);
		chptr->yoffset = getsint(pxlfp, 2);
		chptr->image = (char *)getuint(pxlfp, 4);
		chptr->tfmw = getuint(pxlfp, 4) * mag;
	}
	kanji.flags = 0;
	kanji.count = 0;
	kanji.width = getuint(pxlfp, 2);
	kanji.height = getuint(pxlfp, 2);
	kanji.xoffset= getsint(pxlfp, 2);
	kanji.yoffset = getsint(pxlfp, 2);
	kanji.image = (char *)getuint(pxlfp, 4);
	kanji.tfmw = getuint(pxlfp, 4) * mag;
	for (i = 0, chptr = &fptr->chdir[MINKANJIKU * MAXTEN];
				i < MAXKANJI; i++, chptr++) {
		*chptr = kanji;
		chptr->image = (char *)getuint(pxlfp, 4);
	}
}
