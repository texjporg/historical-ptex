/*
 * DVI previewer for X.
 *
 * Eric Cooper, CMU, September 1985.
 *
 * Code derived from dvi-imagen.c.
 *
 * Modified for X.10 by Bob Scheifler, MIT LCS, January 1986.
 * Modified for jxdvi/dvi2nwp by Atsuo Kawaguchi, ISIR,
 * $Header: dv.c,v 1.3 89/10/28 13:46:35 kono Locked $
 *
 */

#include "atsuo-copyright.h"  

#include "config.h"

#ifndef lint
static char *dv_c = "$Header: dv.c,v 1.3 89/10/28 13:46:35 kono Locked $";
#endif 	lint

#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>

#ifdef JXDVI
# ifndef X11 /* X10 */
#  include <X/Xlib.h>
# else /* X11 */
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
#  include <X11/cursorfont.h>
# endif /* X Version */
#include "window.h"
#endif /* jXDVI */

#include "jxdvi.h"
#include "font.h"
#include "dvi.h"

#ifdef DVI2NWP
#include "add.h"
#endif /* DVI2NWP */

#define PAPER_WIDTH	ROUNDUP(paper_width*pixels_per_inch,shrink_factor)
#define PAPER_HEIGHT	ROUNDUP(paper_height*pixels_per_inch,shrink_factor)
#define X_PAGE_OFFSET	ROUNDUP(pixels_per_inch,shrink_factor)	/* 1 inch */
#define Y_PAGE_OFFSET	ROUNDUP(pixels_per_inch,shrink_factor)	/* 1 inch */

#define PAPER_WIDTH_B4  ROUNDUP(20*pixels_per_inch,shrink_factor*2)
#define PAPER_HEIGHT_B4 ROUNDUP(15*pixels_per_inch,shrink_factor)

struct frame {
	long pxl_h, dvi_h, pxl_v, dvi_v, w, x, y, z;
};

struct frame *stack;
int stackp;

#define PXL_H   stack[stackp].pxl_h
#define PXL_V   stack[stackp].pxl_v
#define DVI_H   stack[stackp].dvi_h
#define DVI_V   stack[stackp].dvi_v
#define WW      stack[stackp].w
#define XX      stack[stackp].x
#define YY      stack[stackp].y
#define ZZ      stack[stackp].z

#ifdef ACCT
/*
 * Accounting stuff
 */
char *loguser = NULL ;
char *loghost = NULL ;
char *acctfile = NULL ;
int npages = 0 ;
#endif ACCT

/*
 * Command line flags.
 */
int debug = 0;
int list_fonts = 0;
int boot_mode = 0;
int dump_mode = 0;
int landscape = 0;
int b4mode = 0;

#ifdef JXDVI
#  ifdef DEFAULT_DPI
int pixels_per_inch = DEFAULT_DPI;
#  else
int pixels_per_inch = 400;
#  endif /* DPI */
#  ifdef DEFAULT_SHRINK
int shrink_factor = DEFAULT_SHRINK;
#  else
int shrink_factor = 5;
#  endif /* Shrink Factor */
#endif JXDVI
#ifdef DVI2NWP
int pixels_per_inch = 400;
int shrink_factor = 1;
int page_step = 1;
#endif /* DVI2NWP */

FILE *dvi_file;				/* user's file */

int font_not_found = 0;
struct font *current_font = NULL;	/* ptr into circular list of fonts */
int n_open_fonts = 0;			/* for LRU management of fonts */

/*
 * DVI preamble and postamble information.
 */
char job_id[300];
int total_pages, maxstack;
int current_page;
double fraction, conv;
long numerator, denominator, magnification;
long page_width, page_height;		/* in sp */

/*
 * Offset in DVI file of postamble, set in find_postamble().
 */
long postamble_offset;

/*
 * Offset in DVI file of last page, set in read_postamble().
 */
long last_page_offset;

/*
 * Table of page offsets in DVI file, indexed by page number - 1.
 * Initialized in prepare_pages().
 */
long *page_offset;

#ifdef JXDVI
#  ifndef X11 /* X10 */
Cursor xdvi_cursor, watch_cursor;
int current_cursor;
/* xdvi cursor */
#define xdvi_width 15
#define xdvi_height 15
#define xdvi_x_hot 7
#define xdvi_y_hot 7
static short xdvi_bits[] = {
     0x0080, 0x01c0, 0x03e0, 0x06b0,
     0x0c98, 0x188c, 0x3086, 0x7fff,
     0x3086, 0x188c, 0x0c98, 0x06b0,
     0x03e0, 0x01c0, 0x0080};

/* watch cursor */
#define watch_width 15
#define watch_height 15
#define watch_x_hot 7
#define watch_y_hot 7
static short watch_bits[] = {
   0x0ff8, 0x0ff8, 0x0ff8, 0x1c1c,
   0x3086, 0x6083, 0x6083, 0x4781,
   0x6003, 0x6003, 0x3006, 0x1c1c,
   0x0ff8, 0x0ff8, 0x0ff8};

#  else /* X11 */
#  endif /* X Version */
#endif /* JXDVI */

int paper_size = A4_PAPER;		/* A4, B4 or LETTER */
float paper_width = A4_WIDTH;		/* paper width in inches */
float paper_height = A4_HEIGHT;		/* paper height in inches */

long screen_w, screen_h, page_w, page_h;
long min_x, max_x, min_y, max_y, base_x, base_y;
long smin_x, smax_x, smin_y, smax_y;
int redisplay = 0;

int backwards = 0;
unsigned long num();
long snum();

char *file_name;
char *dvi_file_name;

jmp_buf re_read;

int	start_page_l = 0;

