#ifndef lint
static char *dv_c = "$Header: dv.c,v 1.7 88/08/21 19:43:31 kagotani Exp $";
#endif 	lint

#include <sys/types.h>
#include <sys/param.h>	/* MAXPATHLEN */
#include <X11/Xlib.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>	/* rindex() */
#include "dvi.h"
#include "pxl.h"

/*
#define PAPER_WIDTH	p_round(paper_width)
#define PAPER_HEIGHT	p_round(paper_height)
#define X_PAGE_OFFSET	0
#define Y_PAGE_OFFSET	0
*/
#define PAPER_WIDTH	ROUNDUP(landscape?height_in_lbpdots:width_in_lbpdots,\
				shrink_factor)
#define PAPER_HEIGHT	ROUNDUP(landscape?width_in_lbpdots:height_in_lbpdots,\
				shrink_factor)
#define X_PAGE_OFFSET	ROUNDUP(lbp_dpi*(100+leftm)/100,shrink_factor)
#define Y_PAGE_OFFSET	ROUNDUP(lbp_dpi*(100+topm)/100,shrink_factor)
/*
#define X_PAGE_OFFSET	ROUNDUP(lbp_dpi*0.63,shrink_factor)
#define Y_PAGE_OFFSET	ROUNDUP(lbp_dpi*0.29,shrink_factor)
*/

#define p_round(x)          ROUNDUP(x, dvi_per_pxl)
#define dvi_round(x)        ((long)((x) * dvi_per_pxl))

#define one(fp)     num (fp, 1)
#define two(fp)     num (fp, 2)
#define stwo(fp)    snum(fp, 2)
#define four(fp)    num (fp, 4)
#define sfour(fp)   snum(fp, 4)

typedef unsigned char ubyte;

struct frame *stack;
int sp;

#define HH      stack[sp].h
#define VV      stack[sp].v
#define WW      stack[sp].w
#define XX      stack[sp].x
#define YY      stack[sp].y
#define ZZ      stack[sp].z

#ifdef DEBUG
int debug = 0;
#define DBG_BITMAP	0x1
#define DBG_DVI		0x2
#define DBG_ALL		(DBG_BITMAP|DBG_DVI)
#endif

/*
 * Command line flags.
 */
int	no_kanji_font = 0;
int	landscape = 0;

int	lbp_dpi = 300;
long	width_in_lbpdots;
long	height_in_lbpdots;

int leftm=0, topm=0; /* 0.01 inch */

#define	DEFAULT_SHRINK	4
int shrink_factor = DEFAULT_SHRINK;

FILE *dvi_file;				/* user's file */

int font_not_found = 0;
struct font **current_fonts;
struct font *current_font = NULL;	/* ptr into circular list of fonts */

/*
 * Table of page offsets in DVI file, indexed by page number - 1.
 * Initialized in prepare_pages().
 */
struct page	*pages;

/*
 * DVI preamble and postamble information.
 */
char	job_id[300];
int	total_pages, maxstack;
int	current_page, making_page;
long	numerator, denominator, magnification;

long	dvi_per_pxl;

/*
 * Offset in DVI file of last page, set in read_postamble().
 */
long last_page_offset;


extern unsigned long	forepix, backpix, highpix;

long	window_w, window_h, page_w, page_h;
long	paper_width, paper_height;
long	origin_x, origin_y;

unsigned long	num();
long		snum();

char	*malloc(), *calloc(), *index();

int	reverse = 0;
int	bwidth = 2;
char	*fore_color;
char	*back_color;
char	*high_color;
char	*brdr_color;
char	*mous_color;
char	*geometry;
char	*shrink;
char	*display;
char	*prog;

