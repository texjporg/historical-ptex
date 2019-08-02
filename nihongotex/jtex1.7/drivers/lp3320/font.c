#include	<stdio.h>
#include	"dvi2ricoh.h"
#include	"font.h"
#include	"commands.h"

extern int	resolution;

struct fontinfo	*fonttbl[FONTHASH];
struct dlfont	dlfont;

struct openlist {
	struct openlist	*olnext;
	struct openlist	*olprev;
	struct fontinfo	*olfont;
};

struct openlist	openfonts[MAXOPENFONT];

void
getfontdefs(dvifp)
	register FILE	*dvifp;
{
	register int	k;
	extern void	readfontdef();

	for (;;) {
		switch (k = getuint(dvifp, 1)) {
		case FNT_DEF1:
		case FNT_DEF2:
		case FNT_DEF3:
		case FNT_DEF4:
			k = getuint(dvifp, k - FNT_DEF1 + 1);
			readfontdef(dvifp, k);
			break;
		case POST_POST:
			return;
		default:
			fatal("Bad byte in font defs");
		}
	}
	/* never reached */
}

void
readfontdef(dvifp, k)
	FILE	*dvifp;
	int	k;
{
	register struct fontinfo	*fptr;
	register int			key;
	extern void	loadfont();
	extern double	actualfactor();
#define	SPACE		(double)fptr->s
#define	DEN		(double)fptr->d
#define	MAG		fptr->fontmag
#define	DVIFONT		fptr->n
#define	FONTNAME	fptr->name

	if ((fptr = (struct fontinfo *)malloc(sizeof(*fptr))) == NULL)
		fatal("can not allocate memory");
	bzero((char *)fptr, sizeof(*fptr));

	key = k % FONTHASH;
	fptr->next = fonttbl[key];
	fonttbl[key] = fptr;

	fptr->k = k;
	fptr->c = getsint(dvifp, 4);
	fptr->s = getsint(dvifp, 4);
	fptr->d = getsint(dvifp, 4);
	fptr->a = getuint(dvifp, 1);
	fptr->l = getuint(dvifp, 1);
	fread(DVIFONT, 1, fptr->a + fptr->l, dvifp);

	/*
	 * fptr->fontspace = fptr->s / 6;
	 */
	MAG = actualfactor((int)(SPACE / DEN * 1000.0 + 0.5)) *
	      actualfactor(usermag) *
	      actualfactor(mag) * resolution * 5.0 + 0.5;

	if (verbose >= 2) {
		fprintf(stderr, "font(%d):\n", k);
		fprintf(stderr, "	name	= '%s'\n", DVIFONT);
		fprintf(stderr, "	mag	= %g\n",
						MAG / (resolution * 5.0));
	}

	if (romfile(DVIFONT, MAG, FONTNAME, fptr)) {
		if (verbose >= 2)
			fprintf(stderr, "	use	= ROM\n");
		romfontinit(fontpath, DVIFONT);
		fptr->flags = ROMFONT;
		return;
	}
	if (!findfile(fontpath, "", fptr))
		fatal("no font %s", DVIFONT);
	if (verbose >= 2)
		fprintf(stderr, "	use	= '%s'\n", FONTNAME);
	loadfont(fptr);
}

void
skipfontdef(dvifp, k)
	int	k;
{
	int	n;

	fseek(dvifp, 4 * 3, 1);
	n = getuint(dvifp, 1);
	n += getuint(dvifp, 1);
	fseek(dvifp, n, 1);
}

void
loadfont(fptr)
	struct fontinfo	*fptr;
{
	if (openfont(fptr) == ERR)
		fatal("can not open font file");

	switch (getuint(fptr->fontfp, 4)) {
	case PXLID:
		pxl_fontdef(fptr, fptr->fontfp);
		break;
	case JPXLID:
		fptr->flags = JPXLFMT;
		jfmt_fontdef(fptr, fptr->fontfp);
		break;
	case JPKID:
		fptr->flags = JPKFMT;
		jfmt_fontdef(fptr, fptr->fontfp);
		break;
	default:
		pk_fontdef(fptr, fptr->fontfp);
		break;
	}
}

struct fontinfo *
findfont(key)
	register int	key;
{
	register struct fontinfo	*fptr;

	for (fptr = fonttbl[key % FONTHASH]; fptr != NULL; fptr = fptr->next) {
		if (fptr->k == key)
			return fptr;
	}
	fatal("%d font not found", key);
}