main(argc, argv)
	int argc;
	char **argv;
{
	char *prog;
	char *option;
	int indian = 1;
#ifdef JXDVI
	int start_page = 0;
	int reverse = 0;
	int bwidth = 2;
	char *display_name = NULL;
	char *fore_color;
	char *back_color;
	char *high_color;
	char *brdr_color;
	char *mous_color;
	char *geometry = NULL, def[32];
#  ifndef X11 /* X10 */
	OpaqueFrame frame;
	int backmap, bdrmap, mouspix;
	Color cdef;
#  else /* X11 */
	int argc_sv = argc;
	char **argv_sv = argv;
	unsigned long mouspix;
	int backmap, bdrmap;
	XColor cdef;
#  endif /* X Version */
#endif JXDVI
#ifdef DVI2NWP
	extern char *(stdinToTempFile());
	int	sig_handler();
#endif /* DVI2NWP */

#ifdef DVI2NWP
	/* Catch SIGINT to stop printing. */
	if ( signal(SIGINT, SIG_IGN) == SIG_DFL ) {
		signal(SIGINT, sig_handler);
	}

#endif /* DVI2NWP */

	if (*(char *) &indian) backwards = 0; else backwards = 1;

	prog = *argv++;
	argc--;

#ifdef JXDVI
#  ifndef X11 /* X10 */
	if ((option = XGetDefault(prog, "ReverseVideo")) &&
	    strcmp(option, "on") == 0)
		reverse = 1;
	if (option = XGetDefault(prog, "BorderWidth"))
		bwidth = atoi(option);
	fore_color = XGetDefault(prog, "ForeGround");
	back_color = XGetDefault(prog, "BackGround");
	high_color = XGetDefault(prog, "Highlight");
	brdr_color = XGetDefault(prog, "Border");
	mous_color = XGetDefault(prog, "Mouse");
	file_name = NULL;
#  else /* X11 */
	if ((display=XOpenDisplay(display_name)) == NULL) {
	    fprintf(stderr, "%s: Can't open display '%s'\n",
		    prog, XDisplayName(display_name));
	    exit(1);
	}
	scr_num = DefaultScreen(display);

	if ((option = XGetDefault(display, prog, "ReverseVideo")) &&
	    strcmp(option, "on") == 0)
		reverse = 1;
	if (option = XGetDefault(display, prog, "BorderWidth"))
		bwidth = atoi(option);
	fore_color = XGetDefault(display, prog, "ForeGround");
	back_color = XGetDefault(display, prog, "BackGround");
	high_color = XGetDefault(display, prog, "Highlight");
	brdr_color = XGetDefault(display, prog, "Border");
	mous_color = XGetDefault(display, prog, "Mouse");
	file_name = NULL;
#  endif /* X Version */
#endif JXDVI
#ifdef DVI2NWP
/* Ignore following Arguments (passed by lpr): 
 *              -x<n>, -y<n>, -n <name> and -h <machine>.
 */
/* Following new switches are added:
 *  [-di] [-v] [-ds] [-o] [-e] [-xo bit] [-yo bit] [-bp page] [-ep page]
 *  [-l] [-boot] [-core]
 *		-di			direct print mode (called from shell)
 *		-v			verbose mode (print several information)
 *		-ds			double side mode (even pages are offset to left)
 *		-o      	odd pages only
 *		-e      	even pages only
 *		-bp page	begin printing from 'page'
 *		-ep page	end printing at 'page'
 *		-xo bit		offset 'bit' horizontally (+ -> right, - ->left)
 *		-yo bit		offset 'bit' vertically (+ -> downward, - -> upward)
 *		-boot		font ladging mode (load font and dump core)
 *		-core		core dump mode (dump core when exits)
 *		-noprint	do not produce actual output on nwp533
 *					(shinoda@cs.titech.junet)
 *		-l		Landscape mode
 *					(setsu@aml.co.jp]
 */
	{
	    /* last 2 chars of command are options */
	    int i;
	    i = strlen(prog)-1;
	    if(i>0)
		if(prog[i]=='L')
		    landscape = 1;
		else if (prog[i]=='B')
		    b4mode = 1;
	    i--;
	    if(i>0)
		if(prog[i]=='L')
		    landscape = 1;
		else if (prog[i]=='B')
		    b4mode = 1;
	}
#endif DVI2NWP
	while (argc) {
	    if (strcmp(*argv,"-boot") == 0) {
		boot_mode = 1;
	    } else if (strcmp(*argv,"-core") == 0) {
		dump_mode = 1;
	    } else
#ifdef DVI2NWP
	    if (strcmp(*argv,"-di") == 0) {
		direct = 1;
	    } else if (strcmp(*argv, "-l") == 0) {
			landscape = 1;
	    } else if (strcmp(*argv, "-b4") == 0) {
		b4mode = 1;
	    } else if (strcmp(*argv,"-v") == 0) {
		verbose = 1;
	    } else if (strcmp(*argv,"-ds") == 0) {
		double_side = 1;
	    } else if (strcmp(*argv,"-o") == 0) {
		odd_pages = 1;
		page_step = 2;
	    } else if (strcmp(*argv,"-e") == 0) {
		even_pages = 1;
		page_step = 2;
	    } else if (strcmp(*argv, "-xo") == 0 && argc > 1) {
		argv++;
		argc--;
		x_offset += atoi(*argv);
		printf("x_offset:%d\n",x_offset);
	    } else if (strcmp(*argv, "-yo") == 0 && argc > 1) {
		argv++;
		argc--;
		y_offset += atoi(*argv);
		printf("y_offset:%d\n",y_offset);
	    } else if (strcmp(*argv, "-bp") == 0 && argc > 1) {
		argv++;
		argc--;
		begin_page_num = atoi(*argv);
		begin_page_num--; /* because page counting starts from 0 */
	    } else if (strcmp(*argv, "-ep") == 0 && argc > 1) {
		argv++;
		argc--;
		end_page_num = atoi(*argv);
		end_page_num--;  /* because page counting starts from 0 */
	    } else if (strncmp(*argv,"-x",2) == 0) {
		argv++; argc--;
	    } else if (strncmp(*argv,"-noprint", 8) == 0) {
		noprint = 1 ;
	    } else if (strncmp(*argv,"-y",2) == 0) {
		argv++; argc--;
	    } else if (strncmp(*argv,"-n",2) == 0) {
# ifdef ACCT
		loguser = argv[1] ;
# endif /* ACCT */
		argv += 2; argc -= 2;
	    } else if (strncmp(*argv,"-h",2) == 0) {
# ifdef ACCT
		loghost = argv[1] ;
# endif /* ACCT */
		argv += 2; argc -= 2;
	    } else {
		if (direct) file_name = *argv;
# ifdef ACCT
		else acctfile = *argv;
# endif /* ACCT */
	    }
#endif DVI2NWP
#ifdef JXDVI
	    /* Switches:
	     *   [-d <level>] [-s <shrink>] [-p <pixels>] [-l] [-list] [-rv] 
	     *   [-fg <color>] [-bg <color>] [-hl <color>] [-bd <color>]
	     *   [-ms <color>] [-core] [=<geometry>] [host:display]
	     *   [-A4 or -B4 or -LETTER]
	     */
	    if (strncmp(*argv, "-d", 2) == 0)
	      debug = isdigit(argv[0][2]) ? atoi(&argv[0][2]) : DBG_ALL;
	    else if (strcmp(*argv, "-l") == 0)
	      landscape = 1;
	    else if (strcmp(*argv, "-list") == 0)
	      list_fonts = 1;
	    else if (strcmp(*argv, "-s") == 0 && argc > 1) {
		argv++;
		argc--;
		shrink_factor = atoi(*argv);
		if (shrink_factor <= 0) goto usage;
	    } else if (strcmp(*argv, "-p") == 0 && argc > 1) {
		argv++;
		argc--;
		pixels_per_inch = atoi(*argv);
		if (pixels_per_inch <= 0) goto usage;
	    } else if (strcmp(*argv, "-page") == 0 && argc > 1) {
		argv++;
		argc--;
		start_page_l = atoi(*argv);
		if ( start_page_l < 0 ) goto usage;
	    } else if (strcmp(*argv, "-A4") == 0) {
		paper_size = A4_PAPER;
		paper_width = A4_WIDTH;
		paper_height = A4_HEIGHT;
	    } else if (strcmp(*argv, "-B4") == 0) {
		paper_size = B4_PAPER;
		paper_width = B4_WIDTH;
		paper_height = B4_HEIGHT;
	    } else if (strcmp(*argv, "-LETTER") == 0) {
		paper_size = LETTER_PAPER;
		paper_width = LETTER_WIDTH;
		paper_height = LETTER_HEIGHT;
	    } else if (strcmp(*argv, "-rv") == 0) {
		reverse = 1;
	    } else if (strcmp(*argv, "-fg") == 0 && argc > 1) {
		argv++;
		argc--;
		fore_color = *argv;
	    } else if (strcmp(*argv, "-bg") == 0 && argc > 1) {
		argv++;
		argc--;
		back_color = *argv;
	    } else if (strcmp(*argv, "-hl") == 0 && argc > 1) {
		argv++;
		argc--;
		high_color = *argv;
	    } else if (strcmp(*argv, "-bd") == 0 && argc > 1) {
		argv++;
		argc--;
		brdr_color = *argv;
	    } else if (strcmp(*argv, "-ms") == 0 && argc > 1) {
		argv++;
		argc--;
		mous_color = *argv;
	    } else if (**argv == '=') {
		geometry = *argv;
	    } else if (**argv != '-') {
		if (index(*argv, ':') != NULL)
		  display_name = *argv;
		else
		  file_name = *argv;
	    } else {
	      usage:
		fprintf(stderr, "Usage: jxdvi [-s <shrink>] [-p <pixels>] [-page <start_page>] [-l] [-list] [-rv] [-fg <color>] [-bg <color>] [-hl <color>] [-bd <color>] [-ms <color>] [-core] [-A4 or -B4 or -LETTER] [=<geometry>] [host:display] dvi_file\n");
		exit(1);
	    }
#endif JXDVI
	    argv++;
	    argc--;
	}

	if (boot_mode == 1) {
		pre_load_font();
	}

#ifdef JXDVI
	if (file_name == NULL)
		goto usage;
#endif JXDVI

#ifdef JXDVI
	if ((dvi_file = fopen(file_name, "r")) == NULL) {
		int n = strlen(file_name);
		char *dvi_name;

		if (strcmp(file_name + n - sizeof(".dvi") + 1, ".dvi") == 0) {
			perror(file_name);
			exit(1);
		}
		dvi_name = malloc((unsigned) n + sizeof(".dvi"));
		sprintf(dvi_name, "%s.dvi", file_name);
		if ((dvi_file = fopen(dvi_name, "r")) == NULL) {
			perror(dvi_name);
			exit(1);
		}
		/* Save the exact file name string. */
		dvi_file_name = dvi_name;	
 	} else {
		dvi_file_name = file_name;
	}
#endif JXDVI
#ifdef DVI2NWP
	if ( direct ) {
		if ((dvi_file = fopen(file_name, "r")) == NULL) {
			int n = strlen(file_name);
			char *dvi_name;
	
			if (strcmp(file_name + n - sizeof(".dvi") + 1, ".dvi") == 0) {
				perror(file_name);
				exit(0);
			}
			dvi_name = malloc((unsigned) n + sizeof(".dvi"));
			sprintf(dvi_name, "%s.dvi", file_name);
			if ((dvi_file = fopen(dvi_name, "r")) == NULL) {
				perror(dvi_name);
				exit(0);
			}
	 	}
	} else {
		dvi_file = stdin;
	}
#endif DVI2NWP

	process_preamble();
	find_postamble();
	read_postamble();
	prepare_pages();
	init_page();

#ifdef JXDVI
#  ifndef X11 /* X10 */
	if (XOpenDisplay(display_name) == NULL) {
	    fprintf(stderr, "%s: Can't open display '%s'\n",
		    prog, XDisplayName(display_name));
	    exit(1);
	}
 	if (reverse) {
		forepix = WhitePixel;
		highpix = WhitePixel;
		backpix = BlackPixel;
		backmap = BlackPixmap;
		bdrmap = WhitePixmap;
		mouspix = WhitePixel;
		GXfunc = GXor;
	} else {
		forepix = BlackPixel;
		highpix = BlackPixel;
		backpix = WhitePixel;
		backmap = WhitePixmap;
		bdrmap = BlackPixmap;
		mouspix = BlackPixel;
		GXfunc = GXand;
	}
#  else /* X11 */
	/* XOpenDisplay is called before XGetDefaults calls. */

	cmap = DefaultColormapOfScreen(DefaultScreenOfDisplay(display));
	if (reverse) {
		forepix = WhitePixel(display,scr_num);
		highpix = WhitePixel(display,scr_num);
		backpix = BlackPixel(display,scr_num);
		mouspix = WhitePixel(display,scr_num);
		GXfunc = GXor;
	} else {
		forepix = BlackPixel(display,scr_num);
		highpix = BlackPixel(display,scr_num);
		backpix = WhitePixel(display,scr_num);
		mouspix = BlackPixel(display,scr_num);
		GXfunc = GXand;
	}
#  endif /* X Version */
#  ifndef X11 /* X10 */
	if (DisplayCells() > 2) {
		if (fore_color && XParseColor(fore_color, &cdef) &&
			XGetHardwareColor(&cdef))
			forepix = cdef.pixel;
		if (back_color && XParseColor(back_color, &cdef) &&
			XGetHardwareColor(&cdef)) {
			backpix = cdef.pixel;
			backmap = XMakeTile(backpix);
		}
		if (high_color && XParseColor(high_color, &cdef) &&
			XGetHardwareColor(&cdef)) {
		    highpix = cdef.pixel;
		}
		if (brdr_color && XParseColor(brdr_color, &cdef) &&
			XGetHardwareColor(&cdef)) {
		    bdrmap = XMakeTile(cdef.pixel);
		}
		if (mous_color && XParseColor(mous_color, &cdef) &&
			XGetHardwareColor(&cdef)) {
		    mouspix = cdef.pixel;
		}
		GXfunc = GXcopy;
	}
#  else /* X11 */
	if ((scr_depth=DefaultDepth(display,scr_num)) > 2) {
/*		image.depth=scr_depth;*/
		bit_plane = 0x01 << (scr_depth - 1);
		put_bitmap=put_color_bitmap;
		if (fore_color && 
		    XParseColor(display,cmap,fore_color,&cdef) &&
		    XAllocColor(display,cmap,&cdef)) {
			forepix = cdef.pixel;
		}
		if (back_color &&
		    XParseColor(display,cmap,back_color,&cdef) &&
		    XAllocColor(display,cmap,&cdef)) {
			backpix = cdef.pixel;
		}
		if (high_color &&
		    XParseColor(display,cmap,high_color,&cdef) &&
		    XAllocColor(display,cmap,&cdef)) {
			highpix = cdef.pixel;
		}
		if (brdr_color &&
		    XParseColor(display,cmap,brdr_color,&cdef) &&
		    XAllocColor(display,cmap,&cdef)) {
		}
		if (mous_color &&
		    XParseColor(display,cmap,mous_color,&cdef) &&
		    XAllocColor(display,cmap,&cdef)) {
			mouspix = cdef.pixel;
		}
	} else {
		put_bitmap=put_mono_bitmap;
	}
#  endif /* X Version */
#  ifndef X11 /* X10 */
	frame.bdrwidth = bwidth;
	frame.height = page_h;
	if (frame.height + (bwidth << 1) > DisplayHeight())
	    frame.height = DisplayHeight() - (bwidth << 1);
	frame.width = page_w;
	if (frame.width + (bwidth << 1) > DisplayWidth())
	    frame.width = DisplayWidth() - (bwidth << 1);
	frame.border = bdrmap;
	frame.background = backmap;
	frame.x = 0;
	frame.y = 0;
	sprintf(def, "=%dx%d+0+0", frame.width, frame.height);
	win = XCreate("DVI Previewer", prog, geometry, def, &frame, 50, 50);
	screen_h = frame.height;
	screen_w = frame.width;
	XMapWindow(win);
	XSelectInput(win, KeyPressed|ButtonPressed|ExposeWindow|ExposeRegion);
	xdvi_cursor = XCreateCursor(xdvi_width, xdvi_height, xdvi_bits, 
				    xdvi_bits, xdvi_x_hot, xdvi_y_hot, 
				    mouspix, backpix, GXcopy);
	watch_cursor = XCreateCursor(watch_width, watch_height, watch_bits, 
				    watch_bits, watch_x_hot, watch_y_hot, 
				    mouspix, backpix, GXcopy);
	current_cursor = XC_XDVI;	/* force to show watch cursor first */
	set_watch_cursor();
#  else /* X11 */
	sattr.background_pixel = backpix;
	sattr.border_pixel = forepix;
	win = XCreateWindow(display, DefaultRootWindow(display), 
			    0, 0, page_w, page_h,
			       bwidth, CopyFromParent, InputOutput,
			       CopyFromParent,
			       CWBackPixel | CWBorderPixel,
			       &sattr);

	hints.flags = 0;
	XSetStandardProperties(display,win,
			       "jxdvi","jxdvi",None,argv_sv,argc_sv,&hints);
	XSelectInput(display,win,
		     KeyPressMask|ButtonPressMask|ExposureMask|
		     StructureNotifyMask);

	watch_cursor = XCreateFontCursor(display,XC_watch);
	circle_cursor = XCreateFontCursor(display,XC_circle);
	current_cursor = XC_circle;	/* force to show watch cursor first */
	set_watch_cursor();

/**/
	gcvalues.function = GXor;
/*	gcvalues.function = GXcopy; */
	gcvalues.foreground = forepix;
	gcvalues.background = backpix;
/*	gcvalues.plane_mask = 0x01;*/


/*
	cmap = DefaultColormapOfScreen(DefaultScreenOfDisplay(display));
	XParseColor(display,cmap,"red",&cdef);
	XAllocColor(display,cmap,&cdef);
	gcvalues.function	= GXcopy;
	gcvalues.foreground	= cdef.pixel;
	gcvalues.background	= WhitePixel(display, scr_num);
*/
	gc = XCreateGC(display,win,
		       GCFunction|GCForeground|GCBackground,
		       &gcvalues);

	gcvalues.function = GXcopy;
	copy_gc = XCreateGC(display,win,
		       GCFunction|GCForeground|GCBackground,
		       &gcvalues);

	gcvalues.foreground = backpix;
	back_gc = XCreateGC(display,win,
		       GCFunction|GCForeground|GCBackground,
		       &gcvalues);

	XMapWindow(display,win);
	XSync(display,0);
	{
	  XEvent event;

	  /* Wait until the window is really mapped.
	   * I'm not sure wheter this is a proper way or not, but
	   * XSync() seems not to work and this way works on Sun3
	   */
	  XNextEvent (display,&event);	/* Discard */
	}

	XGetWindowAttributes(display,win,&wattr);
	screen_h = (long)wattr.height;
	screen_w = (long)wattr.width;
#  endif /* X Version */
#endif JXDVI

#ifdef DVI2NWP
	init_lbp();
#endif DVI2NWP

#ifdef JXDVI
	start_page = setjmp(re_read);
	if (start_page_l == 0)
	  do_pages(0);
	else if ( start_page != 0 ) {
	  start_page_l = 0;
	  do_pages((start_page - 1));
	} else {
	  do_pages(start_page_l -1);
	}
#endif /* JXDVI */
#ifdef DVI2NWP
	do_pages(0);
#endif /* DVI2NWP */
	
	stop_output(0);
}

