/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: tfm.c,v 1.4 89/11/10 16:07:30 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/11/10 16:07:30 $
 */

/*
 * tfm font file functions.
 * ========================
 */

#include <stdio.h>
#include "font.h"
#include "tfm.h"


TFMTable *search_hash_table();
TFMTable *read_tfm_file(), *read_jfm_file();
CharInfo *read_char_info();
int *read_four_bytes();


TFMTable *
read_tfm(fn, tfmname)
char *fn, **tfmname;
{
  register TFMTable *ttp = search_hash_table(fn, tfmname);
  register ushort id;
  register FILE *tfmfp;

  if (!ttp) return(NULL);
  else if (ttp->data.tfm) return(ttp);

  if (!(tfmfp = fopen(*tfmname, "r"))) {
    prerror("can't open %s\n", *tfmname);
    return(NULL);
  }

  id = (ushort) get_unsigned2(tfmfp);

  /* test jfm id field. */
  if (id == JFM_ID) return(read_jfm_file(ttp, tfmfp, id));
  else return(read_tfm_file(ttp, tfmfp, id));
}

static TFMTable *
read_tfm_file(ttp, tfmfp, lf)
register TFMTable *ttp;
register FILE *tfmfp;
register ushort lf;
{
  register ushort lh, bc, ec, nw, nh, nd, ni, nl, nk, ne, np, nc;
  register TFM *tfm;

  ttp->data.tfm = tfm = (TFM *) AllocMemory(sizeof(TFM));
  ttp->is_jfm = 0;

  lh = (ushort) get_unsigned2(tfmfp);
  bc = tfm->bc = (ushort) get_unsigned2(tfmfp);
  ec = tfm->ec = (ushort) get_unsigned2(tfmfp);
  nw = (ushort) get_unsigned2(tfmfp);
  nh = (ushort) get_unsigned2(tfmfp);
  nd = (ushort) get_unsigned2(tfmfp);
  ni = (ushort) get_unsigned2(tfmfp);
  nl = (ushort) get_unsigned2(tfmfp);
  nk = (ushort) get_unsigned2(tfmfp);
  ne = (ushort) get_unsigned2(tfmfp);
  np = (ushort) get_unsigned2(tfmfp);

  nc = ec - bc + 1;

  (void) fseek(tfmfp, (long) (lh * 4), 1);

  /* read tfm charinfo, width, height, depth tables */
  if (!(tfm->info = read_char_info(tfmfp, nc)) ||
      !(tfm->width = read_four_bytes(tfmfp, nw)) ||
      !(tfm->height = read_four_bytes(tfmfp, nh)) ||
      !(tfm->depth = read_four_bytes(tfmfp, nd))) {
    fclose(tfmfp);
    return(NULL);
  }

  fclose(tfmfp);
  return(ttp);
}

static TFMTable *
read_jfm_file(ttp, tfmfp, id)
register TFMTable *ttp;
register FILE *tfmfp;
register ushort id;
{
  register int i;
  register ushort nt, lf, lh, bc, ec, nw, nh, nd, ni, nl, nk, ng, np;
  register JFM *jfm;
  register uint check_sum, design_size; /* header */

  ttp->data.jfm = jfm = (JFM *) AllocMemory(sizeof(JFM));
  ttp->is_jfm = 1;

  /* read jfm table field. */
  nt = jfm->nt = (ushort) get_unsigned2(tfmfp);
  lf = (ushort) get_unsigned2(tfmfp);
  lh = (ushort) get_unsigned2(tfmfp);
  bc = (ushort) get_unsigned2(tfmfp);
  ec = (ushort) get_unsigned2(tfmfp);
  nw = (ushort) get_unsigned2(tfmfp);
  nh = (ushort) get_unsigned2(tfmfp);
  nd = (ushort) get_unsigned2(tfmfp);
  ni = (ushort) get_unsigned2(tfmfp);
  nl = (ushort) get_unsigned2(tfmfp);
  nk = (ushort) get_unsigned2(tfmfp);
  ng = (ushort) get_unsigned2(tfmfp);
  np = (ushort) get_unsigned2(tfmfp);

  /* read jfm header */
  check_sum = get_unsigned4(tfmfp);
  design_size = get_unsigned4(tfmfp);
  fseek(tfmfp, (long) (sizeof(long) * (lh - 2)), 1);

  /* read jfm char_type */
  jfm->type = (JFMCharType *) AllocMemory(sizeof(JFMCharType) * nt);
  for (i = 0; i < nt; i++) {
    jfm->type[i].code = (ushort) get_unsigned2(tfmfp);
    jfm->type[i].index = (ushort) get_unsigned2(tfmfp);
  }

  /* read jfm charinfo, width, height, depth tables */
  if (!(jfm->info = read_char_info(tfmfp, ec + 1)) ||
      !(jfm->width = read_four_bytes(tfmfp, nw)) ||
      !(jfm->height = read_four_bytes(tfmfp, nh)) ||
      !(jfm->depth = read_four_bytes(tfmfp, nd))) {
    fclose(tfmfp);
    return(NULL);
  }

  fclose(tfmfp);
  return(ttp);
}