#ifdef	sun
short twice[256] = {
0x0000, 0x0003, 0x000c, 0x000f, 0x0030, 0x0033, 0x003c, 0x003f,
0x00c0, 0x00c3, 0x00cc, 0x00cf, 0x00f0, 0x00f3, 0x00fc, 0x00ff,
0x0300, 0x0303, 0x030c, 0x030f, 0x0330, 0x0333, 0x033c, 0x033f,
0x03c0, 0x03c3, 0x03cc, 0x03cf, 0x03f0, 0x03f3, 0x03fc, 0x03ff,
0x0c00, 0x0c03, 0x0c0c, 0x0c0f, 0x0c30, 0x0c33, 0x0c3c, 0x0c3f,
0x0cc0, 0x0cc3, 0x0ccc, 0x0ccf, 0x0cf0, 0x0cf3, 0x0cfc, 0x0cff,
0x0f00, 0x0f03, 0x0f0c, 0x0f0f, 0x0f30, 0x0f33, 0x0f3c, 0x0f3f,
0x0fc0, 0x0fc3, 0x0fcc, 0x0fcf, 0x0ff0, 0x0ff3, 0x0ffc, 0x0fff,
0x3000, 0x3003, 0x300c, 0x300f, 0x3030, 0x3033, 0x303c, 0x303f,
0x30c0, 0x30c3, 0x30cc, 0x30cf, 0x30f0, 0x30f3, 0x30fc, 0x30ff,
0x3300, 0x3303, 0x330c, 0x330f, 0x3330, 0x3333, 0x333c, 0x333f,
0x33c0, 0x33c3, 0x33cc, 0x33cf, 0x33f0, 0x33f3, 0x33fc, 0x33ff,
0x3c00, 0x3c03, 0x3c0c, 0x3c0f, 0x3c30, 0x3c33, 0x3c3c, 0x3c3f,
0x3cc0, 0x3cc3, 0x3ccc, 0x3ccf, 0x3cf0, 0x3cf3, 0x3cfc, 0x3cff,
0x3f00, 0x3f03, 0x3f0c, 0x3f0f, 0x3f30, 0x3f33, 0x3f3c, 0x3f3f,
0x3fc0, 0x3fc3, 0x3fcc, 0x3fcf, 0x3ff0, 0x3ff3, 0x3ffc, 0x3fff,
0xc000, 0xc003, 0xc00c, 0xc00f, 0xc030, 0xc033, 0xc03c, 0xc03f,
0xc0c0, 0xc0c3, 0xc0cc, 0xc0cf, 0xc0f0, 0xc0f3, 0xc0fc, 0xc0ff,
0xc300, 0xc303, 0xc30c, 0xc30f, 0xc330, 0xc333, 0xc33c, 0xc33f,
0xc3c0, 0xc3c3, 0xc3cc, 0xc3cf, 0xc3f0, 0xc3f3, 0xc3fc, 0xc3ff,
0xcc00, 0xcc03, 0xcc0c, 0xcc0f, 0xcc30, 0xcc33, 0xcc3c, 0xcc3f,
0xccc0, 0xccc3, 0xcccc, 0xcccf, 0xccf0, 0xccf3, 0xccfc, 0xccff,
0xcf00, 0xcf03, 0xcf0c, 0xcf0f, 0xcf30, 0xcf33, 0xcf3c, 0xcf3f,
0xcfc0, 0xcfc3, 0xcfcc, 0xcfcf, 0xcff0, 0xcff3, 0xcffc, 0xcfff,
0xf000, 0xf003, 0xf00c, 0xf00f, 0xf030, 0xf033, 0xf03c, 0xf03f,
0xf0c0, 0xf0c3, 0xf0cc, 0xf0cf, 0xf0f0, 0xf0f3, 0xf0fc, 0xf0ff,
0xf300, 0xf303, 0xf30c, 0xf30f, 0xf330, 0xf333, 0xf33c, 0xf33f,
0xf3c0, 0xf3c3, 0xf3cc, 0xf3cf, 0xf3f0, 0xf3f3, 0xf3fc, 0xf3ff,
0xfc00, 0xfc03, 0xfc0c, 0xfc0f, 0xfc30, 0xfc33, 0xfc3c, 0xfc3f,
0xfcc0, 0xfcc3, 0xfccc, 0xfccf, 0xfcf0, 0xfcf3, 0xfcfc, 0xfcff,
0xff00, 0xff03, 0xff0c, 0xff0f, 0xff30, 0xff33, 0xff3c, 0xff3f,
0xffc0, 0xffc3, 0xffcc, 0xffcf, 0xfff0, 0xfff3, 0xfffc, 0xffff
};
#endif	sun
#ifdef	vax
short twice[256] = {
0x0000, 0x0300, 0x0c00, 0x0f00, 0x3000, 0x3300, 0x3c00, 0x3f00,
0xc000, 0xc300, 0xcc00, 0xcf00, 0xf000, 0xf300, 0xfc00, 0xff00,
0x0003, 0x0303, 0x0c03, 0x0f03, 0x3003, 0x3303, 0x3c03, 0x3f03,
0xc003, 0xc303, 0xcc03, 0xcf03, 0xf003, 0xf303, 0xfc03, 0xff03,
0x000c, 0x030c, 0x0c0c, 0x0f0c, 0x300c, 0x330c, 0x3c0c, 0x3f0c,
0xc00c, 0xc30c, 0xcc0c, 0xcf0c, 0xf00c, 0xf30c, 0xfc0c, 0xff0c,
0x000f, 0x030f, 0x0c0f, 0x0f0f, 0x300f, 0x330f, 0x3c0f, 0x3f0f,
0xc00f, 0xc30f, 0xcc0f, 0xcf0f, 0xf00f, 0xf30f, 0xfc0f, 0xff0f,
0x0030, 0x0330, 0x0c30, 0x0f30, 0x3030, 0x3330, 0x3c30, 0x3f30,
0xc030, 0xc330, 0xcc30, 0xcf30, 0xf030, 0xf330, 0xfc30, 0xff30,
0x0033, 0x0333, 0x0c33, 0x0f33, 0x3033, 0x3333, 0x3c33, 0x3f33,
0xc033, 0xc333, 0xcc33, 0xcf33, 0xf033, 0xf333, 0xfc33, 0xff33,
0x003c, 0x033c, 0x0c3c, 0x0f3c, 0x303c, 0x333c, 0x3c3c, 0x3f3c,
0xc03c, 0xc33c, 0xcc3c, 0xcf3c, 0xf03c, 0xf33c, 0xfc3c, 0xff3c,
0x003f, 0x033f, 0x0c3f, 0x0f3f, 0x303f, 0x333f, 0x3c3f, 0x3f3f,
0xc03f, 0xc33f, 0xcc3f, 0xcf3f, 0xf03f, 0xf33f, 0xfc3f, 0xff3f,
0x00c0, 0x03c0, 0x0cc0, 0x0fc0, 0x30c0, 0x33c0, 0x3cc0, 0x3fc0,
0xc0c0, 0xc3c0, 0xccc0, 0xcfc0, 0xf0c0, 0xf3c0, 0xfcc0, 0xffc0,
0x00c3, 0x03c3, 0x0cc3, 0x0fc3, 0x30c3, 0x33c3, 0x3cc3, 0x3fc3,
0xc0c3, 0xc3c3, 0xccc3, 0xcfc3, 0xf0c3, 0xf3c3, 0xfcc3, 0xffc3,
0x00cc, 0x03cc, 0x0ccc, 0x0fcc, 0x30cc, 0x33cc, 0x3ccc, 0x3fcc,
0xc0cc, 0xc3cc, 0xcccc, 0xcfcc, 0xf0cc, 0xf3cc, 0xfccc, 0xffcc,
0x00cf, 0x03cf, 0x0ccf, 0x0fcf, 0x30cf, 0x33cf, 0x3ccf, 0x3fcf,
0xc0cf, 0xc3cf, 0xcccf, 0xcfcf, 0xf0cf, 0xf3cf, 0xfccf, 0xffcf,
0x00f0, 0x03f0, 0x0cf0, 0x0ff0, 0x30f0, 0x33f0, 0x3cf0, 0x3ff0,
0xc0f0, 0xc3f0, 0xccf0, 0xcff0, 0xf0f0, 0xf3f0, 0xfcf0, 0xfff0,
0x00f3, 0x03f3, 0x0cf3, 0x0ff3, 0x30f3, 0x33f3, 0x3cf3, 0x3ff3,
0xc0f3, 0xc3f3, 0xccf3, 0xcff3, 0xf0f3, 0xf3f3, 0xfcf3, 0xfff3,
0x00fc, 0x03fc, 0x0cfc, 0x0ffc, 0x30fc, 0x33fc, 0x3cfc, 0x3ffc,
0xc0fc, 0xc3fc, 0xccfc, 0xcffc, 0xf0fc, 0xf3fc, 0xfcfc, 0xfffc,
0x00ff, 0x03ff, 0x0cff, 0x0fff, 0x30ff, 0x33ff, 0x3cff, 0x3fff,
0xc0ff, 0xc3ff, 0xccff, 0xcfff, 0xf0ff, 0xf3ff, 0xfcff, 0xffff,
};
#endif	vax
#if	!(sun || vax)
This array depends on byte order.
Please check your machine's byte order.
#endif	!(sun || vax)

