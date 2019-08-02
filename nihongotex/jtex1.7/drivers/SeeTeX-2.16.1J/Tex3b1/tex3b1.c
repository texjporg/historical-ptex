/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 *
 *	This program may be freely copied, as long as this copyright
 *	message remaines affixed. It may not be sold, altough it may
 *	be distributed with other software which is sold. If the
 *	software is distributed, the source code must be made available.
 *
 *	No warrenty, expressed or implied, is given with this software.
 *	It is presented in the hope that it will prove useful.
 *
 *	Adapted for the 3b1 by Andy Fyfe  (andy@csvax.caltech.edu)
 *	Fixes from John Campbell  (...!arizona!naucse!thunde!jdc)
 */

static char *version="1.2";

#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/window.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/termio.h>
#include "dvistuff.h"

#ifndef __STDC__
#define const
#endif

#ifndef __GNUC__
#define inline
#define volatile
#endif

#ifndef PAGE_HEIGHT
#define PAGE_HEIGHT	11		/* in inches */
#endif

#ifndef PAGE_WIDTH
#define PAGE_WIDTH	8.5		/* in inches */
#endif

#ifndef FONT_DPI
#define FONT_DPI	300
#endif

#ifndef BLACKNESS
#define BLACKNESS	3
#endif

#ifndef SHRINK_1
#define SHRINK_1	3
#endif

#ifndef SHRINK_2
#define SHRINK_2	4
#endif

#ifndef SHRINK_3
#define SHRINK_3	2
#endif

#define SHRINKS 3		/* Be careful if you change this! */

/*
 *	These constants may need to be changed on your implementation
 */

int Debug;

/*
 *	3b1 related variables
 */

static int WindowFD;
static unsigned short *Image[SHRINKS];
static int X_origin[SHRINKS], Y_origin[SHRINKS];

static int globalNumber;
static int globalShrink;
static int defaultShrink;
static int firstPage;

extern char *DVIFileName;

/*
 *	TeX-Dvi related variables
 */

static int maxHeight[SHRINKS];
static int maxWidth[SHRINKS];
static int bytesWide[SHRINKS];

static int imageWidth[SHRINKS], imageHeight[SHRINKS];
static int screenWidth[SHRINKS], screenHeight[SHRINKS];

static int shrink[SHRINKS] = { SHRINK_1, SHRINK_2, SHRINK_3 };

static int page_w[SHRINKS], page_h[SHRINKS], page_num[SHRINKS];

static struct glyph **shrunkenGlyphs[SHRINKS][MAX_FONTFAMILY];

typedef struct mpr_data MprData;

#ifdef __STDC__
extern void *malloc();
#else
extern char *malloc();
#endif
extern volatile void exit();

#define MAXFUNC(a,b) (((a) < (b)) ? (b) : (a))
#define MINFUNC(a,b) (((a) < (b)) ? (a) : (b))

static int inHelp;

static struct uwdata d;
static void displayLeaves(), buildLeaf();
volatile void usage();

static const char clear_str[] = "\033[=1C\033[2J\033[H";
#define clearWindow() write(WindowFD, clear_str, sizeof(clear_str)-1)

volatile void
stop_output(status)
int status;
{
    close(WindowFD);
    dviFini();
    exit(status);
}

static
sig_window()
{
    struct uwdata tmp;
    int i;

    if (ioctl(WindowFD, WIOCGETD, &tmp) < 0) {
	fprintf(stderr, "%s: window ioctl failed: ", ProgName);
	perror("");
	stop_output(1);
    }
    if (tmp.uw_width != d.uw_width || tmp.uw_height != d.uw_height) {
	d.uw_width = tmp.uw_width;
	d.uw_height = tmp.uw_height;
	for (i=0; i<SHRINKS; ++i) {
	    screenWidth[i] = MINFUNC(imageWidth[i], d.uw_width);
	    screenHeight[i] = MINFUNC(imageHeight[i], d.uw_height);
	}
	if (!inHelp) {
	    adjust_origin();
	    displayLeaves();
	}
    }

    signal(SIGWIND, sig_window);
}

