/*
 *	window.c	window part of jxdvi.
 *	$Header: window.c,v 3.2 89/03/14 13:31:21 atsuo Exp $
 */

#include "atsuo-copyright.h"

#include "config.h"

#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>

#ifndef X11 /* X10 */
#  include <X/Xlib.h>
#else /* X11 */
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
#  include <X11/cursorfont.h>
#endif /* X Version */

#include "jxdvi.h"
#include "font.h"
#include "window.h"

#ifndef X11 /* X10 */
/* Window stuff */
Window win;
int forepix, backpix, highpix;
int GXfunc;

#else /* X11 */
/* Page cache stuff. X11 only */
struct leaf *leaf_head=(struct leaf *)NULL, *leaf_tail=(struct leaf *)NULL;
struct leaf *current_leaf=(struct leaf *)NULL;
struct leaf **page_leaf;
int leaf_num=0;
int max_leaf_num;

Pixmap image_pixmap;	/* Pixmap initially image send to */
GC image_gc, clear_gc;	/* gc for image_pixmap */

Display *display;
int scr_depth;
int scr_num;
Window win;
GC gc, copy_gc, back_gc;
unsigned long forepix, backpix, highpix;
Colormap cmap;
XSizeHints hints;
Cursor circle_cursor, watch_cursor;
int current_cursor;
int GXfunc;
XGCValues gcvalues;
XSetWindowAttributes	sattr;
XWindowAttributes	wattr;
/* for color monitors */
int scr_depth = 1;		/* default is Mono-chrome screen */
unsigned long bit_plane = 0x01;
int (*put_bitmap)();

XImage image = {0, 0,		/*  int width, height; */
		0,		/*  int xoffset;       */
		XYPixmap,	/*  int format;	       */
		NULL,		/*  char *data;	       */
		MSBFirst,	/*  int byte_order;    */
		32,		/*  int bitmap_unit;   */
		MSBFirst,	/*  int bitmap_bit_order; */
		8,		/*  int bitmap_pad;       */
/**/		1,		/*  int depth;            */
		0,		/*  int bytes_per_line;   */
		1,		/*  int bits_per_pixel;   */
		0,		/*  unsigned long red_mask;   */
		0,		/*  unsigned long green_mask; */
		0,		/*  unsigned long blue_mask;  */
		NULL,		/*  char *obdata;         */
		{ NULL,		/*  struct _XImage *(*create_image)(); */
		  NULL,		/*  int (*destroy_image)();   */
		  NULL,		/* unsigned long (*get_pixel)(); */
		  NULL,		/* int (*put_pixel)(); */
		  NULL,		/* struct _XImage *(*sub_image)(); */
		  NULL }	/* int (*add_pixel)(); */
};

#endif /* X Version */

extern long page_w, page_h;
extern long min_x, max_x, min_y, max_y, base_x, base_y;

extern char *calloc();

#ifdef X11
/* init_page_cache() initilizes all stuffs needed for maintaining
 * page cache.
 */
init_page_cache(total_page,shrink)
  int total_page, shrink;
{
    page_leaf = (struct leaf **) calloc(total_page, sizeof(struct leaf *));
    max_leaf_num = ( shrink*shrink < MAX_LEAF ) ? shrink*shrink : MAX_LEAF;

    if (scr_depth > 2) {		/* Color */
	/* Make a special pixmap whose depth is 1 */
	XGCValues gcvalues;

	image_pixmap =
	  XCreatePixmap(display,DefaultRootWindow(display),
			(unsigned int)page_w, (unsigned int)page_h,
			1);

	gcvalues.function = GXor;
	image_gc = XCreateGC(display,image_pixmap,
			     GCFunction,
			     &gcvalues);
	gcvalues.function = GXclear;
	clear_gc = XCreateGC(display,image_pixmap,
			     GCFunction,
			     &gcvalues);
    }
}
#endif /* X11 */

#ifdef X11
/* reset_page_cache() frees the all memory and resets the variables
 */
reset_page_cache()
{
  register struct leaf *l, *l1;

  /* free leaves from tail to head. */
  l = l1 = leaf_tail;
  while (l->prev != NULL) {
    if (l->p != NULL) XFreePixmap(display,l->p);
    l1 = l->prev;			/* Save previous leaf's address */
    free(l);
    l = l1;
  }
  free(page_leaf);

  if (scr_depth > 2) {			/* Color */
      XFreeGC(display,image_gc);
      XFreeGC(display,clear_gc);
      XFreePixmap(display,image_pixmap);
  }

  current_leaf = leaf_head = leaf_tail = (struct leaf *)NULL;;
  page_leaf = NULL;
  leaf_num=0;
}
#endif /* X11 */

#ifdef X11
/* get_page_leaf() returns
 *    HIT	If the leaf already made
 *    MISS	leaf not exists, so draw current_leaf
 */
