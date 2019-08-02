#define	FONTHASH	32
#define	MAXNAME		(256 * 2)
#define	MAXFILENAME	1024

#define	MAXCHAR		128

#define	MAXKU		94
#define	MAXTEN		94
#define	MAXSYMKU	8
#define	MINKANJIKU	15

#define	SYMBOLKU	0
#define	HIRAKANAKU	3
#define	KATAKANAKU	4

#define	MAXKHAR		(MAXKU * MAXTEN)
#define	MAXKANJI	(MAXKHAR - MINKANJIKU * MAXKU)

#define	PXLID		1001
#define	JPXLID		0x80000001	/* Japanese kanji pxl format */
#define	JPKID		0x80000002	/* Japanese kanji pk format */

#define	LOADED		0x100
#define	DOWNLOADED	0x200

#define	ROMFONT		0x400

#define	PXLFMT		0x0
#define	PKFMT		0x1

#define	JFMT		0x10
#define	JPXLFMT		(JFMT|PXLFMT)
#define	JPKFMT		(JFMT|PKFMT)

#define	isjfmt(x)	((x) & JFMT)
#define	isromfont(x)	((x) & ROMFONT)
#define	isdownloaded(x)	((x) & DOWNLOADED)
#define	isloaded(x)	((x) & LOADED)
#define	ispkfmt(x)	((x) & PKFMT)
#define	pkflag(x)	((x) & 0xff)

struct chinfo {
	unsigned short	flags;
	unsigned short	count;
	unsigned short	width;
	unsigned short	height;
	short		xoffset;
	short		yoffset;
	int		tfmw;
	char		*image;
};

struct fontinfo {
	struct fontinfo	*next;
	int		flags;
	int		tbyte;
	int		dlbyte;
	FILE		*fontfp;
	struct dlfont	*dlfont;
	int		k;
	int		c;
	int		s;
	int		d;
	int		a;
	int		l;
	char		n[MAXNAME];
	int		fontspace;
	int		fontmag;
	char		name[MAXFILENAME];
	int		mag;
	int		design;
	int		bitmag;
	struct chinfo	*chdir;
};

struct dlfont {
	struct dlfont	*next;
	struct dlfont	*prev;
	struct fontinfo	*font;
	int		fontnum;
	double		compress;
};