#ifdef DVI2NWP
sig_handler(sig)
int	sig;
{
	fprintf(stderr, "dvi2nwp:Interrupted. Exiting\n");
	stop_output(0);
}
#endif /* DVI2NWP */

/*
 * Accounting is added by shinoda@cs.titech.junet
 */
#ifdef ACCT
account(user, host, acctfname)
	char *user, *host, *acctfname ;
{
	FILE *af ;

	if (user == NULL || acctfname == NULL)
		return ;

	if ((af = fopen(acctfname, "a")) == NULL)
		return ;

	fprintf(af, "%6d\t", npages) ;

	if (host)
		fprintf(af, "%s:", host) ;

	fprintf(af, "%s\n", user) ;

	fclose(af) ;
}
#endif ACCT

/*
**      process_preamble reads the information in the preamble and stores
**      it into global variables for later use.
*/
process_preamble()
{
	ubyte   k;

	if (one(dvi_file) != PRE)
		error("xdvi: DVI file doesn't start with preamble");
	if (one(dvi_file) != 2)
		error("xdvi: Wrong version of DVI output for this program");
	numerator     = four(dvi_file);
	denominator   = four(dvi_file);
	magnification = four(dvi_file);
#ifdef DEBUG
	printf("pro_pre numerator:%d\n",numerator);
	printf("pro_pre denominator:%d\n",denominator);
	printf("pro_pre magnification:%d\n",magnification);
#endif
	define_fraction();
	define_conv();
	k = one(dvi_file);
	fread(job_id, sizeof(char), k, dvi_file);
	job_id[k] = '\0';
}