void
magnifychar(cptr, mag)
	register struct chinfo	*cptr;
	int			mag;
{
	int			dstw, dstsw, dstbyte, srcbw;
	register int		k, j, i;
	char			*dst;
	register unsigned char	*sptr;
	register short		*dptr;
	short			*pdptr;

	switch (mag) {
	case 2:
		dstw = ((cptr->width * 2 + 31) >> 5) * sizeof(int);
		dstsw = dstw / 2;
		dstbyte = dstw * (cptr->height * 2);
		if ((dst = (char *)malloc(dstbyte)) == NULL)
			fatal("Unable to allocate memory for character");

		srcbw = ((cptr->width + 31) >> 5) * sizeof(int);

		dptr = (short *)dst;
		sptr = (unsigned char *)cptr->image;
		for (i = 0; i < cptr->height; i++) {
			k = dstsw;
			pdptr = dptr;
			for (j = 0; j < srcbw; j++, sptr++) {
				if (--k < 0)
					continue;
				*dptr++ = twice[*sptr];
			}
			bcopy((char *)pdptr, (char *)(pdptr + dstsw), dstw);
			dptr = pdptr + dstsw * 2;
		}

		cptr->width *= 2;
		cptr->height *= 2;
		cptr->xoffset *= 2;
		cptr->yoffset *= 2;
		/* cptr->tfmw *= 2; */
		free(cptr->image);
		cptr->image = dst;
		break;
	}
}

