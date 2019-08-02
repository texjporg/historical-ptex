/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: pk.c,v 1.7 89/12/28 21:05:35 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/12/28 21:05:35 $
 */

/*
 * pk font file functions.
 * =======================
 */

#include "pk.h"

int		pk_init(),pk_open(),pk_free();
CharEntry	*pk_get_glyph();

static FILE	*pkfp;
static char	pat[8] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01};

FontOps pkops = {
  {"pk", 1.0, pk_init, pk_open, pk_get_glyph, pk_free},
  NULL
};

static int
pk_init()
{
  return(True);
}

static int
pk_open(fe)
register FontEntry *fe;
{
  register uint t;

  if (search_font_file(fe)) {
    if (!(pkfp = open_file(fe)))
      return(False);
    fseek(pkfp, 1L, 0);
    if ((t = get_unsigned1(pkfp)) != PK_ID) goto bad_version;
    return(True);
  }
  return(False);
 bad_version:
  close_file(fe);
  prerror("Bad PK file version %d\n", t);
  return(False);
}

/*      unpack raster packet    */

static void
unpack_raster(p, width, height, b_width)
register char *p;
register int width, height, b_width;
{
  register int n = 0, m, t, h, v;

  for (v = height; v-- > 0; ) {
    for (h = 0; h < width; h += m) {
      if (n == 0) {
	t = get_unsigned1(pkfp);
	n = 8;
      }
      m = min(n, 8 - (h & 7));
      m = min(m, width - h);
      p[h >> 3] |= t >> (h & 7);
      t <<= m; n -= m;
    }
    if (width & 7) p[width >> 3] &= ~pat[width & 7];
    p += b_width;
  }
}

/*      get nybble      */

#define get_nyb()  ((nyb_f^=1)? ((nyb_w=get_unsigned1(pkfp))>>4): nyb_w&0x0f)

/*      unpack run_encoded packet       */

static int repeat_count, dyn_f, nyb_f, nyb_w;

static int
pk_packed_num()
{
  register int i = get_nyb(), j;

  if (i == 0) {
    do {
      j = get_nyb(); i++;
    } while (j == 0);
    while (i > 0) {
      j = (j << 4) + get_nyb(); i--;
    }
    return (j - 15 + ((13 - dyn_f) << 4) + dyn_f);
  } else if (i <= dyn_f) return(i);
  else if (i < 14)
    return(((i - dyn_f - 1) << 4) + get_nyb() + dyn_f + 1);
  else {
    if (repeat_count != 0) {
      prerror("Second repeat count for this row!\n");
      return(-1);
    }
    if (i == 14) repeat_count = pk_packed_num();
    else repeat_count = 1;
    return (pk_packed_num());
  }
}

static void
unpack_run(p, black, width, height, b_width)
register char *p;
register int black, width, height, b_width;
{
  register char *q;
  register int run = 0, h, h1, v;

  nyb_f = 0;
  black ^= 1;
  for (v = 0; v < height; v += repeat_count + 1) {
    repeat_count = 0;
    for (h = 0; h < width; h = h1) {
      if (run == 0) {
	/* get run_count/repeat_count */
	if ((run = pk_packed_num()) < 0) return;
	black ^= 1;
      }
      h1 = min(h + run, width);
      if (black) {
	h1 = min(h1, (h + 8) & ~7);
	p[h >> 3] |= pat[h & 7];
	if (h1 & 7) p[h1 >> 3] &= ~pat[h1 & 7];
      }
      run -= h1 - h;
    }
    q = p;  p += b_width;
    for (h = repeat_count * b_width; --h >= 0; ) {
      *p++ = *q++;
    }
  }
}

