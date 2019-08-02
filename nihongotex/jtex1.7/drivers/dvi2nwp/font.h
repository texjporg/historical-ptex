/*
 *	font.h: include file for font manager of jxdvi
 *
 *	$Header: font.h,v 3.1 89/02/25 22:57:43 atsuo Exp $
 */

#include "atsuo-copyright.h"  

extern long	magnification, denominator, numerator;
extern int	pixels_per_inch;
extern struct font *current_font;
extern int shrink_factor;
extern int list_fonts;
extern unsigned char reverse_byte[];
extern int font_not_found;
extern int n_open_fonts;
extern unsigned long num();
extern int backwards;

typedef unsigned char ubyte;

#define MAX_OPEN_FONTS 8
#define MAX_OPEN_K_FONTS 5
#define MAX_KANJI_CHAR	7000

extern int n_open_k_fonts;	/* for LRU management of kanji fonts */

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
#define RESET_FONTS 5

/* Font file suffixes */
#define PXL_FONT_SUFFIX	".%dpxl"
#define PK_FONT_SUFFIX	".%dpk"
#define NWF_FONT_SUFFIX	".%dnwf"
#define DNF_FONT_SUFFIX	".%dpxl"
#define DNF_FONT_EXT	"pxl"
#define TFM_SUFFIX	".tfm"
#define JFM_SUFFIX	".tfm"
#define NWFADJ_SUFFIX	".nwfadj"
#define NOT_FOUND_SUFFIX ".%dpk (for example) [not found]"

/* Font handling error messages */
#define KF_NOT_FOUND 0
#define  F_NOT_FOUND 1

/*
 * Font type specifier
 */
#define	FT_PXL	0	/* PXL font file */
#define	FT_PK	1	/* PK font file */
#define FT_NWF	2	/* NWF font file */
#define FT_V_NWF 3	/* Vector NWF font file */
#define	FT_DNF	4	/* DaiNippon font file */
#define FT_JFM	5	/* Box shape (calculated from jfm file) font */
#define FT_NOT_FOUND 9	/* font not found */

/*
 * Font List
 */
extern struct font *current_font;

/*
 * Font Kind Database
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
	unsigned int	uc;	/* use count */
	short TeXnumber;	/* font number (in DVI file) */
	short info;		/* index to k_glyph table */
/* #ifdef DNF */
	short h, w;		/* height and width in pixels */
	short bytes_width;	/* scan line width in byte */
	short short_word;	/* scan-line width in short */
	short x, y;		/* x and y offset in pixels */
/* #endif */
	unsigned char *bits;	/* pointer to the bits */
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
	short x, y;		/* x and y offset in pixels */
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
	struct jfm *j;
	short TeXnumber;	/* font number (in DVI file) */
	short preFontNum;	/* font number (when pre-loaded) */
	int scale;		/* scaled size in DVI units */
	int design;		/* design size in DVI units */
	char *fontname;		/* fontname */
	short fn_len;		/* Length of fontname */
	FILE *file;		/* open NWF file or NULL */
	short	file_h, file_w;	/* bitmap h & w in pixels (in font file) */
	short	file_bytes_wide;/* scan-line width in bytes (in font file) */
	short	h, w;		/* bitmap height and width in pixels */
	short	bytes_wide;	/* scan-line width in bytes */
	struct k_glyph *glyph;	/* k_glyph table */
	short	fonttype;	/* FT_NWF or FT_V_NWF */
#ifdef DNF
	struct	JXL4_INFO *jxl4_info	 /* jxl4 information */
#endif
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
#define	J_NP 13		/* number of font parameter words */

/* param stuff */
#define J_NP_SLANT	0
#define J_NP_SPACE	1
#define J_NP_STRETCH	2
#define J_NP_SHRINK	3
#define J_NP_X_HEIGHT	4
#define J_NP_QUAD	5
#define J_NP_EX_SPACE	6
#define J_NP_PARAM_8	7
#define J_NP_PARAM_9	8

struct jfm_char_type {
	unsigned short	code;		/* JIS code */
	unsigned short	index;		/* index to char_info */
};

struct jfm_char_info {
	unsigned char	width_ix;		/* index to width */
	unsigned char	height_depth_ix;	/* index to height & depth */
	unsigned char	italic_ix_tag;		/* index to italic & tag */
	unsigned char	remainder_ix;		/* index to remainder */
};

struct jfm {
	char	fn[128];			/* font name (min10, etc.) */
	short	table[(J_NP+1)];		/* table */
	unsigned long	check_sum;		/* header */
	unsigned long	design_size;		
	struct	jfm_char_type *type;
	struct	jfm_char_info *info;
	unsigned long *width;
	unsigned long *height;
	unsigned long *depth;
	unsigned long *italic;
	unsigned long *param;
};

/*
 * Kanji Font Database
 *   Since NWF format does not contain font information,
 *   we need database for it.
 */
struct kanji_font_data {
	char	*fontname;	/* name of kanji font (including suffix) */
	short	h, w;		/* bitmap height and width in pixels */
	short	bytes_wide;	/* scan-line width in bytes */
};

#ifdef NWF_604
extern struct kanji_font_data kanji_font_db[];
#endif /* NWF_604 */