volatile void
main(argc, argv)
    int argc;
    char **argv;
{
    struct utdata t;
    int fd, i, c, smallWindow = 0;
    extern int optind;
    extern char *optarg;

    ProgName = argv[0];
    firstPage = 1;
    defaultShrink = 0;
    Debug = 0;
    dviDPI = FONT_DPI;
    dviBlackness = BLACKNESS;

    while ((c = getopt(argc, argv, "b:d:f:m:s:D:SV")) != EOF) {
	switch(c) {
	case 'b':
	    dviBlackness = atoi(optarg);
	    break;
	case 'd':
	    dviDPI = atoi(optarg);
	    break;
	case 'f':
	    firstPage = atoi(optarg);
	    break;
	case 'm':
	    defaultShrink = atoi(optarg) - 1;
	    if (defaultShrink < 0)
		defaultShrink = 0;
	    else if (defaultShrink >= SHRINKS)
		defaultShrink = SHRINKS - 1;
	    break;
	case 's':
	    setShrinks(optarg);
	    break;
	case 'D':
	    Debug = atoi(optarg);
	    break;
	case 'S':
	    ++smallWindow;
	    break;
	case 'V':
	    fprintf(stderr,"%s: version %s\n", ProgName, version);
	    usage();
	    break;
	case '?':
	    usage();
	    break;
	}
    }

    for (i = 0; i < SHRINKS; ++i) {
	maxWidth[i]  = ((int)(PAGE_WIDTH *dviDPI) + (shrink[i]-1)) / shrink[i];
	maxHeight[i] = ((int)(PAGE_HEIGHT*dviDPI) + (shrink[i]-1)) / shrink[i];
	bytesWide[i] = ((maxWidth[i] + 15) >> 4) << 1;
	maxWidth[i]  = bytesWide[i] * 8;
    }

    globalShrink = defaultShrink;

    if (optind != argc - 1)
	usage();
    else
	DVIFileName = argv[optind];

    if (dviInit()) {
	dviFini();
	exit(1);
    }

    if (Debug > 0) {
	fprintf(stderr,"page is %d wide, %d high\n",
		dviWidestPage, dviTallestPage);
    }

    if (((double) dviTallestPage * (double) dviWidestPage) > 4.0e7) {
	fprintf(stderr,"[%s] Warning: Your page size is %d wide and %d tall,\n",
		ProgName, dviTallestPage, dviWidestPage);
	fprintf(stderr,"which may be too big to be displayed\n");
    }

    for (i = 0; i < SHRINKS; ++i) {
	page_h[i] = (dviTallestPage + 2 * dviDPI + shrink[i] - 1) / shrink[i];
	page_w[i] = (dviWidestPage + 2 * dviDPI + shrink[i] - 1) / shrink[i];
	page_num[i] = -1;
	imageWidth[i] = MINFUNC(page_w[i], maxWidth[i]);
	imageHeight[i] = MINFUNC(page_h[i], maxHeight[i]);
	if (Debug > 0)
	    fprintf(stderr,"page [mag %d] is (%d, %d), image is (%d,%d)\n",
		i+1, page_w[i], page_h[i], imageWidth[i], imageHeight[i]);
    }

    /*
     * Make sure we're running in a window
     */
    fd = open("/dev/tty", O_RDWR, 0);
    if (fd < 0) {
	fprintf(stderr, "%s: can't open /dev/tty: ", ProgName);
	perror("");
	dviFini();
	exit(1);
    }
    if (ioctl(fd, WIOCGETD, &d) < 0) {
	fprintf(stderr, "%s must be run from a console window\n", ProgName);
	dviFini();
	exit(1);
    }
    close(fd);

    /*
     * Create a new window.
     */
    WindowFD = open("/dev/window", O_RDWR, 0);
    if (WindowFD < 0) {
	fprintf(stderr, "%s: can't open /dev/window: ", ProgName);
	perror("");
	dviFini();
	exit(1);
    }

    /*
     * Set the window to use the full screen or just a part of it
     */
    if (smallWindow) {
	d.uw_x = 0;
	d.uw_y = WLINE(2);
	d.uw_width = 600;
	d.uw_height = WTXTVS * 18;
	d.uw_uflags =
	    BORDHSCROLL | BORDVSCROLL | BORDHELP | BORDCANCEL | BORDRESIZE;
    }
    else {
	d.uw_x = 0;
	d.uw_y = WLINE(1);
	d.uw_width = WINWIDTH;
	d.uw_height = WINHEIGHT - d.uw_y;
	d.uw_uflags = NBORDER;
    }
    for (i = 0; i < SHRINKS; ++i) {
	screenWidth[i] = MINFUNC(imageWidth[i], d.uw_width);
	screenHeight[i] = MINFUNC(imageHeight[i], d.uw_height);
    }
    if (ioctl(WindowFD, WIOCSETD, &d) < 0) {
	fprintf(stderr, "%s: window ioctl failed: ", ProgName);
	perror("");
	stop_output(1);
    }
    inHelp = 1;		/* lie a bit -- no image yet for sig_window */
    signal(SIGWIND, sig_window);
    kbd_nowait();
    clearWindow();

    /*
     * Give the window a name
     */
    t.ut_num = WTXTUSER;
    strncpy(t.ut_text, "TeX 3b1 Previewer", WTXTLEN);
    t.ut_text[WTXTLEN-1] = '\0';
    ioctl(WindowFD, WIOCSETTEXT, &t);
    
    globalNumber = 0;

    dviCurrentPage = firstPage - 1;

    buildLeaf();
    inHelp = 0;
    displayLeaves();

    while (1)
	check_input();
}