define_fraction()
{
	fraction = (((double) numerator * magnification)
	                                 / ((double) denominator * 1000.));
}

define_conv ()
{
	conv = ((fraction * pixels_per_inch) / 100000) / (2.54 * shrink_factor);
}

/*
**      find_postamble locates the beginning of the postamble
**	and leaves the file ready to start reading at that location.
*/
find_postamble()
{
	ubyte byte;
	long offset = -4;        /* At least 4 TRAILERS */

	do {
		offset -= 1;
		fseek(dvi_file, offset, 2);
		byte = one(dvi_file);
	} while (byte == TRAILER);
	if (byte != 2)
		error("xdvi: Wrong version of DVI output for this program");
	offset -= 4;
	fseek(dvi_file, offset, 2);
	postamble_offset = sfour(dvi_file);
	fseek(dvi_file, postamble_offset, 0);
}

/*
**      read_postamble reads the information in the postamble,
**	storing it into global variables.
**      It also takes care of reading in all of the PXL files for the fonts
**      used in the job.
*/
read_postamble()
{
        ubyte   cmnd;

        if (one(dvi_file) != POST)
	    error("xdvi: Postamble doesn't begin with POST");
	last_page_offset = four(dvi_file);
	if (numerator != four(dvi_file)
	          ||  denominator != four(dvi_file)
		  ||  magnification != four(dvi_file))
	    error("xdvi: Postamble doesn't match preamble");
/*
 *	page_height = pixel_round(four(dvi_file));
 *	page_width = pixel_round(four(dvi_file));
 */
	page_height = four(dvi_file);
	page_width = four(dvi_file);

	maxstack = two(dvi_file);
	total_pages = two(dvi_file);
	do {
	    switch(cmnd = one(dvi_file)) {
	        case FNTDEF1:
	        case FNTDEF2:
	        case FNTDEF3:
	        case FNTDEF4:
			{
				int	n, cksm, sc, d_sc, len;
				char	fn[128];

				n = num(dvi_file, cmnd - FNTDEF1 + 1);
				cksm = four(dvi_file);
				sc = four(dvi_file);
				d_sc = four(dvi_file);
				len = one(dvi_file); len += one(dvi_file);
				/* one() must be evaluated in this order */
				fread(fn, sizeof(char), len, dvi_file);
				fn[len] = '\0';
			    font_manager(DEFINE_FONT,(int)cmnd,n,cksm,sc,d_sc,len,fn);
			}
		    break;
		case POSTPOST:
		    break;
		default:
		    error("xdvi: Non-fntdef cmnd found in postamble");
	    }
	} while (cmnd != POSTPOST);
	if (font_not_found)
#ifdef JXDVI
		error("jxdvi: Not all PXL files were found");
#endif
#ifdef DVI2NWP
		error("dvi2nwp: Not all PXL files were found");
#endif
	list_fonts = 0;
}

prepare_pages()
{
	int i;

        stack = (struct frame *) malloc((unsigned) sizeof(struct frame) * (maxstack+1));
        if (stack == NULL)
		error("xdvi: Can't allocate stack space (%d frames)", maxstack);
	page_offset = (long *) malloc((unsigned) total_pages * sizeof(long));
        if (page_offset == NULL)
		error("xdvi: Can't allocate page directory (%d pages)", total_pages);
	i = total_pages;
	page_offset[--i] = last_page_offset;
	fseek(dvi_file, last_page_offset, 0);
	/*
	 * Follow back pointers through pages in the DVI file,
	 * storing the offsets in the page_offset table.
	 */
	while (i > 0) {
		num(dvi_file, 1+4+(9*4));
		fseek(dvi_file, page_offset[--i] = four(dvi_file), 0);
	}
}

free_pages()
{
        if ( stack != NULL ) free( (char *)stack );
	if ( page_offset != NULL ) free( (char *)page_offset );
}

/*
**      define_font reads the rest of the fntdef command and then reads in
**      the specified PXL file, adding it to the global linked-list holding
**      all of the fonts used in the job.
*/
/* define_font(cmnd) */

/* open_pxl_file(font) */
/* read_pxl_bitmap(ch, g) */

/*
 * Find font #n and move it to the head of the list.
 */
/* change_font(n) */

/*
 * Close the PXL file for the least recently used font.
 */
close_lru()
{
        register struct font *f;

	f = current_font->prev;
	for (;;) {
		if (f->file != NULL)
                        break;
		f = f->prev;
		if (f == current_font->prev)
			error("xdvi: Can't find an open PXL file to close");
	}
	fclose(f->file);
	f->file = NULL;
	n_open_fonts -= 1;
}

