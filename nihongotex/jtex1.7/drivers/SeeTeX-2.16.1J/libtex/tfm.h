/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * TFM file information.
 */

/*
 * TFM files start with a series of unsigned 16 bit integers.  We
 * read this into the structure `tfm_header'.  These are type i32
 * so that they may be used as integer quantities without concern
 * as to sign extension.
 */
struct tfmheader {
#ifdef ASCIIJTEX
	i32	th_id;		/* ASCII-JFM ID number (11:JFM/0:not) */
	i32	th_nt;		/* number of words in JFM char-type table */
#endif /* ASCIIJTEX */
	i32	th_lf;		/* length of the file (16 bit words) */
	i32	th_lh;		/* length of the header data (words) */
	i32	th_bc;		/* beginning character */
	i32	th_ec;		/* ending character (inclusive) */
	i32	th_nw;		/* number of words in width table */
	i32	th_nh;		/* number of words in height table */
	i32	th_nd;		/* number of words in depth table */
	i32	th_ni;		/* words in italic correction table */
	i32	th_nl;		/* words in ligature/kern table */
	i32	th_nk;		/* words in kern table */
	i32	th_ne;		/* words in extensible character table */
	i32	th_np;		/* number of font parameter words */
};

/*
 * The remainder of the TFM file comprises the following information,
 * all of which are 32 bit quantities:
 *
 * header:	array [0..lh-1] of stuff
#ifdef ASCIIJTEX
 * char_type:	array [0..nt-1] of char_type_word
#endif
 * char_info:	array [bc..ec] of char_info_word
 * width:	array [0..nw-1] of fix_word
 * height:	array [0..nh-1] of fix_word
 * depth:	array [0..nd-1] of fix_word
 * italic:	array [0..ni-1] of fix_word
 * lig_kern:	array [0..nl-1] of lig_kern_command
 * kern:	array [0..ne-1] of extensible_recipie
 * param:	array [0..np-1] of fix_word
 */

#ifdef ASCIIJTEX
/*
 * A char_type_word is build of two unsigned sixteen-bit quantities.
 * The first is a Kanji code in JIS coding which does not have default
 * char_info (these code must be sorted). The second is an index
 * into the char_info table.
 */
struct char_type_word {
	i16	ct_code;	/* kanji code */
	i16	ct_info;	/* char_info index */
};
#endif /* ASCIIJTEX */

/*
 * A char_info_word is built of four unsigned eight-bit quantities.  The first
 * is an index into the width table (this saves 24 bits for every
 * character that has the same width as another character).  The
 * second is a composite height and depth index.  The third is a
 * composite italic index and tag, and the fourth is a remainder.
 *
 * XXX needs explaining
 */
struct char_info_word {
	char	ci_width;	/* width index */
	char	ci_h_d;		/* height and depth index */
	char	ci_i_t;		/* italic index and tag */
	char	ci_remainder;	/* ??? */
};

/*
 * These macros split up h_and_d and i_and_t values.
 */
#define	T_CI_H(ci) (((ci)->ci_h_d >> 4) & 0xf)
#define	T_CI_D(ci) ((ci)->ci_h_d & 0xf)
#define	T_CI_I(ci) (((ci)->ci_i_t >> 2) & 0x3f)
#define	T_CI_T(ci) ((ci)->ci_i_t & 3)

#if defined(ASCIIJTEX) && defined(FAKEKANJI)
/*
 * A glue_kern_word is built of four unsigned eight-bit quantities.
 * If the first is greater than or equals to 128, this word is the
 * last word of the sequence. The second is a character type of the
 * next character. If the third is less than 128, remainder (i.e.
 * the forth) is an index for glue table, otherwise for kern table.
 *   NOTE: in TFM (not JFM), this word is named lig_kern
 *	   and has different meanings.
 */
struct glue_kern_word {
	unsigned char	gk_seqend;	/* end of sequence or not */
	unsigned char	gk_type;	/* type of next char */
	unsigned char	gk_gk;		/* glue or kern */
	unsigned char	gk_remainder;	/* index for glue or kern */
};

/*
 * A glue_3words is built or three signed thirty-two-bit quantities.
 * NE indicates the number of words in this table.
 *   NOTE: in TFM (not JFM), this word is named extention.
 */
struct glue_3words {
	i32	g_width;
	i32	g_stretch;
	i32	g_shrink;
};
#endif /* ASCIIJTEX && FAKEKANJI */

/*
 * This structure contains everything one might need to know about
 * a TFM file at run-time.
 *
 * XXX incomplete, or wrong, as yet
 */
struct tfmdata {
	struct	tfmheader t_hdr;	/* full header */
#ifdef ASCIIJTEX
	struct	char_type_word *t_ct;	/* char type */
#endif /* ASCIIJTEX */
	struct	char_info_word *t_ci;	/* char info */
	i32	*t_width;		/* widths table */
	i32	*t_height;		/* heights */
	i32	*t_depth;		/* depths */
#if defined(ASCIIJTEX) && defined(FAKEKANJI)
	i32	*t_italic;		/* italic table */
	struct	glue_kern_word *t_gk;	/* glue_kern type (only in JFM) */
	i32	*t_kern;		/* kern table */
	struct	glue_3words    *t_glue;	/* glue table (only in JFM) */
#endif /* ASCIIJTEX && FAKEKANJI */
};
