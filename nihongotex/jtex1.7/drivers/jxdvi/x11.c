#ifndef lint
static char *x11_c = "$Header: x11.c,v 1.91 88/09/06 23:39:16 kagotani Exp $";
#endif 	lint

#include <sys/types.h>
#include <sys/param.h>	/* MAXPATHLEN */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <stdio.h>
#include <ctype.h>
#include "dvi.h"
#include "pxl.h"

#define	DEFAULT_SHRINK	4
extern int shrink_factor;
extern int debug;

extern struct font *current_font;
extern int	current_page, making_page;
extern int	total_pages;
extern struct page	*pages;

#define xdvi_width 15
#define xdvi_height 15
#define xdvi_x_hot 7
#define xdvi_y_hot 7
static char xdvi_bits[] = {
     0x80, 0x00, 0xc0, 0x01, 0xe0, 0x03, 0xb0, 0x06, 0x98, 0x0c, 0x8c, 0x18,
     0x86, 0x30, 0xff, 0x7f, 0x86, 0x30, 0x8c, 0x18, 0x98, 0x0c, 0xb0, 0x06,
     0xe0, 0x03, 0xc0, 0x01, 0x80, 0x00};

static Display	*display;
static int	screen;
static Window	window;
static XSizeHints	xsh;

static GC	windowGC, pixmapGC, c_pixmapGC;
GC	new_gc();

static Pixmap	tmp_pixmap;
Pixmap	new_pixmap();

static Cursor	watch_cur, xdvi_cur, down_cur, up_cur;

unsigned long forepix, backpix, highpix, brdrpix, mouspix;

extern long window_w, window_h, page_w, page_h;
extern long origin_x, origin_y;

char *malloc(), *calloc(), *index();

static XWindowAttributes	wa;

struct pixmap   *pixmaps;
int     total_pixmaps;

extern int reverse;
extern int bwidth;
extern char *fore_color;
extern char *back_color;
extern char *high_color;
extern char *brdr_color;
extern char *mous_color;
extern char *geometry;
extern char *shrink;
extern char *prog;

open_display( display_name )
char	*display_name;
{
	if ((display = XOpenDisplay(display_name)) == NULL) {
	    fprintf(stderr, "%s: Can't open display '%s'\n",
		    prog, XDisplayName(display_name));
	    exit(1);
	}
}


xdefaults()
{
	char *option;

	if (option = XGetDefault(display, prog, "BorderWidth"))
		bwidth = atoi(option);
	if ( reverse == 0
	  && (option = XGetDefault(display, prog, "ReverseVideo"))
	  && strcmp(option, "on") == 0 ) {
		reverse = 1;
	}
	if (fore_color == NULL)
		fore_color = XGetDefault(display, prog, "ForeGround");
	if (back_color == NULL)
		back_color = XGetDefault(display, prog, "BackGround");
	if (high_color == NULL)
		high_color = XGetDefault(display, prog, "Highlight");
	if (brdr_color == NULL)
		brdr_color = XGetDefault(display, prog, "Border");
	if (mous_color == NULL)
		mous_color = XGetDefault(display, prog, "Mouse");
	if (shrink == NULL)
		if (shrink = XGetDefault(display, prog, "Shrink")) {
			shrink_factor = atoi(shrink) ;
			if (shrink_factor <= 0) {
				fprintf(stderr, "Bad shrink parameter in defaults file\n") ;
				shrink_factor = DEFAULT_SHRINK ;
			}
		}
}


