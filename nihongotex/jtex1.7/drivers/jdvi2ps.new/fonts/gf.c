/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: gf.c,v 1.7 89/12/28 21:05:32 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/12/28 21:05:32 $
 */

/*
 * gf font file functions.
 * =======================
 */

#include "gf.h"

int		gf_init(), gf_open(), gf_free();
CharEntry	*gf_get_glyph();

void	gf_beginc(), gf_paint(), gf_skip();

static  FILE	*gffp;
static	int	min_m,max_m,min_n,max_n;
static	int	rowbytes, gf_m0, gf_m, gf_n, paint_switch;
static	uchar	*row_ptr;
static	uchar	*raster;

#define row_start(n) (&(raster[n*rowbytes]))
#define m_byte(m) ((m)>>3)
#define m_bit(m) (7-(m)&07)

FontOps gfops = {
  {"gf", 1.0, gf_init, gf_open, gf_get_glyph, gf_free},
  NULL
};

static int
gf_init()
{
  return(True);
}

static int
gf_open(fe)
register FontEntry *fe;
{
  register uint t;

  if (search_font_file(fe)) {
    if (!(gffp = open_file(fe)))
      return(False);
    fseek(gffp, 1L, 0);
    if ((t = get_unsigned1(gffp)) != gf_version) goto bad_version;
    return(True);
  }
  return(False);
 bad_version:
  close_file(fe);
  prerror("Bad GF file version %d\n", t);
  return(False);
}

/* All gf file input goes through the following routines */

#define skip1byte()   getc(gffp)
#define skip_bytes(n) fseek(gffp,(long)(n),1)
#define getbyte() get_unsigned1(gffp)
#define get2bytes() get_unsigned2(gffp)
#define get3bytes() get_unsigned3(gffp)
#define get4bytes() get_unsigned4(gffp)

static int
gettochar(ptr)
register CharEntry *ptr;
{
  register int c;		/* the character code to be returned */
  register int x;		/* temporary */

  for(;;) switch(getbyte()) {
  case yyy: skip1byte();	/* intended to fall through 3 times */
  case paint3: case skip3: skip1byte();
  case paint2: case skip2: skip1byte();
  case paint1: case skip1: skip1byte(); continue;

  case boc:			/* set bitmap width,height,offset */
    c = get4bytes();
    get4bytes();		/* backpointer */
    min_m = get4bytes();
    max_m = get4bytes();
    min_n = get4bytes();
    max_n = get4bytes();
    ptr->width = max_m - min_m + 1;
    ptr->height = max_n - min_n + 1;
    ptr->xOffset = - min_m;
    ptr->yOffset = max_n;
    return(c & 0377);

  case boc1:			/* set bitmap width,height,offset */
    c = getbyte();
    x = getbyte();		/* del_m */
    max_m = getbyte();
    min_m = max_m - x;
    x = getbyte();		/* del_n */
    max_n = getbyte();
    min_n = max_n - x;
    ptr->width = max_m - min_m + 1;
    ptr->height = max_n - min_n + 1;
    ptr->xOffset = - min_m;
    ptr->yOffset = max_n;
    return(c);

  case pre:
    if (getbyte() != gf_version) {
      prerror("gf : file version error\n");
      exit(1);
    }
    skip_bytes(getbyte());
    continue;

  case xxx1: skip_bytes(getbyte()); continue;
  case xxx2: skip_bytes(get2bytes()); continue;
  case xxx3: skip_bytes((int) get3bytes()); continue;
  case xxx4: skip_bytes((int) get4bytes()); continue;

  case post: return(-1);

  case char_loc: case char_loc0: case postpost: case undefined_cases:
    prerror("gf : file command error\n");
    exit(1);

  default: /* do nothing */;
  }
}

/* readbits reads a raster description from the gf file and uses the external
   routines to actually process the raster information.
*/

static void
readbits()
{
  register uchar b;

  gf_beginc();
  for (;;) {
    b = getbyte();
    if (b <= last_paint) gf_paint(b - paint_0);
    if (b < new_row_0) {
      switch(b) {
      case paint1: gf_paint(getbyte()); continue;
      case paint2: gf_paint(get2bytes()); continue;
      case paint3: gf_paint((int) get3bytes()); continue;
      case boc: case boc1: prerror("gf : bad gf format [boc,boc1]\n"); exit(1);
      case eoc: return;
      case skip0: gf_skip(0); continue;
      case skip1: gf_skip(getbyte()); continue;
      case skip2: gf_skip(get2bytes()); continue;
      case skip3: gf_skip((int) get3bytes()); continue;
      }
    } else if (b <= last_new_row) {
      gf_skip(0);
      gf_paint(b - new_row_0);
    } else {
      switch(b) {
      case xxx1: skip_bytes(getbyte()); continue;
      case xxx2: skip_bytes(get2bytes()); continue;
      case xxx3: skip_bytes((int) get3bytes()); continue;
      case xxx4: skip_bytes((int) get4bytes()); continue;
      case yyy: get4bytes(); continue;
      case no_op: continue;
      default: prerror("gf : bad command\n"); exit(1);
      }
    }
  }
}