get_page_leaf(page)
     int page;
{
  register struct leaf *l, *ll;

  /* if the leaf already exists */
  if (page_leaf[page] != NULL) {
    current_leaf = l = page_leaf[page];

    /* linked list keeping job */
    if (l == leaf_head) {	/* l is already head of list */
      ;
    } else {
      if (l == leaf_tail) {	/* l is tail of list and the length of
				 * the list is at least 2
				 */
	(l->prev)->next = NULL;
	leaf_tail = l->prev;
      } else {			/* l is in the  middle of list and the 
				 * length of the list is at least 3
				 */
	(l->prev)->next = l->next;
	(l->next)->prev = l->prev;
      }

      /* make l head of list */
      leaf_head->prev = l;
      l->next = leaf_head;
      l->prev = NULL;
      leaf_head = l;
    }


    return HIT;
  } 

  /* if not, make new leaf unless the number of leaves != max_leaf_num */
  if (leaf_num < max_leaf_num) {
    l = (struct leaf *) calloc(1,sizeof(struct leaf));
    if (l == NULL) {
      error("jxdvi: Can't alloc new leaf");
    }
    leaf_num++;
    l->p = XCreatePixmap(display,DefaultRootWindow(display),
			 (unsigned int)page_w, (unsigned int)page_h,
			 scr_depth);
    l->page = page;
    page_leaf[page] = l;

    /* Enter the leaf into linked list. Note that l points the most
     * recently used leaf.
     */
    if (leaf_head == NULL) {
      leaf_head = l;
      leaf_tail = l;
    } else {
      l->next = leaf_head;
      leaf_head->prev = l;
      leaf_head = l;
    }

    /* Fill the leaf with background color */
    XFillRectangle(display,l->p,back_gc,0,0,page_w,page_h);

    /* set current_leaf and return MISS */
    current_leaf = l;
    return MISS;
  }

  /* Now, we exhausted pixmap resource, so we re-use existing pixmap for
   * the page based on LRU strategy.
   */
  {
    /* linked list keeping job */
    l = leaf_tail;		/* leaf_tail is never NULL */
    if ( l->prev != NULL ) {	/* there are some leaves */
	leaf_tail = l->prev;
	l->prev = NULL;
	l->next = leaf_head;
	leaf_head->prev = l;
	leaf_head = l;
    }

    /* Now, l points the leaf to be re-used. */
    page_leaf[l->page] = NULL;
    l->page = page;
    page_leaf[page] = l;

    /* Fill the leaf with background color */
    XFillRectangle(display,l->p,back_gc,0,0,page_w,page_h);

    current_leaf = l;
    return MISS;
  }
}
#endif /* X11 */

#ifndef X11 /* X10 */
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
#else /* X11 */
put_mono_bitmap(bitmap, x, y, pix)
  register struct bitmap *bitmap;
  register long x, y;
  unsigned long pix;
{
    register XImage *i = &image;
    
    if ((bitmap->w == 0) || (bitmap->h == 0)) return;

    i->width = bitmap->w;
    i->height = bitmap->h;
    i->data = (char *) bitmap->bits;
    i->bytes_per_line = (int) bitmap->bytes_wide;
    
    /* In order to maintain complete page image in a page cache, we
     * send a bitmap as the following:
     *    1. PutImage the bitmap to the current leaf.
     *    3. CopyArea the bitmap in the leaf to the window if needed.
     */
    /* First, send the image to the current leaf */
    XPutImage(display,current_leaf->p,gc,i,0,0,
	      x - base_x, y - base_y,
	      i->width, i->height);
    
    /* Then CopyPlane the image to the window */
    if ( (x>min_x && y>min_y && x<max_x && y<max_y) ) {	/* window only */
	XCopyArea(display,current_leaf->p,win,copy_gc,
		  x - base_x, y - base_y, i->width, i->height,
		  x - base_x, y - base_y);
    }
}

put_color_bitmap(bitmap, x, y, pix)
  register struct bitmap *bitmap;
  register long x, y;
  unsigned long pix;
{
    register XImage *i = &image;
    
    if ((bitmap->w == 0) || (bitmap->h == 0)) return;
    
    i->width = bitmap->w;
    i->height = bitmap->h;
    i->data = (char *) bitmap->bits;
    i->bytes_per_line = 	(int) bitmap->bytes_wide;

    /* In order to maintain complete page image in a page cache, we
     * send a bitmap as the following:
     *    1. PutImage the bitmap to a temporary pixmap, image_pixmap.
     *    2. CopyImage the bitmap from image_pixmap to the current leaf.
     *    3. CopyArea the bitmap in the leaf to the window if needed.
     */
    /* First, send the image to image_pixmap */
    XPutImage(display,image_pixmap,image_gc,i,0,0,
	      x - base_x, y - base_y,
	      i->width, i->height);
    
    /* Then CopyPlane the image to the current_leaf */
    XCopyPlane(display,image_pixmap,current_leaf->p,copy_gc,
	       x - base_x, y - base_y, i->width, i->height,
	       x - base_x, y - base_y,
	       1);

    /* And finally, CopyArea the image to the current window */
    if ( (x>min_x && y>min_y && x<max_x && y<max_y) ) {	/* window only */
	XCopyArea(display,current_leaf->p,win,copy_gc,
		   x - base_x, y - base_y, i->width, i->height,
		   x - base_x, y - base_y);
    }
}
#endif /* X Version */

#ifdef JXDVI
put_rectangle(x, y, w, h, pix)
	long x, y, w, h;
	int pix;
#ifndef X11 /* X10 */
{
	if (x < max_x && x + w >= min_x && y < max_y && y + h >= min_y) {
		XPixSet(win, x - base_x, y - base_y, w, h, pix);
	}
}
#else /* X11 */
{
    long xw, yh;

    /* In order to maintain complete page image in a page cache, we
     * put a rectangle as the following:
     *    1. FillRectangle to the current_leaf.
     *    2. CopyArea the rectangle in the leaf to the current
     *       window if needed.
     */
    xw = x+w;
    yh = y+w;

    /* First, FillRectangle to the current_leaf */
    XFillRectangle(display,current_leaf->p,gc,
		   x - base_x, y - base_y, w, h);

    /* Then, CopyArea the recangle to the current window */
    if (x>=min_x && y>=min_y && x<=max_x && y<=max_y) {	
	XCopyArea(display,current_leaf->p,win,copy_gc,
		   x - base_x, y - base_y, w, h,
		   x - base_x, y - base_y);
    }
}
#endif /* X Version */
#endif /* JXDVI */