init_window()
{
	XColor	fg_color, bg_color, cdef;

	screen = DefaultScreen(display);

/*
 * Color
 */
	if (reverse) {
		forepix = WhitePixel(display, screen);
		highpix = WhitePixel(display, screen);
		backpix = BlackPixel(display, screen);
		brdrpix = BlackPixel(display, screen);
		mouspix = WhitePixel(display, screen);
	} else {
		forepix = BlackPixel(display, screen);
		highpix = BlackPixel(display, screen);
		backpix = WhitePixel(display, screen);
		brdrpix = WhitePixel(display, screen);
		mouspix = BlackPixel(display, screen);
	}
	if (DisplayCells(display, screen) > 2) {
		if (fore_color
		 && XParseColor(display, DefaultColormap(display, screen),
					fore_color, &fg_color)) {
			forepix = fg_color.pixel;
		}
		if (back_color
		 && XParseColor(display, DefaultColormap(display, screen),
					back_color, &bg_color)) {
			backpix = bg_color.pixel;
		}
		if (high_color
		 && XParseColor(display, DefaultColormap(display, screen),
					high_color, &cdef)) {
			highpix = cdef.pixel;
		}
		if (brdr_color
		 && XParseColor(display, DefaultColormap(display, screen),
					brdr_color, &cdef)) {
			brdrpix = cdef.pixel;
		}
		if (mous_color
		 && XParseColor(display, DefaultColormap(display, screen),
					mous_color, &cdef)) {
			mouspix = cdef.pixel;
		}
	}
	debugprintf(("forepix = %d\n",forepix));
	debugprintf(("backpix = %d\n",backpix));

/*
 * Window
 */
	bzero(&xsh, sizeof(xsh));
	/*
	if (geometry) XParseGeometry(geometry, &x, &y, &width, &height);
	*/
	if (geometry) {
		int	bitmask;
		bitmask = XGeometry(display, screen, geometry, "", bwidth,
					1, 1, 1, 1, &(xsh.x), &(xsh.y),
					&(xsh.width), &(xsh.height));
		if (bitmask & (XValue | YValue)) {
			xsh.flags |= USPosition;
		}
		if (bitmask & (WidthValue | HeightValue)) {
			xsh.flags |= USSize;
		}
	} else {
		xsh.flags = (PPosition | PSize);
		xsh.width = MIN(page_w,DisplayWidth(display,screen)-bwidth*2);
		xsh.height = MIN(page_h,DisplayHeight(display,screen)-bwidth*2);
		xsh.x = 0;
		xsh.y = 0;
	}

	/*
	debugprintf(("x=%d,y=%d,w=%d,h=%d\n",x,y,width,height));
	*/

	window = XCreateSimpleWindow(display, DefaultRootWindow(display),
		xsh.x, xsh.y, xsh.width, xsh.height, bwidth, brdrpix, backpix);
	XSetStandardProperties(display, window, prog, prog, None, NULL, 0, &xsh);
	get_window_size();
	XMapWindow(display, window);
	XSelectInput(display, window, KeyPressMask|ButtonPressMask|ExposureMask);

/*
 * Cursor
 */
	tmp_pixmap = XCreateBitmapFromData(display, window, 
			xdvi_bits, xdvi_width, xdvi_height );
	xdvi_cur = XCreatePixmapCursor(display, tmp_pixmap, tmp_pixmap,
			&fg_color, &bg_color,
			(unsigned int)xdvi_x_hot, (unsigned int)xdvi_y_hot);
	watch_cur = XCreateFontCursor(display, XC_watch);
	down_cur = XCreateFontCursor(display, XC_based_arrow_down);
	up_cur = XCreateFontCursor(display, XC_based_arrow_up);
	XFreePixmap(display, tmp_pixmap);

/*
 * GC
 */
	windowGC = new_gc(window);
	XSetFunction(display, windowGC, GXcopy);
	XSetGraphicsExposures(display, windowGC, False);
	XSetForeground(display, windowGC, forepix);
	XSetBackground(display, windowGC, backpix);

	tmp_pixmap = new_pixmap(1, 1);
	pixmapGC = new_gc(tmp_pixmap);
	XSetFunction(display, pixmapGC, GXor);
	XSetFillStyle(display, pixmapGC, FillSolid);
#ifdef ONE_PLANE_PIXMAP
	XSetForeground(display, pixmapGC, 1);
	XSetBackground(display, pixmapGC, 0);
#else
	XSetForeground(display, pixmapGC, forepix);
	XSetBackground(display, pixmapGC, backpix);
#endif
	c_pixmapGC = new_gc(tmp_pixmap);
	XSetFunction(display, c_pixmapGC, GXcopy);
	XSetFillStyle(display, c_pixmapGC, FillSolid);
#ifdef ONE_PLANE_PIXMAP
	XSetForeground(display, c_pixmapGC, 0);
	XSetBackground(display, c_pixmapGC, 0);
#else
	XSetForeground(display, c_pixmapGC, backpix);
	XSetBackground(display, c_pixmapGC, backpix);
#endif
	/* Don't say XFreePixmap(display, tmp_pixmap); */
}


GC
new_gc(d)
Drawable	d;
{
	XGCValues	gcval;
#ifdef ONE_PLANE_PIXMAP
	return  XCreateGC(display, d, 0L, &gcval);
#else
	return  XCreateGC(display, window, 0L, &gcval);
#endif
}
 

Pixmap
new_pixmap(w, h)
int	w, h;
{
#ifdef ONE_PLANE_PIXMAP
	return  XCreatePixmap(display, window, w, h, 1);
#else
	return  XCreatePixmap(display, window, w, h,
				DefaultDepth(display, screen));
#endif
}
 

