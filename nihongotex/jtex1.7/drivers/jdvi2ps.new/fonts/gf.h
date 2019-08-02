/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: gf.h,v 1.4 89/12/28 21:06:07 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/12/28 21:06:07 $
 */

/* This is a header file for gf font file. */

#include "font.h"
#include "common.h"

/*
 * gf file constants
 */

#define	NGFCHARS	256
#define	FIRSTGFCHAR	0
#define	LASTGFCHAR	255

/* The following macros describe gf file format */

#define paint_0		0
#define last_paint	63
#define paint1		64
#define paint2		65
#define paint3		66
#define boc		67
#define boc1		68
#define eoc		69
#define skip0		70
#define skip1		71
#define skip2		72
#define skip3		73
#define new_row_0	74
#define last_new_row	238
#define xxx1		239
#define xxx2		240
#define xxx3		241
#define xxx4		242
#define yyy		243
#define no_op		244
#define char_loc	245
#define char_loc0	246
#define pre		247
#define post		248
#define postpost	249
#define undefined_cases	250: case 251: case 252: case 253: case 254: case 255
#define gf_version	131

/*
 * Data Structures
 * ===============
 */

/*
 * Font infomation.
 */

typedef struct gf_entry {
	int designsize;
	int checksum;
	int hppp;
	int vppp;
	int font_min_m;
	int font_max_m;
	int font_min_n;
	int font_max_n;
	CharEntry ch[NGFCHARS];
} GfEntry;

typedef struct font_entry {
  /* common fields (defined in font.h) */
  CommonEntry common;
  /* gf file specific fields */
  GfEntry *gf;
} FontEntry;