main(argc, argv)
	int argc;
	char **argv;
{
	char *file;

	prog = *argv++;
	if ( rindex(prog, '/') )  prog = rindex(prog, '/') + 1;
	argc--;
	file = NULL;
	width_in_lbpdots = 17 * lbp_dpi / 2;
	height_in_lbpdots = 11 * lbp_dpi;
	while (argc) {
		if (strcmp(*argv, "-k") == 0)
			no_kanji_font = 1;
#ifdef DEBUG
		else if (strncmp(*argv, "-DEBUG", 6) == 0)
			debug = isdigit(argv[0][2]) ? atoi(&argv[0][2])
						    : DBG_ALL;
#endif
		else if (strcmp(*argv, "-s") == 0 && argc > 1) {
			argv++;
			argc--;
			shrink_factor = atoi(*argv);
			if (shrink_factor <= 0) usage();
		} else if (strcmp(*argv, "-dpi") == 0 && argc > 1) {
			long	new_lbp_dpi;
			argv++;
			argc--;
			new_lbp_dpi = atoi(*argv); /* DPI of LBP */
			if (new_lbp_dpi <= 0) usage();
			width_in_lbpdots
				= width_in_lbpdots * new_lbp_dpi / lbp_dpi;
			height_in_lbpdots
				= height_in_lbpdots * new_lbp_dpi / lbp_dpi;
			lbp_dpi = new_lbp_dpi;
		} else if (strcmp(*argv, "-portrait") == 0) {
			landscape = 0;
		} else if (strcmp(*argv, "-landscape") == 0) {
			landscape = 1;
		} else if (strcmp(*argv, "-paper") == 0) {
			argv++;
			argc--;
			if (strcmp(*argv, "a4") == 0
			  ||strcmp(*argv, "A4") == 0) {
				width_in_lbpdots = 210 * lbp_dpi / 25.4;
				height_in_lbpdots = 297 * lbp_dpi / 25.4;
			} else if (strcmp(*argv, "b4") == 0
			         ||strcmp(*argv, "B4") == 0) {
				width_in_lbpdots = 258 * lbp_dpi / 25.4;
				height_in_lbpdots = 364 * lbp_dpi / 25.4;
			} else if (strcmp(*argv, "letter") == 0) {
				width_in_lbpdots = 17 * lbp_dpi / 2;
				height_in_lbpdots = 11 * lbp_dpi;
			}
		} else if (strcmp(*argv, "-page") == 0) {
			argv++;
			argc--;
			current_page = atoi(*argv) - 1;
			if (current_page < 0) usage();
		} else if (strcmp(*argv, "-topmargin") == 0) {
			argv++;
			argc--;
			topm = atoi(*argv);
		} else if (strcmp(*argv, "-leftmargin") == 0) {
			argv++;
			argc--;
			leftm = atoi(*argv);
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
		} else if (strcmp(*argv, "-geometry") == 0 && argc > 1) {
			argv++;
			argc--;
			geometry = *argv;
		} else if (strcmp(*argv, "-display") == 0 && argc > 1) {
			argv++;
			argc--;
			display = *argv;
		} else if (**argv != '-') {
			file = *argv;
		} else {
			usage();
		}
		argv++;
		argc--;
	}
	if (file == NULL)
		usage();
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
	open_display( display );
	xdefaults();
	setup_fontpath() ;
	process_preamble();
	find_postamble();
	read_postamble();
	prepare_pages();

	define_conv();
	init_page();
	init_window();
	init_pixmap();

	reset_fonts();

	do_pages();
	stop_output(0);
}


usage()
{
	fprintf(stderr, "Usage: xdvi [-s <shrink>]\n");
	fprintf(stderr, "            [-dpi <pixels>]\n");
	fprintf(stderr, "            [-topmargin <margin 1/100inch>]\n");
	fprintf(stderr, "            [-leftmargin <margin 1/100inch>]\n");
	fprintf(stderr, "            [-portrait | -landscape]\n");
	fprintf(stderr, "            [-paper {letter,a4,b4}]\n");
	fprintf(stderr, "            [-page <startpage>]\n");
	fprintf(stderr, "            [-k] /*suppresses displaying kanji*/\n");
	fprintf(stderr, "            [-rv]\n");
	fprintf(stderr, "            [-fg <color>]\n");
	fprintf(stderr, "            [-bg <color>]\n");
	fprintf(stderr, "            [-hl <color>]\n");
	fprintf(stderr, "            [-bd <color>]\n");
	fprintf(stderr, "            [-ms <color>]\n");
	fprintf(stderr, "            [-geometry <geometry>]\n");
	fprintf(stderr, "            [-display <display>]\n");
	fprintf(stderr, "            dvi_file[.dvi]\n");
	exit( 1 );
}

