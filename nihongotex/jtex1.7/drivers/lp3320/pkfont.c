#include	<stdio.h>
#include	"dvi2ricoh.h"
#include	"font.h"

#define	WORD		int
#define	WORDSIZE	sizeof(WORD)

#define	PK_COMMAND	240
#define	PK_XXX1		240
#define	PK_XXX2		241
#define	PK_XXX3		242
#define	PK_XXX4		243
#define PK_YYY		244
#define PK_POST		245
#define PK_NO_OP	246
#define	PK_PRE		247

#define	PK_ID		89

#define	pk_roundup(x)	(int)((x) + 0.5)

int	pk_flag;
int	pk_dyn_f;
int	pk_rcount;

pk_fontdef(fptr, fp)
	register struct fontinfo	*fptr;
	register FILE			*fp;
{
	register int	i;

	fseek(fp, 0, 0);
	if ((i = getuint(fp, 1)) != PK_PRE)
		fatal("The first byte(%d) of pk_file(%s) must be %d",
						i, fptr->name, PK_PRE);
	if ((i = getuint(fp, 1)) != PK_ID)
		fatal("pk_file(%s): invalid ID %d", fptr->name, i);
	/*
	 * skip comments
	 */
	i = getuint(fp, 1);
	fseek(fp, i, 1);
	fptr->design = getuint(fp, 4);
	fseek(fp, 4, 1);	/* checksum */
	fptr->mag = getuint(fp, 4) * 72.27 * 5 / 65536 + 0.5;
	fseek(fp, 4, 1);	/* vppp */
	fptr->flags = PKFMT;

	pk_chardef(fptr, fp);
}

pk_chardef(fptr, fp)
	register struct fontinfo	*fptr;
	register FILE			*fp;
{
	register int		i, ch, fmt, offset;
	register struct chinfo	*cptr;

	pk_specials(fptr, fp);
	fptr->chdir = (struct chinfo *)malloc(sizeof(struct chinfo) * MAXCHAR);
	if (fptr->chdir == NULL)
		fatal("can not allocate memory");
	bzero((char *)fptr->chdir, sizeof(struct chinfo) * MAXCHAR);

	while (pk_flag != PK_POST) {
		offset = ftell(fp) - 1;
		fmt = pk_flag & 0x7;
		if (fmt == 7) {
			i = getuint(fp, 4);
			ch = getuint(fp, 4);
		} else if (fmt > 3) {
			i = ((pk_flag & 0x3) << 16) + getuint(fp, 2);
			ch = getuint(fp, 1);
		} else {
			i = ((pk_flag & 0x3) << 8) + getuint(fp, 1);
			ch = getuint(fp, 1);
		}
		if (ch > MAXCHAR)
			fatal("character code %x is out of range in %s",
							ch, fptr->name);
		cptr = &fptr->chdir[ch];
		if (cptr->image != NULL)
			fatal("%x is redefined in pk file(%s)", ch, fptr->name);
		cptr->flags = pk_flag;
		cptr->image = (char *)offset;
		fseek(fp, i, 1);

		pk_specials(fptr, fp);
	}
}

pk_specials(fptr, fp)
	register struct fontinfo	*fptr;
	register FILE			*fp;
{
	register int	i, j;

	while ((pk_flag = getuint(fp, 1)) >= PK_COMMAND) {
		switch (pk_flag) {
		case PK_XXX1: 
		case PK_XXX2: 
		case PK_XXX3: 
		case PK_XXX4:
			i = getuint(fp, pk_flag - PK_XXX1 + 1);
			fseek(fp, i, 1);
			break;
		case PK_YYY:
			fseek(fp, 4, 1);
			break;
		case PK_POST:
			return;
		case PK_NO_OP:
			break;
		default:
			fatal("unexpected command(%d) in pk_file(%s)",
							pk_flag, fptr->name);
		}
	}
}

/*
 * expand the pk_format to bitmap
 */