volatile void
usage()
{
    fprintf(stderr, "%s: usage: %s [options] dvi-file\n",
	ProgName, ProgName);
    fprintf(stderr, "options:\n");
    fprintf(stderr, "    -b <blackness>\n");
    fprintf(stderr, "    -d <dvi Dots Per Inch>\n");
    fprintf(stderr, "    -f <first (phyical) page>\n");
    fprintf(stderr, "    -m <default magnification>\n");
    fprintf(stderr, "    -s <shrink factors (eg 342)>\n");
    fprintf(stderr, "    -D <debug value>\n");
    fprintf(stderr, "    -S      [use small screen]\n");
    fprintf(stderr, "    -V      [print version]\n");
    exit(1);
}

setShrinks(values)
char *values;
{
    int s1, s2, s3;

    s1 = values[0] - '0';
    s2 = values[1] - '0';
    s3 = values[2] - '0';

    if (s1 < 0 || s1 > 9 || s2 < 0 || s2 > 9 || s3 < 0 || s3 > 9) {
	fprintf(stderr,
	    "%s: Illegal shrink values %s: must be 3 single digits\n",
	    ProgName, values);
	exit(1);
    }

    shrink[0] = s1;
    shrink[1] = s2;
    shrink[2] = s3;
}

static void
clearPixmap(shk)
int shk;
{
    int size = maxHeight[shk] * bytesWide[shk];

    if (Image[shk] == 0) {
	Image[shk] = (unsigned short *)malloc(size);
	if (Image[shk] == 0) {
	    fprintf(stderr, "%s: can't malloc %d bytes %s image array: ",
		ProgName, size, shk == 0 ? "small" : "large");
	    perror("");
	    stop_output(1);
	}
    }
    memset(Image[shk], 0, size);
}

/*
 *	display the normal sized leaf
 */
