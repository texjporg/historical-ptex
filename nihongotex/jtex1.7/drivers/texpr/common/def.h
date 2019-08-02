/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

#define	LASERSHOT			/* please edit this line and SELJFONT*/

#define	MIN24_NEW	0x0001		/* MINCHO'83 24*24 */
#define MIN32_NEW	0x0002		/* MINCHO'83 32*32 */
#define MIN40_NEW	0x0004		/* MINCHO'83 40*40 */
#define GOT24_NEW	0x0008		/* GOTHIC'83 24*24 */
#define GOT32_NEW	0x0010		/* GOTHIC'83 32*32 */
#define GOT40_NEW	0x0020		/* GOTHIC'83 40*40 */
#define MIN24		0x0040		/* MINCHO'78 24*24 */
#define MIN32		0x0080		/* MINCHO'78 32*32 */
#define MIN40		0x0100		/* MINCHO'78 40*40 */
#define GOT24		0x0200		/* GOTHIC'78 24*24 */
#define GOT32		0x0400		/* GOTHIC'78 32*32 */
#define GOT40		0x0800		/* GOTHIC'78 40*40 */

#ifdef	LBP_8_A2
#define	DPI	300			/* resolution is 300 dot/inch */
#define	SPC	1			/* proportional spacing */
#define	OFS	0			/* no offset */
#endif

#ifdef	LBP_8_AJ2
#define	DPI	240			/* resolution is 240 dot/inch */
#define	SPC	1
#define	OFS	0
#define	DIVF	0
#define	SELJFONT	(MIN24 | MIN32)
#endif

#ifdef	LBP_8II_A2
#define	DPI	300
#define	SPC	2			/* kerning */
#define	OFS	2			/* offset */
#endif

#ifdef	LBP_8II_AJ2
#define	DPI	240
#define	SPC	2
#define	OFS	2
#define	DIVF	0
#define	SELJFONT	(MIN24 | MIN32)
#endif

#ifdef	LASERSHOT
#define	DPI	240
#define	SPC	2
#define	OFS	2
#define	DIVF	1
#define	SELJFONT	(MIN24_NEW | MIN32_NEW | GOT24_NEW | GOT32_NEW | GOT40_NEW)
#endif

#define	NCHAR	128			/* number of char in a char-set */
#define	MAX_W	64			/* max char box size is 64x64 */
#define	MAX_H	64

#define	MAX_FONT  256			/* at most 256 font in TeX */
#define	MAX_STACK 100			/* stack size of dvi2lbp */

#define	PXL_ID	1001			/* pixel file id */
/*
#define PK_FORMAT*/			/* comment out this line, if
					   you use "pxl" format */

#define	LIB	"/usr/lib/tex/fonts"	/* default font library */

#define	LPR	"/usr/ucb/lpr","lpr","-d","-h","-Plbp" /* execl arg */

#define	fix(x)	(int)((x) + 0.5)
					/* scaled point to dot */
#define	sp_to_dot(x) fix((float)(x)*mag*(DPI/(65536*72.27)))
					/* sp_to_dot truncate */
#define	sp_to_dot0(x) (int)((float)(x)*mag*(DPI/(65536*72.27)))

#define	PXL_ID	1001			/* pixel file id */
#define	JFM_ID	3			/* jfm file id */

typedef struct char_type{
	short	type;
	short	code;
}CHAR_TYPE;

typedef	struct font {
	int	c;			/* check sum */
	int	d;			/* design_size */
	float	fmag;			/* font mag */
	char	*a;			/* area name */
	char	*n;			/* font name */
	int	down;			/* down load flag */
	short	id;			/* font ID */
	short	size;			/* jfont size */
	struct font_ch *ch;		/* char table */
	CHAR_TYPE *ctype;		/* char type table */
	char	face;			/* font face */
}	FONT;

typedef	struct font_ch {
	int	width;			/* unit = dot */
	int	height;
	int	xoff;
	int	yoff;
	char	*pxl;
	int	tfmw;			/* unit = sp */
}	FONT_CH;

