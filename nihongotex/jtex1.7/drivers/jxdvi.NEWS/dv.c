/*
 * DVI previewer for X.
 *
 * Eric Cooper, CMU, September 1985.
 *
 * Code derived from dvi-imagen.c.
 *
 * Modified for X.10 by Bob Scheifler, MIT LCS, January 1986.
 * Modified for jxdvi/dvi2nwp by Atsuo Kawaguchi, ISIR,
 * $Header: dv.c,v 1.4 88/02/29 15:22:57 atsuo Exp $
 *
 */
#ifndef lint
static char *dv_c = "$Header: dv.c,v 1.4 88/02/29 15:22:57 atsuo Exp $";
#endif 	lint

#include <sys/types.h>
#include <X/Xlib.h>
#include <stdio.h>
#include <ctype.h>
#include "jxdvi.h"
#include "font.h"
#include "dvi.h"
#include "pxl.h"

#ifdef DVI2NWP
#include "add.h"
#endif

/* These are probably site dependent */
#define FONT_SUFFIX	".%dpxl"

#define PAPER_WIDTH	ROUNDUP(17*pixels_per_inch,shrink_factor*2)
#define PAPER_HEIGHT	ROUNDUP(11*pixels_per_inch,shrink_factor)
#define X_PAGE_OFFSET	ROUNDUP(pixels_per_inch,shrink_factor)
#define Y_PAGE_OFFSET	ROUNDUP(pixels_per_inch,shrink_factor)

#define pixel_round(x)      ((long) (conv * (double) (x) + 0.5))
#define dvi_round(x)        ((long) ((double) (x) / conv + 0.5))

/* (moved to font.h)
 typedef unsigned char ubyte;*/

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

/* (moved to font.h)
#define DBG_BITMAP	0x1
#define DBG_DVI		0x2
#define DBG_ALL		(DBG_BITMAP|DBG_DVI)
*/

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
int	boot_mode = 0;
int	dump_mode = 0;

int pixels_per_inch = 400;
#ifdef JXDVI
int shrink_factor = 5;
#endif JXDVI
#ifdef DVI2NWP
int shrink_factor = 1;
#endif DVI2NWP

FILE *dvi_file;				/* user's file */

int font_not_found = 0;
struct font *current_font = NULL;	/* ptr into circular list of fonts */
#define MAX_OPEN_FONTS 8
int n_open_fonts = 0;			/* for LRU management of fonts */

/*
 * DVI preamble and postamble information.
 */
char job_id[300];
int total_pages, maxstack;
int current_page;
double fraction, conv;
long numerator, denominator, magnification;

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
#define xdvi_width 15
#define xdvi_height 15
#define xdvi_x_hot 7
#define xdvi_y_hot 7
static short xdvi_bits[] = {
   0x0080, 0x01c0, 0x03e0, 0x06b0,
   0x0c98, 0x188c, 0x3086, 0x7fff,
   0x3086, 0x188c, 0x0c98, 0x06b0,
   0x03e0, 0x01c0, 0x0080};

Window win;
int forepix, backpix, highpix;

extern char reverse_byte[];

int GXfunc;
#endif JXDVI

long screen_w, screen_h, page_w, page_h;
long min_x, max_x, min_y, max_y, base_x, base_y;
long smin_x, smax_x, smin_y, smax_y;
int redisplay = 0;

int backwards = 0;
unsigned long num();
long snum();

char *malloc(), *calloc(), *index();

