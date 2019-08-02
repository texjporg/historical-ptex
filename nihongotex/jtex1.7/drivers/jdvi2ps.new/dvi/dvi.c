/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: dvi.c,v 1.4 90/01/16 10:30:55 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 90/01/16 10:30:55 $
 */

/*
 * DVI file functions.
 */

#include "dvi.h"
#include "../fonts/public.h"
#include <sys/types.h>
#include "dvi_public.h"
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>

/*
 * Constants and macro functions.
 */
#define DVIFORMAT		2
#define move_down(a) v += (a)
#define move_over(b) h += (b)

#define AppEHandler (*app_info->error_handler)
#define AppInitDVI  (*app_info->init_dvi)
#define AppCheckSheet  (*app_info->check_sheet)
#define AppBOP (*app_info->bop)
#define AppEOP (*app_info->eop)
#define AppSetFntNum (*app_info->set_fnt_num)
#define AppSetChar (*app_info->set_char)
#define AppSetRule (*app_info->set_rule)
#define AppDoSpecial (*app_info->do_special)

/*
 * Global variables
 */
DVI dvi_info;
DVIPages *CurrentPage;
FontEntry *CurrentFont;

/*
 * Private functions
 */
bool ReadPostamble();
bool CheckDVIFile();
void set_font_num();
bool read_font_def();

/*
 * Private variables
 */
typedef struct		/* stack entry */
{
  int h, v, w, x, y, z;  /* what's on stack */
} Stack;

static time_t mtime = 0;

static int h;			/* current horizontal position              */
static int v;			/* current vertical position                */
static Stack *stack = NULL;     /* stack */
static AppInfo *app_info;

/*
 * InitDVIFile:
 *	Opens the dvi file, and checks for valid codes etc.
 *	Reads the postamble (if enabled)
 *	Leaves the file pointer at the start of the first page.
 */

bool
InitDVIFile(filename, resolution, user_mag, preload, font_path, tfm_path, appinfo)
register char *filename, *font_path, *tfm_path;
register int resolution, user_mag;
register bool preload;
register AppInfo *appinfo;
{
  register int i;
  register FILE *fp;
  register char *extension;
  static char fullname[MAXPATHLEN], directory[MAXPATHLEN], name[MAXPATHLEN];

  if (appinfo) app_info = appinfo;
  else return(False);

  /*
   * If the filename has no extension, or if it
   * has an extension and it is not '.dvi' then
   * cat .dvi onto the filename.
   */
  /* get directory */
  if (extension = strrchr(filename, '/')) {
    bcopy(filename, directory, extension - filename);
    directory[extension - filename] = '\0';
    strcpy(name, extension + 1);
  } else {
    getwd(directory);
    strcpy(name, filename);
  }
  if ((extension = strrchr(name, '.')) && !strcmp(extension, ".dvi"))
    *extension = '\0';

  strcpy(fullname, directory);
  strcat(fullname, "/");
  strcat(fullname, name);
  strcat(fullname, ".dvi");

  /*
   * Open the file
   */
#ifdef DEBUG
  prerror("Opening file \"%s\"\n", fullname);
#endif
  if (access(fullname, 04) != 0) {
    AppEHandler("%s: Cant access DVI file", fullname);
    return(False);
  }

  /*
   * We must free all file descriptors used by font libraries before opening
   * new DVI file, because all of file descriptors could be exhausted.
   */
  if (!AppInitDVI(fullname, name)) goto false;
  if (dvi_info.dvifp) {
    /*
     * close the dvifile.
     */
    fclose(dvi_info.dvifp);
    mtime = 0;

    /*
     * Close the fonts and free up memory.
     */
    CloseFonts();
  }

  if ((fp = fopen(fullname, "r")) == NULL) {
    AppEHandler("%s: Cant open DVI file", fullname);
    return(False);
  }

  if (!CheckDVIFile(fp)) goto false;
  /*
   * Read the magic number and version number
   */
  if ((i = get_unsigned1(fp)) != PRE) {
    AppEHandler("%s: not a dvi file (bad magic number %d)", fullname,i);
    goto false;
  }
  if ((i = get_signed1(fp)) != DVIFORMAT) {
    AppEHandler("%s: dvi format %d not supported", fullname, i);
    goto false;
  }

  /*
   * Load font information from postable.
   */
  if (!ReadPostamble(fp, resolution, user_mag, preload, font_path, tfm_path))
    goto false;

  /* Return to start of first page. */
  fseek(fp, 14L, 0);

  /*
   * Skip i more bytes of preamble.
   */
  fseek(fp, (long) get_unsigned1(fp), 1);

  /*
   * Set up the page fseek pointer table.
   * We are now at page 0.
   */
  if (dvi_info.pages) free((char *) dvi_info.pages);
  if (!(dvi_info.pages =
	(DVIPages *) malloc(sizeof(DVIPages) * dvi_info.NumberOfPage)))
    goto false;
  for (i = dvi_info.NumberOfPage; i-- > 0; ) {
    dvi_info.pages[i].sheet_table = 0;
    dvi_info.pages[i].sheet_page = 0;
    dvi_info.pages[i].size_known = 0;
    dvi_info.pages[i].minh = MAXINT;
    dvi_info.pages[i].maxh = NEGMAXINT;
    dvi_info.pages[i].minv = MAXINT;
    dvi_info.pages[i].maxv = NEGMAXINT;
  }
  dvi_info.last_known_sheet = 0;
  dvi_info.pages[0].sheet_table = ftell(fp);

  if (stack) free((char *) stack);
  if (!(stack = (Stack *) malloc(sizeof(Stack) * dvi_info.stack_size)))
    goto false;

#ifdef DEBUG
  prerror("sheet_table[%d] = %d\n", dvi_info.last_known_sheet, ftell(fp));
#endif

  dvi_info.dvifp = fp;
  strcpy(dvi_info.filename, fullname);

  return(True);

 false:
  fclose(fp);
  dvi_info.dvifp = NULL;
  mtime = 0;
  return(False);
}