static void
displayLeaves()
{
    struct urdata r;
    struct utdata t;
    int i, l;
    static int last_width = 0, last_height;

    r.ur_srcbase = (unsigned short *)Image[globalShrink];
    r.ur_srcwidth = bytesWide[globalShrink];
    r.ur_dstbase = 0;
    r.ur_dstwidth = 0;
    r.ur_srcx = X_origin[globalShrink];
    r.ur_srcy = Y_origin[globalShrink];
    r.ur_dstx = 0;
    r.ur_dsty = 0;
    r.ur_width = screenWidth[globalShrink];
    r.ur_height = screenHeight[globalShrink];
    r.ur_srcop = SRCSRC;
    r.ur_dstop = DSTSRC;
    r.ur_pattern = 0;

    if ((r.ur_width < last_width && r.ur_width < d.uw_width) ||
	(r.ur_height < last_height && r.ur_height < d.uw_height))
	clearWindow();

    if (ioctl(WindowFD, WIOCRASTOP, &r) < 0) {
	fprintf(stderr, "[%s]: displayLeaves: ioctl(RASTOP): ", ProgName);
	perror("");
	dump_rast(&r);
    }

    t.ut_num = WTXTSLK1;
    sprintf(t.ut_text,
	"Page %3d of %3d (%4d,%4d)     Current 1/%d     ",
	dviCurrentPage+1, dviTotalPages,
	X_origin[globalShrink], Y_origin[globalShrink],
	shrink[globalShrink]);

    l = strlen(t.ut_text);
    for (i=0; i<SHRINKS; ++i) {
	if (page_num[i] == dviCurrentPage) {
	      t.ut_text[l++] = '[';
	      t.ut_text[l++] = '1';
	      t.ut_text[l++] = '/';
	      t.ut_text[l++] = shrink[i] + '0';
	      t.ut_text[l++] = ']';
	}
	else {
	      t.ut_text[l++] = ' ';
	      t.ut_text[l++] = ' ';
	      t.ut_text[l++] = ' ';
	      t.ut_text[l++] = ' ';
	      t.ut_text[l++] = ' ';
	}
        t.ut_text[l++] = ' ';
    }
    t.ut_text[l] = '\0';

    ioctl(WindowFD, WIOCSETTEXT, &t);

    last_width = r.ur_width;
    last_height = r.ur_height;
}

static void
buildLeaf()
{
    clearPixmap(globalShrink);

    if (dviCurrentPage < 0)
	dviCurrentPage = 0;
    if (dviCurrentPage >= dviTotalPages)
	dviCurrentPage = dviTotalPages - 1;

    dviPreparePage(dviCurrentPage);

    page_num[globalShrink] = dviCurrentPage;
}
    
/*
 *	interfaces to dvistuff
 */

/*
 *	Whenever a new font is registers, we create a shrunken Glyph
 *	table for it. However, we don't shrink the glyphs -- that's
 *	done on the fly by the putChar routine.
 */

DviFont *
applicationNewFont(f, key)
struct font *f;
int key;
{
    int shk;

    if (key < 0 || key > MAX_FONTFAMILY) {
	fprintf(stderr,"[%s] bogus key in Newfont = %d\n",
		ProgName, key);
	stop_output(1);
    }
    
    for (shk = 0; shk < SHRINKS; ++shk) {
	if (shrunkenGlyphs[shk][key] == 0) {
	    int lth = sizeof(struct glyph *) * MAX_GLYPH;
	    struct glyph **g;
	    
	    g = (struct glyph **) malloc( lth );
	    bzero(g, lth);
	    shrunkenGlyphs[shk][key] = g;
	}
    }

    return(f);
}

/*
 *	When we reset a font, we only need to free the storage for the
 *	shrunken glyphs. We keep the glyph table available because we're
 *	very likely to fill it in again.
 */

void
applicationResetFont(fi, key)
struct fontinfo *fi;
int key;
{
    int i, shk;
    struct glyph **theseGlyphs;
    
    for (shk=0; shk<SHRINKS; ++shk) {
	theseGlyphs = shrunkenGlyphs[shk][key];
	if (theseGlyphs != 0) {
	    for (i = 0; i < MAX_GLYPH; i++) {
		struct glyph *g;
		
		g = theseGlyphs[i];
		
		if (g != 0) {
		    if ( g -> g_raster != 0) {
			free(g -> g_raster);
		    }
		    free(g);
		    theseGlyphs[i] = 0;
		}
	    }
	}
    }
}

static inline void
reverse_raster(g)
struct glyph *g;
{
    unsigned short rev(), *p;
    int i; 

    p = (unsigned short *)g->g_raster;
    for (i = 0; i < (g->g_height + 1) * ((g->g_width + 15) >> 4); ++i) {
	*p = rev(*p);
	p++;
    }
}