main(argc, argv)
	int argc;
	char **argv;
{
	char *prog, *file;
	char *option;
#ifdef JXDVI
	char *display = NULL;
	OpaqueFrame frame;
	int reverse = 0;
	int bwidth = 2;
	char *fore_color;
	char *back_color;
	char *high_color;
	char *brdr_color;
	char *mous_color;
	char *geometry = NULL, def[32];
	int backmap, bdrmap, mouspix;
	Color cdef;
#endif JXDVI
#ifdef DVI2NWP
	extern char *(stdinToTempFile());
#endif
	int indian = 1;

	if (*(char *) &indian) backwards = 0; else backwards = 1;

	prog = *argv++;
	argc--;

#ifdef JXDVI
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
	file = NULL;
#endif JXDVI
#ifdef DVI2NWP
/* Ignore following Arguments (passed by lpr): 
 *              -x<n>, -y<n>, -n <name> and -h <machine>.
 */
/* Following new switches are added:
 *  [-di] [-v] [-ds] [-o] [-e] [-xo bit] [-yo bit] [-bp page] [-ep page]
 *	[-boot] [-core]
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
 */
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
        } else if (strcmp(*argv,"-v") == 0) {
			verbose = 1;
		} else if (strcmp(*argv,"-ds") == 0) {
			double_side = 1;
		} else if (strcmp(*argv,"-o") == 0) {
			odd_pages = 1;
		} else if (strcmp(*argv,"-e") == 0) {
			even_pages = 1;
		} else if (strcmp(*argv, "-xo") == 0 && argc > 1) {
			argv++;
			argc--;
			x_offset = atoi(*argv);
printf("x_offset:%d\n",x_offset);
		} else if (strcmp(*argv, "-yo") == 0 && argc > 1) {
			argv++;
			argc--;
			y_offset = atoi(*argv);
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
#ifdef ACCT
	    loguser = argv[1] ;
#endif
            argv += 2; argc -= 2;
        } else if (strncmp(*argv,"-h",2) == 0) {
#ifdef ACCT
	    loghost = argv[1] ;
#endif
            argv += 2; argc -= 2;
	} else {
		if (direct) file = *argv;
#ifdef ACCT
		else acctfile = *argv;
#endif ACCT
	}
#endif DVI2NWP
#ifdef JXDVI
		if (strncmp(*argv, "-d", 2) == 0)
			debug = isdigit(argv[0][2]) ? atoi(&argv[0][2]) : DBG_ALL;
		else if (strcmp(*argv, "-l") == 0)
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
				display = *argv;
			else
				file = *argv;
		} else {
		    usage:
			fprintf(stderr, "Usage: xdvi [-s <shrink>] [-p <pixels>] [-l] [-rv] [-fg <color>] [-bg <color>] [-hl <color>] [-bd <color>] [-ms <color>] [=<geometry>] [host:display] dvi_file\n");
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
	if (file == NULL)
		goto usage;
#endif JXDVI

#ifdef JXDVI
	if ((dvi_file = fopen(file, "r")) == NULL) {
		int n = strlen(file);
		char *dvi_name;

		if (strcmp(file + n - sizeof(".dvi") + 1, ".dvi") == 0) {
			perror(file);
			exit(1);
		}
		dvi_name = malloc((unsigned) n + sizeof(".dvi"));
		sprintf(dvi_name, "%s.dvi", file);
		if ((dvi_file = fopen(dvi_name, "r")) == NULL) {
			perror(dvi_name);
			exit(1);
		}
 	}
#endif JXDVI
#ifdef DVI2NWP
	if ( direct ) {
		if ((dvi_file = fopen(file, "r")) == NULL) {
			int n = strlen(file);
			char *dvi_name;
	
			if (strcmp(file + n - sizeof(".dvi") + 1, ".dvi") == 0) {
				perror(file);
				exit(0);
			}
			dvi_name = malloc((unsigned) n + sizeof(".dvi"));
			sprintf(dvi_name, "%s.dvi", file);
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
	if (XOpenDisplay(display) == NULL) {
	    fprintf(stderr, "%s: Can't open display '%s'\n",
		    prog, XDisplayName(display));
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
			XGetHardwareColor(&cdef))
			highpix = cdef.pixel;
		if (brdr_color && XParseColor(brdr_color, &cdef) &&
			XGetHardwareColor(&cdef))
			bdrmap = XMakeTile(cdef.pixel);
		if (mous_color && XParseColor(mous_color, &cdef) &&
			XGetHardwareColor(&cdef))
			mouspix = cdef.pixel;
	}
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
	XDefineCursor(win,
	    XCreateCursor(xdvi_width, xdvi_height, xdvi_bits, xdvi_bits,
			  xdvi_x_hot, xdvi_y_hot, mouspix, backpix, GXcopy));

#endif JXDVI
#ifdef DVI2NWP
	init_lbp();
#endif DVI2NWP

	do_pages();
	
	stop_output(0);
}

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
	fseek(dvi_file, sfour(dvi_file), 0);
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
	int page_width, page_height;

        if (one(dvi_file) != POST)
	    error("xdvi: Postamble doesn't begin with POST");
	last_page_offset = four(dvi_file);
	if (numerator != four(dvi_file)
	          ||  denominator != four(dvi_file)
		  ||  magnification != four(dvi_file))
	    error("xdvi: Postamble doesn't match preamble");
	page_height = pixel_round(four(dvi_file));
	page_width = pixel_round(four(dvi_file));
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
				len = one(dvi_file) + one(dvi_file);
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

/*
**      define_font reads the rest of the fntdef command and then reads in
**      the specified PXL file, adding it to the global linked-list holding
**      all of the fonts used in the job.
*/
/* define_font(cmnd) */

open_pxl_file(font)
	struct font *font;
{
	char filename[300];
	extern int errno;

	if (font->file == NULL) {
		sprintf(filename, "%s/%s",
				FONT_DIRECTORY, font->fontname);
		if (n_open_fonts == MAX_OPEN_FONTS)
			close_lru();
		font->file = fopen(filename, "r");
		if (font->file == NULL) {
			font_not_found = 1;
#ifdef JXDVI
			printf("%s [not found]\n", font->fontname);
#endif
#ifdef DVI2NWP
			fprintf(stderr,"%s [not found]\n", font->fontname);
#endif
			return (0);
		}
		n_open_fonts += 1;
	}
	if (list_fonts)
		printf("%s\n", font->fontname);
	return (1);
}

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
	register struct font *f;
	register struct glyph *g;

	f = current_font;
	for (;;) {
	    open_pxl_file(f);
	    for (g = &f->glyph[0]; g < &f->glyph[MAXCHARS]; g += 1) {
#	ifdef JXDVI
		if (g->bitmap.bits) free(g->bitmap.bits);
#	endif JXDVI
#	ifdef DVI2NWP
		if (g->bitmap.nbits) free(g->bitmap.nbits);
#	endif DVI2NWP
	    }
	    read_glyphs(f);
	    f = f->next;
	    if (f == current_font) break;
	}
}