#ifdef V_NWF
extern int m_v_font_fd, g_v_font_fd;
#endif /* V_NWF */

/*
 *  pxl.h:
 *	Originally Written by Eric C. Cooper, CMU
 *	Modified by Atsuo, K., ISIR for jxdvi/dvi2nwp
 *	 $Header: font.h,v 3.1 89/02/25 22:57:43 atsuo Exp $
 */

#define BITS_PER_LONG	32
#define BITS_PER_SHORT	16
#define BITS_PER_BYTE	8

#define BYTES_PER_LONG (BITS_PER_LONG/BITS_PER_BYTE)
#define BYTES_PER_SHORT (BITS_PER_SHORT/BITS_PER_BYTE)

#define SHORTS_PER_LONG (BITS_PER_LONG/BITS_PER_SHORT)

#define ROUNDUP(x,y) (((x)+(y)-1)/(y))

/*
 * Bitmap structure for raster ops.
 */	
struct bitmap{
	short h, w;		/* height and width in pixels */
	short bytes_wide;	/* scan-line width in bytes */
	short short_wide;	/* scan-line width in shorts */
	unsigned char *bits;	/* pointer to the bits */
};

#define MAXCHARS 128		/* make 256 for 8 bit characters */

/*
 * Per-character information.
 * There is one of these for each character in a font.
 * All fields are filled in at font definition time,
 * except for the bitmap, which is "faulted in"
 * when the character is first referenced.
 */
struct glyph {
	long addr;		/* address of bitmap in PXL file */
	long dvi_adv;		/* DVI units to move reference point */
	short x, y;		/* x and y offset in pixels */
	struct bitmap bitmap;	/* bitmap for character */
	short pxl_adv;		/* pixels to move reference point */
/**/	short turn_on;		/* needed for PK file */
/**/	short dyn_f;		/* needed for PK file */
};

/*
 * The layout of a font information block.
 * There is one of these for every loaded font or
 * magnification thereof.
 *
 * Also note the strange units.  The design size is in 1/2^20 point
 * units (also called micro-points), and the individual character widths
 * are in the TFM file in 1/2^20 ems units, i.e. relative to the design size.
 */

struct font {
	struct font *next;	/* link to next font info block */
	struct font *prev;	/* link to previous font info block */
	int TeXnumber;		/* font number (in DVI file) */
	int scale;		/* scaled size in DVI units */
	int design;		/* design size in DVI units */
 	short fonttype;		/* FT_PXL or FT_PK */
	char *fontname;		/* PXL file name */
	short fn_len;		/* Length of fontname */
	FILE *file;		/* open PXL file or NULL */
	struct glyph glyph[MAXCHARS];
};

/*
 * jxl4.h - jxl4 font format structure definition.
 *
 *		Copyright (C) 1988 dit Co., Ltd.
 */

/*
 * $Id: jxl4.h,v 1.5 89/08/11 11:08:35 ditstaff Exp $
 */

#define JXL4ID	101157

struct JXL4 {
    int check_sum;      /* font file checksum */
    int magnification;  /* resolution = `dpi * 5' */
    int design_size;    /* designsize = `point-size * 2^20' */
    int fixed_pxl_size; /* pxl nums of fixed glyph */
    int fixed_tfm_size; /* tfm_width of fixed glyph */
    int tfm_ptr;        /* bytes to tfm entry */
    int tfm_size;       /* entry nums of tfm (max = 256) */
    int x_info_ptr;     /* bytes to Xinfo node */
    int x_info_size;    /* entry nums of Xinfo */
    int y_info_ptr;     /* bytes to Yinfo node */
    int y_info_size;    /* entry nums of Yinfo */
    int dir_size;       /* entry nums of PXLdir */
    int dir_ptr_0;      /* [0] -> bytes to PXLdir */
    int dir_ptr_1;      /* [1] -> Level.1 K-Char Position */
    int dir_ptr_2;      /* [2] -> Level.2 K-Char Position */
};

struct PXL_DIR {
    char tfm_index : 8;
    int  rat_index : 24;
    short x_info_index : 16;
    short y_info_index : 16;
};

struct X_INFO {
    short x_offset;
    short x_pix;
    short x_offset_p;
};

struct Y_INFO {
    short y_offset;
    short y_pix;
    short y_offset_p;
} ;

struct TFM_TABLE {
    int entry;
} ;

struct JXL4_INFO {
    struct JXL4      *jxl4;
    struct X_INFO    *xinfo;
    struct Y_INFO    *yinfo;
    struct PXL_DIR   *pxldir;
    struct TFM_TABLE *tfmtable;
} ;

#define SIZEof_JXL4			60
#define SIZEof_TFM_TABLE	4
#define SIZEof_X_INFO		6
#define SIZEof_Y_INFO		6
#define SIZEof_PXL_DIR		8

/*
 * config.h - Configuration file for dviimp
 *
 *		Copyright (C) 1988 dit Co., Ltd.
 */

/*
 * $Id: config.h,v 1.8 89/11/10 17:14:03 void Exp $
 */

/* #include "defs.h"
#include "site.h"
*/
#define	read4(fd, p)	(void)read((fd), (char *)&(p), 4)
#define	read2(fd, p)	(void)read((fd), (char *)&(p), 2)