static CharInfo *
read_char_info(tfmfp, nc)
register FILE *tfmfp;
register int nc;
{
  register CharInfo *ci, *ret;

  ret = ci = (CharInfo *) AllocMemory(sizeof(CharInfo) * (nc));

  while (--nc >= 0) {
    ci->width_ix = (uchar) get_unsigned1(tfmfp);
    ci->height_depth_ix = (uchar) get_unsigned1(tfmfp);
    ci->italic_ix_tag = (uchar) get_unsigned1(tfmfp);
    ci->remainder_ix = (uchar) get_unsigned1(tfmfp);
    ci++;
  }
  if (feof(tfmfp))
    return(NULL);
  else return(ret);
}

static int *
read_four_bytes(tfmfp, nc)
register FILE *tfmfp;
register int nc;
{
  register int *tab, *ret;

  ret = tab = (int *) AllocMemory(sizeof(int) * nc);
  while (--nc >= 0)
    *tab++ = get_signed4(tfmfp);
  if (feof(tfmfp))
    return(NULL);
  else return(ret);
}

static int
HASH(name)
char name[];
{
  register int i, h = 0, l = strlen(name);

  for (i = 0; i < l; ++i)
    h += name[i] << i;
  return(h % TFMTABLESIZE);
}

/* This flag represents whether the hash table is initialized */
static int initialized = True;
static TFMTable *tfm_table[TFMTABLESIZE];

static TFMTable *
search_hash_table(fn, tfmname)
register char *fn, **tfmname;
{
  register TFMTable **ttp = &tfm_table[HASH(fn)];

  /* Search in the hash tree */
  while (*ttp) {
    if (!strcmp(fn, (*ttp)->name)) {
      /* If found */
      *tfmname = (*ttp)->filename;
      return(*ttp);
    } else ttp = &(*ttp)->next;
  }

  /* If not found */
  if (!search_tfm_file(fn, tfmname)) {
    prerror("%s.tfm [not found]\n", fn);
    return(NULL);
  }

  /* allocate memory for PxlEntry */
  *ttp = (TFMTable *) AllocMemory(sizeof(TFMTable));
  (*ttp)->name = fn;
  (*ttp)->filename = *tfmname;
  (*ttp)->next = (TFMTable *) NULL;
  (*ttp)->data.tfm = (TFM *) NULL;
  initialized = False;

  return(*ttp);
}

void
free_tfm()
{
  register int i;

  if (initialized) return;
  for (i = 0; i < TFMTABLESIZE; ++i)
    tfm_table[i] = (TFMTable *) NULL;
  initialized = True;
}

ushort
find_type(jfm, c)
register JFM *jfm;
register int c;
{
  register ushort nt = jfm->nt;
  register ushort min = jfm->type[1].code, max = jfm->type[nt - 1].code;
  register int i;

  if (c >= min && c <= max) {
    for (i = 1; i < nt; ++i)
      if (jfm->type[i].code == c) {
	goto found;
      }
  }
  return((ushort) 0);

found:
  return(jfm->type[i].index);
}