read_glyphs (fontp)
        register struct font *fontp;
{
	register struct glyph *g;
	long checksum, magnify, design_size, font_dir_ptr, pxl_id_word;

	/* seek to trailer info */
	fseek(fontp->file, (long) -(5 * BYTES_PER_LONG), 2);
        checksum = four(fontp->file);
        magnify = four(fontp->file);
        design_size = four(fontp->file);
        font_dir_ptr = sfour(fontp->file) * 4;
        pxl_id_word = four(fontp->file);
#ifdef lint
	magnify = design_size = pxl_id_word = magnify;
#endif
	/* seek to font directory */
	fseek(fontp->file, font_dir_ptr, 0);
	for (g = &fontp->glyph[0]; g < &fontp->glyph[MAXCHARS]; g += 1) {
#	ifdef JXDVI
		g->bitmap.bits = NULL;
#	endif JXDVI
#	ifdef DVI2NWP
		g->bitmap.nbits = NULL;
#	endif DVI2NWP
		g->bitmap.w = two(fontp->file);	/* leave this for shrink_bitmap */
		g->bitmap.h = two(fontp->file);	/* leave this for shrink_bitmap */
		g->x = stwo(fontp->file) / shrink_factor;
		g->y = stwo(fontp->file) / shrink_factor;
		g->addr = four(fontp->file) * 4;
		/*
		**  The TFM-width word is kind of funny in the units
		**  it is expressed in.  It works this way:
		**
		**  If a glyph has width 'w' in a font with design-size
		**  'd' (both in same units), the TFM-width word is
		**
		**                    (w/d) * 2^20
		**
		**  Therefore, in order to find the glyph width in
		**  DVI units (1 / 2^16 points), we take the design-size
		**  'd' (in DVI's), the magnification 'm' of the PXL file
		**  and the TFM-width word 't' to the width (in DVI's)
		**  as follows:
		**
		**                     dmt
		**                w = -----
		**                    2^20
		**
		**  But the magnification of the PXL file is just the
		**  scaled size 's' over the design size, so the final
		**  expression for the width is
		**
		**                     st
		**                w = ----
		**                    2^20
		**
		*/      
		g->dvi_adv =
			((double) fontp->scale * four(fontp->file)) / (1 << 20);
		g->pxl_adv = pixel_round(g->dvi_adv);
	}
}

#define nope(str)       error("xdvi: %s not implemented", str)
#define correct()       (PXL_H = pixel_round(DVI_H))

do_pages()
{
    ubyte ch;

	min_x = 0;
	min_y = 0;
	max_x = screen_w;
	max_y = screen_h;
	base_x = min_x;
	base_y = min_y;

#ifdef JXDVI
	current_page = 0;
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
				begin_page();
				break;

			    case EOP:
				if (stackp > 0)
				    error("Stack not empty at EOP (%d)",
				    	   stackp);
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
				a = one(dvi_file) + one(dvi_file);
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
}