int
nextstring( string )
char	**string;
{
	static char buffer[128];
	XEvent	event;
	int nbytes = 0;

	if ( current_page < total_pages-1
	  && !pages[making_page = current_page+1].complete ) {
		debugprintf(("next_page:make=%d\n", making_page));
		make_ahead(down_cur);
	}
	if ( current_page > 0
	  && !pages[making_page = current_page-1].complete ) {
		debugprintf(("prev_page:make=%d\n", making_page));
		make_ahead(up_cur);
	}
	XDefineCursor(display, window, xdvi_cur);
#ifdef DEBUG
	if (debug) {
		int	i;
		for (i=0; i<total_pixmaps; i++) {
			debugprintf(("%d, ",pixmaps[i].page));
		}
		debugprintf(("\n"));
	}
#endif
	XNextEvent(display, &event);
	debugprintf(("event.type=%d\n",event.type));
	if (event.type == NoExpose) {
		nbytes = 0;
	} else if (event.type == Expose) {
		if ( event.xexpose.count == 0 ) {
			get_window_size();
		}
		realize_part(
			origin_x + event.xexpose.x, origin_y + event.xexpose.y,
			event.xexpose.width, event.xexpose.height,
			event.xexpose.x, event.xexpose.y);
		nbytes = 0;
	} else if (event.type == ButtonPress) {
		unsigned int state = event.xbutton.state;
		unsigned int detail = event.xbutton.button;

		if (detail == Button1) {
			if (state & ShiftMask)
				*string = "h"; /* left */
			else
				*string = "b"; /* previous page */
			nbytes = 1;
		} else if (detail == Button2) {
			if (state & ShiftMask)
				*string = "k"; /* up */
			else
				*string = "j"; /* down */
			nbytes = 1;
		} else if (detail == Button3) {
			if (state & ShiftMask)
				*string = "l"; /* right */
			else
				*string = "f"; /* next page */
			nbytes = 1;
		}
	} else if (event.type == KeyPress) {
		*string = buffer;
		nbytes = XLookupString(&event.xkey, *string, sizeof(buffer),
								NULL, NULL);
	}
	return nbytes;
}


clear_page()
{
	XFillRectangle(display, pages[making_page].pixmap, c_pixmapGC,
						0, 0, page_w, page_h);
}


make_ahead(cur)
Cursor	cur;
{
	if (!pages[making_page].pixmap) {
		find_pixmap();
	}
	if (pages[making_page].pixmap) {
		XDefineCursor(display, window, cur);
		restore_context();
		while (!XPending(display) && !pages[making_page].complete) {
			make_one();
		}
		save_context();
	}
}


find_pixmap()
{
	int	i, imax, d, dmax;
	for (i = 0, dmax = -1; i < total_pixmaps; i ++) {
		if (pixmaps[i].page < 0) {
			imax = i;
			break;
		}
		if ((d = ABS(pixmaps[i].page - current_page)) > dmax) {
			imax = i;
			dmax = d;
		}
	}
	debugprintf(("Found pixmap %d for page %d\n",imax,making_page));
	if (pixmaps[imax].page >= 0) {
		if (dmax == 0 || total_pixmaps == 2 && dmax <= 1) {
			debugprintf(("Can not find pixmap for page %d\n", making_page));
			return;
		}
		pages[pixmaps[imax].page].pixmap = 0;
		pages[pixmaps[imax].page].complete = 0;
		debugprintf(("So page %d is swapped out\n",pixmaps[imax].page));
	}
	pixmaps[imax].page = making_page;
	pages[making_page].pixmap = pixmaps[imax].pixmap;
	pages[making_page].dviptr = pages[making_page].dvioff;
	if (pages[making_page].complete) {
		debugprintf(("BUG: !!! complete page %d without pixmap !!!\n"));
	}
	pages[making_page].complete = 0;
}


beep()
{
	XBell(display, 100);
}


put_rectangle(x, y, w, h)
long x, y, w, h;
{
	XFillRectangle(display, pages[making_page].pixmap, pixmapGC,
			x, y, w, h);
}


put_bitmap(bitmap, x, y)
register struct bitmap *bitmap;
register long x, y;
{
	register XImage  *image;
	int	free();
  
	if ((bitmap->w == 0) || (bitmap->h == 0)) return;
	image = XCreateImage(display, DefaultVisual(display, screen),
			     1, XYBitmap, 0,
			     bitmap->bits, bitmap->w, bitmap->h,
			     8, bitmap->bytes_wide);
	image->byte_order = MSBFirst;
	image->bitmap_bit_order = MSBFirst;
	image->f.destroy_image = free;
	XPutImage(display, pages[making_page].pixmap, pixmapGC, image,
		  0, 0, x, y, bitmap->w, bitmap->h );
	XDestroyImage(image);
	XFlush(display);
}