/*
 * CheckDVIFile:
 *	Checks that this is the same file -- has not been modified since
 *	it was opened.
 */

static bool
CheckDVIFile(fp)
register FILE *fp;
{
  static struct stat stat_buf; /* For checking file changes. */

  if (fp == NULL) {
    AppEHandler("No DVI file open");
    return(False);
  }

  if (fstat(fileno(fp), &stat_buf) != 0) {
    AppEHandler("%s: dvifile fstat failed", dvi_info.filename);
    return(False);
  }

  if (!mtime) mtime = stat_buf.st_mtime;
  else if (stat_buf.st_mtime != mtime) {
    AppEHandler("%s: dvifile modified", dvi_info.filename);
    return(False);
  }

  return(True);
}

/*
 * ReadPostamble:
 *	This  routine  is  used  to  read  in  the  postamble  values.	It
 *	initializes the magnification and checks  the stack height prior  to
 *	starting printing the document.
 */

static bool
ReadPostamble(fp, resolution, user_mag, preload, font_path, tfm_path)
register FILE *fp;
register int resolution, user_mag;
register bool preload;
register char *font_path, *tfm_path;
{
  register uchar   byte;
  register uint i;

  if (!CheckDVIFile(fp)) return(False);

  /*
   * find_postamble_ptr
   *	Move to the end of the dvifile and find the start of the postamble.
   */
  /* Get postamble pointer (trailer 4 bytes are of no use) */
  fseek(fp, -5L, 2);
  for ( ; ; ) {
    if ((i = get_unsigned1(fp)) == DVIFORMAT) {
      fseek(fp, -5L, 1);
      fseek(fp, (long) get_unsigned4(fp), 0);
      break;
    } else if (i != 223) {
      AppEHandler("%s: Bad DVI file: bad end of file", dvi_info.filename);
      return(False);
    }
    fseek(fp, -2L, 1);
  }

  if (get_unsigned1(fp) != POST) {
    AppEHandler("%s: Bad DVI file: no POST at head of postamble",
		dvi_info.filename);
    return(False);
  }

  fseek(fp, 4L, 1); /* discard last page pointer */
  dvi_info.num = get_unsigned4(fp);
  dvi_info.den = get_unsigned4(fp);
  dvi_info.mag = get_unsigned4(fp);

  /*
   * Init font libraries
   */
  InitFont(dvi_info.num, dvi_info.den,
	   (user_mag > 0 ? user_mag : dvi_info.mag),
	   resolution, font_path, tfm_path);

  dvi_info.hpd = VConv(get_unsigned4(fp));
  /* height-plus-depth of tallest page */
  dvi_info.wpw = HConv(get_unsigned4(fp));
  /* width of widest page */
  dvi_info.stack_size = get_unsigned2(fp);
  dvi_info.NumberOfPage = get_unsigned2(fp);

/*
 *	Read the font  definitions as they  are in the  postamble of the  DVI
 *	file.  Note that the font directory  is not yet loaded.  In order  to
 *	adapt ourselves to the existing "verser" the following font paramters
 *	are  copied   onto   output   fontno  (4   bytes),  chksum,  fontmag,
 *	fontnamelength (1 byte), fontname.  At the end, a -1 is put onto  the
 *	file.
 */

  if (preload) {
    while (((byte = get_unsigned1(fp)) >= FNT_DEF1) && byte <= FNT_DEF4)
      if (!read_font_def(fp, get_unsigned(fp, byte - FNT_DEF1 + 1)))
	return(False);
  } else return(True);

  if (byte != POST_POST) {
    AppEHandler("%s: Bad DVI file: no postpostamble after fontdefs",
		dvi_info.filename);
    return(False);
  } else return(True);
}