set_char(ch)
	ubyte ch;
{
	struct glyph *g;

	font_manager(GET_GLYPH, (unsigned int)ch, &g,
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
 	register char *bp ;
 	register char c ;
 
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
	if (debug)
		return;
#ifdef JXDVI
	if (!redisplay)
	    clear_page();

	put_border(0, 0, page_w, page_h, 1);
#endif JXDVI

#ifdef DVI2NWP
	clear_image_memory();
#endif DVI2NWP
}

end_page()
{
	int ch, arg, sign, number, next_page;
	char *string;
	int nbytes;
#ifdef JXDVI
	XEvent event;
#endif JXDVI

#ifdef lint
	number = 0;
#endif

	if (debug) {
		if (++current_page == total_pages)
			exit(0);
		return;
	}

#ifdef JXDVI
	if (redisplay) {
	    min_x = smin_x;
	    max_x = smax_x;
	    min_y = smin_y;
	    max_y = smax_y;
	    redisplay = 0;
	}
	arg = 0;
	for (;;) {
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
			min_x = 0;
			min_y = 0;
			max_x = screen_w;
			max_y = screen_h;
			next_page = current_page + 1;
			break;
		    case 'p':
		    case 'b':
		    case '\b':
			/* scroll backward */
			min_x = 0;
			min_y = 0;
			max_x = screen_w;
			max_y = screen_h;
			next_page = current_page - 1;
			break;
		    case 'u':
			if (min_y == 0) goto bad;
			min_y -= screen_h;
			if (min_y < 0)
			    min_y = 0;
			base_y = min_y;
			max_y = min_y + screen_h;
			next_page = current_page;
			break;
		    case 'd':
			if (min_y >= page_h - screen_h) goto bad;
			min_y += screen_h;
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
			min_x -= screen_w;
			if (min_x < 0)
			    min_x = 0;
			base_x = min_x;
			max_x = min_x + screen_w;
			next_page = current_page;
			break;
		    case 'r':
			if (min_x >= page_w - screen_w) goto bad;
			min_x += screen_w;
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
			    shrink_factor = number;
			    min_x = 0;
			    min_y = 0;
			    max_x = screen_w;
			    max_y = screen_h;
			    init_page();
			    define_conv();
			    reset_fonts();
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
		    default:
			goto bad;
		}
		if (0 <= next_page && next_page < total_pages) {
			current_page = next_page;
			fseek(dvi_file, page_offset[current_page], 0);
			break;
		}
	bad:
		XFeep(0);
		arg = 0;		/* throw away numeric argument */
		continue;
	}
#endif JXDVI
#ifdef DVI2NWP
	print_frame(current_page);
	if ( odd_pages || even_pages ) {
		current_page += 2;
	} else {
		current_page++;
	}
	if (0 <= current_page && current_page < total_pages &&
		current_page <= end_page_num) {
		fseek(dvi_file, page_offset[current_page], 0);
	} else {
		stop_output(0);
	}
#endif DVI2NWP
}

special(nbytes)
	unsigned long nbytes;
{
	char *cmd;
	int i;

	cmd = malloc((unsigned) nbytes+1);
	if (cmd == NULL)
		error("xdvi: Can't allocate memory for special (%d bytes)", nbytes);
	for (i = 0; i < nbytes; i += 1)
		cmd[i] = getc(dvi_file);
	cmd[i] = '\0';
	fprintf(stderr, "special ``%s'' not implemented\n", cmd);
	free(cmd);
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
#ifdef JXDVI
	report_statistics();
#endif JXDVI
#ifdef DVI2NWP
	end_lbp();
	if ( direct && verbose )
		report_statistics();
#endif DVI2NWP
#endif
#ifdef ACCT
	if (direct == 0)
		account(loguser, loghost, acctfile) ;
#endif

	if (dump_mode || boot_mode) {
	    font_manager(CLOSE_FONT_FILE, NULL, NULL, NULL, NULL,NULL,NULL,NULL);
		dump_mode = 0;
		boot_mode = 0;
#	ifdef DVI2NWP
		direct = 0;
#	endif DVI2NWP
		abort();
	}

#ifdef JXDVI
	exit(sig);
#endif JXDVI
#ifdef DVI2NWP
	exit(0);
#endif DVI2NWP
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
	page_h = PAPER_HEIGHT;
	page_w = PAPER_WIDTH;
}

