/*
 *	window.h	include file for jxdvi.
 *	$Header: window.h,v 3.2 89/03/14 13:30:56 atsuo Exp $
 */

#include "atsuo-copyright.h"

#ifdef X11
#define MAX_LEAF 16

/* begin_page()'s result */
#define DO_EOP 		0
#define CONTINUE 	1

/* get_page_leaf()'s result */
#define HIT 0		/* leaf already made */
#define MISS 1		/* leaf not exists, so draw current_leaf */

struct leaf {
  struct leaf *prev;		/* link to previous k_bitmap block */
  struct leaf *next;		/* link to next k_bitmap block */
  Pixmap p;			/* leaf Pixmap */
  int page;			/* page no. of this leaf */
  int flag;			/* leaf's state. See below. */
};

/* Value of leaf.flag */
#define COMPLETED 0
#define COPY_WIN  1

extern struct leaf *leaf_head, *leaf_tail;
extern struct leaf *current_leaf;
extern struct leaf **page_leaf;
extern int leaf_num;
extern int max_leaf_num;
extern GC gc, copy_gc, back_gc;

/* Display and window stuff */
extern Display *display;
extern int scr_depth;
extern int scr_num;
extern unsigned long forepix, backpix, highpix;
extern XImage image;
extern Colormap cmap;
extern XSizeHints hints;
extern Cursor circle_cursor, watch_cursor;
extern int current_cursor;
extern int GXfunc;
extern XGCValues gcvalues;
extern XSetWindowAttributes sattr;
extern XWindowAttributes wattr;
extern int scr_depth;		/* default is Mono-chrome screen */
extern unsigned long bit_plane;
extern int put_mono_bitmap(), put_color_bitmap();
extern Pixmap image_pixmap;	/* Used only for Color Screen */
extern GC image_gc, clear_gc;	/*             "              */
extern int (*put_bitmap)();

/* Cursors */
extern Cursor circle_cursor, watch_cursor;
/* current_circle takes XC_circle or XC_watch as its value. */
extern int current_cursor;

/* Cursor functions
 *    set_watch_cursor()
 *    set_circle_cursor()
 */
#define set_watch_cursor() {				\
  if (current_cursor != XC_watch) {			\
  	XDefineCursor(display,win,watch_cursor);	\
	XSync(display,0);				\
	current_cursor = XC_watch;			\
  }							\
}
#define set_circle_cursor() {				\
  if (current_cursor != XC_circle) {			\
  	XDefineCursor(display,win,circle_cursor);	\
	XSync(display,0);				\
	current_cursor = XC_circle;			\
  }							\
}
#endif /* X11 */

extern Window win;

#ifndef X11 /* X10 */
/* Window stuff */
extern Window win;
extern int forepix, backpix, highpix;
extern unsigned char reverse_byte[];
extern int GXfunc;

/* Cursors */
extern Cursor xdvi_cursor, watch_cursor;
/* current_cursor takes XC_XDVI or XC_WATCH as its value. */
#define XC_XDVI  0
#define XC_WATCH 1
extern int current_cursor;
/* Cursor functions
 *    set_watch_cursor()
 *    set_xdvi_cursor()
 */
#define set_watch_cursor() {				\
  if (current_cursor != XC_WATCH) {			\
  	XDefineCursor(win,watch_cursor);		\
	XSync(0);					\
	current_cursor = XC_WATCH;			\
  }							\
}
#define set_xdvi_cursor() {				\
  if (current_cursor != XC_XDVI) {			\
  	XDefineCursor(win,xdvi_cursor);			\
	XSync(0);					\
	current_cursor = XC_XDVI;			\
  }							\
}
#endif /* X10 */