static void
gf_beginc()
{
  register int num_bytes;

  rowbytes = UWIDTH(max_m - min_m + 1) * UBYTES;
  num_bytes=(max_n - min_n + 1) * rowbytes;	/* bitmap used size */
  /*	gf_m = gf_m0 = min_m;*/
  gf_m = gf_m0 = 0;
  /*	gf_n = min_n;*/
  gf_n = 0;
  row_ptr = row_start(gf_n);	/* set bitmap area top address */
  bzero(raster, num_bytes);	/* bitmap area clear */
  paint_switch = 0;
}

static void
gf_paint(d)
register int d;
{
  if (d > 0) {
    if (paint_switch) {
      while (d-- > 0) {
	row_ptr[m_byte(gf_m)] |= 1 << m_bit(gf_m);
	gf_m++;
      }
    } else {
      gf_m += d;
    }
  }
  paint_switch ^= 1;
}

static void
gf_skip(d)
register int d;
{
  gf_n += d+1;
  gf_m = gf_m0;
  row_ptr = row_start(gf_n);
  paint_switch = 0;
}

static int
read_gf_header(fe)
register FontEntry *fe;
{
  register int c;
  register CharEntry *ptr;

  if (!(gffp = open_file(fe))) return(False);

  /* Seek postamble */
  fseek(gffp, -5L, 2);		/* skip four 223's */
  do {
    c = getbyte();
    fseek(gffp, -2L, 1);
  } while (c == 223);
  if (c != gf_version) {
    prerror("gf : bad gf version\n");	/* check version number */
    return(False);
  }
  fseek(gffp, -3L, 1);		/* back up to the pointer */
  if (fseek(gffp, (long) get4bytes(), 0) < 0) {
    prerror("gf : bad postamble point\n");
    return(False);
  }
  if (getbyte() != post) {
    prerror("gf : bad postamble\n");
    return(False);
  }

  /* Allocate memory for gf entries */
  fe->gf = (GfEntry *) AllocMemory(sizeof(GfEntry));
  ptr = fe->gf->ch;
  for (c = FIRSTGFCHAR; c <= LASTGFCHAR; ++c) {
    ptr[c].where.isloaded = False;
    ptr[c].where.address.fileOffset = 0;
    ptr[c].glyph_attribute = 0;
  }

  /* read postamble of font file */
  get4bytes();	/* ignore back pointer to font-wide xxx commands */
  fe->gf->designsize = get4bytes();
  fe->gf->checksum = get4bytes();
  fe->gf->hppp = get4bytes();
  fe->gf->vppp = get4bytes();
  fe->gf->font_min_m = get4bytes();
  fe->gf->font_max_m = get4bytes();
  fe->gf->font_min_n = get4bytes();
  fe->gf->font_max_n = get4bytes();

  for (;;) {
    switch (getbyte()) {
    case char_loc0:
      if ((c = getbyte()) < FIRSTGFCHAR || LASTGFCHAR < c) {
	prerror("gf font bad charcter data\n");
	return(False);
      }
      getbyte(); /* character width in pixels, rounded if necessary */
      ptr[c].tfmw = fix(((float) get4bytes() * (float) fe->common.s)
			/ (float)(1 << 20));	/* advance width */
      /* bitmap offset address of font file */
      ptr[c].where.address.fileOffset = get4bytes();
      ptr[c].where.isloaded = False;
      continue;
    case char_loc:
      if ((c = getbyte()) < FIRSTGFCHAR || LASTGFCHAR < c) {
	prerror("gf font bad charcter data\n");
	return(False);
      }
      get4bytes(); /* character width in pixels, rounded if necessary */
      get4bytes(); /* skip dy */
      ptr[c].tfmw = fix(((float) get4bytes() * (float) fe->common.s)
			/ (float)(1 << 20));	/* advance width */
      /* bitmap offset address of font file */
      ptr[c].where.address.fileOffset = get4bytes();
      ptr[c].where.isloaded = False;
      continue;
    case postpost:
      break;
    }
    break;
  }

  return(True);
}

static CharEntry *
gf_get_glyph(fe,c)
register FontEntry *fe;
register int c;
{
  register CharEntry *ptr;
  register int	nchars;

  if (c < FIRSTGFCHAR || LASTGFCHAR < c) {
    prerror("gf font bad charcter data\n");
    return(NULL);
  }
  /* If gf header is not loaded, then load header */
  if (!fe->gf && !read_gf_header(fe)) return(NULL);

  ptr = &fe->gf->ch[c];
  /* If bad character is specified ,then return NULL */
  if (ptr->where.address.fileOffset == 0) {
    ptr->where.address.bitmap = NULL;
    return(NULL);
  }

  /* If the bitmap is alread loaded on memory, then simply return. */
  if (ptr->where.isloaded) return(ptr);

  if (!(gffp = open_file(fe))) return(NULL);
  fseek(gffp, ptr->where.address.fileOffset, 0);

  gettochar(ptr);			/* read bitmap infomation */

  nchars = UWIDTH(ptr->width) * UBYTES;
  raster = (uchar *) AllocMemory(nchars * ptr->height); /* keep bitmap area */

  readbits();			/* make bitmap */

  ptr->where.address.bitmap = (char *) raster; /* bitmap address set */
  ptr->where.isloaded = True;
  fe->common.access_count++;

  return(ptr);
}

static gf_free(fe)
FontEntry *fe;
{
  return(True);
}