#ifdef JXDVI
clear_page()
{
	XClear(win);
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

put_rectangle(x, y, w, h, pix)
	long x, y, w, h;
	int pix;
{
#ifdef JXDVI
	if (x < max_x && x + w >= min_x && y < max_y && y + h >= min_y)
		XPixSet(win, x - base_x, y - base_y, w, h, pix);
#endif JXDVI
#ifdef DVI2NWP
	put_rectangle_to_image(x, y, w, h);
#endif DVI2NWP
}

#ifdef JXDVI
put_bitmap(bitmap, x, y, pix)
	register struct bitmap *bitmap;
	register long x, y;
	int pix;
{
	if ((bitmap->w == 0) || (bitmap->h == 0)) return;
	if (x < max_x && x + bitmap->w >= min_x &&
	    y < max_y && y + bitmap->h >= min_y)
		XBitmapBitsPut(win, x - base_x, y - base_y,
					bitmap->w, bitmap->h, (char *) bitmap->bits,
				pix, backpix, NULL, GXfunc, AllPlanes);
}
#endif JXDVI
#ifdef JXDVI
sample(bitmap, x, y, w, h)
	register struct bitmap *bitmap;
	int x, y, w, h;
{
	register char *ptr, *endp;
	register int b, i, j, m, n;

	ptr = bitmap->bits
		+ (y * bitmap->bytes_wide)
		+ (x / BITS_PER_BYTE);
	endp = bitmap->bits + (bitmap->h * bitmap->bytes_wide);
	b = (1 << (x % BITS_PER_BYTE));
	n = 0;
	for (i = 0; i < h && ptr < endp; i += 1, ptr += bitmap->bytes_wide) {
		for (m = b, j = 0; j < w; j += 1, m <<= 1) {
			if (*ptr & m)
				n += 1;
		}
	}
/*      return (n >= (i * w) / 3);*/
/* In some cases (for example, 400dpi font shrunk by 2), shrink_bitmap results
 * underlined font. This is corrected by Atsuo, K. by changing sample() so
 * that it returns (n>=(h*w)/3) instead (n>=(i*w)/3). 
 */
        return (n >= (h * w) / 3);
}

shrink_bitmap(bitmap, x_factor, y_factor)
	register struct bitmap *bitmap;
	int x_factor, y_factor;
{
	char *shrunk_bits;
	int shrunk_height, shrunk_width, shrunk_bytes_wide;
	register char *ptr;
	char *cp;
	register int x, y, b, m;

	shrunk_height = ROUNDUP(bitmap->h, y_factor);
	shrunk_width = ROUNDUP(bitmap->w, x_factor);
	shrunk_bytes_wide = ROUNDUP(shrunk_width, BITS_PER_SHORT)*BYTES_PER_SHORT;
		/* width must be multiple of 16 bits for raster_op */
	ptr = shrunk_bits = calloc((unsigned) shrunk_height * shrunk_bytes_wide, 1);
	if (ptr == NULL)
		error("Can't allocate shrunken bitmap (%d by %d)",
			shrunk_height, shrunk_width);
	for (y = 0; y < bitmap->h; y += y_factor) {
		b = 0;
		m = (1 << 0);
		cp = ptr;
		for (x = 0; x < bitmap->w; x += x_factor) {
			if (sample(bitmap, x, y, x_factor, y_factor))
				*ptr |= m;
			else
				*ptr &= ~m;
			b += 1;
			m <<= 1;
			if (b % BITS_PER_BYTE == 0) {
				b = 0;
				m = (1 << 0);
				ptr += 1;
			}
		}
		ptr = cp + shrunk_bytes_wide;
	}
	free(bitmap->bits);
	bitmap->bits = shrunk_bits;
	bitmap->h = shrunk_height;
	bitmap->w = shrunk_width;
	bitmap->bytes_wide = shrunk_bytes_wide;
}
#endif JXDVI
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
#ifdef JXDVI
print_bitmap(bitmap)
	register struct bitmap *bitmap;
{
	register char *ptr;
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
#endif JXDVI
#ifdef DVI2NWP
print_nbitmap(bitmap)
	register struct bitmap *bitmap;
{
	register char *ptr;
	register int x, y, i;

	ptr = bitmap->nbits;
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
#endif DVI2NWP
#ifdef JXDVI
print_bitmap2(bitmap)
	register struct bitmap *bitmap;
{
	register char *ptr;
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
