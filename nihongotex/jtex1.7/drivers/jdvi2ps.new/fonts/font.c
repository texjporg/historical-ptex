/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: font.c,v 1.13 90/02/15 15:54:39 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 90/02/15 15:54:39 $
 */

/*
 * font file manipulation functions.
 * ================================
 */

#include "public.h"
#include "common.h"
#include <string.h>
#include <malloc.h>

/*
 * Default font paths
 */
#ifndef DEFAULT_FONT_PATH
#define DEFAULT_FONT_PATH "/usr/lib/tex/fonts"
#endif
#ifndef DEFAULT_TFM_PATH
#define DEFAULT_TFM_PATH "/usr/lib/tex/fonts"
#endif

#define READ 4	/* Code for access() */

/*
 * Private data structures.
 */
typedef struct {
  double mag;
  uint resolution;
  char *font_path, *tfm_path;
} FontInfo;

FontInfo fontinfo;
int hconv, vconv; /* converts DVI units to pixels */

/*
 * number of currently opened files
 */
static int nopen = 0;

/*
 * The font entry table for lsb of font number in dvi.
 * The real cell is allocated dynamically when reads font definition operator
 * in dvi file.
 */
#define MAXFONTENTRY 0x100
static FontEntry *FontEntryTable[MAXFONTENTRY];

/*
 * Files which are now opened.
 */
static FontEntry *OpenFiles[MAXOPEN];

/*
 * Top font operation pointer.
 * Named font is searched in order according to this list.
 */
static FontOps *fontops = (FontOps *) NULL;

/*
 * private functions
 */
void FreeMemory(), ResetMemory();
double actual_factor();

/*
 * A macro function for adding new font file format.
 * Newly defined one is tested earlier then others.
 */
#define DEFFONT(x) { extern FontOps x;\
		     if ((*x.op.init_font)()) {\
		     x.next = fontops; fontops = &x;} }

/*
 * init_font():
 *
 * Initialize font utilities.
 * You must define available fonts using DEFFONT() macro.
 */

#include "init_font.c"

/*
 * unregister_file(i)
 *
 * Close file registered in OpenFiles[i] and unregister it.
 */
static void
unregister_file(i)
register int i;
{
  if (!OpenFiles[i]) return;
#ifdef DEBUG
  prerror("Closing font '%s' (fp:%x, idx:%d, used:%d)\n",
	  OpenFiles[i]->common.name, OpenFiles[i]->common.font_file_fd,
	  i, OpenFiles[i]->common.access_count);
#endif
  if (OpenFiles[i]->common.font_file_fd)
    fclose(OpenFiles[i]->common.font_file_fd);
  OpenFiles[i]->common.font_file_fd = NULL;
  OpenFiles[i] = OpenFiles[--nopen];
  OpenFiles[nopen] = (FontEntry *) NULL;
}

/*
 * open_file(fe):
 *
 * Keeps a cache (OpenFiles[]) of up to MAXOPEN open files.
 * When the file is not found in the cache, it must be opened.  In this case,
 * the next empty slot in the cache is assigned, or if the cache is full,
 * the least used font file is closed and its slot reassigned for the new
 * file.  Identification of the least used file is based on the counts of
 * the number of times each file (variable access_count in CommonEntry) has
 * been "accessed" by this routine.
 */

FILE *
open_file(fe)
register FontEntry *fe;
{
  /* If file is alread opened, then return it. */
  if (fe->common.font_file_fd) return(fe->common.font_file_fd);
  /* If less than MAXOPEN, then just open it. */
  /* Else, close the least used file. */
  else if (nopen >= MAXOPEN) {
    register int i, least_used;

  close_one:
    for (i = least_used = 0; i < nopen; ++i)
      if (OpenFiles[least_used]->common.access_count >
	  OpenFiles[i]->common.access_count)
	least_used = i;

    unregister_file(least_used);
  }

  /* Confirm accessibility of the file. */
  if (access(fe->common.name, READ) != 0) {
    prerror("Cant access font file %s", fe->common.name);
    return NULL;
  }
  /* Open the file.  If cannot be opened, then close one more file. */
  if ((fe->common.font_file_fd = fopen(fe->common.name, "r")) == NULL)
    goto close_one;

#ifdef DEBUG
  prerror("Opened (%d, %d) font '%s'\n", fe->common.k, nopen, fe->common.name);
#endif

  OpenFiles[nopen++] = fe;

  return(fe->common.font_file_fd);
}