static int
read_pk_header(fe)
register FontEntry *fe;
{
  register int cmd, c;

  if (!(pkfp = open_file(fe))) return(False);
  fseek(pkfp, 0L, 0);

  for ( ; ; ) {
    switch(cmd = get_unsigned1(pkfp)) {
    case PK_PRE:
      if (get_unsigned1(pkfp) != PK_ID) {
	prerror("Bad PK file version %s\n", fe->common.name);
	return(False);
      }
      fseek(pkfp, (long) get_unsigned1(pkfp), 1);          /* comment */
      get_signed4(pkfp);                         /* design size */

      if ((c = get_signed4(pkfp)) && fe->common.c && c != fe->common.c) {
	prerror("Bad PK checksum %s\n", fe->common.name);
	return(False);
      }
      fseek(pkfp, 8L, 1);                        /* hppp,vppp */
      fe->pk = (PkEntry *) AllocMemory(sizeof(PkEntry));
      for (c = 0; c < NPKCHARS; ++c) {
	fe->pk->ch[c].where.isloaded = False;
	fe->pk->ch[c].where.address.fileOffset = 0;
      }
      continue;

    case PK_XXX1: case PK_XXX2: case PK_XXX3: case PK_XXX4:
      fseek(pkfp, (long) get_unsigned(pkfp, cmd - PK_XXX1 + 1), 1);
      continue;

    case PK_YYY:
      get_signed4(pkfp);
      continue;

    case PK_NO_OP:
      continue;

    case PK_POST:
      break;

    default:
      if (cmd <= PK_FLAG) {
	register CharEntry *ptr;
	register long pl;
	register int cmd1 = cmd & 0x7;

	if (cmd1 == 0x7) {             /* long form */
	  pl = get_signed4(pkfp);
	  ptr = &fe->pk->ch[get_signed4(pkfp)];
	} else if (cmd1 & 0x4) {             /* extended short form */
	  pl = ((cmd1 & 0x3) << 16) + get_unsigned2(pkfp);
	  ptr = &fe->pk->ch[get_unsigned1(pkfp)];
	} else {                        /* short form */
	  pl = ((cmd1 & 0x3) << 8) + get_unsigned1(pkfp);
	  ptr = &fe->pk->ch[get_unsigned1(pkfp)];
	}
	ptr->where.isloaded = False;
	ptr->where.address.fileOffset = ftell(pkfp);
	/* Use tfmw entry to save flag */
	ptr->tfmw = cmd;
	fseek(pkfp, pl, 1);
      } else prerror("PK : illegal PK command %d\n", cmd);
      continue;
    }
    break;
  }
  return(True);
}
    
static CharEntry *
pk_get_glyph(fe, c)
register FontEntry *fe;
register int c;
{
  register CharEntry *ptr;
  register int b_width, flag, nbytes;
  register char *pixel;

  if (!fe->pk && !read_pk_header(fe)) return(NULL);

  ptr = &fe->pk->ch[c];
  /* If bad character is specified, then return NULL */
  if (ptr->where.address.fileOffset == 0) {
    ptr->where.address.bitmap = NULL;
    return(NULL);
  }

  /* If the bitmap is alread loaded on memory, then simply return. */
  if (ptr->where.isloaded) return(ptr);

  if (!(pkfp = open_file(fe))) return(NULL);
  fseek(pkfp, ptr->where.address.fileOffset, 0);

  /* Get flag from tfmw entry */
  nbytes = (flag = ptr->tfmw) & 0x7;

  if (nbytes == 7) {             /* long form */
    ptr->tfmw = fix(((float) get_signed4(pkfp) * (float) fe->common.s)
		    / (float)(1 << 20));
    get_signed4(pkfp);         /* dx */
    nbytes = 4;
  } else if (nbytes >= 4) {             /* extended short form */
    ptr->tfmw = fix(((float) get_unsigned3(pkfp) * (float) fe->common.s)
		    / (float) (1 << 20));
    nbytes = 2;
  } else {                        /* short form */
    ptr->tfmw = fix(((float) get_unsigned3(pkfp) * (float) fe->common.s)
		    / (float) (1 << 20));
    nbytes = 1;
  }
  get_unsigned(pkfp, nbytes); /* dy/dm */
  ptr->width = get_unsigned(pkfp, nbytes);
  ptr->height = get_unsigned(pkfp, nbytes);
  ptr->xOffset = get_signed(pkfp, nbytes);
  ptr->yOffset = get_signed(pkfp, nbytes);

  b_width = UWIDTH(ptr->width) * UBYTES;
  pixel = ptr->where.address.bitmap =
    (char *) AllocMemory (ptr->height * b_width);
  bzero(pixel, ptr->height * b_width);

  if ((flag >> 4) == 14)
    unpack_raster(pixel, ptr->width, ptr->height, b_width);
  else {
    dyn_f = flag >> 4;
    unpack_run(pixel, (flag & 8) >> 3, ptr->width, ptr->height, b_width);
  }

  ptr->where.isloaded = True;
  ptr->glyph_attribute = 0;
  fe->common.access_count++;

  return(ptr);
}

static pk_free(fe)
FontEntry *fe;
{
  return(True);
}
