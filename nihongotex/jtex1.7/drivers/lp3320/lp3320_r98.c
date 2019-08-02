#include	<stdio.h>
#include	<signal.h>
#include	<ctype.h>
#include	"dvi2ricoh.h"
#include	"commands.h"
#include	"font.h"

#define	RESOLUTION	240
#define	UNIT		720
#ifndef	R98
#define dottounit(x)	((x) * (UNIT / RESOLUTION))
#else	R98
#ifdef	MODE160DPI
#define dottounith(x)	((x) * 160 / RESOLUTION )
#define dottounitv(x)	((x) * 120 / RESOLUTION )
#else	MODE160DPI
#define dottounith(x)	((x) * 240 / RESOLUTION )
#define dottounitv(x)	((x) * 240 / RESOLUTION )
#endif	MODE160DPI
#define	dottodot(x)	(x)
#endif	R98
int	resolution = RESOLUTION;
char	*defaultpath = FONTAREA;
			/* 25.4 mm = 1 inch = RESOLUTION dots */
int	xmargin = (15 * RESOLUTION * 10) / 254;
int	ymargin = (25 * RESOLUTION * 10) / 254;

#define	FNUM_32		10
#define	FNUM_24		11
#define	DLNUMBEG	12
#define	MAXDLFONT	8

int	lastfnum = DLNUMBEG;

/* devmode.code */
#define	ANK		0
#define	KANJI		1

/* devmode.prop */
#define	FIXED		0
#define	PROP		1

/* devmode.modify */
#define	NONE		0
#define	BOLD		1
#define	SHADOW		2

struct devmode {
	int	mag;
	int	code;
	int	prop;
	int	modify;
	int	ank_fnum;
	int	kanji_fnum;
} devmode;

struct romchwidth {
	int	code;
	int	width;
	int	offset;
};

struct romfontinfo {
	int	point;
	int	font;
	int	mag;
	int	modify;
};

#ifdef	USEEXACTROMFONT
struct romfontinfo	minromfontinfo[] = {
	{5,	FNUM_32,	0,	NONE},
#ifdef	HAS_F24M
	{7,	FNUM_24,	1,	NONE},
#endif	HAS_F24M
	{10,	FNUM_32,	1,	NONE},
#ifdef	HAS_F24M
	{14,	FNUM_24,	2,	NONE},
#endif	HAS_F24M
	{20,	FNUM_32,	2,	NONE},
	{-1}
};

struct romfontinfo	gothromfontinfo[] = {
	{-1}
};
#else	USEEXACTROMFONT
struct romfontinfo	minromfontinfo[] = {
	{5,	FNUM_32,	0,	NONE},
#ifdef	HAS_F24M
	{6,	FNUM_24,	1,	NONE},
	{7,	FNUM_24,	1,	NONE},
	{8,	FNUM_24,	1,	NONE},
	{9,	FNUM_24,	1,	NONE},
#else	HAS_F24M
	{6,	FNUM_32,	0,	NONE},
	{7,	FNUM_32,	0,	NONE},
	{8,	FNUM_32,	0,	NONE},
	{9,	FNUM_32,	1,	NONE},
#endif	HAS_F24M
	{10,	FNUM_32,	1,	NONE},
	{11,	FNUM_32,	1,	NONE},
	{12,	FNUM_32,	1,	NONE},
#ifdef	HAS_F24M
	{14,	FNUM_24,	2,	NONE},
#else	HAS_F24M
	{14,	FNUM_32,	1,	NONE},
#endif	HAS_F24M
	{16,	FNUM_32,	2,	NONE},
	{-1}
};