void
applicationPutChar(hh, vv, charCode)
int hh;
int vv;
int charCode;
{
    register struct glyph *g;
    int x,y;
    int key;
#if 0
    struct urdata r;
#endif
    
    key = dviCurrentFont -> family;

    g = shrunkenGlyphs[globalShrink][key][charCode];

    if (g == 0) {
	g = dviShrinkGlyph(dviCurrentFont -> f -> f_gly[charCode],
	    shrink[globalShrink], shrink[globalShrink]);
	shrunkenGlyphs[globalShrink][key][charCode] = g;
	reverse_raster(g);
    }

    if (g == 0 || !HASRASTER(g)) return;

    hh /= shrink[globalShrink];
    vv /= shrink[globalShrink];

    x = hh - g -> g_xorigin;
    y = vv - g -> g_yorigin;

    bitblt(Image[globalShrink], x, y, bytesWide[globalShrink],
	g->g_raster, g->g_width, g->g_height);
#if 0
    r.ur_srcbase = (unsigned short *)g->g_raster;
    r.ur_srcwidth = ((g->g_width + 15) >> 4) * 2;
    r.ur_dstbase = (unsigned short *)Image[globalShrink] + ((x>1024)?64:0);
    r.ur_dstwidth = bytesWide[globalShrink];
    r.ur_srcx = 0;
    r.ur_srcy = 0;
    r.ur_dstx = (x > 1024) ? x-1024 : x;
    r.ur_dsty = y;
    r.ur_width = g->g_width;
    r.ur_height = g->g_height;
    r.ur_srcop = SRCSRC;
    r.ur_dstop = DSTOR;
    r.ur_pattern = 0;

    if (ioctl(WindowFD, WIOCRASTOP, &r) < 0) {
	fprintf(stderr, "[%s]: applicationPutChar: ioctl(RASTOP): ", ProgName);
	perror("");
	dump_rast(&r);
    }
#endif
}

void 
applicationSetRule(hh, vv, h, w)
int hh, vv;
int h, w;
{
    int nh, nw;

    hh /= shrink[globalShrink];
    vv /= shrink[globalShrink];

    nh = h / shrink[globalShrink];
    nw = w / shrink[globalShrink];

    if (nh == 0 && h != 0)
	nh = 1;
    if (nw == 0 && w != 0)
	nw = 1;

    put_rectangle(hh, vv - nh, nw, nh);
}

#define	COMLEN	128

void
applicationDoSpecial(cp)
char *cp;
{
    char command[COMLEN], *orig_cp;
    register int len;

    orig_cp = cp;
    while (isspace(*cp)) ++cp;
    len = 0;
    while (!isspace(*cp) && *cp && len < COMLEN-1) command[len++] = *cp++;
    command[len] = '\0';
    if (strcmp(command, "ps::[begin]") == 0) psfigBegin(cp);
    else if (strcmp(command, "ps::plotfile") == 0) 	/* do nothing */;
    else if (strcmp(command, "ps::[end]") == 0) 	/* do nothing */;
    else if (strcmp(command, "ps:plotfile") == 0)	/* do nothing */;
    else if (strcmp(command, "ps:") == 0)		/* do nothing */;
    else if (strcmp(command, "ps::") == 0) 		/* do nothing */;
    else fprintf(stderr, "[%s] special \"%s\" not implemented\n",
		ProgName, orig_cp);
}

/*
 * Draw the bounding box for a \psfig special.
 *
 * expected format of the command string is
 *
 * width height bbllx bblly bburx bbury
 *
 * *ll* means lower-left, *ur* means upper-right.
 *
 * We just draw the bounding box.
 */
psfigBegin(cp)
char *cp;
{
  int bbllx, bblly;
  int bburx, bbury;
  int width, height;

  sscanf(cp, " %d %d %d %d %d %d ",
	 &width, &height,
	 &bbllx, &bblly, &bburx, &bbury);

  bbllx = ( dviHH) / shrink[globalShrink];
  bblly = ( dviVV) / shrink[globalShrink];

  width = fromSP(width) / shrink[globalShrink];
  height = fromSP(height) / shrink[globalShrink];

  put_border( bbllx, bblly, width, height, 1);
}

