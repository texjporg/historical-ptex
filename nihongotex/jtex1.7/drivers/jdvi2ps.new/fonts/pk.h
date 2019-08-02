/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: pk.h,v 1.4 89/12/28 21:06:14 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/12/28 21:06:14 $
 */

/* This is a header file for pk font file. */

#include "font.h"
#include "common.h"

/*
 * pk file constants
 */

#define	NPKCHARS	128
#define	min(x,y)	((x)<=(y) ? (x) : (y))

/* The following macros describe pk file format */

#define PK_ID		89
#define PK_FLAG		239
#define PK_XXX1		240
#define PK_XXX2		241
#define PK_XXX3		242
#define PK_XXX4		243
#define PK_YYY		244
#define PK_POST		245
#define PK_NO_OP	246
#define PK_PRE		247

/*
 * Data Structures
 * ===============
 */

/*
 * Font infomation.
 */

typedef struct pk_entry {
	int	n;
	CharEntry ch[NPKCHARS];
} PkEntry;

typedef struct font_entry {
  /* common fields (defined in font.h) */
  CommonEntry common;
  /* pk file specific fields */
  PkEntry *pk;
} FontEntry;