struct romfontinfo	gothromfontinfo[] = {
	{5,	FNUM_32,	0,	BOLD},
#ifdef	HAS_F24M
	{6,	FNUM_24,	1,	BOLD},
	{7,	FNUM_24,	1,	BOLD},
	{8,	FNUM_24,	1,	BOLD},
	{9,	FNUM_24,	1,	BOLD},
#else	HAS_F24M
	{6,	FNUM_32,	0,	BOLD},
	{7,	FNUM_32,	0,	BOLD},
	{8,	FNUM_32,	0,	BOLD},
	{9,	FNUM_32,	1,	BOLD},
#endif	HAS_F24M
	{10,	FNUM_32,	1,	BOLD},
	{11,	FNUM_32,	1,	BOLD},
	{12,	FNUM_32,	1,	BOLD},
#ifdef	HAS_F24M
	{14,	FNUM_24,	2,	NONE},
#else	HAS_F24M
	{14,	FNUM_32,	1,	BOLD},
#endif	HAS_F24M
	{16,	FNUM_32,	2,	NONE},
	{-1}
};
#endif	USEEXACTROMFONT

#define	MAXROMCHWIDTH	200

struct romchwidth	romchwidth[MAXROMCHWIDTH];
int			romchnum;

void
setdevfont(kflag, fnum)
	int	kflag, fnum;
{
	if (kflag) {
		if (fnum == devmode.kanji_fnum)
			return;
		devmode.kanji_fnum = fnum;
	} else {
		if (fnum == devmode.ank_fnum)
			return;
		devmode.ank_fnum = fnum;
	}
#ifndef	R98
	/* フォント選択 p.7-78 */
	fprintf(outfp, "\033\022S%d ", fnum);
#else	R98
	/* フォント選択 p.7-33 */
	fprintf(outfp, "\03406F1-0");
	fprintf(outfp, "%d", fnum);
#endif	R98
}

void
setdevmode(mag, code, prop, modify)
	int	mag, code, prop, modify;
{
	if (devmode.mag != mag) {
#ifndef	R98
		/* 文字の拡大率指定 p.7-56 */
		fprintf(outfp, "\033\022X%d,%d,%d,%d ", mag, mag, mag, mag);
#else	R98
#ifdef	BASEBUG
		/* 拡大率を変えるとベースラインが変わるのでした */
		fprintf(outfp, "\033\022V%d ",
			dottounitv((calcpixel(devstat.vv, factor) + ymargin)));
#endif	BASEBUG
		if (mag == 0)
			/* 漢字の縮小率指定 p.7-29 */
			fprintf(outfp, "\034m1/2,1/2,L.");
		else
			/* 文字拡大 p.7-32 */
			fprintf(outfp, "\033e%d%d", mag, mag);
#endif	R98
		devmode.mag = mag;
	}

	if (devmode.code != code) {
		if (code == ANK)
#ifndef	R98
			/* 漢字シフトアウト p.7-32 */
			fprintf(outfp, "\033N");
#else	R98
			/* HSパイカモード p.7-9 */
			fprintf(outfp, "\033N");
#endif	R98
		else
#ifndef	R98
			/* 漢字シフトイン p.7-31 */
			fprintf(outfp, "\033K");
#else	R98
			/* 漢字横印字モード p.7-10 */
			fprintf(outfp, "\033K");
#endif	R98
		devmode.code = code;
	}

	if (devmode.prop != prop) {
		if (prop == PROP)
#ifndef	R98
			/* プロポーショナル印字指定 p.7-35 */
			fprintf(outfp, "\033P");
#else	R98
			/* プロポーショナルモード p.7-10 */
			fprintf(outfp, "\033P");
#endif	R98
		else
#ifndef	R98
			/* プロポーショナル印字終了 p.7-37 */
			fprintf(outfp, "\033Q");
#else	R98
			/* 他のモードならなんでも良い */
			fprintf(outfp, "\033N");
#endif	R98
		devmode.prop = prop;
	}

	if (devmode.modify != modify) {
		switch (modify) {
		default:
		case NONE:
#ifndef	R98
			/* ボールド・シャドウ終了 p.7-55 */
			fprintf(outfp, "\033&");
#else	R98
			/* 強調印字モード解除 p.7-14 */
			fprintf(outfp, "\033\"");
#endif	R98
			break;
		
		case BOLD:
#ifndef	R98
			/* ボールド指定 p.7-54 */
			fprintf(outfp, "\033O");
#else	R98
			/* 強調印字モード設定 p.7-14 */
			fprintf(outfp, "\033!");
#endif	R98
			break;
		
		case SHADOW:
#ifndef	R98
			/* シャドウ指定 p.7-54 */
			fprintf(outfp, "\033W");
#else	R98
			/* シャドウ印字モード設定 p.8-40 */
			fprintf(outfp, "\033\034");
#endif	R98
			break;
		}
		devmode.modify = modify;
	}
}

