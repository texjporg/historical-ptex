/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: tfm.h,v 1.3 89/09/19 19:55:50 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/09/19 19:55:50 $
 */

/* This is a header file for tfm font file. */

#include "common.h"

/*
 * TFM file information.
 */

/*
 * The remainder of the TFM file comprises the following information,
 * all of which are 32 bit quantities:
 *
 * header:	array [0..lh-1] of stuff
 * char_info:	array [bc..ec] of char_info_word
 * width:	array [0..nw-1] of fix_word
 * height:	array [0..nh-1] of fix_word
 * depth:	array [0..nd-1] of fix_word
 * italic:	array [0..ni-1] of fix_word
 * lig_kern:	array [0..nl-1] of lig_kern_command
 * kern:	array [0..ne-1] of extensible_recipie
 * param:	array [0..np-1] of fix_word
 */

/*
 * A char_info_word is built of four unsigned eight-bit quantities.  The first
 * is an index into the width table (this saves 24 bits for every
 * character that has the same width as another character).  The
 * second is a composite height and depth index.  The third is a
 * composite italic index and tag, and the fourth is a remainder.
 *
 */
typedef struct {
  uchar   width_ix;               /* index to width */
  uchar   height_depth_ix;        /* index to height & depth */
  uchar   italic_ix_tag;          /* index to italic & tag */
  uchar   remainder_ix;           /* index to remainder */
} CharInfo;

typedef struct tfm {
  uchar bc, ec;
  CharInfo *info;
  int *width, *height, *depth;
} TFM;

/* .jfm file format */
#define JFM_ID  11

/*
 * jfm file information
 */

typedef struct {
  ushort  code;           /* JIS code */
  ushort  index;          /* index to char_info */
} JFMCharType;

typedef struct jfm {
  ushort nt;
  JFMCharType *type;
  CharInfo *info;
  int *width, *height, *depth;
} JFM;

typedef struct tfm_table {
  char *name, *filename;
  struct tfm_table *next;
  uchar is_jfm;
  union {
    TFM *tfm;
    JFM *jfm;
  } data;
} TFMTable;


/*
 * These macros split up h_and_d and i_and_t values.
 */
#define HEIGHT(h_d) ( ((h_d)>>4)&0x0f )
#define DEPTH(h_d)  ( (h_d)&0x0f )
#define ITALIC(h_d) ( ((h_d)>>2)&0x3f )

#define TFMTABLESIZE 97

extern TFMTable *read_tfm();
extern void free_tfm();
extern ushort find_type();
