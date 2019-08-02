/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: pxl.c,v 1.8 89/12/28 21:05:00 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/12/28 21:05:00 $
 */

/*
 * pxl font file functions.
 * ========================
 */

#include "pxl.h"

int pxl_init(), pxl_open(), pxl_free();
CharEntry *pxl_get_glyph();
FontOps pxlops = {
  {"pxl", 5.0, pxl_init, pxl_open, pxl_get_glyph, pxl_free},
  NULL
};


static int
pxl_init()
{
  return(True);
}

static int
pxl_open(fe)
register FontEntry *fe;
{
  register uint t;

  if (search_font_file(fe)) {
    register FILE *pxlfp;

    if (!(pxlfp = open_file(fe)))
      return(False);
    fseek(pxlfp, 0L, 0);
    if ((t = get_unsigned4(pxlfp)) != PXLID) goto bad_version;
    fseek(pxlfp, -4L, 2);
    if ((t = get_unsigned4(pxlfp)) != PXLID) goto bad_version;
    return(True);
  }
  return(False);
 bad_version:
  close_file(fe);
  prerror("Bad PXL file version %d\n", t);
  return(False);
}

static int
read_pxl_header(fe)
register FontEntry *fe;
{
  register int i;
  register uint t;
  register FILE *pxlfp;
  register PxlEntry *pxl;

  if (!(pxlfp = open_file(fe)))
    return(False);
  fseek(pxlfp, -20L, 2);
  t = get_unsigned4(pxlfp);
  if ((fe->common.c != 0) && (t != 0) && (fe->common.c != t))
    prerror("Bad PXL checksum %s\n", fe->common.name);

  /* allocate memory for PxlEntry */
  fe->pxl = pxl = (PxlEntry *) AllocMemory(sizeof(PxlEntry));

  pxl->magnification = get_unsigned4(pxlfp);
  pxl->designsize = get_unsigned4(pxlfp);

  fseek(pxlfp, (long) (get_unsigned4(pxlfp) * 4), 0);

  /* allocate memory for CharEntry */
  for (i = FIRSTPXLCHAR; i <= LASTPXLCHAR; ++i) {
    pxl->ch[i].width = (ushort) get_unsigned2(pxlfp);
    pxl->ch[i].height = (ushort) get_unsigned2(pxlfp);
    pxl->ch[i].xOffset= (short) get_signed2(pxlfp);
    pxl->ch[i].yOffset = (short) get_signed2(pxlfp);
    pxl->ch[i].where.isloaded = False;
    pxl->ch[i].where.address.fileOffset = (long) (get_unsigned4(pxlfp) * 4);
    pxl->ch[i].tfmw =
      ((float)get_unsigned4(pxlfp) * (float) fe->common.s) / (float) (1<<20);
  }
  return(True);
}

static CharEntry *
pxl_get_glyph(fe, c)
register FontEntry *fe;
register int c;
{
  register CharEntry *ptr;
  register FILE *pxlfp;
  register int uwidth, i, col, restbytes;
  register UTYPE *dp0, *dp, *sp;
  static uchar buf[4];

  if (c < FIRSTPXLCHAR || LASTPXLCHAR < c) {
    prerror("pxl font bad charcter data\n");
    return(NULL);
  }
  /* If pxl header is not loaded, then load header */
  if (!fe->pxl && !read_pxl_header(fe)) return(NULL);

  ptr = &fe->pxl->ch[c];
  /* If bad character is specified, then return NULL */
  if (ptr->where.address.fileOffset <= 0) {
    ptr->where.address.bitmap = NULL;
    return(NULL);
  }
  /* If the bitmap is already loaded on memory, then simply return. */
  if (ptr->where.isloaded) return(ptr);

  if (!(pxlfp = open_file(fe))) return(NULL);
  fseek(pxlfp, ptr->where.address.fileOffset, 0);

  uwidth = UWIDTH(ptr->width);
  dp = dp0 = (UTYPE *) AllocMemory(uwidth * ptr->height * UBYTES);
  ptr->where.address.bitmap = (char *) dp;
  for (col = ptr->height; col-- > 0; ) {
    restbytes = 0;
    for(i = uwidth; i-- > 0; ) {
      if (restbytes == 0) {
	fread((char *) buf, sizeof(uchar), 4, pxlfp);
	sp = (UTYPE *) buf;
	restbytes = 4;
      }
      *dp++ = *sp++;
      restbytes -= UBYTES;
    }
    dp = dp0 += uwidth;
  }
  ptr->where.isloaded = True;
  ptr->glyph_attribute = 0;
  fe->common.access_count++;

  return(ptr);
}

static int
pxl_free(fe)
FontEntry *fe;
{
  return(True);
}