void
setfont(ku)
	int	ku;
{
	int		fnum;

	switch (ku) {
	default:
	case SYMBOLKU:
		ku = 0;
		break;
	case HIRAKANAKU:
		ku = 1;
		break;
	case KATAKANAKU:
		ku = 2;
		break;
	}
	fnum = curfont->dlfont->fontnum + ku;
	setdevfont(FALSE, fnum);
}

void
setpos(nx, ny)
	register int	nx, ny;
{
	register int	 o;

	o = calcpixel(devstat.hh, factor);
	if (nx != o) {
#ifndef	R98
		/* 水平方向AP設定(絶対位置3) p.7-41 */
		fprintf(outfp, "\033\022H%d ", dottounit(nx + xmargin));
#else	R98
		/* 水平方向AP指定(絶対位置2) p.8-42 */
		fprintf(outfp, "\033\022H%d ", dottounith(nx + xmargin));
#endif	R98
		devstat.hh = calctfmwidth(nx, factor);
	}

	o = calcpixel(devstat.vv, factor);
	if (ny != o) {
#ifndef	R98
		/* 垂直方向AP設定(絶対位置3) p.7-44 */
		fprintf(outfp, "\033\022V%d ", dottounit(ny + ymargin));
#else	R98
		/* 垂直方向AP設定(絶対位置2) p.8-42 */
                fprintf(outfp, "\033\022V%d ", dottounitv(ny + ymargin));
#endif	R98
		devstat.vv = calctfmwidth(ny, factor);
	}
}

void
emitinit()
{
#ifndef	R98
	fprintf(outfp, "\033\rP");		/* software reset */
	fprintf(outfp, "\033\022Q2 ");		/* spacing unit 1/720" */
	fprintf(outfp, "\033N");		/* select ANK mode */
#else	R98
	/* ソフトウェアリセット p.7-32 */
	fprintf(outfp, "\033c1");
	/* 240 DPIモード p.8-2 */
	fprintf(outfp, "\034d240"); 
	/* スペーシングユニット 1/240"(240dpiのとき) */
	/* HSパイカモード p.7-9 */
	fprintf(outfp, "\033N");
#endif	R98
	devmode.code = ANK;
#ifndef	R98
	fprintf(outfp, "\033\022P2,64 ");	/* select pure black pattern */
	fprintf(outfp, "\033Q");		/* disable proportional font */
#endif	R98
	devmode.prop = FIXED;
#ifndef	R98
	/* フォントのアサイン p.7-77 */
	fprintf(outfp, "\033\022A%d,0,960@CHUMINCYO\033 ", FNUM_32);
#else	R98
	/* フォントのアサイン p.8-45 */
	fprintf(outfp, "\033\022A%d,0,960@CHUMINCYO\033 ", FNUM_32);
#endif	R98
#ifdef	HAS_F24M
#ifndef	R98
	/* フォントのアサイン p.7-77 */
	fprintf(outfp, "\033\022A%d,0,720@CHUMINCYO\033 ", FNUM_24);
#else	R98
	/* フォントのアサイン p.8-45 */
	fprintf(outfp, "\033\022A%d,0,720@CHUMINCYO\033 ", FNUM_24);
#endif	R98
#endif	HAS_F24M
	devmode.mag = 1;
	devmode.modify = NONE;
	devmode.ank_fnum = -1;
	devmode.kanji_fnum = FNUM_32;
#ifndef	R98
	/* フォント選択 p.7-33 */
	fprintf(outfp, "\033\022S%d ", FNUM_32);
	/* エラーリセット p.7-6 */
	fprintf(outfp, "\033\032R");		/* XXX */
#else	R98
	/* フォント選択 p.7-33 */
	fprintf(outfp, "\03406F1-0%c", FNUM_32);
	/* エラーリセット、対応なし */
#endif	R98
}