/*
**      process_preamble reads the information in the preamble and stores
**      it into global variables for later use.
*/
process_preamble()
{
	ubyte   k;

	if ((k=one(dvi_file)) != PRE)
		error("xdvi: DVI file doesn't start with preamble");
	if (one(dvi_file) != 2)
		error("xdvi: Wrong version of DVI output for this program");
	numerator     = four(dvi_file);
	denominator   = four(dvi_file);
	magnification = four(dvi_file);
	k = one(dvi_file);
	fread(job_id, sizeof(char), k, dvi_file);
	job_id[k] = '\0';
}


define_conv()
{
	double fraction, conv;
	/*
	** In fact, `conv' should(may) be calculated as follows.
	** But `texpr' ignores this, so...
	**
	**fraction = ((double) numerator * magnification)
	**                                 / ((double) denominator * 1000.);
	**conv = (fraction * lbp_dpi) / (100000 * 2.54 * shrink_factor);
	*/
	conv = (double)lbp_dpi / ((double)72.27*0x10000L*shrink_factor);
	dvi_per_pxl = (double)72.27*0x10000L*shrink_factor / lbp_dpi;
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

	if (one(dvi_file) != POST)
	    error("xdvi: Postamble doesn't begin with POST");
	last_page_offset = four(dvi_file);
	if (numerator != four(dvi_file)
		  ||  denominator != four(dvi_file)
		  ||  magnification != four(dvi_file))
	    error("xdvi: Postamble doesn't match preamble");
	paper_height = four(dvi_file);
	paper_width = four(dvi_file);
	maxstack = two(dvi_file);
	total_pages = two(dvi_file);
	maxstack *= total_pages;
	do {
	    switch(cmnd = one(dvi_file)) {
		case FNTDEF1:
		case FNTDEF2:
		case FNTDEF3:
		case FNTDEF4:
		    define_font(cmnd);
		    break;
		case POSTPOST:
		    break;
		default:
		    error("xdvi: Non-fntdef cmnd found in postamble");
	    }
	} while (cmnd != POSTPOST);
	if (font_not_found)
		error("xdvi: Not all PXL files were found");
}


prepare_pages()
{
	int i;

	pages = (struct page*)calloc(sizeof(struct page), total_pages);
	if (pages == NULL)
		error("xdvi: Can't allocate page directory (%d pages)",
							total_pages);
	for (i = 0; i < total_pages; i++) {
		pages[i].stack = (struct frame *)
				malloc(sizeof(struct frame) * (maxstack+1));
		pages[i].font = current_font;
	}

	i = total_pages;
	fseek(dvi_file, pages[--i].dvioff = last_page_offset, 0);
	/*
	 * Follow back pointers through pages in the DVI file,
	 * storing the offsets in the "pages" table.
	 */
	while (i > 0) {
		num(dvi_file, 1+4+(9*4));
		fseek(dvi_file, pages[--i].dvioff = four(dvi_file), 0);
	}
}


do_pages()
{
	origin_x = 0;
	origin_y = 0;
	making_page = current_page;
	main_loop();
}


restore_context()
{
	fseek(dvi_file, pages[making_page].dviptr, 0);
	stack = pages[making_page].stack;
	sp = pages[making_page].sp;
	current_font = pages[making_page].font;
}


save_context()
{
	pages[making_page].sp = sp;
	pages[making_page].font = current_font;
	pages[making_page].dviptr = ftell(dvi_file);
}