/*
 * ProcessPage:
 *	Rasterises the page specified as new_sheet in the dvifile
 *      into page_pmem.
 */

bool
ProcessPage(new_sheet)
int new_sheet;
{
  register bool skip_mode = True;
  register int cmd;	    /* current command */
  register int w;	    /* current horizontal spacing */
  register int x;	    /* current horizontal spacing */
  register int y;	    /* current vertical spacing */
  register int z;	    /* current vertical spacing */
  register int sp;	    /* stack pointer */
  register int val, val2;   /* temporarys */
  register int file_sheet;
  static int counter[10];

  if (!CheckDVIFile(dvi_info.dvifp)) return(False);

  /* Check against page limits. */
  if (new_sheet < 0) new_sheet = 0;
  else if (new_sheet >= dvi_info.NumberOfPage)
    new_sheet = dvi_info.NumberOfPage - 1;

  if (!AppCheckSheet(new_sheet)) goto false;

  /* Do we already know where the page is ? */
  if (new_sheet <= dvi_info.last_known_sheet) {
    fseek(dvi_info.dvifp, dvi_info.pages[new_sheet].sheet_table, 0);
    file_sheet = new_sheet;
  } else {
    file_sheet = dvi_info.last_known_sheet;
    fseek(dvi_info.dvifp, dvi_info.pages[file_sheet].sheet_table, 0);
  }

  while (file_sheet <= new_sheet) {
    if (file_sheet == new_sheet) skip_mode = False;
    CurrentPage = &dvi_info.pages[file_sheet];
#ifdef DEBUG
    prerror("sheet %d starts at %d\n", file_sheet,
	    (int) CurrentPage->sheet_table);
#endif

    while ((cmd = get_unsigned1(dvi_info.dvifp)) != EOP) {
      switch(cmd) {

      case SET1: case SET2: case SET3: case SET4:
	if (skip_mode) fseek(dvi_info.dvifp, (long) (cmd - SET1 + 1), 1);
	else move_over(SetChar(get_unsigned(dvi_info.dvifp, cmd - SET1 + 1)));
	break;

      case SET_RULE:
	if (skip_mode) fseek(dvi_info.dvifp, 8L, 1);
	else {
	  val = get_unsigned4(dvi_info.dvifp);
	  val2 = get_unsigned4(dvi_info.dvifp);
	  move_over(SetRule(val, val2));
	}
	break;

      case PUT1: case PUT2: case PUT3: case PUT4:
	if (skip_mode) fseek(dvi_info.dvifp, (long) (cmd - PUT1 + 1), 1);
	else (void) SetChar(get_unsigned(dvi_info.dvifp, cmd - PUT1 + 1));
	break;

      case PUT_RULE:
	if (skip_mode) fseek(dvi_info.dvifp, 8L, 1);
	else {
	  val = get_unsigned4(dvi_info.dvifp);
	  val2 = get_unsigned4(dvi_info.dvifp);
	  (void) SetRule(val, val2);
	}
	break;

      case NOP:
	break;

      case BOP:
	/*
	 * These are the 10 counters.
	 * Discard previous page pointer.
	 */
	for (val = 0; val < 10; val++)
	  counter[val] = get_unsigned4(dvi_info.dvifp);
	(void) fseek(dvi_info.dvifp, 4L, 1);

	/*
	 * The first counter is the page number.
	 */
	CurrentPage->sheet_page = counter[0];

	/*
	 * Show what is happening.
	 */
#ifdef DEBUG
	prerror("File \"%s\"   Page %d   %s\n", dvi_info.filename,
		CurrentPage->sheet_page, skip_mode ? "Skipping" : "Processing");
#endif

	if (!skip_mode) AppBOP();

	h = v = w = x = y = z = 0;
	sp = 0;
	CurrentFont = NULL;
	break;

      case PUSH:
	if (sp >= dvi_info.stack_size) {
	  AppEHandler("%s: Bad DVI file: stack overflow", dvi_info.filename);
	  goto false;
	}
	stack[sp].h = h; stack[sp].v = v; stack[sp].w = w;
	stack[sp].x = x; stack[sp].y = y; stack[sp].z = z;
	sp++;
	break;

      case POP:
	if (--sp < 0) {
	  AppEHandler("%s: Bad DVI file: stack underflow", dvi_info.filename);
	  goto false;
	}
	h = stack[sp].h; v = stack[sp].v; w = stack[sp].w;
	x = stack[sp].x; y = stack[sp].y; z = stack[sp].z;
      break;

      case RIGHT1: case RIGHT2: case RIGHT3: case RIGHT4:
	if (skip_mode) fseek(dvi_info.dvifp, (long) (cmd - RIGHT1 + 1), 1);
	else move_over(get_signed(dvi_info.dvifp, cmd - RIGHT1 + 1));
	break;

      case W0:
	if (!skip_mode) move_over(w);
	break;

      case W1: case W2: case W3: case W4:
	if (skip_mode) fseek(dvi_info.dvifp, (long) (cmd - W1 + 1), 1);
	else move_over(w = get_signed(dvi_info.dvifp, cmd - W1 + 1));
      break;

      case X0:
	if (!skip_mode) move_over(x);
	break;

      case X1: case X2: case X3: case X4:
	if (skip_mode) fseek(dvi_info.dvifp, (long) (cmd - X1 + 1), 1);
	else move_over(x = get_signed(dvi_info.dvifp, cmd - X1 + 1));
	break;

      case DOWN1: case DOWN2: case DOWN3: case DOWN4:
	if (skip_mode) fseek(dvi_info.dvifp, (long) (cmd - DOWN1 + 1), 1);
	else move_down(get_signed(dvi_info.dvifp, cmd - DOWN1 + 1));
	break;

      case Y0:
	if (!skip_mode) move_down(y);
	break;

      case Y1: case Y2: case Y3: case Y4:
	if (skip_mode) fseek(dvi_info.dvifp, (long) (cmd - Y1 + 1), 1);
	else move_down(y = get_signed(dvi_info.dvifp, cmd - Y1 + 1));
	break;

      case Z0:
	if (!skip_mode) move_down(z);
	break;

      case Z1: case Z2: case Z3: case Z4:
	if (skip_mode) fseek(dvi_info.dvifp, (long) (cmd - Z1 + 1), 1);
	else move_down(z = get_signed(dvi_info.dvifp, cmd - Z1 + 1));
	break;

      case FNT1: case FNT2: case FNT3: case FNT4:
	if (skip_mode) fseek(dvi_info.dvifp, (long) (cmd - FNT1 + 1), 1);
	else set_font_num(get_unsigned(dvi_info.dvifp, cmd - FNT1 + 1));
	break;

      case XXX1: case XXX2: case XXX3: case XXX4:
	val = get_unsigned(dvi_info.dvifp, cmd - XXX1 + 1);
	if (skip_mode) fseek(dvi_info.dvifp, (long) val, 1);
	else {
	  val2 = ftell(dvi_info.dvifp);
	  AppDoSpecial(dvi_info.dvifp, val, HConv(h), VConv(v));
	  fseek(dvi_info.dvifp, (long) (val2 + val), 0);
	}
	break;

      case FNT_DEF1: case FNT_DEF2: case FNT_DEF3: case FNT_DEF4:
	read_font_def(dvi_info.dvifp,
		      get_unsigned(dvi_info.dvifp, cmd - FNT_DEF1 + 1));
	break;

      case PRE:
	AppEHandler("%s: Bad DVI file: preamble found within main section",
		    dvi_info.filename);
	goto false;

      case POST:
	fseek(dvi_info.dvifp, -1L, 1);
	goto false;

      case POST_POST:
	AppEHandler("%s: Bad DVI file: postpostamble found within main section",
		    dvi_info.filename);
	goto false;

      default:
	if (cmd >= FONT_00 && cmd <= FONT_63) {
	  if (!skip_mode) set_font_num(cmd - FONT_00);
	} else if (cmd >= SETC_000 && cmd <= SETC_127) {
	  if (!skip_mode) move_over(SetChar(cmd - SETC_000));
	} else {
	  AppEHandler("%s: Bad DVI file: undefined command (%d) found",
		      dvi_info.filename, cmd);
	  goto false;
	}
      }
    }

    /*
     * End of page.
     */
    if (!skip_mode && !CurrentPage->size_known) {
      CurrentPage->minh = CurrentPage->minh * hconv - (hconv >> 1);
      CurrentPage->maxh = CurrentPage->maxh * hconv;
      CurrentPage->minv = CurrentPage->minv * vconv - (vconv >> 1);
      CurrentPage->maxv = CurrentPage->maxv * vconv;
      CurrentPage->size_known = 1;
    }

    /*
     * The file is now at the start of the next page.
     */
    file_sheet++;
    if (file_sheet < dvi_info.NumberOfPage &&
	file_sheet > dvi_info.last_known_sheet) {
      dvi_info.last_known_sheet = file_sheet;
      dvi_info.pages[file_sheet].sheet_table = ftell(dvi_info.dvifp);
    }
  }

  return(AppEOP());

 false:
  return(False);
}


