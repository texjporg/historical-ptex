/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: pxl.h,v 1.3 89/09/19 19:55:48 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/09/19 19:55:48 $
 */

/* This is a header file for pxl font file. */
#include "font.h"
#include "common.h"

/*
 * pxl file constants
 */
#define  NPXLCHARS      128
#define  FIRSTPXLCHAR     0
#define  LASTPXLCHAR    127
#define  PXLID         1001

/*
 * Data Structures
 * ===============
 */

/*
 * Font information.
 */

typedef struct pxl_entry {
  uint magnification;	/* magnification read from PXL file */
  uint designsize;	/* design size read from PXL file */
  CharEntry ch[NPXLCHARS];/* character information */
} PxlEntry;

typedef struct font_entry
{
  /* common fields (defined in font.h) */
  CommonEntry common;
  /* pxl file specific fields */
  PxlEntry *pxl;
} FontEntry;