reset_fonts()
{
	font_manager(RESET_FONTS,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
}

#define nope(str)       error("xdvi: %s not implemented", str)
#define correct()       (PXL_H = pixel_round(DVI_H))

do_pages(start_page)
	int start_page;
{
    ubyte ch;

	min_x = 0;
	min_y = 0;
	max_x = screen_w;
	max_y = screen_h;
	base_x = min_x;
	base_y = min_y;

#ifdef JXDVI
	current_page = start_page;
	if (0 <= current_page && current_page < total_pages) {
	    fseek(dvi_file, page_offset[current_page], 0);
	} else {
	    error("Page %d not exist",current_page);
	}
#ifdef X11
    init_page_cache(total_pages,shrink_factor);
#endif /* X11 */
#endif JXDVI
#ifdef DVI2NWP
	if ( odd_pages &&  (begin_page_num%2) ) begin_page_num++;
	if ( even_pages && !(begin_page_num%2) ) begin_page_num++;
	current_page = begin_page_num;
	if (0 <= current_page && current_page < total_pages &&
			current_page <= end_page_num) {
		fseek(dvi_file, page_offset[current_page], 0);
	} else {
	    
		stop_output(0);
	}
#endif DVI2NWP
	for (;;) {
		ch = one(dvi_file);
/*printf("%d ", ch); fflush(stdout);*/
/**/ /* print_dvi(ch); */
/**/ /* printf("{pxl_h:%3d dvi_h:%8d pxl_v:%3d dvi_v:%8d w:%6d x:%6d y:%6d z:%6d}\n",PXL_H,DVI_H,PXL_V,DVI_V,WW,XX,YY,ZZ); */

		if (debug & DBG_DVI)
			print_dvi(ch);
		if (ch <= SETCHAR0 + 127) {
			set_char(ch);
			DVI_H += current_font->glyph[ch].dvi_adv;
			PXL_H += current_font->glyph[ch].pxl_adv;
			correct();
		} else if (FNTNUM0 <= ch  &&  ch <= FNTNUM0 + 63) {
			font_manager(CHANGE_FONT,(unsigned long) (ch - FNTNUM0),
					NULL,NULL,NULL,NULL,NULL,NULL);
		} else {
			long a, b;

			switch (ch) {
			    case SET1:
				nope("PUT1");
				break;

				case SET2:
				{
					unsigned int	code;
					long	dvi_adv;
					short	pxl_adv;

					code = two(dvi_file);
					set2_char(code,&dvi_adv,&pxl_adv);
					DVI_H += dvi_adv;
					PXL_H += pxl_adv;
					correct();
				}
				break;
			    case SETRULE:
				a = sfour(dvi_file); b = sfour(dvi_file);
				if (a > 0  &&  b > 0) {
				    correct();
				    set_rule(pixel_round(a), pixel_round(b));
				}
				DVI_H += b;
				PXL_H =  pixel_round(DVI_H);
				break;

			    case PUT1:
				nope("PUT1");
				break;

			    case PUTRULE:
				a = sfour(dvi_file); b = sfour(dvi_file);
				if (a > 0  &&  b > 0) {
				    correct();
				    set_rule(pixel_round(a), pixel_round(b));
				}
				break;

			    case NOP:
				break;

			    case BOP:
				num(dvi_file, 11*4);
				stackp = 0;
				DVI_H = dvi_round(X_PAGE_OFFSET);
				PXL_H = X_PAGE_OFFSET;
				DVI_V = dvi_round(Y_PAGE_OFFSET);
				PXL_V = Y_PAGE_OFFSET;
				WW = XX = YY = ZZ = 0;
#ifdef JXDVI
#ifndef X11 /* X10 */
				begin_page();
				set_watch_cursor();
#else /* X11 */
			{ 
			  int next;
			  next = begin_page();
			  switch (next) {
			  case DO_EOP:	/* seek EOP */
			    if ((current_page+1) == total_pages) {
			      fseek(dvi_file, (postamble_offset-1), 0);
			    } else {
			      fseek(dvi_file, 
				    page_offset[(current_page+1)]-1, 0);
			    }
			    break;
			  default:
			    set_watch_cursor();
			    break;
			  }
			}
#endif /* X Version */
#endif /* JXDVI */
#ifdef DVI2NWP
				begin_page();
#endif /* DVI2NWP */
				break;
			    case EOP:
				if (stackp > 0)
				    error("Stack not empty at EOP (%d)",
				    	   stackp);
#ifdef JXDVI
#  ifndef X11 /* X10 */
				set_xdvi_cursor();
#  else /* X11 */
				set_circle_cursor();
#  endif /* X Version */
#endif JXDVI
				end_page();
				if (ftell(dvi_file) > last_page_offset)
				    return;
				break;

			    case PUSH:
				stackp++;
				if (stackp > maxstack)
				    error("xdvi: More PUSHes than were promised");
				stack[stackp] = stack[stackp - 1];
				break;

			    case POP:
				stackp--;
				if (stackp < 0)
				    error("xdvi: More POPs than PUSHes");
				break;

			    case RIGHT1:
			    case RIGHT2:
			    case RIGHT3:
			    case RIGHT4:
				DVI_H += snum(dvi_file, ch - RIGHT1 + 1);
				PXL_H = pixel_round(DVI_H);
				break;

			    case X0:
			    case X1:
			    case X2:
			    case X3:
			    case X4:
				if (ch != X0)
				    XX = snum(dvi_file, ch - X0);
				DVI_H += XX;
				PXL_H += pixel_round(XX);
				correct();
				break;

			    case W0:
			    case W1:
			    case W2:
			    case W3:
			    case W4:
				if (ch != W0)
				    WW = snum(dvi_file, ch - W0);
				DVI_H += WW;
				PXL_H = pixel_round(DVI_H);
				break;

			    case Y0:
			    case Y1:
			    case Y2:
			    case Y3:
			    case Y4:
				if (ch != Y0)
				    YY = snum(dvi_file, ch - Y0);
				DVI_V += YY;
				PXL_V = pixel_round(DVI_V);
				break;

			    case Z0:
			    case Z1:
			    case Z2:
			    case Z3:
			    case Z4:
				if (ch != Z0)
				    ZZ = snum(dvi_file, ch - Z0);
				DVI_V += ZZ;
				PXL_V = pixel_round(DVI_V);
				break;

			    case DOWN1:
			    case DOWN2:
			    case DOWN3:
			    case DOWN4:
				DVI_V += snum(dvi_file, ch - DOWN1 + 1);
				PXL_V = pixel_round(DVI_V);
				break;

			    case FNT1:
			    case FNT2:
			    case FNT3:
			    case FNT4:
/*				change_font(num(dvi_file, ch - FNT1 + 1));*/
				font_manager(CHANGE_FONT,num(dvi_file, ch - FNT1 + 1),
					NULL,NULL,NULL,NULL,NULL,NULL);
				break;

			    case XXX1:
			    case XXX2:
			    case XXX3:
			    case XXX4:
				a = num(dvi_file, ch - XXX1 + 1);
				if(a > 0)
				    special((unsigned long) a);
				break;

			    case FNTDEF1:
			    case FNTDEF2:
			    case FNTDEF3:
			    case FNTDEF4:
				fseek(dvi_file, (long) (12 + ch - FNTDEF1 + 1), 1);
				a = one(dvi_file); a += one(dvi_file);
				/* one() must be evaluated in this order */
				fseek(dvi_file, (long) a, 1);
				break;

			    case PRE:
				error("xdvi: Shouldn't happen: PRE encountered.");
				break;

			    case POST:
				error("xdvi: Shouldn't happen: POST encountered.");
				break;

			    case POSTPOST:
				error("xdvi: Shouldn't happen: POSTPOST encountered.");
				break;

			    default:
				error("xdvi: Unknown op-code %d, offset %d",
					ch, ftell(dvi_file));
			} /* end switch*/
		} /* end else (ch not a SETCHAR or FNTNUM) */
	} /* end for */
}

set2_char(code, dvi_adv, pxl_adv)
unsigned long code;
long	*dvi_adv;
short	*pxl_adv;
{
	struct glyph *g;

	font_manager(GET_GLYPH, (unsigned int)code, &g,
			NULL, NULL, NULL, NULL, NULL);

#ifdef JXDVI
	put_bitmap(&(g->bitmap), (PXL_H - g->x), (PXL_V - g->y), forepix);
#endif JXDVI
#ifdef DVI2NWP
	put_bitmap_to_image(&(g->bitmap), (PXL_H - g->x), (PXL_V - g->y));
#endif DVI2NWP

	*dvi_adv = g->dvi_adv;
	*pxl_adv = g->pxl_adv;

/**/ /*printf("c:%8x dvi_adv:%d pxl_adv:%4d\n",code,g->dvi_adv,g->pxl_adv);
*/
/**/ /* printf("c:%8x x:%d y:%d\n",code,g->x, g->y);
*/
}

set_char(ch)
	ubyte ch;
{
	struct glyph *g;

	font_manager(GET_GLYPH, (Ptr)ch, (Ptr)&g,
			NULL, NULL, NULL, NULL, NULL);

#ifdef JXDVI
	put_bitmap(&(g->bitmap), (PXL_H - g->x), (PXL_V - g->y), forepix);
#endif JXDVI
#ifdef DVI2NWP
	put_bitmap_to_image(&(g->bitmap), (PXL_H - g->x), (PXL_V - g->y));
#endif DVI2NWP
}

#ifdef JXDVI
reverse_bytes(bitmap)
 	register struct bitmap *bitmap;
{
 	register long x, y;
 	register unsigned char *bp ;
 	register unsigned char c ;
 
 	bp = bitmap->bits ;
 	for ( y = 0 ; y < bitmap->h ; y++) {
 		for ( x = 0 ; x < bitmap->bytes_wide ; x += 2) {
 			c = *bp ;
 			*bp++ = *(bp + 1) ;
 			*bp++ = c ;
 		}
 	}
}
#endif JXDVI

set_rule(h, w)
	long h, w;
{
	/* (w,h) specifies lower left corner of rule box */

#ifdef JXDVI
	if ( h == 0 ) h = 1;
	if ( w == 0 ) w = 1;
	put_rectangle(PXL_H, PXL_V - h, w, h, forepix);
#endif JXDVI
#ifdef DVI2NWP
	put_rectangle_to_image(PXL_H, PXL_V - h, w, h);
#endif DVI2NWP
}

begin_page()
{
#ifdef JXDVI
#ifdef X11
  int result;
#endif /* X11 */
#endif /* JXDVI */

  if (debug)
    return;

#ifdef JXDVI
#ifndef X11 /* X10 */
  if (!redisplay)
    clear_page();

  put_border(0, 0, page_w, page_h, 1);
#else /* X11 */
  result = get_page_leaf(current_page);
  if (result == HIT && current_leaf->flag == COMPLETED) {
    return DO_EOP;		/* leaf found */
  }
  current_leaf->flag = COPY_WIN;

  if (!redisplay)
    clear_page();
  put_border(0, 0, page_w, page_h, 1);

  return CONTINUE;				/* build entire leaf */
#endif /* X Version */
#endif /* JXDVI */

#ifdef DVI2NWP
	clear_image_memory();
#endif DVI2NWP
}

end_page()
#ifdef JXDVI
{
    int ch, arg, sign, number, next_page;
    char *string;
    int nbytes;
    XEvent event;
#ifdef X11 /* X11 only */
    char keybuf[5];
#endif /* X11 */
    
#ifdef lint
    number = 0;
#endif
    
    if (debug) {
	if (++current_page == total_pages)
	  exit(0);
	return;
    }
    
    if (redisplay) {
	min_x = smin_x;
	max_x = smax_x;
	min_y = smin_y;
	max_y = smax_y;
	redisplay = 0;
    }
    arg = 0;
    
#ifdef X11
    if (current_leaf->flag == COPY_WIN) {
	/* leaf is build on the window */
	current_leaf->flag = COMPLETED;
    } else {
	/* leaf is already existed in cache */
	XCopyArea(display,current_leaf->p,win,copy_gc,
		  min_x, min_y, page_w, page_h, 0, 0);
    }
    XFlush(display);
#endif /* X11 */
    
    for (;;) {
#ifndef X11 /* X10 */
	XNextEvent (&event);
	
	switch (event.type) {
	  case ExposeWindow:
	    screen_h = ((XExposeEvent *)(&event))->height;
	    screen_w = ((XExposeEvent *)(&event))->width;
	    max_x = min_x + screen_w;
	    max_y = min_y + screen_h;
	    string = "\f";
	    nbytes = 1;
	    break;
	  case ExposeRegion:
	    smin_x = min_x;
	    smax_x = max_x;
	    smin_y = min_y;
	    smax_y = max_y;
	    min_x = min_x + ((XExposeEvent *)(&event))->x;
	    min_y = min_y + ((XExposeEvent *)(&event))->y;
	    max_x = min_x + ((XExposeEvent *)(&event))->width;
	    max_y = min_y + ((XExposeEvent *)(&event))->height;
	    redisplay = 1;
	    string = "\f";
	    nbytes = 1;
	    break;
	  case ButtonPressed:
	    {
		short detail = ((XButtonPressedEvent *) (&event))->detail;
		switch (detail & ValueMask) {
		  case LeftButton:
		    if (detail & ShiftMask)
		      string = "l";
		    else
		      string = "b";
		    nbytes = 1;
		    break;
		  case MiddleButton:
		    if (detail & ShiftMask)
		      string = "u";
		    else
		      string = "d";
		    nbytes = 1;
		    break;
		  case RightButton:
		    if (detail & ShiftMask)
		      string = "r";
		    else
		      string = "f";
		    nbytes = 1;
		    break;
		}
	    }
	    break;
	  case KeyPressed:
	    string = XLookupMapping (&event, &nbytes);
	    break;
	}
#  else /* X11 */
	XNextEvent (display,&event);
	
	switch (event.type) {
	  case Expose:
	    smin_x = min_x;
	    smax_x = max_x;
	    smin_y = min_y;
	    smax_y = max_y;
	    min_x = min_x + ((XExposeEvent *)(&event))->x;
	    min_y = min_y + ((XExposeEvent *)(&event))->y;
	    max_x = min_x + ((XExposeEvent *)(&event))->width;
	    max_y = min_y + ((XExposeEvent *)(&event))->height;
	    redisplay = 1;
	    string = "\f";
	    nbytes = 1;
	    break;
	  case ConfigureNotify:			/* Resize */
	    screen_w = ((XConfigureEvent *)(&event))->width;
	    screen_h = ((XConfigureEvent *)(&event))->height;
/*printf("w:%d h:%d\n",screen_w,screen_h);*/
	    max_x = min_x + screen_w;
	    max_y = min_y + screen_h;
	    break;
	  case ButtonPress:
	    {
		XButtonPressedEvent *bev;
		
		bev = (XButtonPressedEvent *)(&event);
		switch(bev->button)
		  {
		    case Button1:			/* Left Button */
		      if(bev->state & ShiftMask)
			string = "l";
		      else
			string = "b";
		      nbytes = 1;
		      break;
		    case Button2:			/* Middle Button */
		      if (bev->state & ShiftMask)
			string = "u";
		      else
			string = "d";
		      nbytes = 1;
		      break;
		    case Button3:			/* Right Button */
		      if (bev->state & ShiftMask)
			string = "r";
		      else
			string = "f";
		      nbytes = 1;
		      break;
		  }
	    }
	    break;
	  case KeyPress:
	    nbytes = XLookupString(((XKeyEvent *)(&event)),
				   keybuf,5,NULL,NULL);
	    string = keybuf;
	    /*		  nbytes = strlen(keybuf); */
	    break;
	  default:
	    continue;
	}
#  endif /* X Version */
	
	if (nbytes == 0) continue;
	if (nbytes > 1) goto bad;
	switch (ch = *string) {
	  case 'q':
	  case '\003':	/* control-C */
	  case '\004':	/* control-D */
	    stop_output(0);
	    break;
	  case 'n':
	  case 'f':
	  case ' ':
	    /* scroll forward */
	    base_x = smin_x = min_x = 0;
	    base_y = smin_y = min_y = 0;
	    smax_x = max_x = screen_w;
	    smax_y = max_y = screen_h;
	    next_page = current_page + 1;
	    break;
	  case 'p':
	  case 'b':
	  case '\b':
	    /* scroll backward */
	    base_x = smin_x = min_x = 0;
	    base_y = smin_y = min_y = 0;
	    smax_x = max_x = screen_w;
	    smax_y = max_y = screen_h;
	    next_page = current_page - 1;
	    break;
	  case 'u':
	    if (min_y == 0) goto bad;
	    min_y -= (4*screen_h/5);
	    if (min_y < 0)
	      min_y = 0;
	    base_y = min_y;
	    max_y = min_y + screen_h;
	    next_page = current_page;
	    break;
	  case 'd':
	    if (min_y >= page_h - screen_h) goto bad;
	    min_y += (4*screen_h/5);
	    if (min_y > page_h - screen_h)
	      min_y = page_h - screen_h;
	    if (min_y < 0)
	      min_y = 0;
	    base_y = min_y;
	    max_y = min_y + screen_h;
	    next_page = current_page;
	    break;
	  case 'l':
	    if (min_x == 0) goto bad;
	    min_x -= (4*screen_w/5);
	    if (min_x < 0)
	      min_x = 0;
	    base_x = min_x;
	    max_x = min_x + screen_w;
	    next_page = current_page;
	    break;
	  case 'r':
	    if (min_x >= page_w - screen_w) goto bad;
	    min_x += (4*screen_w/5);
	    if (min_x > page_w - screen_w)
	      min_x = page_w - screen_w;
	    if (min_x < 0)
	      min_x = 0;
	    base_x = min_x;
	    max_x = min_x + screen_w;
	    next_page = current_page;
	    break;
	  case 's':
	    if (!arg) {
		int shrink = shrink_factor;
		long fac1, fac2;
		shrink_factor = 1;
		fac1 = ROUNDUP(PAPER_WIDTH, screen_w);
		fac2 = ROUNDUP(PAPER_HEIGHT, screen_h);
		if (fac1 < fac2)
		  number = fac2;
		else
		  number = fac1;
		shrink_factor = shrink;
	    }
	    if (number <= 0) goto bad;
	    if (number != shrink_factor) {
		set_watch_cursor();
		shrink_factor = number;
		clear_page();
		init_page();
		define_conv();
		reset_fonts();
#ifdef X11
		reset_page_cache();
		init_page_cache(total_pages,shrink_factor);
#endif X11
		smin_x = min_x = 0;
		smin_y = min_y = 0;
		smax_x = max_x = screen_w;
		smax_y = max_y = screen_h;
	    }
	  case '\f':
	    /* redisplay current page */
	    next_page = current_page;
	    break;
	  case '\r':
	  case '\n':
	    /* go to relative page */
	    min_x = 0;
	    min_y = 0;
	    max_x = screen_w;
	    max_y = screen_h;
	    next_page = current_page + (arg ? number : 1);
	    break;
	  case 'g':
	    /* go to absolute page */
	    min_x = 0;
	    min_y = 0;
	    max_x = screen_w;
	    max_y = screen_h;
	    next_page = (arg ? number : total_pages) - 1;
	    break;
	  case '0': case '1': case '2': case '3': case '4':
	  case '5': case '6': case '7': case '8': case '9':
	    if (! arg) {
		arg = 1;
		sign = 1;
		number = 0;
	    }
	    number = 10*number + sign*(ch - '0');
	    continue;
	  case '-':
	    if (! arg) {
		arg = 1;
		sign = -1;
		number = 0;
		continue;
	    } else
	      goto bad;
	  case 'R': /* Re-read DVI file */
	    set_watch_cursor();
	    if (dvi_file != NULL) {
		fclose(dvi_file);
	    }
	    if ((dvi_file = fopen(dvi_file_name, "r")) == NULL) {
		perror(dvi_file_name);
		exit(1);
	    }
	    process_preamble();
	    find_postamble();
	    read_postamble();
	    free_pages();
	    prepare_pages();
	    init_page();
#ifdef X11
	    reset_page_cache();
#endif X11
	    longjmp(re_read,( current_page + 1 ));
	    break;	/* not reached */
	  default:
	    goto bad;
	}
	if (0 <= next_page && next_page < total_pages) {
	    current_page = next_page;
	    fseek(dvi_file, page_offset[current_page], 0);
	    break;
	}
      bad:
#  ifndef X11 /* X10 */
	XFeep(0);
#  else /* X11 */
	XBell(display,50);
#  endif /* X Version */
	arg = 0;		/* throw away numeric argument */
	continue;
    }
}
#endif /* JXDVI */
#ifdef DVI2NWP /* end_page() */
{
    int s;
    
    if (debug) {
	if (++current_page == total_pages)
	  exit(0);
	return;
    }

    s = print_frame(current_page);
    switch (s) {
      case PRINT_SUCCEEDED:
	current_page += page_step;
	break;
      case PRINT_PREV:
	if (current_page != begin_page_num)
	  current_page -= page_step;
	break;
      case PRINT_THIS:	/* Not to increment current_page */
	break;
      default: 		/* will never come here */
	break;
    }
    
  NOT_FINISHED:
    if (0<=current_page && current_page<total_pages &&
	current_page<=end_page_num) {
	fseek(dvi_file, page_offset[current_page], 0);
    } else if ( (s=finish_last_page()) != PRINT_SUCCEEDED) {
	current_page -= page_step;
	switch (s) {
	  case PRINT_PREV:
	    if (current_page != begin_page_num)
	      current_page -= page_step;
	    break;
	  case PRINT_THIS:
	    break;
	  default: 		/* will never come here */
	    break;
	}
	goto NOT_FINISHED;
    } else {
	stop_output(0);
    }
}
#endif /* DVI2NWP */

static char cmd0[2000];
static char *cmd = cmd0;
static int cmd_size = 2000;

special(nbytes)
	unsigned long nbytes;
{
	char *cmd;
	int i;

	if(nbytes>cmd_size) {
	    cmd = malloc((unsigned) nbytes+1);
	    cmd_size = nbytes+1;
	    if (cmd == NULL)
		error("xdvi: Can't allocate memory for special (%d bytes)",
		nbytes);
	    for (i = 0; i < nbytes; i += 1)
		    cmd[0] = getc(dvi_file);
	    return;
	}
	for (i = 0; i < nbytes; i += 1)
		cmd[i] = getc(dvi_file);
	cmd[i] = '\0';
#ifdef DVI2NWP
	applicationDoSpecial(cmd);
#else  DVI2NWP
	fprintf(stderr, "special ``%s'' not implemented\n", cmd);
#endif DVI2NWP
}

/*
**
**      Read size bytes from the FILE fp, constructing them into a
**      signed/unsigned integer.
**
*/
unsigned long
num(fp, size)
    register FILE *fp;
    register int size;
{
    register int i;
    register long x;

    x = 0;
    for ( i=0; i < size; i += 1)
        x = x * 0x100 + (unsigned) getc(fp);
    return (x);
}

long
snum(fp, size)
	register FILE *fp;
	register int size;
{
        register int i;
	register long x;

	x = getc(fp) & 0xff;
	if (x & 0x80)
        	x -= 0x100;
	for (i = 1; i < size; i += 1)
	    x = x * 0x100 + (unsigned) getc(fp);
	return (x);
}

stop_output(sig)
{
#ifdef STATISTICS
# ifdef JXDVI
    report_statistics();
# endif /* JXDVI */
# ifdef DVI2NWP
    if ( direct && verbose )
      report_statistics();
# endif /* DVI2NWP */
#endif /* STATISTICS */

    if (dump_mode || boot_mode) {
	font_manager(CLOSE_FONT_FILE, NULL, NULL, NULL, NULL,NULL,NULL,NULL);
#ifdef JXDVI
# ifdef X11
	reset_page_cache();
# endif /* X11 */
#endif /* JXDVI */
#ifdef DVI2NWP
	end_lbp();
	direct = 0;
	page_step = 1;
#endif /* DVI2NWP */
	dump_mode = 0;
	boot_mode = 0;
	abort();		/* make core */
    }
    
#ifdef JXDVI
    exit(sig);
#endif /* JXDVI */
#ifdef DVI2NWP
# ifdef ACCT
    if (direct == 0)
      account(loguser, loghost, acctfile) ;
# endif /* ACCT */
/**/ /* dump_frame();*/
    exit(0);
#endif /* DVI2NWP */
}

/* VARARGS1 */
error(message, a, b, c, d, e, f)
	char *message;
{
	fprintf(stderr, message, a, b, c, d, e, f);
	putc('\n', stderr);
	exit(0);
}

init_page()
{
  	long save;

	page_h = PAPER_HEIGHT;
	page_w = PAPER_WIDTH;

	if ( b4mode )
	  {
	    page_h = PAPER_HEIGHT_B4;
	    page_w = PAPER_WIDTH_B4;
	  }

	if ( landscape )
	  {
	    save = page_w;
	    page_w = page_h;
	    page_h = save;
	  }
}

#ifdef JXDVI
clear_page()
{
#  ifndef X11 /* X10 */
    XClear(win);
    XFlush();
#  else /* X11 */
    XClearWindow(display,win);
    if (scr_depth > 2) {
	XFillRectangle(display,image_pixmap,
		       clear_gc,
		       0,0,page_w,page_h);
    }
    XSync(display,0);
#  endif /* X Version */
}
#endif JXDVI
#ifdef JXDVI
put_border(x, y, w, h, t)
	long x, y, w, h, t;
{
	put_rectangle(x, y, w, t, highpix);
	put_rectangle(x, y, t, h, highpix);
	put_rectangle(x, y + h - t, w, t, highpix);
	put_rectangle(x + w - t, y, t, h, highpix);
}
#endif JXDVI

#ifdef DVI2NWP
put_rectangle(x, y, w, h, pix)
	long x, y, w, h;
	int pix;
{
	put_rectangle_to_image(x, y, w, h);
}
#endif DVI2NWP

#ifdef JXDVI
print_char(ch, g)
	ubyte ch;
	struct glyph *g;
{
	printf("char %d", ch);
	if (isprint(ch))
		printf(" (%c)", ch);
	putchar('\n');
	printf("x = %d, y = %d, pxl = %d, dvi = %d\n",
		g->x, g->y, g->pxl_adv, g->dvi_adv);
	print_bitmap(&g->bitmap);
}
#endif JXDVI

print_bitmap(bitmap)
	register struct bitmap *bitmap;
{
	register unsigned char *ptr;
	register int x, y, i;

	ptr = bitmap->bits;
	if (ptr == NULL)
		return;
	printf("w = %d, h = %d, bytes wide = %d\n",
		bitmap->w, bitmap->h, bitmap->bytes_wide);
	for (y = 0; y < bitmap->h; y += 1) {
		for (x = 0; x < bitmap->bytes_wide; x += 1) {
			for (i = 0; i < BITS_PER_BYTE; i += 1)
				if (*ptr & (1 << i))
					putchar('@');
				else
					putchar(' ');
			ptr += 1;
		}
		putchar('\n');
	}
}

#ifdef JXDVI
print_bitmap2(bitmap)
	register struct bitmap *bitmap;
{
	register unsigned char *ptr;
	register int x, y, i;

	ptr = bitmap->bits;
	if (ptr == NULL)
		return;

	for (y = 0; y < bitmap->h; y += 1) {
		for (x = 0; x < bitmap->bytes_wide; x += 1) {
			for (i = 0; i < BITS_PER_BYTE; i += 1)
				if (*ptr & (0x80 >> i))
					putchar('@');
				else
					putchar(' ');
			ptr += 1;
		}
		putchar('\n');
	}
}
#endif JXDVI

print_dvi(ch)
	ubyte ch;
{
	printf("%4d %4d ", PXL_H, PXL_V);
	if (ch <= SETCHAR0 + 127) {
		printf("SETCHAR%-3d", ch - SETCHAR0);
		if (isprint(ch))
			printf(" (%c)", ch);
	} else if (FNTNUM0 <= ch  &&  ch <= FNTNUM0 + 63) {
		printf("FNTNUM%d", ch - FNTNUM0);
	} else {
		switch (ch) {
		    case SET1:
			printf("SET1");
			break;
		    case SET2:
			printf("SET2");
			break;
		    case SETRULE:
			printf("SETRULE");
			break;
		    case PUT1:
			printf("PUT1");
			break;
		    case PUTRULE:
			printf("PUTRULE");
			break;
		    case NOP:
			printf("NOP");
			break;
		    case BOP:
			printf("BOP");
			break;
		    case EOP:
			printf("EOP");
			break;
		    case PUSH:
			printf("PUSH");
			break;
		    case POP:
			printf("POP");
			break;
		    case RIGHT1:
		    case RIGHT2:
		    case RIGHT3:
		    case RIGHT4:
			printf("RIGHT%d", ch - RIGHT1 + 1);
			break;
		    case X0:
		    case X1:
		    case X2:
		    case X3:
		    case X4:
			printf("X%d", ch - X0);
			break;
		    case W0:
		    case W1:
		    case W2:
		    case W3:
		    case W4:
			printf("W%d", ch - W0);
			break;
		    case Y0:
		    case Y1:
		    case Y2:
		    case Y3:
		    case Y4:
			printf("Y%d", ch - Y0);
			break;
		    case Z0:
		    case Z1:
		    case Z2:
		    case Z3:
		    case Z4:
			printf("Z%d", ch - Z0);
			break;
		    case DOWN1:
		    case DOWN2:
		    case DOWN3:
		    case DOWN4:
			printf("DOWN%d", ch - DOWN1 + 1);
			break;
		    case FNT1:
		    case FNT2:
		    case FNT3:
		    case FNT4:
			printf("FNT%d", ch - FNT1 + 1);
			break;
		    case XXX1:
		    case XXX2:
		    case XXX3:
		    case XXX4:
			printf("XXX%d", ch - XXX1 + 1);
			break;
		    case FNTDEF1:
		    case FNTDEF2:
		    case FNTDEF3:
		    case FNTDEF4:
			printf("FNTDEF%d", ch - FNTDEF1 + 1);
			break;
		    case PRE:
			printf("PRE");
			break;
		    case POST:
			printf("POST");
			break;
		    case POSTPOST:
			printf("POSTPOST");
			break;
		    default:
			error("xdvi: Unknown op-code %d, offset %d",
				ch, ftell(dvi_file));
		} /* end switch*/
	} /* end else (ch not a SETCHAR or FNTNUM) */
	putchar('\n');
}

#ifndef two
two(fp)
register FILE *fp;
{
	register int i;

	i = getc(fp)<<8;
	i += getc(fp);
	return i;
}
#endif /* two */
#ifndef four
four(fp)
register FILE *fp;
{
	register int i;

	i = getc(fp)<<24;
	i += getc(fp)<<16;
	i += getc(fp)<<8;
	i += getc(fp);
	return i;
}
#endif /* four */