struct chinfo *
loadimage(fptr, cptr)
	register struct fontinfo	*fptr;
	register struct chinfo		*cptr;
{
	register int	i;

	if (isloaded(cptr->flags)) {
#if DEBUG > 1
		if (cptr->image != NULL)
			showfont(cptr, stderr);
#endif
		return cptr;
	}
	if (cptr->image == NULL) {
		cptr->flags |= LOADED;
		return cptr;
	}

	if (openfont(fptr) == ERR)
		fatal("can not open font file");
	fseek(fptr->fontfp, (int)cptr->image, 0);
	if (ispkfmt(fptr->flags))
		pk_translate(fptr, cptr);
	else {
		i = ((cptr->width + 31) >> 5) * cptr->height;
		if ((cptr->image = (char *)malloc(i * sizeof(int))) == NULL)
			fatal("Unable to allocate memory for character");
		fread(cptr->image, sizeof(int), i, fptr->fontfp);
	}
	cptr->flags |= LOADED;
	magnifychar(cptr, fptr->bitmag);
#if DEBUG > 1
	showfont(cptr, stderr);
#endif
	return cptr;
}

void
loadallimage(dosort, limit)
	int	dosort;
	int	limit;
{
	register int			i, ch, j, first, max;
	register struct fontinfo	*fptr;
	register struct chinfo		*cptr;
	extern void			closeallfont();
	double				compress;
	struct dlfont			*dlptr, *newdl;
#define iskana(x)	( \
			 ((x) >= SYMBOLKU * MAXKU && \
			  (x) < (SYMBOLKU + 1) * MAXKU) || \
			 ((x) >= HIRAKANAKU * MAXKU && \
			  (x) < (HIRAKANAKU + 1) * MAXKU) || \
			 ((x) >= KATAKANAKU * MAXKU && \
			  (x) < (KATAKANAKU + 1) * MAXKU))

	i = FONTHASH - 1;
	do {
		for (fptr = fonttbl[i]; fptr != NULL; fptr = fptr->next) {
			if (isromfont(fptr->flags))
				continue;
			fptr->tbyte = 0;
			fptr->dlbyte = 0;
			if (isjfmt(fptr->flags))
				max = MAXKU * MAXTEN;
			else
				max = MAXCHAR;
			for (ch = 0, cptr = &fptr->chdir[0];
						ch < max; ch++, cptr++) {
				if (cptr->count == 0)
					continue;
				if (loadimage(fptr, cptr) == NULL)
					fatal("character not found");
				if (cptr->count < limit)
					continue;
				if (isjfmt(fptr->flags) && !iskana(ch))
					continue;
				j = (cptr->width + 15) / 16 * 2 * cptr->height;
				fptr->tbyte += j * cptr->count;
				fptr->dlbyte += j;
			}
		}
	} while (--i >= 0);
	closeallfont();

	if (!dosort)
		return;

	dlfont.next = &dlfont;
	dlfont.prev = &dlfont;
	i = FONTHASH - 1;
	do {
		for (fptr = fonttbl[i]; fptr != NULL; fptr = fptr->next) {
			/*
			if (isromfont(fptr->flags))
				continue;
			*/
			if (fptr->dlbyte == 0)
				continue;
			compress = (double)fptr->tbyte / fptr->dlbyte;
			for (dlptr = dlfont.next;
				dlptr != &dlfont; dlptr = dlptr->next) {
				if (dlptr->compress < compress)
					break;
			}
			newdl = (struct dlfont *)malloc(sizeof(struct dlfont));
			if (newdl == NULL)
				fatal("Unable to allocate memory for dowload");
			fptr->dlfont = newdl;
			newdl->font = fptr;
			newdl->compress = compress;
			newdl->next = dlptr;
			newdl->prev = dlptr->prev;
			dlptr->prev->next = newdl;
			dlptr->prev = newdl;
		}
	} while (--i >= 0);
}