/*
 * close_file(fe):
 *
 * Close font file.
 * Search font entry 'fe' registered in OpenFiles[], and close file.
 */

void
close_file(fe)
register FontEntry *fe;
{
  register int i;

  /* reset least used file pointer to NULL */
  for (i = 0; i < nopen; ++i)
    if (OpenFiles[i] == fe) {
      unregister_file(i);
      return;
    }
}

/*
 * search_font_file(fe):
 *
 * Search font file.
 * For each font directory specified by font_path,
 * this makes font path name and repeatedly tests whether it is readable.
 */

int
search_font_file(fe)
FontEntry *fe;
{
  register uint font_mag = (uint) fix(fe->common.font_mag * fe->common.font_ops->multiplier);

  /* calculate real font magnification for file name extension */
  return(test_file_access(fontinfo.font_path, fe->common.n, font_mag,
			  fe->common.font_ops->extension, &fe->common.name));
}

/*
 * search_tfm_file(fn, tfmname):
 *
 * Search tfm file.
 * For each font directory specified by tfm_path,
 * this makes tfm path name and repeatedly tests whether it is readable.
 */

int
search_tfm_file(fn, tfmname)
register char *fn, **tfmname;
{
  return(test_file_access(fontinfo.tfm_path, fn, (uint) 0, "tfm", tfmname));
}

/*
 * test_file_access(paths, name, mag, suffix, path)
 *
 * Test file access in search paths.
 * File name is made from name, mag and suffix.
 * An accessible file path name will be stored in a pointer 'path'.
 */

static int
test_file_access(paths, name, mag, suffix, path)
register char *paths, *name, *suffix, **path;
register uint mag;
{
  register char *tcp, *tcp1;
  static char curarea[MAXPATHLEN], buf[MAXPATHLEN];

  do { 
    if (!(tcp = strchr(paths, ':')))
      tcp = strlen(paths) + paths;
    strncpy(curarea, paths, tcp - paths);
    tcp1 = curarea + (tcp - paths);
    *tcp1++ = '/'; *tcp1++ = '\0';
    if (mag) sprintf(buf, "%s%s.%d%s", curarea, name, mag, suffix);
    else sprintf(buf, "%s%s.%s", curarea, name, suffix);
    if (access(buf, READ) == 0) goto found;
    if (*tcp) paths = tcp + 1;
    else paths = tcp;
  } while (*paths != '\0');
  return(False);

 found:
  *path = AllocMemory((ulong) (strlen(buf) + 1));
  strcpy(*path, buf);
  return(True);
}

/*
 * Followings are public functions for applications.
 */

/*
 * InitFont(num, den, mag, resolution, fpath, tpath):
 *
 * Initialize font utilities.
 * (1) Set conversion factor used in sp-to-pxl conversion macros VConv() and
 *     HConv().
 * (2) Set resolution and mag.
 *     They are used in a function OpenFont().
 * (3) Set font path and tfm font path.
 */

void
InitFont(num, den, mag, resolution, fpath, tpath)
register uint num, den, mag, resolution;
register char *fpath, *tpath;
{
  static bool Initialized = False;
  register double conv;
  extern char *getenv();

  fontinfo.resolution = resolution;
  fontinfo.mag = actual_factor(mag);

  conv = ((double) num / (double) den) * fontinfo.mag *
    ((double) resolution / 254000.0);
  hconv = vconv = fix(1.0 / conv);

  if (fpath) fontinfo.font_path = fpath;
  else if (!fontinfo.font_path) {
    if (!(fontinfo.font_path = getenv("TEXFONTPATH")))
      fontinfo.font_path = DEFAULT_FONT_PATH;
  }
  if (tpath) fontinfo.tfm_path = tpath;
  else if (!fontinfo.tfm_path) {
    if (!(fontinfo.tfm_path = getenv("TEXTFMPATH")))
      fontinfo.tfm_path = DEFAULT_TFM_PATH;
  }

#ifdef DEBUG
  prerror("Font path: %s\n", fontinfo.font_path);
  prerror("Tfm path: %s\n", fontinfo.tfm_path);
#endif

  if (!Initialized) {
    init_font();
    Initialized = True;
  }
}