make_one()
{
	ubyte ch;

	ch = one(dvi_file);
	if (ch <= SETCHAR0 + 127) {
		put_char((long)ch, 1);
	} else if (FNTNUM0 <= ch  &&  ch <= FNTNUM0 + 63) {
		change_font((unsigned long) (ch - FNTNUM0));
	} else {
		long a, b;

		switch (ch) {
		case SET1:
		case SET2:
		case SET3:
		case SET4:
			put_char(snum(dvi_file, ch - SET1 + 1), 1);
			break;

		case SETRULE:
			a = sfour(dvi_file); b = sfour(dvi_file);
			if (a > 0  &&  b > 0) {
			    put_rule(a, b);
			}
			HH += b;
			break;

		case PUT1:
		case PUT2:
		case PUT3:
		case PUT4:
			put_char(snum(dvi_file, ch - PUT1 + 1), 0);
			break;

		case PUTRULE:
			a = sfour(dvi_file); b = sfour(dvi_file);
			if (a > 0  &&  b > 0) {
			    put_rule(a, b);
			}
			break;

		case NOP:
			break;

		case BOP:
			num(dvi_file, 11*4);
			sp = 0;
			HH = dvi_round(X_PAGE_OFFSET);
			VV = dvi_round(Y_PAGE_OFFSET);
			WW = XX = YY = ZZ = 0;
			begin_page();
			break;

		case EOP:
			if (sp > 0)
			    error("Stack not empty at EOP (%d)",
				   sp);
			end_page();
			break;

		case PUSH:
			sp++;
			if (sp > maxstack)
			    error("xdvi: More PUSHes than were promised");
			stack[sp] = stack[sp - 1];
			break;

		case POP:
			sp--;
			if (sp < 0)
			    error("xdvi: More POPs than PUSHes");
			break;

		case RIGHT1:
		case RIGHT2:
		case RIGHT3:
		case RIGHT4:
			HH += snum(dvi_file, ch - RIGHT1 + 1);
			break;

		case X0:
		case X1:
		case X2:
		case X3:
		case X4:
			if (ch != X0)
			    XX = snum(dvi_file, ch - X0);
			HH += XX;
			break;

		case W0:
		case W1:
		case W2:
		case W3:
		case W4:
			if (ch != W0)
			    WW = snum(dvi_file, ch - W0);
			HH += WW;
			break;

		case Y0:
		case Y1:
		case Y2:
		case Y3:
		case Y4:
			if (ch != Y0)
			    YY = snum(dvi_file, ch - Y0);
			VV += YY;
			break;

		case Z0:
		case Z1:
		case Z2:
		case Z3:
		case Z4:
			if (ch != Z0)
			    ZZ = snum(dvi_file, ch - Z0);
			VV += ZZ;
			break;

		case DOWN1:
		case DOWN2:
		case DOWN3:
		case DOWN4:
			VV += snum(dvi_file, ch - DOWN1 + 1);
			break;

		case FNT1:
		case FNT2:
		case FNT3:
		case FNT4:
			change_font(num(dvi_file, ch - FNT1 + 1));
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
			fseek(dvi_file, (long)(12+ch-FNTDEF1+1), 1);
			a = one(dvi_file) + one(dvi_file);
			fseek(dvi_file, (long) a, 1);
			break;

		case PRE:
			error("xdvi: Shouldn't happen: PRE");
			break;

		case POST:
			error("xdvi: Shouldn't happen: POST");
			break;

		case POSTPOST:
			error("xdvi: Shouldn't happen: POSTPOST");
			break;

		default:
			error("xdvi: Unknown op-code %d, offset %d",
				ch, ftell(dvi_file));
		} /* end switch*/
	} /* end else (ch not a SETCHAR or FNTNUM) */
}


put_char(ch, update)
long	ch;
int	update;
{
	register struct glyph *g;

	if ( ch > 255 ) {
		int c;
		register FONT_CH *p;
		char get_char_type();

		c = get_char_type((unsigned short)ch);
		p = &current_font->ch[c];

		/*
		if (current_font->size > 0) {
			put_kanji(p_round(HH-p->xoff), p_round(VV-p->yoff), ch);
		}
		*/
		put_kanji(p_round(HH-p->xoff), p_round(VV-p->yoff), ch);

		if (update) {
			HH += p->tfmw;
		}
	} else {
		g = &current_font->glyph[ch];
		if (g->bitmap.bits == NULL) {
			read_pxl_bitmap(ch, g);
		}
	
		put_bitmap(&g->bitmap, p_round(HH) - g->x, p_round(VV) - g->y);
		if ( update ) {
			HH += g->adv;
		}
	}
}


