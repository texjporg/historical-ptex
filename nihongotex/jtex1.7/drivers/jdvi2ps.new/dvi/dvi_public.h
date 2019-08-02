/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: dvi_public.h,v 1.3 90/01/11 10:51:40 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 90/01/11 10:51:40 $
 */

/*
 * type definitions for handling DVI file.
 */

#include <stdio.h>
#include <sys/param.h>
#include "types.h"

typedef struct {
  void (*error_handler)();
  bool (*init_dvi)(), (*check_sheet)(), (*bop)(), (*eop)();
  void (*set_fnt_num)(), (*set_char)(), (*set_rule)(), (*do_special)();
} AppInfo;

typedef struct {
  int sheet_page;	/* Page number of each sheet. */
  long sheet_table;	/* pointers to start of each page in file */
  int size_known;
  int minh, minv, maxh, maxv;
  int pminh, pminv, pmaxh, pmaxv;
} DVIPages;

typedef struct {
  char filename[MAXPATHLEN];
  FILE *dvifp; /* File pointer */
  uint num;     /* numerator specified in preamble          */
  uint den;     /* denominator specified in preamble        */
  uint mag;     /* magnification specified in preamble      */
  uint hpd;
  uint wpw;
  ushort stack_size;   /* stack size */
  ushort NumberOfPage;
  ushort last_known_sheet;	/* Points to table at next unread sheet */
  DVIPages *pages;
} DVI;

extern bool InitDVIFile();
extern bool ProcessPage();
extern DVI dvi_info;
extern DVIPages *CurrentPage;