pk_translate(fptr, cptr)
	register struct fontinfo	*fptr;
	register struct chinfo		*cptr;
{
	register int		i;
	register FILE		*fp;

	fp = fptr->fontfp;
	cptr->flags = getuint(fp, 1);
	pk_dyn_f = (cptr->flags >> 4) & 0xf;
	i = cptr->flags & 0x7;
	if (i == 7) {
		fseek(fp, 4, 1);
		cptr->tfmw = getuint(fp, 4) * (double)fptr->s / (double)(1<<20);
		fseek(fp, 8, 1);
		/* horizontal escape */
		/* vertical escape */
		cptr->width = getuint(fp, 4);
		cptr->height = getuint(fp, 4);
		cptr->xoffset = getsint(fp, 4);
		cptr->yoffset = getsint(fp, 4);
	} else if (i > 3) {
		fseek(fp, 2, 1);
		cptr->tfmw = getuint(fp, 3) * (double)fptr->s / (double)(1<<20);
		fseek(fp, 2, 1);	/* horizontal escape */
		cptr->width = getuint(fp, 2);
		cptr->height = getuint(fp, 2);
		cptr->xoffset = getsint(fp, 2);
		cptr->yoffset = getsint(fp, 2);
	} else {
		fseek(fp, 2, 1);
		cptr->tfmw = getuint(fp, 3) * (double)fptr->s / (double)(1<<20);
		fseek(fp, 1, 1); /* horzontal escapement */
		cptr->width = getuint(fp, 1);
		cptr->height = getuint(fp, 1);
		cptr->xoffset = getsint(fp, 1);
		cptr->yoffset = getsint(fp, 1);
	}

	i = ((cptr->width + 31) >> 5) * cptr->height;
	if ((cptr->image = (char *)malloc(i * sizeof(int))) == NULL)
		fatal("Unable to allocate memory for character");

	if (pk_dyn_f == 14)
		pk_bitmap(cptr, (WORD *)cptr->image, fp);
	else
		pk_pkchar(cptr, (WORD *)cptr->image, fp);
}

#ifdef vax
int	pk_power[] = {
	0x01000000, 0x02000000, 0x04000000, 0x08000000,
	0x10000000, 0x20000000, 0x40000000, 0x80000000,
	0x00010000, 0x00020000, 0x00040000, 0x00080000,
	0x00100000, 0x00200000, 0x00400000, 0x00800000,
	0x00000100, 0x00000200, 0x00000400, 0x00000800,
	0x00001000, 0x00002000, 0x00004000, 0x00008000,
	0x00000001, 0x00000002, 0x00000004, 0x00000008,
	0x00000010, 0x00000020, 0x00000040, 0x00000080
};

int	pk_gpower[] = {
	0x00000000,
	0x01000000, 0x03000000, 0x07000000, 0x0f000000,
	0x1f000000, 0x3f000000, 0x7f000000, 0xff000000,
	0xff010000, 0xff030000, 0xff070000, 0xff0f0000,
	0xff1f0000, 0xff3f0000, 0xff7f0000, 0xffff0000,
	0xffff0100, 0xffff0300, 0xffff0700, 0xffff0f00,
	0xffff1f00, 0xffff3f00, 0xffff7f00, 0xffffff00,
	0xffffff01, 0xffffff03, 0xffffff07, 0xffffff0f,
	0xffffff1f, 0xffffff3f, 0xffffff7f, 0xffffffff
};
#else
int	pk_power[] = {
	0x00000001, 0x00000002, 0x00000004, 0x00000008,
	0x00000010, 0x00000020, 0x00000040, 0x00000080,
	0x00000100, 0x00000200, 0x00000400, 0x00000800,
	0x00001000, 0x00002000, 0x00004000, 0x00008000,
	0x00010000, 0x00020000, 0x00040000, 0x00080000,
	0x00100000, 0x00200000, 0x00400000, 0x00800000,
	0x01000000, 0x02000000, 0x04000000, 0x08000000,
	0x10000000, 0x20000000, 0x40000000, 0x80000000
};

