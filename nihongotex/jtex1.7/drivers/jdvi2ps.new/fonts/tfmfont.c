/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: tfmfont.c,v 1.7 90/01/08 18:56:17 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 90/01/08 18:56:17 $
 */

/*
 * tfm font file functions.
 * ========================
 */

#include "tfmfont.h"


int tfm_init(), tfm_open(), tfm_free();
CharEntry *box_get_glyph(), *blank_get_glyph();

/* magnifications are unused in tfm fonts */
FontOps tfmboxops = {
  {"tfm", 0.0, tfm_init, tfm_open, box_get_glyph, tfm_free},
  NULL};
FontOps tfmblankops = {
  {"tfm", 0.0, tfm_init, tfm_open, blank_get_glyph, tfm_free},
  NULL};


static int
tfm_init()
{
  return(True);
}

static int
tfm_open(fe)
register FontEntry *fe;
{
  register TfmEntry *tfmfont;
  register TFMTable *tp;
  register int cnum;

  /* allocate memory for TfmEntry */
  fe->tfmfont = tfmfont = (TfmEntry *) AllocMemory(sizeof(TfmEntry));

  if (!(tp = read_tfm(fe->common.n, &fe->common.name)))
    return(False);

  if (tp->is_jfm) return(False);

  tfmfont->tfm = tp->data.tfm;
  cnum = tfmfont->tfm->ec - tfmfont->tfm->bc + 1;
  tfmfont->ch = (CharEntry *) AllocMemory(sizeof(CharEntry) * cnum);
  for (; --cnum >= 0; ) {
    tfmfont->ch[cnum].where.isloaded = False;
    tfmfont->ch[cnum].glyph_attribute = 0;
  }
  prerror("Font %s [%d] not found: Using dummy [box/blank] font.\n",
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
  register int index = c - fe->tfmfont->tfm->bc;
  register CharEntry *ptr = &fe->tfmfont->ch[index];
  register TFM *tfm = fe->tfmfont->tfm;

  /* If the bitmap is already loaded on memory, then simply return. */
  if (ptr->where.isloaded) return(ptr);

  if (tfm->info[index].width_ix == 0)
      return(NULL);

  ptr->xOffset = (short) 0;
  ptr->yOffset = (short) 0;
  ptr->tfmw = (int) ((double) fe->common.s *
		     (double) tfm->width[tfm->info[index].width_ix] /
		     (double) (0x1 << 20));
  ptr->width = (ushort) 0;
  ptr->height = (ushort) 0;
  ptr->where.isloaded = True;
  ptr->where.address.bitmap = (char *) NULL;

  return(ptr);
}

/*
 * Read a Box font.
 */
static CharEntry *
box_get_glyph(fe, c)
register FontEntry *fe;
register int c;
{
  register int index = c - fe->tfmfont->tfm->bc, size;
  register CharEntry *ptr = &fe->tfmfont->ch[index];
  register TFM *tfm = fe->tfmfont->tfm;

  /* If the bitmap is already loaded on memory, then simply return. */
  if (ptr->where.isloaded) return(ptr);

  if (tfm->info[index].width_ix == 0)
      return(NULL);

  ptr->xOffset = (short) 0;
  ptr->yOffset = (short)
    VConv((int) ((double) fe->common.s *
		 (double) tfm->height[HEIGHT(tfm->info[index].height_depth_ix)] /
		 (double) (0x1 << 20)));
  ptr->tfmw = (int) ((double) fe->common.s *
		     (double) tfm->width[tfm->info[index].width_ix] /
		     (double) (0x1 << 20));
  ptr->width = (ushort) HConv((int) ptr->tfmw);
  ptr->height = (ushort)
    (ptr->yOffset +
     VConv((int) ((double) fe->common.s *
		  (double) tfm->depth[DEPTH(tfm->info[index].height_depth_ix)] /
		  (double) (0x1 << 20))));

  ptr->where.isloaded = True;
  fe->common.access_count++;

  size = UWIDTH(ptr->width) * UBYTES * ptr->height;
  ptr->where.address.bitmap = (char *) AllocMemory(size);
  bzero(ptr->where.address.bitmap, size);

  DrawRectangle(ptr->where.address.bitmap, (int) ptr->width, (int) ptr->height,
		(int) (UWIDTH(ptr->width) * UBYTES), 0, 0,
		(int) ptr->width, (int) ptr->height, 2);

  return(ptr);
}

static int
tfm_free(fe)
FontEntry *fe;
{
  free_tfm();
  return(True);
}