/*
 * OpenFont(fe, c, s, d, a, l, n):
 *
 * Find existing font which meets a requirement (font number k) in the fe.
 * The different font format is available if it is defined in InitFont().
 * This attempts to open font in order according to the list which InitFont()
 * builds.
 */

int
OpenFont(fe, c, s, d, a, l, n)
register FontEntry *fe;
register unsigned int c, s, d;
register unsigned char a, l;
register char n[];
{
  register FontOps *op;

  fe->common.c = c; /* checksum */
  fe->common.s = s; /* space size */
  fe->common.d = d; /* design size */
  fe->common.a = a; /* area length for font name */
  fe->common.l = l; /* device length */
  fe->common.n = AllocMemory((ulong) (a + l + 1));
  strncpy(fe->common.n, n, (int) (a + l));
  fe->common.n[a + l] = '\0';
  fe->common.font_mag =
    actual_factor((uint) fix(((float) s / (float) d) * 1000.0)) *
      fontinfo.mag * (double) fontinfo.resolution;

  if (!fontops) {
    prerror("Font doesn't initialized.\n");
    exit(1);
  }
  /* search new font */
  for (op = fontops; op; op = op->next) {
    /* set font manipulation operators */
    fe->common.font_ops = &op->op;
    if ((*op->op.open_font)(fe))
      goto found;
  }
  fe->common.font_ops = (FontOpStruct *) NULL;
  prerror("Font %s [%d] not found.\n", fe->common.n, fix(fe->common.font_mag));
  return(False);

found:
#ifdef DEBUG
  prerror("Font found (%d, %s, %s)\n",
	  fe->common.k, fe->common.n, fe->common.name);
#endif

  return(True);
}

/*
 * GetFont(k, flag):
 *
 * Get font entry of font number k defined in dvi files.
 * If the required font is found, the results are returned.
 * When the required font entry is not found, this creates new entry
 * in FontEntryTable[] and returns it only if the flag is True.
 * Otherwise, this returns NULL.
 */

FontEntry *
GetFont(k, flag)
register unsigned int k;
int flag;
{
  register FontEntry **fe = &FontEntryTable[k & 0xff];

  /* search font entry numbered k in FontEntryTable[] */
  while (*fe) {
    if ((*fe)->common.k == k) goto found;
    else fe = &(*fe)->common.next;
  }
  if (flag) {
    (*fe) = (FontEntry *) AllocMemory((ulong) sizeof(FontEntry));
    (*fe)->common.k = k;
    (*fe)->common.access_count = 0;
    (*fe)->common.font_file_fd = (FILE *) NULL;
    (*fe)->common.font_ops = (FontOpStruct *) NULL;
    (*fe)->common.next = (FontEntry *) NULL;
    (*fe)->specific = NULL;
    goto found;
  }
  return(NULL);

found:
  return(*fe);
}

/*
 * GetGlyph(fe, c):
 *
 * Get glyph of character code c from font fe.
 * This is used for set_char operator in dvi files.
 */

CharEntry *
GetGlyph(fe, c)
FontEntry *fe;
int c;
{
  return((*fe->common.font_ops->get_glyph)(fe, c));
}

/*
 * CloseFonts:
 *	Closes all the font files, and frees up all the memory.
 */

void
CloseFonts()
{
  register FontEntry **fe = FontEntryTable;
  register int i;

#ifdef DEBUG
  prerror("Closing %d font files\n", nopen);
#endif

  /* Close file and reset OpenFiles[] */
  while (nopen > 0) unregister_file(0);

  /* Free fonts */
  for (i = 0; i++ < MAXFONTENTRY;) {
    if (*fe) {
      register FontEntry *f = *fe;

      while (f) {
	if (!(*f->common.font_ops->free_font)(f)) {
	  prerror("Can't free font %s\n", f->common.name);
	  exit(1);
	}
#ifdef DEBUG
	prerror("Freeing font %s\n", f->common.name);
#endif
	f = f->common.next;
      }
    }
    (*fe++) = (FontEntry *) NULL;
  }
  ResetMemory();
}

