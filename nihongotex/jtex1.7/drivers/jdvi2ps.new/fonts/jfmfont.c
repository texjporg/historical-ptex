/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: jfmfont.c,v 1.5 90/01/08 18:56:21 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 90/01/08 18:56:21 $
 */

/*
 * jfm font file functions.
 * ========================
 */

#include "jfmfont.h"


int jfm_init(), jfm_blank_open(), jfm_box_open(), jfm_free();
CharEntry *box_get_glyph(), *blank_get_glyph();

/* magnifications are unused in jfm fonts */
FontOps jfmboxops = {
  {"jfm", 0.0, jfm_init, jfm_box_open, box_get_glyph, jfm_free},
  NULL};
FontOps jfmblankops = {
  {"jfm", 0.0, jfm_init, jfm_blank_open, blank_get_glyph, jfm_free},
  NULL};


static int
jfm_init()
{
  return(True);
}

static int
jfm_blank_open(fe)
register FontEntry *fe;
{
  register JfmEntry *jfmfont;
  register TFMTable *tp;

  /* allocate memory for JfmEntry */
  fe->jfmfont = jfmfont = (JfmEntry *) AllocMemory(sizeof(JfmEntry));

  if (!(tp = read_tfm(fe->common.n, &fe->common.name)))
    return(False);

  if (!tp->is_jfm) return(False);

  jfmfont->jfm = tp->data.jfm;
  jfmfont->width = 0;
  jfmfont->height = 0;
  jfmfont->bitmap = (char *) NULL;
  jfmfont->ch = (KCharEntry *) NULL;
/*
 * Do not output warning messages because this blank font will be often
 * used as dummy kanji font.
 *
 * prerror("Font %s [%d] not found: Using dummy blank font.\n",
 *         fe->common.n, fix(fe->common.font_mag));
 */
  return(True);
}

static int
jfm_box_open(fe)
register FontEntry *fe;
{
  register JfmEntry *jfmfont;
  register TFMTable *tp;
  register ushort dot;
  register int size;

  /* allocate memory for JfmEntry */
  fe->jfmfont = jfmfont = (JfmEntry *) AllocMemory(sizeof(JfmEntry));

  if (!(tp = read_tfm(fe->common.n, &fe->common.name)))
    return(False);

  if (!tp->is_jfm) return(False);

  jfmfont->jfm = tp->data.jfm;
  dot = fe->common.font_mag * fe->common.d / (double) ((0x1 << 16) * 72);
  jfmfont->width = dot;
  jfmfont->height = dot;
  size = UWIDTH(dot) * UBYTES * dot;
  jfmfont->bitmap = (char *) AllocMemory((ulong) size);
  jfmfont->ch = (KCharEntry *) NULL;
  bzero(jfmfont->bitmap, size);

  DrawRectangle(jfmfont->bitmap, (int) jfmfont->width, (int) jfmfont->height,
		(int) (UWIDTH(jfmfont->width) * UBYTES),
		0, 0, (int) jfmfont->width, (int) jfmfont->height, 2);
  prerror("Font %s [%d] not found: Using dummy box font.\n",
	  fe->common.n, fix(fe->common.font_mag));

  return(True);
}

/*
 * Read a Blank font.
 */
static CharEntry *
blank_get_glyph(fe, c)
register FontEntry *fe;
register int c;
{
  CharEntry *ch;
  register JFM *jfm;

  if (get_kchar_entry(&fe->jfmfont->ch, c, &ch)) goto found;

  jfm = fe->jfmfont->jfm;
  ch->where.isloaded = True;
  ch->where.address.bitmap = (char *) NULL;
  ch->width = (ushort) 0;
  ch->height = (ushort) 0;
  ch->xOffset = (short) 0;
  ch->yOffset = (short) 0;
  ch->tfmw = (double) fe->common.s *
    jfm->width[jfm->info[find_type(jfm, c)].width_ix] / (0x1 << 20);
  ch->glyph_attribute = 0;
  fe->common.access_count++;

 found:
  return(ch);
}

/*
 * Read a Box font.
 */
static CharEntry *
box_get_glyph(fe, c)
register FontEntry *fe;
register int c;
{
  CharEntry *ch;
  register JFM *jfm;
  register ushort type;

  if (get_kchar_entry(&fe->jfmfont->ch, c, &ch)) goto found;

  jfm = fe->jfmfont->jfm;
  type = find_type(jfm, c);
  ch->where.isloaded = True;
  ch->where.address.bitmap = fe->jfmfont->bitmap;
  ch->width = fe->jfmfont->width;
  ch->height = fe->jfmfont->height;
  ch->xOffset = (short) 0;
  ch->yOffset = (short)
    VConv((int) ((double) fe->common.s *
		 (double)jfm->height[HEIGHT(jfm->info[type].height_depth_ix)] /
		 (double) (0x1 << 20)));
  ch->tfmw = (double) fe->common.s *
    jfm->width[jfm->info[type].width_ix] / (0x1 << 20);
  ch->glyph_attribute = 0;
  fe->common.access_count++;

 found:
  return(ch);
}

static int
jfm_free(fe)
FontEntry *fe;
{
  free_tfm();
  return(True);
}