int	pk_gpower[] = {
	0x00000000,
	0x00000001, 0x00000003, 0x00000007, 0x0000000f,
	0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff,
	0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,
	0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,
	0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff,
	0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
	0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff,
	0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
};
#endif

pk_bitmap(cptr, bp, fp)
	register struct chinfo	*cptr;
	register WORD		*bp;
	register FILE		*fp;
{
	register int	image, shift, j, i;
#define	BOBP	(WORDSIZE * 8 - 1)

	pk_init_bit();
	i = cptr->height;
	while (--i >= 0) {
		image = 0;
		shift = BOBP;
		j = cptr->width;
		while (--j >= 0) {
			if (pk_bit(fp))
				image |= pk_power[shift];
			if (--shift == -1) {
				*bp++ = image;
				image = 0;
				shift = BOBP;
			}
		}
		if (shift < BOBP)
			*bp++ = image;
	}
#undef BOBP
}

pk_pkchar(cptr, bp, fp)
	register struct chinfo	*cptr;
	register WORD		*bp;
	register FILE		*fp;
{
	register int	on, count, h_bit, word, word_weight, rows_left, i;
	register WORD	*pbp;
#define	BOBP	(WORDSIZE * 8)

	on = pkflag(cptr->flags) & 0x8;
	rows_left = cptr->height;
	h_bit = cptr->width;
	pk_rcount = 0;
	word_weight = BOBP;
	word = 0;
	pbp = bp;

	pk_init_nyb();
	while (rows_left > 0) {
		count = pk_packed_num(fp);
		while (count > 0) {
			if (count < word_weight && count < h_bit) {
				if (on)
					word |= pk_gpower[word_weight] -
					    pk_gpower[word_weight - count];
				h_bit -= count;
				word_weight -= count;
				count = 0;
			} else if (count >= h_bit && h_bit <= word_weight) {
				if (on)
					word |= pk_gpower[word_weight] -
					    pk_gpower[word_weight - h_bit];
				*bp++ = word;
				rows_left -= pk_rcount + 1;
				i = bp - pbp;
				while (--pk_rcount >= 0) {
					bcopy((char *)pbp, (char *)bp,
					    i * WORDSIZE);
					bp += i;
				}
				pk_rcount = 0;
				pbp = bp;
				word = 0;
				word_weight = BOBP;
				count -= h_bit;
				h_bit = cptr->width;
			} else {
				if (on)
					word |= pk_gpower[word_weight];
				*bp++ = word;
				word = 0;
				count -= word_weight;
				h_bit -= word_weight;
				word_weight = BOBP;
			}
		}
		on = !on;
	}
#undef BOBP
}

int
pk_packed_num(fp)
	register FILE	*fp;
{
	register int	i, j;

tryagain:
	switch (i = pk_nyb(fp)) {
	case 0:
		do {
			i++;
		} while ((j = pk_nyb(fp)) == 0);
		while (--i >= 0)
			j = (j << 4) + pk_nyb(fp);
		return j - 15 + (13 - pk_dyn_f)*16 + pk_dyn_f;
	case 14:
		pk_rcount = pk_packed_num(fp);
		goto tryagain;
	case 15:
		pk_rcount = 1;
		goto tryagain;
	}
	if (i <= pk_dyn_f)
		return i;
	return (i - pk_dyn_f - 1)*16 + pk_nyb(fp) + pk_dyn_f + 1;
}

static int	pk_nyb_count;
static int	pk_bit_count;

int
pk_init_nyb()
{
	pk_nyb_count = 0;
}

int
pk_nyb(fp)
	register FILE	*fp;
{
	static int	save;

	if (pk_nyb_count == 1) {
		pk_nyb_count = 0;
		return save & 0xf;
	}
	save = getc(fp);
	pk_nyb_count = 1;
	return (save >> 4) & 0xf;
}

int
pk_init_bit()
{
	pk_bit_count = 0;
}

int
pk_bit(fp)
	register FILE	*fp;
{
	static int	save;

	if (--pk_bit_count == -1) {
		save = getc(fp);
		pk_bit_count = 7;
	}
	return (save >> pk_bit_count) & 1;
}
