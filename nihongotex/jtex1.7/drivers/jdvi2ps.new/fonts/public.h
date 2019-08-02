/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: public.h,v 1.4 89/12/28 21:06:17 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/12/28 21:06:17 $
 */

#include "font.h"

typedef struct font_entry {
  CommonEntry common;
  char *specific;
} FontEntry;

/*
 * public functions to applications
 */
extern void InitFont();
extern int OpenFont();
extern void CloseFonts();
extern FontEntry *GetFont();
extern CharEntry *GetGlyph();
extern void FontsForAll();

/*
 * public utility functions
 */
extern void FillRectangle(), DrawRectangle();
extern int get_signed();
extern unsigned int get_unsigned();
extern void prerror();

/*
 * Macro functions
 */
#define GetFontAttribute(fe) (fe->common.app_attribute)
#define PutFontAttribute(fe,att) (GetFontAttribute(fe)=(long)att)
#define GetGlyphAttribute(fe,c) (GetGlyph(fe,c)->glyph_attribute)
#define PutGlyphAttribute(fe,c,att) (GetGlyphAttribute(fe,c)=att)
