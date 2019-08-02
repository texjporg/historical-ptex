/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: tfmfont.h,v 1.3 89/09/19 19:55:52 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/09/19 19:55:52 $
 */

#include "font.h"
#include "tfm.h"

typedef struct tfm_entry {
  TFM *tfm;
  CharEntry *ch;
} TfmEntry;

typedef struct font_entry
{
  /* common fields (defined in font.h) */
  CommonEntry common;
  /* tfm file specific fields */
  TfmEntry *tfmfont;
} FontEntry;