static void
CalcMinMax(minh, minv, width, height)
register int minh, minv, width, height;
{
  if (!CurrentPage->size_known) {
    register int maxh = minh + width - 1, maxv = minv + height - 1;

    if (minh < CurrentPage->minh) CurrentPage->minh = minh;
    if (maxh > CurrentPage->maxh) CurrentPage->maxh = maxh;
    if (minv < CurrentPage->minv) CurrentPage->minv = minv;
    if (maxv > CurrentPage->maxv) CurrentPage->maxv = maxv;
#ifdef DEBUG
    if (maxh > 10000 || maxv > 10000)
      prerror("Too high value for h, v\n");
    if (width <= 0 || height <= 0)
      prerror("Invalid width and height\n");
#endif
  }
}

/*
 * set_font_num:/
 *	This routine is used to specify the font to be used in printing future
 *	chars.
 */

static void
set_font_num(k)
register int k;
{
  if (!(CurrentFont = GetFont(k, False))) {
    CurrentFont = NULL;
    AppEHandler("%s: Bad DVI file: font %d undefined", dvi_info.filename, k);
  }
  AppSetFntNum(CurrentFont);
}


/*
 * Font functions
 * ==============
 */

/*
 * read_font_def:
 */

static bool
read_font_def(fp, k)
register FILE *fp;
register uint k;
{
  register FontEntry *fe;
  register uint c, s, d, a, l;
  static char n[MAXPATHLEN];

  if (!(fe = GetFont(k, True)))
    return(False);
  c = get_unsigned4(fp); /* checksum */
  s = get_unsigned4(fp); /* space size */
  d = get_unsigned4(fp); /* design size */
  a = get_unsigned1(fp); /* area length for font name */
  l = get_unsigned1(fp); /* device length */
  fread(n, 1, a + l, fp);
  if (!fe->common.font_ops) {
    if (!OpenFont(fe, c, s, d, a, l, n))
      return(False);
  }

  return(True);
}

static int
SetChar(c)
register int c;
{
  register CharEntry *ptr = GetGlyph(CurrentFont, c);
  register int hh = HConv(h), vv = VConv(v);

  if (!ptr) return(0);
  AppSetChar(ptr, c, h, v, hh, vv);
  CalcMinMax(hh - ptr->xOffset, vv - ptr->yOffset, ptr->width, ptr->height);
  return(ptr->tfmw);
}

SetRule(a, b)
register int a, b;
{
  register int hh = HConv(h), vv = VConv(v - a);
  register int ehh = HConv(h + b), evv = VConv(v);

  AppSetRule(a, b, hh, vv, ehh, evv);
  CalcMinMax(hh, vv, ehh - hh, evv - vv);
  return(b);
}