load_kfont(f, dots)
struct font	*f;
int	dots;
{
	XFontStruct	*font = NULL;
	char	xdefaultname[100];
	char	*sized_default, *default_default, xdefault[1000], *xdefp;
	char	*fontname, xfontname[100];
	if ( f->xf ) {
		XFreeFont(display, f->xf);
		f->xf = NULL;
	}
	strcpy(xdefault, ":");
	sprintf(xdefaultname, "KanjiFont.%d", dots);
	if (sized_default = XGetDefault(display, prog, xdefaultname)) {
		strcat(xdefault, sized_default);
		strcat(xdefault, ":");
	}
	if (default_default = XGetDefault(display, prog, "KanjiFont")) {
		strcat(xdefault, default_default);
	} else {
		strcat(xdefault, "k%d:k14.%d:xdvi%d");
	}
	xdefp = xdefault;
	while ( font == NULL && xdefp ) {
		fontname = xdefp + 1;
		xdefp = index(fontname, ':');
		if (xdefp)  *xdefp = '\0';
		sprintf(xfontname, fontname, dots);
		font = XLoadQueryFont(display, xfontname);
	}
	if ( font ) {
		if ( f->xgc == NULL ) {
			f->xgc = new_gc(tmp_pixmap);
			XSetFunction(display, f->xgc, GXor);
#ifdef ONE_PLANE_PIXMAP
			XSetForeground(display, f->xgc, 1);
			XSetBackground(display, f->xgc, 0);
#else
			XSetForeground(display, f->xgc, forepix);
			XSetBackground(display, f->xgc, backpix);
#endif
		}
		f->xf = font;
		XSetFont(display, f->xgc, f->xf->fid);
	} else {
		f->xf = NULL;
		f->xgc = NULL;
	}
}


put_kanji(x, y, c)
int	x;
int	y;
int	c;
{
	int	size;
	size = ROUNDUP(current_font->size, shrink_factor);
	if ( current_font->xgc ) {
		XChar2b	str16[1];
		str16[0].byte1 = c/0x100;
		str16[0].byte2 = c%0x100;
		XDrawString16(display, pages[making_page].pixmap,
			current_font->xgc,
			x, y-current_font->xf->descent, str16, 1);
	} else {
		put_rectangle(x, y-size, size, size);
	}
}


xfree()
{
	/*register*/ struct font *f;
	int	i;

	f = current_font;
	for (;;) {
	    if ( f->xgc ) {
		XFreeFont(display, f->xf);
		XFreeGC(display, f->xgc);
	    }
	    f = f->next;
	    if (f == current_font) break;
	}
	XFreeGC(display, windowGC);
	XFreeGC(display, pixmapGC);
	XFreeGC(display, c_pixmapGC);
	XFreePixmap(display, tmp_pixmap);
	free_pixmap();
}


init_pixmap()
{
	int	i;
	XDefineCursor(display, window, xdvi_cur);
	free_pixmap();
	total_pixmaps = MIN(total_pages, shrink_factor*shrink_factor);
	debugprintf(("total_pixmaps=%d\n",total_pixmaps));
	pixmaps = (struct pixmap*)
			malloc(sizeof(struct pixmap) * total_pixmaps);
	for (i = 0; i < total_pixmaps; i ++) {
		pixmaps[i].pixmap = new_pixmap(page_w, page_h);
		if ( pixmaps[i].pixmap == 0 ) {
			total_pixmaps = i;
			break;
		}
		pixmaps[i].page = -1;
	}
}


free_pixmap()
{
	int	i;
	if ( pixmaps ) {
		for (i = 0; i < total_pixmaps; i ++) {
			XFreePixmap(display, pixmaps[i].pixmap);
		}
		free( pixmaps );
		pixmaps = NULL;
	}
}


realize_page()
{
	realize_part(origin_x, origin_y, window_w, window_h, 0, 0);
}


realize_part(src_x, src_y, width, height, dest_x, dest_y)
int	src_x;
int	src_y;
int	width;
int	height;
int	dest_x;
int	dest_y;
{
	if (!pages[making_page = current_page].complete) {
		debugprintf(("realize_part:make=%d\n", making_page));
		XClearWindow(display, window);
		make_ahead(watch_cur);
	}
	if (pages[current_page].complete) {
#ifdef ONE_PLANE_PIXMAP
		XCopyPlane(display, pages[current_page].pixmap,
			window, windowGC,
			src_x, src_y, width, height, dest_x, dest_y, 0x1);
#else
		XCopyArea(display, pages[current_page].pixmap,
			window, windowGC,
			src_x, src_y, width, height, dest_x, dest_y);
#endif
	}
}


get_window_size()
{
	XGetWindowAttributes(display, window, &wa);
	window_h = wa.height;
	window_w = wa.width;
}
