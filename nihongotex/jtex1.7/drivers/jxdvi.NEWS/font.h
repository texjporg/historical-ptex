/*
 *	font.h: include file for font manager of jxdvi
 *
 *	$Header: font.h,v 1.1 88/02/06 11:28:15 atsuo Exp $
 */

#include "atsuo-copyright.h"  

typedef unsigned char ubyte;

#define DBG_BITMAP  0x1
#define DBG_DVI     0x2
#define DBG_ALL     (DBG_BITMAP|DBG_DVI)

typedef char *Ptr;

#define JIS_QUESTION_MARK	0x2129	/* JIS code of '?' */

/* Return values of Font Manager */
#define ERROR 2
#define TRUE  1
#define FALSE 0

/* Message to Font Manager */
#define DEFINE_FONT 0
#define GET_GLYPH   1
#define CHANGE_FONT 2
#define PRE_DEFINE_FONT 3
#define	CLOSE_FONT_FILE	4

/* Site dependent information */
#define FONT_DIRECTORY  "/usr/lib/tex/fonts"
#define PXLFONT_SUFFIX ".%dpxl"
#define NWFFONT_SUFFIX ".%dnwf"
#define TFM_SUFFIX  ".tfm"
#define TFM_DIRECTORY "/usr/lib/tex/fonts"

/*
** Font Kind Database
 */
#define F_ALPHA 0
#define F_KANJI 1

extern int font_kind[];
extern int current_font_kind;
extern int boot_mode;

/*
** Kanji-Bitmap structure for raster ops.
*/
struct k_bitmap{
	struct k_bitmap *next;	/* link to next k_bitmap block */
	struct k_bitmap *prev;	/* link to previous k_bitmap block */
	unsigned int	uc;		/* use count */
	int TeXnumber;			/* font number (in DVI file) */
	int	info;				/* index to k_glyph table */
#ifdef JXDVI
	char *bits;				/* pointer to the bits */
#endif JXDVI
#ifdef DVI2NWP
	char *nbits;			/* pointer to the non-reversed bits */
#endif
};

/* Kanji font bitmaps.
 * The kanji bitmaps are maintained in kanji_font_bitmap. Kanji_font_bitmap
 * is an 
 * array of pointers linking to k_bitmap lists. Each entry corresponds to
 * one of kanji char's.
 */

extern struct k_bitmap *kanji_font_bitmap[];

/*
 * Character information.
 * All fields are filled in at font definition time,
 * except for the bitmap, which is "faulted in"
 * when the character is first referenced.
 */
struct k_glyph {
	long dvi_adv;		/* DVI units to move reference point */
	short x, y;			/* x and y offset in pixels */
	short pxl_adv;		/* pixels to move reference point */
};

/*
 * The layout of a kanji-font information block.
 * There is one of these for every loaded font or
 * magnification thereof.
 *
 * Also note the strange units.  The design size is in 1/2^20 point
 * units (also called micro-points), and the individual character widths
 * are in the TFM file in 1/2^20 ems units, i.e. relative to the design size.
 *
 * And note that kanji font bitmaps are maintained seperately in
 * kanji font bitmap list. (by Atsuo, K.)
 */

struct k_font {
	struct k_font *next;	/* link to next k_font info block */
	struct k_font *prev;	/* link to previos k_font info block */
	int TeXnumber;			/* font number (in DVI file) */
	int	preFontNum;			/* font number (when pre-loaded) */
	int scale;				/* scaled size in DVI units */
	int design;				/* design size in DVI units */
	char *fontname;			/* fontname */
	FILE *file;				/* open NWF file or NULL */
	short	file_h, file_w;	/* bitmap h & w in pixels (in font file) */
	short	file_bytes_wide;/* scan-line width in bytes (in font file) */
	short	h, w;			/* bitmap height and width in pixels */
	short	bytes_wide;		/* scan-line width in bytes */
	struct k_glyph *glyph;	/* k_glyph table */
#ifdef STATISTICS
	int	use;				/* font use count */
	int	load;				/* bitmap read count */
	int	load_bytes;			/* bitmap bytes read in */
#endif
};

/* .jfm file format */
#define JFM_ID	11

#define	J_ID 0		/* id */
#define	J_NT 1		/* number of words in the char_type table */
#define	J_LF 2		/* length of entire file */
#define	J_LH 3		/* length of header */
#define	J_BC 4		/* smallest char_type */
#define	J_EC 5		/* largest char_type */
#define	J_NW 6		/* number of width table */
#define	J_NH 7		/* number of height table */
#define	J_ND 8		/* number of depth table */
#define	J_NI 9		/* number of italic correctiuon table */
#define	J_NL 10		/* number of glue/kern talbe */
#define	J_NK 11
#define	J_NG 12
#define	J_NP 13

struct jfm_char_type {
	unsigned short	code;		/* JIS code */
	unsigned short	index;		/* index to char_info */
};

struct jfm_char_info {
	unsigned char	width_ix;			/* index to width */
	unsigned char	height_depth_ix;	/* index to height & depth */
	unsigned char	italic_ix_tag;		/* index to italic & tag */
	unsigned char	remainder_ix;		/* index to remainder */
};

struct jfm {
	char	fn[128];					/* font name (min10, etc.) */
	short	table[(J_NP+1)];				/* table */
	unsigned long	check_sum;				/* header */
	unsigned long	design_size;		
	struct	jfm_char_type *type;
	struct	jfm_char_info *info;
	unsigned long *width;
	unsigned long *height;
	unsigned long *depth;
	unsigned long *italic;
};

/* Kanji Font Database
 * Since NWF format does not contain font information,
 * we need database for it.
 */
struct kanji_font_data {
	char	*fontname;		/* name of kanji font (including suffix) */
	short	h, w;			/* bitmap height and width in pixels */
	short	bytes_wide;		/* scan-line width in bytes */
};

extern struct kanji_font_data kanji_font_db[];
