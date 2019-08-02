/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: jfmfont.h,v 1.5 90/01/08 18:56:34 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 90/01/08 18:56:34 $
 */

#include "font.h"
#include "tfm.h"

typedef struct tfm_entry {
  ushort width, height;
  char *bitmap;
  JFM *jfm;
  KCharEntry *ch;
} JfmEntry;

typedef struct font_entry
{
  /* common fields (defined in font.h) */
  CommonEntry common;
  /* tfm file specific fields */
  JfmEntry *jfmfont;
} FontEntry;