void
FontsForAll(func)
void (*func)();
{
  register int i;

  for (i = 0; i < MAXFONTENTRY; ++i) {
    register FontEntry *f = FontEntryTable[i];

    while (f) {
      (*func)(f);
      f = f->common.next;
    }
  }
}

/*
 * Memory management procedures for font manipulations
 */

typedef struct mem_type {
  char *buf;
  struct mem_type *next;
  ulong used, size;
} MemType;

#define ALLOCSIZE 512000
static char initial_buf[ALLOCSIZE];
static MemType start = {initial_buf, (MemType *) NULL, 0, ALLOCSIZE};
static MemType *current = &start;

char *
AllocMemory(size)
register ulong size;
{
  size = ((size + 3) / 4) * 4; /* align into 4 bytes boundary for SUN4 */
#ifdef DEBUG
  if (size > 10000)
    prerror("AllocMemory: Allocate %d bytes\n", size);
#endif

  if (current->used + size >= current->size) {
    register unsigned alloc_size = (unsigned) (size + sizeof(MemType));
    register char *buf;

    if (alloc_size < ALLOCSIZE) alloc_size = ALLOCSIZE;
    if (!(buf = malloc(alloc_size))) {
      prerror("Out of memory for font structures\n");
      exit(1);
    }
    current->next = (MemType *) buf;
    current = (MemType *) buf;
    current->buf = buf + sizeof(MemType);
    current->used = size;
    current->size = alloc_size - sizeof(MemType);
    current->next = (MemType *) NULL;
#ifdef DEBUG
    prerror("AllocMemory: Allocate more %d bytes\n", alloc_size);
#endif

    return(current->buf);
  } else {
    current->used += size;

    return(current->buf + current->used - size);
  }
}

static void
ResetMemory()
{
  register MemType *memp;
#ifdef DEBUG
  register int usage = start.used;
#endif

  for (memp = start.next; memp; memp = memp->next) {
    free(memp->buf - sizeof(MemType));
#ifdef DEBUG
    usage += memp->used;
#endif
  }
  start.buf = initial_buf;
  start.next = (MemType *) NULL;
  start.used = 0;
  start.size = ALLOCSIZE;
  current = &start;

#ifdef DEBUG
  prerror("Current memory usage is %d bytes.\n", usage);
#endif
}

/*
 * actual_factor:
 *	compute the actual size factor given the approximation.
 */

double
actual_factor(unmodsize)
register uint unmodsize;  /* actually factor * 1000 */
{
  register float realsize; /* the actual magnification factor */

  realsize = (float) unmodsize / 1000.0;
  /* a real hack to correct for rounding in some cases--rkf */
  if (unmodsize == 1095) realsize = 1.095445;	/*stephalf*/
  else if (unmodsize == 1315) realsize = 1.314534;	/*stepihalf*/
  else if (unmodsize == 2074) realsize = 2.0736;	/*stepiv*/
  else if (unmodsize == 2488) realsize = 2.48832;  /*stepv*/
  else if (unmodsize == 2986) realsize = 2.985984;	/*stepiv*/
  /* the remaining magnification steps are represented with sufficient
     accuracy already */
  return(realsize);
}

int
get_kchar_entry(top, c, ret)
register KCharEntry **top;
register int c;
register CharEntry **ret;
{
  /* search kanji char entry */
  while (*top) {
    if ((*top)->code == (ushort) c) {
      *ret = &(*top)->ch;
      return(1);
    } else if ((*top)->code < c)
      top = &(*top)->left;
    else
      top = &(*top)->right;
  }

  /* allocate memory for CharEntry and bitmap */
  *top = (KCharEntry *) AllocMemory((ulong) sizeof(KCharEntry));

  (*top)->code = (ushort) c;
  (*top)->left = (KCharEntry *) NULL;
  (*top)->right = (KCharEntry *) NULL;
  *ret = &(*top)->ch;
  return(0);
}