put_border(x, y, w, h, t)
int x, y, w, h, t;
{
    put_rectangle(x, y, w, t);
    put_rectangle(x, y, t, h);
    put_rectangle(x, y + h - t, w, t);
    put_rectangle(x + w - t, y, t, h);
}

/*
 *	Prepare everything for re-reading a .dvi file
 */
reReadFile()
{
    int i;

    dviResetAll();

    if (dviInit()) {
	fprintf(stderr,"Unable to reload dvi file (%s), exiting\n",
		DVIFileName);
	stop_output(1);
    }

    for (i = 0; i < SHRINKS; ++i) {
	page_h[i] = (dviTallestPage + 2 * dviDPI + shrink[i] - 1) / shrink[i];
	page_w[i] = (dviWidestPage + 2 * dviDPI + shrink[i] - 1) / shrink[i];
	page_num[i] = -1;
	imageWidth[i] = MINFUNC(page_w[i], maxWidth[i]);
	imageHeight[i] = MINFUNC(page_h[i], maxHeight[i]);
	if (Debug > 0)
	    fprintf(stderr,"page [mag %d] is (%d, %d), image is (%d,%d)\n",
		i+1, page_w[i], page_h[i], imageWidth[i], imageHeight[i]);
    }

    buildLeaf();

    globalNumber = 0;
}

kbd_nowait()
{
    struct termio t;
    struct umdata m;

    ioctl(WindowFD, TCGETA, &t);
    t.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHONL);
    t.c_cc[VMIN] = 1;
    ioctl(WindowFD, TCSETA, &t);

    m.um_flags = MSDOWN;
    m.um_icon = 0;
    if (ioctl(WindowFD, WIOCSETMOUSE, &m) < 0) {
	fprintf(stderr, "%s: ioctl WIOSETMOUSE: ", ProgName);
	perror("");
    }
}

check_input()
{
    int ch;
    int direction;
    
#define BOGUSDIR -1
#define FOREWARD 0
#define BACKWARD 1
#define ABSOLUTE 2
    
    direction = BOGUSDIR;

    ch = getKeyboard();

    switch(ch) {
    case 'h':
	X_origin[globalShrink] -= dviDPI/4/shrink[globalShrink];
	break;

    case 'k':
	Y_origin[globalShrink] -= dviDPI/2/shrink[globalShrink];
	break;

    case 'l':
	X_origin[globalShrink] += dviDPI/4/shrink[globalShrink];
	break;

    case 'j':
	Y_origin[globalShrink] += dviDPI/2/shrink[globalShrink];
	break;

    case 'H':
	X_origin[globalShrink] = 0;
	break;

    case 'K':
	Y_origin[globalShrink] = 0;
	break;

    case 'L':
	X_origin[globalShrink] =
	    imageWidth[globalShrink] - screenWidth[globalShrink];
	break;

    case 'J':
	Y_origin[globalShrink] =
	    imageHeight[globalShrink] - screenHeight[globalShrink];
	break;

    case 'q':
    case 'x':
    case '\003':	/* control-C */
    case '\004':	/* control-D */
	stop_output(0);
	
    case 'n':
    case 'f':
    case ' ':
    case '\n':
    case '\r':
	/* scroll forward */
	direction = FOREWARD;
	break;
	
    case 'p':
    case 'b':
    case '\b':
    case '\177' : /* DEL */
	/* scroll backward */
	direction = BACKWARD;
	break;
	
    case 'g':
    case 'G':
	/* go to absolute page */
	direction = ABSOLUTE; /* may not be, but tough */
	break;
	
    case '\f':
    case 'r' :
	X_origin[globalShrink] = Y_origin[globalShrink] = 0;
	break;

#if SHRINKS >= 2
    case 'm':
	change_mag(screenWidth[globalShrink]/2, screenHeight[globalShrink]/2,
	    globalShrink == 1 ? 4 : 2);
	break;
#endif

#if SHRINKS >= 3
    case 'M':
	change_mag(screenWidth[globalShrink]/2, screenHeight[globalShrink]/2,
	    globalShrink == 2 ? 4 : 1);
	break;
#endif

    case 'R':
	reReadFile();
	break;
	
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
	globalNumber = 10*globalNumber + (ch - '0');
	return;
    
    case '?':
	help_screen();
	break;

    case '\000':
	break;

    default:
	globalNumber = 0;
	return;
    }
    
    adjust_origin();

    if (direction != BOGUSDIR) {
	if (globalNumber == 0)
	    globalNumber = 1;

/* 
 *	Turn pages 
 */
	if (direction == BACKWARD)
	    dviCurrentPage -= globalNumber;
	else if (direction == FOREWARD)
	    dviCurrentPage += globalNumber;
	else
	    dviCurrentPage = globalNumber - 1;	/* pages globalNumbered at 0 */
	
	globalShrink = defaultShrink;
	X_origin[0] = Y_origin[0] = 0;
	X_origin[1] = Y_origin[1] = 0;
	X_origin[2] = Y_origin[2] = 0;

	dviCurrentPage = MINFUNC(dviCurrentPage, dviTotalPages-1);
	dviCurrentPage = MAXFUNC(dviCurrentPage, 0);
    }

    if (dviCurrentPage != page_num[globalShrink])
	buildLeaf();
    
    displayLeaves();
    globalNumber = 0;

    return;
}