void
emitterm()
{
#ifndef	R98
	fprintf(outfp, "\033\rP");		/* XXX software reset */
#else	R98
	fprintf(outfp, "\033c1");		/* XXX software reset */
#endif	R98
}

void
startpage() 
{
	/*
	 * set HMI for kanji
	 */
	setdevmode(1, KANJI, FIXED, NONE);
#ifndef	R98
	/* HMIセット p.7-40 */
	fprintf(outfp, "\033\037");
	putc(dottounit(calcpixel(romchwidth[0].width, factor)) + 1, outfp);
#else	R98
	/* 文字幅設定 p.8-4 */
	fprintf (outfp, "\034p2,%d/240.",
		(calcpixel(romchwidth[0].width, factor)));
#endif	R98
}

void
feedpage(ncopies)
	int	ncopies;
{
#ifndef	R98
	fprintf(outfp, "\033\022N%d ", ncopies);/* set n pages copy */
	fprintf(outfp, "\r\f");			/* feed page(s) */
#else	R98
	/* ページコピー枚数設定 p.8-14 */
	fprintf(outfp, "\034x%d.", ncopies);
	fprintf(outfp, "\r\f");			/* feed page(s) */
#endif	R98
}

dospecial(fp, len)
	register FILE	*fp;
	register int	len;
{
	while (--len >= 0)
		putc(getc(fp), outfp);
}

void
delfont(dlptr)
	struct dlfont	*dlptr;
{
	register int			ch, max;
	register struct fontinfo	*fptr;
	register struct chinfo		*cptr;

	fptr = dlptr->font;
	if (isdownloaded(fptr->flags)) {
		if (isjfmt(fptr->flags))
			max = MAXKU * MAXTEN;
		else
			max = MAXCHAR;
		for (ch = 0, cptr = &fptr->chdir[0]; ch < max; ch++, cptr++)
			cptr->flags &= ~DOWNLOADED;

		fptr->flags &= ~DOWNLOADED;
	}
	fptr->dlfont = NULL;
	dlptr->next->prev = dlptr->next;
	dlptr->prev->next = dlptr->prev;
	free((char *)dlptr);
}

void
delallfont()
{
	register struct dlfont	*dlptr;

	while ((dlptr = dlfont.next) != &dlfont)
		delfont(dlptr);
	/*
	 * delete all font
	 */
#ifndef	R98
	fprintf(outfp, "\033[@\033]");
#else	R98
#endif	R98
}

void
dlhead(fptr, fontnum)
	register struct fontinfo	*fptr;
	register int			fontnum;
{
	/*
	 * header
	 */
#ifndef	R98
	fprintf(outfp, "\033[3,1@%s", fptr->n);
	fprintf(outfp, "\033[%s\r\r\r\r", fptr->n);
	fprintf(outfp, "240,240,,,720,1200,,");
	fprintf(outfp, "%d,,,,,,,,0,0,0,,,P,10000000,0,,,,,,0,0,", fontnum);
	fprintf(outfp, "\033 \033 ");
#else	R98
#endif	R98
	return;
}

void
dltail()
{
	/*
	 * trailer
	 */
#ifndef	R98
	fprintf(outfp, "\033]\n");
#else	R98
#endif	R98
}

void
dlch(fptr, cptr, ch)
	register struct fontinfo	*fptr;
	register struct chinfo		*cptr;
	register int			ch;
{
	register int		ws, wi, i, move;
	register int		*ptr;

	ws = (cptr->width + 15) / 16;
	wi = (cptr->width + 31) / 32;
#ifndef	R98
	move = dottounit(calcpixel(cptr->tfmw, factor)) * 10;
#else	R98
	move = dottounith(calcpixel(cptr->tfmw, factor)) * 10;
#endif	R98
	/*
	 * glyph information
	 */
#ifndef	R98
	fprintf(outfp, "\033^%02x,%d,%d,%d,%d,%d,%d@",
			ch,
			ws * 16, -cptr->xoffset,
			move,
			cptr->height, -cptr->yoffset,
			move);
#else	R98
#endif	R98
	/*
	 * bit image
	 */
	ptr = (int *)cptr->image;
	for (i = 0; i < cptr->height; i++) {
#ifndef	R98
		fwrite(ptr, sizeof(short), ws, outfp);
#else	R98
#endif	R98
		ptr += wi;
	}
	cptr->flags |= DOWNLOADED;
}