void
closeallfont()
{
	register int	i;

	openfonts[0].olprev = &openfonts[MAXOPENFONT - 1];
	openfonts[0].olnext = &openfonts[1];
	for (i = 1; i < MAXOPENFONT - 1; i++) {
		openfonts[i].olprev = &openfonts[i - 1];
		openfonts[i].olnext = &openfonts[i + 1];
		if (openfonts[i].olfont != NULL) {
			fclose(openfonts[i].olfont->fontfp);
			openfonts[i].olfont->fontfp = NULL;
			openfonts[i].olfont = NULL;
		}
	}
	openfonts[MAXOPENFONT - 1].olprev = &openfonts[MAXOPENFONT - 2];
	openfonts[MAXOPENFONT - 1].olnext = &openfonts[0];
	if (openfonts[MAXOPENFONT - 1].olfont != NULL) {
		fclose(openfonts[MAXOPENFONT - 1].olfont->fontfp);
		openfonts[MAXOPENFONT - 1].olfont->fontfp = NULL;
		openfonts[MAXOPENFONT - 1].olfont = NULL;
	}
}

int
openfont(fptr)
	register struct fontinfo	*fptr;
{
	register struct openlist	*olptr;

	if (fptr->fontfp != NULL)
		return OK;
	olptr = openfonts[0].olnext;
	if (olptr->olfont != NULL) {
		fclose(olptr->olfont->fontfp);
		olptr->olfont->fontfp = NULL;
	}
	if ((fptr->fontfp = fopen(fptr->name, "r")) == NULL)
		return ERR;
	olptr->olfont = fptr;
	/*
	openfonts[0].olnext = olptr->olnext;
	olptr->olnext->olprev = &openfonts[0];
	*/
	olptr->olprev->olnext = olptr->olnext;
	olptr->olnext->olprev = olptr->olprev;
	olptr->olnext = &openfonts[0];
	olptr->olprev = openfonts[0].olprev;
	openfonts[0].olprev->olnext = olptr;
	openfonts[0].olprev = olptr;
	return OK;
}

#ifdef DEBUG
showfont(cptr, fp)
	struct chinfo	*cptr;
	FILE		*fp;
{
	register int	i, j, bw, h;
	register int	*bp;
	extern int	pk_power[];
#define	WORDSIZE	sizeof(int)
#define	BOW		(sizeof(int) * 8)

	fprintf(fp,
	  "\nwidth=%d, height=%d, xoff=%d, yoff=%d, tfmw=%d, advance=%d\n",
	  cptr->width, cptr->height, cptr->xoffset, cptr->yoffset, cptr->tfmw,
	  calcpixel(cptr->tfmw, factor));
	if (!isloaded(cptr->flags))
		return;
	bw = (cptr->width + BOW - 1) / BOW;
	h = cptr->height;
	bp = (int *)cptr->image;
	while (--h >= 0) {
		j = bw;
		while (--j >= 0) {
			i = BOW;
			while  (--i >= 0) {
				if (*bp & pk_power[i])
					putc('*', fp);
				else
					putc(' ', fp);
			}
			bp++;
		}
		putc('\n', fp);
	}

#undef WORDSIZE
#undef BOW
}
#endif