#ifdef NOSHARED_LIB
const unsigned short patwhite[16] =
    { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
      0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff };
#else
extern unsigned short patwhite[16];
#endif

put_rectangle(x, y, w, h)
int x, y, w, h;
{
    struct urdata r;

    r.ur_srcbase = 0;
    r.ur_srcwidth = 0;
    r.ur_dstbase = (unsigned short *)Image[globalShrink] + ((x>1024)?64:0);
    r.ur_dstwidth = bytesWide[globalShrink];
    r.ur_srcx = 0;
    r.ur_srcy = 0;
    r.ur_dstx = (x > 1024) ? x-1024 : x;
    r.ur_dsty = y;
    r.ur_width = w;
    r.ur_height = h;
    r.ur_srcop = SRCPAT;
    r.ur_dstop = DSTOR;
    r.ur_pattern = (unsigned short *)patwhite;

    if (ioctl(WindowFD, WIOCRASTOP, &r) < 0) {
	fprintf(stderr, "[%s]: put_rectangle: ioctl(RASTOP): ", ProgName);
	perror("");
	dump_rast(&r);
    }
}

dump_rast(p)
struct urdata *p;
{
    fprintf(stderr,
	"\tsrc %x[%d] @ (%d,%d); dst %x[%d] @ (%d,%d)\n",
	p->ur_srcbase, p->ur_srcwidth, p->ur_srcx, p->ur_srcy,
	p->ur_dstbase, p->ur_dstwidth, p->ur_dstx, p->ur_dsty);
    fprintf(stderr,
	"\tsrcop = %d, dstop = %d, size (%d,%d), pattern = %x\n",
	p->ur_srcop, p->ur_dstop, p->ur_width, p->ur_height, p->ur_pattern);
}

get_char()
{
    char ch;
    int count;

    count = read(WindowFD, &ch, 1);
    if (count == 0)
	return 'q';
    else if (count < 0)
	return -1;
    else
	return ch;
}

getKeyboard()
{
    char ch;

    ch = get_char();
    if (ch != '\033')
	return ch;

    /*
     * if the char is escape, try and interpret escape sequences and return
     * an equivalent single character.  mouse clicks on border icons return
     * a key sequence.  resize icon handled by sigwind.
     */

    switch (get_char()) {
    case '[':
	switch(get_char()) {
	case '?':	/* mouse button (among other things?) */
	    return get_mouse();
	case 'A':	/* up arrow */
	case 'T':	/* shift up arrow */
	    return 'k';
	case 'B':	/* down arrow */
	case 'S':	/* shift down arrow */
	    return 'j';
	case 'C':	/* right arrow */
	    return 'l';
	case 'D':	/* left arrow */
	    return 'h';
	case 'H':	/* home */
	    return 'r';
	case 'U':	/* page down */
	    return 'J';
	case 'V':	/* page up */
	    return 'K';
	}
	break;
    case 'O':
	switch(get_char()) {
	case 'm':	/* help */
	case 'M':	/* shift help */
	    return '?';
	case 'k':	/* exit */
	case 'K':	/* shift exit */
	case 'w':	/* cancl */
	case 'W':	/* shift cancl */
	    return 'q';
	case 'v':	/* open */
	    return 'R';
	}
	break;
    case 'N':
	switch(get_char()) {
	case 'g':	/* prev */
	    return 'b';
	case 'h':	/* next */
	    return 'f';
	case 'L':	/* shift right arrow */
	    return 'l';
	case 'K':	/* shift left arrow */
	    return 'h';
	}
	break;
    case '9':		/* beg */
	globalNumber = 1;
	return 'g';
    case '0':		/* end */
	globalNumber = dviTotalPages;
	return 'g';
    }
    return -1;
}