void
downloadfont(dlptr)
	struct dlfont	*dlptr;
{
	register struct fontinfo	*fptr;
	register struct chinfo		*cptr;
	register int			ch, fnum;

	fptr = dlptr->font;
	fnum = lastfnum;
	dlptr->fontnum = fnum;
	if (isjfmt(fptr->flags)) {
		int	needdl;

		/*
		 * download only `symbol', `hirakana' and `katakana'
		 */
		needdl = FALSE;
		cptr = &fptr->chdir[SYMBOLKU * MAXKU];
		for (ch = '!'; ch < '!' + MAXKU; ch++, cptr++) {
			if (cptr->count > 1) {
				needdl = TRUE;
				break;
			}
		}
		if (!needdl) {
			fnum++;
		} else {
			dlhead(fptr, fnum++);
			cptr = &fptr->chdir[SYMBOLKU * MAXKU];
			for (ch = '!'; ch < '!' + MAXKU; ch++, cptr++) {
				if (cptr->count <= 1)
					continue;
				dlch(fptr, cptr, ch);
			}
			dltail();
		}

		needdl = FALSE;
		cptr = &fptr->chdir[HIRAKANAKU * MAXKU];
		for (ch = '!'; ch < '!' + MAXKU; ch++, cptr++) {
			if (cptr->count > 1) {
				needdl = TRUE;
				break;
			}
		}

		if (!needdl) {
			fnum++;
		} else {
			dlhead(fptr, fnum++);
			cptr = &fptr->chdir[HIRAKANAKU * MAXKU];
			for (ch = '!'; ch < '!' + MAXKU; ch++, cptr++) {
				if (cptr->count <= 1)
					continue;
				dlch(fptr, cptr, ch);
			}
			dltail();
		}

		needdl = FALSE;
		cptr = &fptr->chdir[KATAKANAKU * MAXKU];
		for (ch = '!'; ch < '!' + MAXKU; ch++, cptr++) {
			if (cptr->count > 1) {
				needdl = TRUE;
				break;
			}
		}
		if (!needdl) {
			fnum++;
		} else {
			dlhead(fptr, fnum++);
			cptr = &fptr->chdir[KATAKANAKU * MAXKU];
			for (ch = '!'; ch < '!' + MAXKU; ch++, cptr++) {
				if (cptr->count <= 1)
					continue;
				dlch(fptr, cptr, ch);
			}
			dltail();
		}
	} else {
		dlhead(fptr, fnum++);
		cptr = &fptr->chdir[0];
		for (ch = 0; ch < MAXCHAR; ch++, cptr++) {
			if (cptr->count <= 1)
				continue;
			dlch(fptr, cptr, ch);
		}
		dltail();
	}
	lastfnum = fnum;
	fptr->flags |= DOWNLOADED;
}

void
download()
{
	register struct dlfont	*dlptr;
	register int		i;

	loadallimage(TRUE, 2);
#ifndef	DEBUG
	for (dlptr = dlfont.next, i = 0;
	     dlptr != &dlfont && i < MAXDLFONT;
	     dlptr = dlptr->next, i++) {
		downloadfont(dlptr);
	}
#endif DEBUG
}

int
emitchar(ch, cptr)
	int		ch;
	struct chinfo	*cptr;
{
	setpos(calcpixel(curstat.h, factor), calcpixel(curstat.v, factor));
	devstat.hh += cptr->tfmw;
	setdevmode(1, ANK, PROP, NONE);

	if (!isprint(ch) || ch == ' ' || ch == '\\')
#ifndef	R98
		/* コントロールコードプリント p.7-38 */
		fprintf(outfp, "\033\022K1 ");
	putc(ch, outfp);
#else	R98
		;
	putc(ch, outfp);
#endif	R98

	return cptr->tfmw;
}