put_rule(h, w)
long	h;	/* height DVI-pt */
long	w;	/* width  DVI-pt */
{
	/* (HH,VV) specifies lower left corner of rule box */
	put_rectangle(p_round(HH), p_round(VV-h), p_round(w), p_round(h));
}


begin_page()
{
	clear_page();
	put_border(0, 0, page_w, page_h, 1);
	debugprintf(("making %d\n",making_page));
}


end_page()
{
	debugprintf(("end %d\n",making_page));
	pages[making_page].complete = 1;
}


main_loop()
{
	int ch, arg, sign, number, next_page;
	char *string;
	int nbytes;

#ifdef lint
	number = 0;
#endif
	realize_page();
	arg = 0;
	for (;;) {
		nbytes = nextstring( &string );

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
			next_page = current_page + 1;
			break;
		case 'p':
		case 'b':
		case '\b':
			/* scroll backward */
			next_page = current_page - 1;
			break;
		case 'k':
			if (origin_y == 0) goto bad;
			origin_y -= window_h/4;
			if (origin_y < 0)
			    origin_y = 0;
			next_page = current_page;
			realize_page();
			continue;
			break;
		case 'j':
			if (origin_y >= page_h - window_h) goto bad;
			origin_y += window_h/4;
			if (origin_y > page_h - window_h)
			    origin_y = page_h - window_h;
			if (origin_y < 0)
			    origin_y = 0;
			next_page = current_page;
			realize_page();
			continue;
			break;
		case 'h':
			if (origin_x == 0) goto bad;
			origin_x -= window_w/4;
			if (origin_x < 0)
			    origin_x = 0;
			next_page = current_page;
			realize_page();
			continue;
			break;
		case 'l':
			if (origin_x >= page_w - window_w) goto bad;
			origin_x += window_w/4;
			if (origin_x > page_w - window_w)
			    origin_x = page_w - window_w;
			if (origin_x < 0)
			    origin_x = 0;
			next_page = current_page;
			realize_page();
			continue;
			break;
		case 's':
			if (!arg) {
			    int shrink = shrink_factor;
			    long fac1, fac2;
			    shrink_factor = 1;
			    fac1 = ROUNDUP(PAPER_WIDTH, window_w);
			    fac2 = ROUNDUP(PAPER_HEIGHT, window_h);
			    number = MAX(fac1,fac2);
			}
			if (number <= 0) goto bad;
			if (number != shrink_factor) {
			    shrink_factor = number;
			    origin_x = 0;
			    origin_y = 0;
			    define_conv();
			    init_page();
			    init_pixmap();
			    reset_fonts();
			}
		case '\f':
			/* redisplay current page */
			next_page = current_page;
			break;
		case '\r':
		case '\n':
			/* go to relative page */
			next_page = current_page + (arg ? number : 1);
			break;
		case 'g':
			/* go to absolute page */
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
		if (next_page < 0 || total_pages <= next_page ) {
			goto bad;
		}
		current_page = next_page;
		realize_page();
		arg = 0;		/* throw away numeric argument */
		continue;
	bad:
		beep();
		if (!pages[making_page].complete) realize_page();
		arg = 0;		/* throw away numeric argument */
		continue;
	}
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
	register long x;

	for (x = 0; size > 0; size --) {
		x = x * 0x100 + (unsigned) getc(fp);
	}
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
	xfree();
	exit(sig);
}


/* VARARGS1 */
error(message, a, b, c, d, e, f)
	char *message;
{
	fprintf(stderr, message, a, b, c, d, e, f);
	putc('\n', stderr);
	exit(1);
}


init_page()
{
	int	i;
	page_h = PAPER_HEIGHT;
	page_w = PAPER_WIDTH;
	for (i = 0; i < total_pages; i ++) {
		pages[i].dviptr = pages[i].dvioff;
		pages[i].pixmap = 0;
		pages[i].complete = 0;
	}
}


put_border(x, y, w, h, t)
	long x, y, w, h, t;
{
	put_rectangle(x, y, w, t);
	put_rectangle(x, y, t, h);
	put_rectangle(x, y + h - t, w, t);
	put_rectangle(x + w - t, y, t, h);
}