get_mouse()
{
    int param[4], n;
    int ch;

    for (n=0; n<4; ++n) {
	param[n] = 0;
	ch = get_char();
	while (ch >= '0' && ch <= '9') {
	    param[n] = 10 * param[n] + ch - '0';
	    ch = get_char();
	}
	if (n < 3 && ch != ';')
	    return -1;
	if (n == 3 && ch != 'M')
	    return -1;
    }

    change_mag(param[0], param[1], param[2]);

    return '\000';
}

change_mag(x, y, z)
int x, y, z;
{
    int prev = globalShrink;
    int next = 0;
    int k, u, v;

    for (k = 0; k<MAXFUNC(3,SHRINKS); ++k)
	if (z & (1<<(2-k)))
	    next = k;

    u = (X_origin[prev] + x) * shrink[prev];
    v = (Y_origin[prev] + y) * shrink[prev];

    X_origin[next] = u / shrink[next] - screenWidth[next]/2;
    Y_origin[next] = v / shrink[next] - screenHeight[next]/2;

    globalShrink = next;
}

adjust_origin()
{
    if (X_origin[globalShrink] >=
      imageWidth[globalShrink] - screenWidth[globalShrink])
	X_origin[globalShrink] =
	    imageWidth[globalShrink] - screenWidth[globalShrink];
    if (X_origin[globalShrink] < 0)
	X_origin[globalShrink] = 0;
    if (Y_origin[globalShrink] >=
      imageHeight[globalShrink] - screenHeight[globalShrink])
	Y_origin[globalShrink] =
	    imageHeight[globalShrink] - screenHeight[globalShrink];
    if (Y_origin[globalShrink] < 0)
	Y_origin[globalShrink] = 0;
}

help_screen()
{
    int i;
    char buffer[80];
    static char *help_message_1[] = {
	"    h, j, k, l, H, J, K, L, <ff>, r,\n",
	"       <arrows>, <page>, <home>:        move around the page\n",
	"    <spc>, f, n, <nl>, <cr>, <next>:    next page       (*)\n",
	"    b, p, <bs>, <del>, <prev>:          previous page   (*)\n",
	"    g, G, <beg>, <end>:                 goto page       (*)\n",
	"    m, M:                               change the magnification\n",
	"    R, <open>:                          re-open dvi file\n",
	"    ?, <help>:                          this help screen\n",
	"    q, x, ^C, ^D, <cancl>, <exit>:      exit\n",
	"\n",
	"The (*) commands can be prefixed by a number.  The program can\n",
	NULL };

    static char *help_message_2[] = {
	"be used to change magnifications (using one of the 3 buttons)\n",
	"and move a point to the centre of the screen.\n",
	"\n",
	"    Press any key to continue\n",
	NULL };

    inHelp = 1;
    clearWindow();

    for (i = 0; i < sizeof (help_message_1)/sizeof (char *); ++i)
       write (WindowFD, help_message_1[i], strlen(help_message_1[i]));

    sprintf(buffer, 
	"display the dvi file at %d, %d and %d dpi.  The mouse can also\n",
	dviDPI/shrink[0], dviDPI/shrink[1], dviDPI/shrink[2]);
    write (WindowFD, buffer, strlen(buffer));

    for (i = 0; i < sizeof (help_message_2)/sizeof (char *); ++i)
       write (WindowFD, help_message_2[i], strlen(help_message_2[i]));

    getKeyboard();

    clearWindow();
    inHelp = 0;
}