void
romfontinit()
{
	register FILE			*fp;
	register struct romchwidth	*wptr;
	register char			**ptr;
	register int			i;
	char				file[BUFSIZ];

	for (ptr = fontpath; *ptr != NULL; ptr++) {
		sprintf(file, "%s/widthtable.lp3320", *ptr);
		fp = fopen(file, "r");
		if (fp == NULL)
			continue;
		for (wptr = romchwidth, i = 0; ; wptr++, i++) {
			if (i == MAXROMCHWIDTH)
				fatal("too many rom font width");
			wptr->code = -1;
			fscanf(fp, "%d%d%d",
				&wptr->code, &wptr->width, &wptr->offset);
			if (wptr->code < 0)
				break;
			wptr->width = wptr->width * 655360.0 / (1<<20);
			wptr->offset = wptr->offset * 655360.0 / (1<<20);
		}
		romchnum = i;
		fclose(fp);
		break;
	}
	if (*ptr == NULL)
		fatal("can not find rom font width");
}

int
emitromchar(ch)
	int	ch;
{
	register int		i, width, x, y;
	struct romchwidth	*ptr;
	struct romfontinfo	*finfo;
#define	YOFF	-2
#define	XOFF	0

	ptr = &romchwidth[1];
	i = romchnum;
	while (--i >= 0) {
		if (ch == ptr->code)
			goto found;
		else if (ch < ptr->code)
			break;
		ptr++;
	}
	/*
	 * not found
	 */
	ptr = &romchwidth[0];
found:
	finfo = (struct romfontinfo *)curfont->chdir;
	x = calcpixel(ptr->offset, factor) + XOFF;
	y = YOFF;
	width = ptr->width;
	if (finfo->mag == 0) {
		/*
		 * half size
		 */
		x /= 2;
		y /= 2;
	} else {
		x *= finfo->mag;
		y *= finfo->mag;
	}

	setpos(calcpixel(curstat.h, factor) - x,
				calcpixel(curstat.v, factor) - y);
	devstat.hh += romchwidth[0].width;

	setdevmode(finfo->mag, KANJI, FIXED, finfo->modify);
	setdevfont(TRUE, finfo->font);

#ifndef	R98
	putc((ch >> 8) & 0x7f, outfp);
	putc((ch & 0x7f), outfp);
#else	R98
	putc((ch >> 8) & 0x7f, outfp);
	putc((ch & 0x7f), outfp);
#endif	R98

	return (width * finfo->point) / 10;

#undef XOFF
#undef YOFF
}

int
emitimage(code, cptr)
	int		code;
	struct chinfo	*cptr;
{
	register int	wb, ww, x, y;
	register int	*ptr;

	x = calcpixel(curstat.h, factor) - cptr->xoffset + xmargin;
	y = calcpixel(curstat.v, factor) - cptr->yoffset + ymargin;
	/*
	if (x < 0)
		x = 0;
	else if (x > MAX_X - cptr->width)
		x = MAX_X - cptr->width;
	if (y < 0)
		y = 0;
	else if (y > MAX_Y - cptr->height)
		y = MAX_Y - cptr->height;
	*/

#ifndef	R98
	/* イメージ転送 p.7-70 */
	fprintf(outfp, "\033\022G%d,%d,%d,%d,%d,%d,%d@",
		3, cptr->width, cptr->height, 1, 1, dottounit(x), dottounit(y));
#else	R98
	/* 描画座標指定 p.8-5 */
	fprintf(outfp, "\034e%d,%d.", dottodot(x), dottodot(y));
	/* 領域指定イメージ p.8-12 */
	fprintf(outfp, "\034i%d,%d,%d,%d/%d,%d/%d,%d.",
		cptr->width, cptr->height, 0, 1, 1, 1, 1,
		((cptr->width+7)/8)*cptr->height);
#endif	R98

	wb = (cptr->width + 7) / 8;
	ww = ((cptr->width + 31) / 32);
	ptr = (int *)cptr->image;
	for (x = 0; x < cptr->height; x++, ptr += ww)
#ifndef	R98
		fwrite((char *)ptr, 1, wb, outfp);
#else	R98
		fwrite((char *)ptr, 1, wb, outfp);
#endif	R98

	/*
	devstat.hh = UNDEFPOS;
	devstat.vv = UNDEFPOS;
	*/
	
#ifdef	DEBUG
	fflush (outfp);
#endif	DEBUG
	return cptr->tfmw;
}

void
emitrule(w, h)
	int	w;
	int	h;
{
	register int	i, j, x1, y1, x2, y2;

	if (w <= 0 || h <= 0)
		return;

	x1 = calcpixel(curstat.h, factor) + xmargin;
	y1 = calcpixel(curstat.v - h, factor) + ymargin;
	i = calcpixel(w, factor);
	if (i < 1)
		i = 1;
	j = calcpixel(h, factor);
	if (j < 1)
		j = 1;
	x2 = x1 + i;
	y2 = y1 + j;
	/*
	x2 = calcpixel(curstat.h + w, factor) + xmargin;
	y2 = calcpixel(curstat.v, factor) + ymargin;
	if (x1 < 0)
		x1 = 0;
	if (y1 < 0)
		y1 = 0;
	if (x2 > MAX_X)
		x2 = MAX_X;
	if (y2 > MAX_Y)
		y2 = MAX_Y;
	if (x1 > x2 || y1 > y2)
		return;

	i = x2 - x1 + 1;
	j = y2 - y1 + 1;
	*/

#ifndef	R98
	/* 領域指定あみかけ p.7-67 */
	fprintf(outfp, "\033\022R%d,%d,%d,%d ",
		dottounit(x1), dottounit(y1), dottounit(i), dottounit(j));
#else	R98
	/* 描画座標指定 p.8-5 */
	fprintf(outfp, "\034e%d,%d.", dottodot(x1), dottodot(y1));
	/* 領域指定網掛け p.8-11 */
	fprintf(outfp, "\034s3,64,%d,%d.", i, j);
#endif	R98
}

int
romfile(dviname, mag, name, fptr)
	char		*dviname;
	int		mag;
	char		*name;
	struct fontinfo	*fptr;
{
	register int		i;
	struct romfontinfo	*finfo, *ptr;
	int			pnt;
#define	POINT	(RESOLUTION * 5)

	if (strncmp(dviname, "min", 3) == 0) {
		finfo = minromfontinfo;
		dviname += 3;
	} else if (strncmp(dviname, "goth", 4) == 0) {
		finfo = gothromfontinfo;
		dviname += 4;
	} else
		return 0;

	pnt = 0;
	sscanf(dviname, "%d", &pnt);
	if (pnt < 0)
		pnt = 10;
	pnt = pnt * mag / POINT;
	for (ptr = finfo; ptr->point > 0; ptr++) {
		if (ptr->point > pnt)
			break;
	}
	if (ptr != finfo)
		--ptr;
#ifdef	USEEXACTROMFONT
	if (ptr->point != pnt)
		return 0;
#endif	USEEXACTROMFONT
	fptr->chdir = (struct chinfo *)ptr;
	return 1;
#undef	POINT
}
/*
int
romfile(dviname, mag, name, fptr)
	char		*dviname;
	int		mag;
	char		*name;
	struct fontinfo	*fptr;
{
	register int	i;
#define	TENPNT		(RESOLUTION * 5)
#define	DM		0.8
#define	ADJMAG(s)	((int)(TENPNT * ((s) + DM)))

	if (strcmp(dviname, "min10") == 0) {
		if (mag == TENPNT) {
			fptr->chdir = (struct chinfo *)1;
			return 1;
		}
		if (mag < TENPNT * 2)
			return 0;

		for (i = 2; i < 8; i++) {
			if (mag <= ADJMAG(i)) {
				fptr->chdir = (struct chinfo *)i;
				return 1;
			}
		}
		fptr->chdir = (struct chinfo *)8;
		return 1;
	}
	if (strcmp(dviname, "min5") == 0) {
		if (mag == TENPNT) {
			fptr->chdir = (struct chinfo *)0;
			return 1;
		}
	}
	return 0;
#undef TENPNT
#undef DM
#undef ADJMAG
}
*/
