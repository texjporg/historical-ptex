/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

/***********************************************************************
 *
 *	Tpic/tpic2 specials
 *
 ***********************************************************************/

/*
 * Improvements by Martin Jourdan, INRIA (jourdan@minos.inria.fr),
 * April 1990.
 *
 * 1. Accomodation of Tpic 2 output language; most of the stuff was
 *    written by Tim Morgan, but I modified it a little.
 * 2. Shading is now completely effective.  There are five levels of
 *    shading:
 *	fillval < 0.1:		white
 *	0.1 <= fillval < 0.35:	lightgray 25% density
 *	0.35 <= fillval <= 0.65: midgray 50% density
 *	0.65 < fillval <= 0.9:	darkgray 75% density
 *	fillval > 0.9:		black
 *    (see functions "getTile", "fill_path" and "fill_arc")
 *    As requested by the "specification" of Tpic 2, default fillval
 *    is 0.5, except if textures are used (see below).
 *    Shading applies to *complete* ellipses/circles ("fill_arc") and
 *    to arbitrary closed but *convex* polygons ("fill_path").  So
 *    triangular solid arrowheads come out nicely.  However the
 *    specification of Tpic2 does not *guarantee* that polygons are
 *    actually convex, it's just an assumption I make for sake of
 *    speed (?); cross your fingers!
 *    In addition to being shaded, objects can be invisible or dotted
 *    or dashed (in the source text); however I assume, as is
 *    presently the case, that, *in the output*, only solid or
 *    invisible objects can be shaded; more precisely a shaded
 *    dotted/dashed object is output twice, first as an invisible
 *    shaded object then as a non-shaded dotted/dashed object.  Beware
 *    of changes by Tim Morgan!
 * 3. Textures are supported (for poor ole guys who only have tpic1):
 *    the patterns are analyzed to compute their density (ratio of
 *    "black" bits to total number of bits), which is then used as a
 *    valid fillval that overrides the default of 0.5.
 * 4. Other changes:
 *  - I stole from "xdvi" a fast routine to draw complete ellipses and
 *    circles, because XDrawArc is awfully slow on my machine.
 *  - I took care of the very small arcs generated by tpic2 for dotted
 *    ellipses, which put too heavy a strain on my X server.
 *  - I made changes to pen size work (see "set_pen_size").
 *  - I added dotted/dashed decoding for splines but did not do more
 *    because I can't figure out what "flush_spline" does...
 * Remains to be done:
 *  - make better use of updateRegion
 *  - move some initializations (wDepth, GCArcMode) to DviPage.c, at
 *    window/GC creation time
 *  - clean up code; for instance, some routines work on
 *    Tpic-coordinates while some others work on X-coordinates...
 *  - implement dotted/dashed splines?
 */

/*
 * Support drawing routines for TeXsun and TeX
 *
 *      Copyright, (C) 1987, 1988 Tim Morgan, UC Irvine
 *
 * At the time these routines are called, the values of hh and vv should
 * have been updated to the upper left corner of the graph (the position
 * the \special appears at in the dvi file).  Then the coordinates in the
 * graphics commands are in terms of a virtual page with axes oriented the
 * same as the Imagen and the SUN normally have:
 *
 *                      0,0
 *                       +-----------> +x
 *                       |
 *                       |
 *                       |
 *                      \ /
 *                       +y
 *
 * Angles are measured in the conventional way, from +x towards +y.
 * Unfortunately, that reverses the meaning of "counterclockwise"
 * from what it's normally thought of.
 *
 * A lot of floating point arithmetic has been converted to integer
 * arithmetic for speed.  In some places, this is kind-of kludgy, but
 * it's worth it.
 */


#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include "dvi-simple.h"
#include <assert.h>

#include "DviPageP.h"

extern char *ProgName;

#include	<math.h>
#include	<ctype.h>

#define texPicConvHoriz(w,x)\
  ( ((int) x * (int) w -> dviPage.pixelsPerInchHoriz) / (int) 1000 )

#define texPicConvVert(w,x)\
  ( ((int) x * (int) w -> dviPage.pixelsPerInchVert) / (int) 1000 )

static double shading = -1.0;

/*
 * Draw a line from (fx,fy) to (tx,ty) -- Tpic-coordinates
 * Right now, we ignore pen_size.
 */
static void line_btw(w, fx, fy, tx, ty)
     DviPageWidget w;
     int fx, fy, tx, ty;
{
    int rx = fastFromSpHoriz(w, w -> dviPage.dviStackPointer -> h);
    int ry = fastFromSpVert(w, w -> dviPage.dviStackPointer -> v);
  
    fx = texPicConvHoriz(w, fx);
    fy = texPicConvVert(w, fy);
    tx = texPicConvHoriz(w, tx);
    ty = texPicConvVert(w, ty);
  
    if ( fx == tx && fy == ty ) {
	fx += rx;
	fy += ry;

	if ( XPointInRegion(w -> dviPage.updateRegion, fx, fy) ) {
	    XDrawPoint(XtDisplay(w), XtWindow(w),
		       w -> dviPage.paintGC, fx, fy);
	}
    }
    else {
	XDrawLine(XtDisplay(w), XtWindow(w),
		  w -> dviPage.paintGC,
		  fx + rx, fy + ry, tx + rx, ty + ry);
    }
}

/*
 * Draw a dot at (x,y) -- Tpic-coordinates
 */
static void dot_at(w, x, y)
     DviPageWidget w;
     int x;
     int y;
{
    line_btw(w, x, y, x+1, y);
}

/*
 * Return a tile for shading
 * Three levels of shading (in addition to black and white) are
 * available, because I use standard bitmaps found in the X distribution.
 */
static Pixmap
getTile(w, shading)
    DviPageWidget w;
    double shading;
{
    static Pixmap	shadeTiles[3];  
    static int		wDepth = 0;
    Pixmap		the_tile;

    if (shading < 0.35) {	/* lightgray 25% density */
	the_tile = shadeTiles[0];
	if (!the_tile) {	/* initialize tile */
#include <X11/bitmaps/light_gray>

	    if (wDepth == 0) {	/* this code belongs to DviPage.c */
		/* the depth won't change during a session */
		XWindowAttributes wAttrs;
		XGetWindowAttributes(XtDisplay(w), XtWindow(w), &wAttrs);
		wDepth = wAttrs.depth;
	    }
	    shadeTiles[0] = the_tile =
		XCreatePixmapFromBitmapData
		    (XtDisplay(w), XtWindow(w),
		     light_gray_bits,
		     light_gray_width,
		     light_gray_height,
		     w -> dviPage.paintGC -> values.foreground,
		     w -> dviPage.paintGC -> values.background,
		     wDepth);
	}
    } else if (shading > 0.65) { /* darkgray 75% density */
	the_tile = shadeTiles[2];
	if (!the_tile) {
#include <X11/bitmaps/flipped_gray>

	    if (wDepth == 0) {
		XWindowAttributes wAttrs;
		XGetWindowAttributes(XtDisplay(w), XtWindow(w), &wAttrs);
		wDepth = wAttrs.depth;
	    }
	    shadeTiles[2] = the_tile =
		XCreatePixmapFromBitmapData
		    (XtDisplay(w), XtWindow(w),
		     flipped_gray_bits,
		     flipped_gray_width,
		     flipped_gray_height,
		     w -> dviPage.paintGC -> values.foreground,
		     w -> dviPage.paintGC -> values.background,
		     wDepth);
	}
    } else {
	the_tile = shadeTiles[1];
	if (!the_tile) {	/* midgray 50% density */
#include <X11/bitmaps/gray>

	    if (wDepth == 0) {
		XWindowAttributes wAttrs;
		XGetWindowAttributes(XtDisplay(w), XtWindow(w), &wAttrs);
		wDepth = wAttrs.depth;
	    }
	    shadeTiles[1] = the_tile =
		XCreatePixmapFromBitmapData
		    (XtDisplay(w), XtWindow(w),
		     gray_bits,
		     gray_width,
		     gray_height,
		     w -> dviPage.paintGC -> values.foreground,
		     w -> dviPage.paintGC -> values.background,
		     wDepth);
	}
    }
    return the_tile;
}

/*
 * Shade the given polygon -- X-coordinates.  Reset shading.
 * The polygon is assumed to be convex!
 */
static void
fill_path(w, xxyy, path_len)
    DviPageWidget w;
    XPoint *xxyy;
    int path_len;
{
    if (shading >= 0.0) {
	if (shading < 0.1) {	/* white */
	    XFillPolygon(XtDisplay(w), XtWindow(w), w -> dviPage.clearGC,
			 xxyy, path_len, Convex, CoordModeOrigin);
	} else if (shading > 0.9) { /* black */
	    XFillPolygon(XtDisplay(w), XtWindow(w), w -> dviPage.paintGC,
			 xxyy, path_len, Convex, CoordModeOrigin);
	} else {		/* gray -- one of three levels */
	    XGCValues	values;
	    /* temporarily set fill_style to Tiled */
	    values.fill_style = FillTiled;
	    values.tile = getTile(w, shading);
	    XChangeGC(XtDisplay(w), w -> dviPage.paintGC,
		      (unsigned) (GCFillStyle | GCTile),
		      &values);
	    /* fill */
	    XFillPolygon(XtDisplay(w), XtWindow(w), w -> dviPage.paintGC,
			 xxyy, path_len, Convex, CoordModeOrigin);
	    /* reset fill_style to Solid */
	    values.fill_style = FillSolid;
	    XChangeGC(XtDisplay(w), w -> dviPage.paintGC,
		      (unsigned) GCFillStyle,
		      &values);
	}
    }
    shading = -1.0;		/* reset shading */
}

/*
 * Shade the given ellipse or circle -- X-coordinates.  Reset shading.
 * The ellipse or circle is assumed to be complete!
 */
static void
fill_arc(w, x, y, wd, ht)
    DviPageWidget w;
    int x, y;
    unsigned int wd, ht;
{
    static arcModeInit = 0;

    if (!arcModeInit) {
	/* This code belongs to DviPage.c */
	XGCValues	values;

	values.arc_mode = ArcPieSlice;
	XChangeGC(XtDisplay(w), w -> dviPage.paintGC,
		  (unsigned) GCArcMode,
		  &values);
	XChangeGC(XtDisplay(w), w -> dviPage.clearGC,
		  (unsigned) GCArcMode,
		  &values);
	arcModeInit = 1;
    }
  
    if (shading >= 0.0) {
	if (shading < 0.1) {
	    XFillArc(XtDisplay(w), XtWindow(w), w -> dviPage.clearGC,
		     x, y, wd, ht, 0, 360*64);
	} else if (shading > 0.9) {
	    XFillArc(XtDisplay(w), XtWindow(w), w -> dviPage.paintGC,
		     x, y, wd, ht, 0, 360*64);
	} else {
	    XGCValues	values;

	    values.fill_style = FillTiled;
	    values.tile = getTile(w, shading);
	    XChangeGC(XtDisplay(w), w -> dviPage.paintGC,
		      (unsigned) (GCFillStyle | GCTile),
		      &values);

	    /* XFillArc is awfully slow but it's worth it */
	    XFillArc(XtDisplay(w), XtWindow(w), w -> dviPage.paintGC,
		     x, y, wd, ht, 0, 360*64);

	    values.fill_style = FillSolid;
	    XChangeGC(XtDisplay(w), w -> dviPage.paintGC,
		      (unsigned) GCFillStyle,
		      &values);
	}
    }
    shading = -1.0;
}

/*****************************************************************************/

#define	TWOPI		(3.14159265359*2.0)
#define	MAX_PEN_SIZE	7	/* Max pixels of pen width */

/* Unfortunately, these values also appear in dvisun.c */
#define	xRESOLUTION(w)	( w -> dviPage.pixelsPerInchHoriz )
#define	yRESOLUTION(w)	( w -> dviPage.pixelsPerInchVert )

/*
 * Set the size of the virtual pen used to draw in milli-inches
 */

/* ARGSUSED */
static void set_pen_size(w, cp)
     DviPageWidget w;
     char *cp;
{
    int ps;
    XGCValues	values;
  
    if (sscanf(cp, " %d ", &ps) != 1) {
	error(0,0, "illegal .ps command format: %s", cp);
	return;
    }
    w -> dviPage.penSize
	= (ps*(xRESOLUTION(w)+yRESOLUTION(w)) + 1000) / 2000;
    if (w -> dviPage.penSize < 1)
	w -> dviPage.penSize = 1;
    else if (w -> dviPage.penSize > MAX_PEN_SIZE)
	w -> dviPage.penSize = MAX_PEN_SIZE;

    /* update paintGC -> line_width */
    values.line_width = w -> dviPage.penSize;
    XChangeGC(XtDisplay(w), w -> dviPage.paintGC,
	      (unsigned) GCLineWidth,
	      &values);
}


/*
 * Print (if requested) the line defined by previous path commands.
 * Shade if necessary.
 */
static void flush_path(w, invis)
    DviPageWidget w;
    int invis;
{
    register int i;
    int *xx = w -> dviPage.xx;
    int *yy = w -> dviPage.yy;
    int path_len = w -> dviPage.pathLen;
    XPoint xxyy[MAXPOINTS];
    int rx = fastFromSpHoriz(w, w -> dviPage.dviStackPointer -> h);
    int ry = fastFromSpVert(w, w -> dviPage.dviStackPointer -> v);
  
    for (i=1; i <= path_len; i++) {
	/* Convert Tpic-coordinates to X-coordinates */
	xxyy[i-1].x = texPicConvHoriz(w, xx[i]) + rx;
	xxyy[i-1].y = texPicConvVert(w, yy[i]) + ry;
    }
  
    fill_path(w, xxyy, path_len);

    if (!invis) {
      if ( path_len > 2 ) {
	/* A single call to XDrawLines is better than */
	/* multiple calls to line_btw */
	XDrawLines(XtDisplay(w), XtWindow(w),
		   w -> dviPage.paintGC,
		   xxyy, path_len, CoordModeOrigin);
      }
      /* unless, of course, you're drawing a dotted line this way */
      else {
	for (i=1; i < path_len; i++) {
	  line_btw(w, xx[i], yy[i], xx[i+1], yy[i+1]);
	}
      }
    }
  
    w -> dviPage.pathLen = 0;
}

/*
 * Print a dashed line along the previously defined path, with
 * the dashes/inch defined.
 * Assume no shading takes place.
 */
static void flush_dashed(w, cp, dotted)
    DviPageWidget w;
    char *cp;
    int dotted;
{
    int i, numdots, x0, y0, x1, y1;
    int cx0, cy0, cx1, cy1;
    float inchesperdash;
    double d, spacesize, a, b, dx, dy, milliperdash;
  
    int *xx = w -> dviPage.xx;
    int *yy = w -> dviPage.yy;
    int path_len = w -> dviPage.pathLen;
  
    if (sscanf(cp, " %f ", &inchesperdash) != 1) {
	error(0,0,"illegal format for dotted/dashed line: %s", cp);
	return;
    }
    if (path_len <= 1 || inchesperdash <= 0.0) {
	error(0,0,"illegal conditions for dotted/dashed line");
	return;
    }
    milliperdash = inchesperdash * 1000.0;
    x0 = xx[1];	y0 = yy[1];
    x1 = xx[2];	y1 = yy[2];
    dx = x1 - x0;
    dy = y1 - y0;
    if (dotted) {
	numdots = sqrt(dx*dx + dy*dy) / milliperdash + 0.5;
	for (i=0; i <= numdots; i++) {
	    a = (float) i / (float) numdots;
	    cx0 = x0 + a*dx + 0.5;
	    cy0 = y0 + a*dy + 0.5;
	    dot_at(w,cx0, cy0);
	}
    }
    else {
	d = sqrt(dx*dx + dy*dy);
	if (d <= 2.0*milliperdash)
	    line_btw(w, x0, y0, x1, y1);
	else {
	    numdots = d / (2.0*milliperdash) + 1.0;
	    spacesize = (d - numdots * milliperdash) / (numdots - 1);
	    for (i=0; i<numdots-1; i++) {
		a = i * (milliperdash + spacesize) / d;
		b = a + milliperdash / d;
		cx0 = x0 + a*dx + 0.5;
		cy0 = y0 + a*dy + 0.5;
		cx1 = x0 + b*dx + 0.5;
		cy1 = y0 + b*dy + 0.5;
		line_btw(w, cx0, cy0, cx1, cy1);
		b += spacesize / d;
	    }
	    cx0 = x0 + b*dx + 0.5;
	    cy0 = y0 + b*dy + 0.5;
	    line_btw(w, cx0, cy0, x1, y1);
	}
    }
  
    w -> dviPage.pathLen = 0;
}


/*
 * Add a point to the current path
 */
static void add_path(w, cp)
    DviPageWidget w;
    char *cp;
{
    int pathx, pathy;
  
    if (++( w -> dviPage.pathLen) >= MAXPOINTS) {
	error(0,0, "Too many points");
	return;
    }
    if (sscanf(cp, " %d %d ", &pathx, &pathy) != 2) {
	error(0,0,"Malformed path command");
	return;
    }
    w-> dviPage.xx[w -> dviPage.pathLen] = pathx;
    w-> dviPage.yy[w -> dviPage.pathLen] = pathy;
}

/*
 * Draw a *complete* ellipse or circle with the indicated center and radices.
 * Tpic-coordinates.
 * Stolen from xdvi.
 */
static	void
draw_ellipse(w, xc, yc, xr, yr)
    DviPageWidget w;
    int xc, yc, xr, yr;
{
    double angle, theta;
    int n, px0, py0, px1, py1;

    angle = (xr + yr) / 2.0;
    theta = sqrt(1.0 / angle);
    n = TWOPI / theta + 0.5;
    if (n < 12) n = 12;
    else if (n > 80) n = 80;
    n /= 2;
    theta = TWOPI / n;

    angle = 0.0;
    px0 = xc + xr;		/* cos(0) = 1 */
    py0 = yc;			/* Sin(0) = 0 */
    while ((angle += theta) <= TWOPI) {
	px1 = xc + xr*cos(angle) + 0.5;
	py1 = yc + yr*sin(angle) + 0.5;
	line_btw(w, px0, py0, px1, py1);
	px0 = px1;
	py0 = py1;
    }
    line_btw(w, px0, py0, xc + xr, yc);
}

/*
 * Draw an arc
 */
static void arc(w, cp, invis)
    DviPageWidget w;
    char *cp;
    int invis;
{
    int xc, yc, xrad, yrad;
    float start_angle, end_angle;
    int angS, angE;
    Bool doit = True;
    
    int rx = fastFromSpHoriz(w, w -> dviPage.dviStackPointer -> h);
    int ry = fastFromSpVert(w, w -> dviPage.dviStackPointer -> v);
  
    if (sscanf(cp, " %d %d %d %d %f %f ", &xc, &yc, &xrad, &yrad,
	       &start_angle, &end_angle) != 6) {
	error(0,0,"illegal arc specification: %s", cp);
	return;
    }

    if (shading >= 0.0)
	if (start_angle > 0.0 || end_angle < 6.282)
	    error(0,0,"Attempt to fill an incomplete arc");
	else			/* this call could be made more elegant */
	    fill_arc(w,
		     texPicConvHoriz(w, xc) + rx - texPicConvHoriz(w, xrad),
		     texPicConvVert(w, yc) + ry - texPicConvVert(w, yrad),
		     (unsigned) (2 * texPicConvHoriz(w, xrad)),
		     (unsigned) (2 * texPicConvVert(w, yrad)));

    if (invis)
	return;

    /* We have a specialized fast way to draw closed circles/ellipses */
    if (start_angle <= 0.0 && end_angle >= 6.282) {
	draw_ellipse(w, xc, yc, xrad, yrad);
	return;
    }
  
    xc = texPicConvHoriz(w, xc);
    yc = texPicConvVert(w, yc);
    xrad = texPicConvHoriz(w, xrad);
    yrad = texPicConvVert(w, yrad);
  
    angS = (int) ( (start_angle * 360 * 64) / TWOPI);
    angE = (int) ( ((end_angle - start_angle) * 360 * 64) / TWOPI);
  
    if ( w -> dviPage.haveBackingStore ) {
	doit = XRectInRegion( w -> dviPage.updateRegion,
			     xc + rx - xrad, yc + ry - yrad,
			     2 * xrad, 2 * yrad) != RectangleOut;
    }
    
    if (doit) {
	/* beware of very small arcs (actually dots) generated by tpic2 */
	/* for dotted ellipses; threshold is half a degree but can be tuned */
	if (angE < 32)
	    XDrawPoint(XtDisplay(w), XtWindow(w), w -> dviPage.paintGC,
		       xc + rx + (int) (xrad * cos(start_angle)),
		       yc + ry + (int) (yrad * sin(start_angle)));
	else
	    XDrawArc(XtDisplay(w), XtWindow(w), w -> dviPage.paintGC,
		     xc + rx - xrad, yc + ry - yrad,
		     2 * xrad, 2 * yrad, -angS, -angE);
    }
}


/*
 * APPROXIMATE integer distance between two points
 */
#define	dist(x0, y0, x1, y1)	(abs(x0-x1)+abs(y0-y1))

/*
 * Draw a spline along the previously defined path
 */
static void flush_spline(widget, cp)
    DviPageWidget widget;
    char *cp;
{
    int xp, yp, N, lastx=(-1), lasty;
    int t1, t2, t3, steps, j;
    register int i, w;
  
    int *xx = widget -> dviPage.xx;
    int *yy = widget -> dviPage.yy;
    int path_len = widget -> dviPage.pathLen;
  
    /* Dotted/dashed variables and decoding */
    float inchesperdash;
    enum {spSolid, spDotted, spDashed} spStyle;

    if (cp && *cp) {
	if (sscanf(cp, " %f ", &inchesperdash) != 1) {
	    error(0,0,"illegal format for dotted/dashed line: %s", cp);
	    spStyle = spSolid;
	} else if (inchesperdash < 0.0) {
	    spStyle = spDotted;
	    inchesperdash = -inchesperdash;
	} else if (inchesperdash > 0.0)
	    spStyle = spDashed;
	else spStyle = spSolid;
    } else spStyle = spSolid;

#ifdef	lint
    lasty = -1;
#endif
    N = path_len + 1;
    xx[0] = xx[1];	yy[0] = yy[1];
    xx[N] = xx[N-1];	yy[N] = yy[N-1];
    for (i=0; i<N-1; i++) {	/* interval */
	steps = (dist(xx[i], yy[i], xx[i+1], yy[i+1]) +
		 dist(xx[i+1], yy[i+1], xx[i+2], yy[i+2])) / 80;
	for (j=0; j<steps; j++) { /* points within */
	    w = (j*1000 + 500) / steps;
	    t1 = w * w / 20;
	    w -= 500;
	    t2 = (750000 - w * w) / 10;
	    w -= 500;
	    t3 = w * w / 20;
	    xp = (t1*xx[i+2] + t2*xx[i+1] + t3*xx[i] + 50000) / 100000;
	    yp = (t1*yy[i+2] + t2*yy[i+1] + t3*yy[i] + 50000) / 100000;
	    if (lastx > -1) line_btw(widget, lastx, lasty, xp, yp);
	    lastx = xp;
	    lasty = yp;
	}
    }
  
    widget -> dviPage.pathLen = 0;
}

static double txshading = -1.0;

/*
 * Process textures (.tx command): analyse density and set txshading
 */
static void
Texture(cp)
register char *cp;
{
    int blackbits = 0, totalbits = 0;

    while (*cp) {
	switch (*cp) {
	case '0':
	    totalbits += 4;
	    break;
	case '1':
	case '2':
	case '4':
	case '8':
	    blackbits += 1;
	    totalbits += 4;
	    break;
	case '3':
	case '5':
	case '6':
	case '9':
	case 'a':
	case 'A':
	case 'c':
	case 'C':
	    blackbits += 2;
	    totalbits += 4;
	    break;
	case '7':
	case 'b':
	case 'B':
	case 'd':
	case 'D':
	case 'e':
	case 'E':
	    blackbits += 3;
	    totalbits += 4;
	    break;
	case 'f':
	case 'F':
	    blackbits += 4;
	    totalbits += 4;
	    break;
	case ' ':
	    break;
	default:
	    error(0,0,"Invalid character in .tx pattern: \"%c\"", *cp);
	    break;
	}
	cp++;
    }
    txshading = (double) blackbits / (double) totalbits;
}                               /* end of Texture */

/*
 * Set shading for the *next* polygon, circle, or ellipse
 */
/* ARGSUSED */
static void shade_last(w, cp)
    DviPageWidget w;
    char *cp;
{
    if (cp && *cp) {
	if (sscanf(cp, "%lf", &shading) != 1)
	    error(0,0,"illegal format for shading value: %s", cp);
	else if (shading < 0.0 || shading > 1.0) {
	    error(0,0,"bad shading value: %f", shading);
	    shading = 0.5;
	}
    }
    else if (txshading >= 0.0)	/* account for textures */
	shading = txshading;
    else shading = 0.5;		/* default value as requested by tpic2 */

    /* The following code is totally useless
       (as are w->dviPage.{whiten,blacken,shade}, BTW) */
/*
    w -> dviPage.whiten = w -> dviPage.blacken = w -> dviPage.shade = False;
    if (shading > 0.9) w -> dviPage.blacken = True;
    else if (shading >= 0.0 && shading < 0.1) w -> dviPage.whiten = True;
    else if (shading >= 0.1 && shading <= 0.9) w -> dviPage.shade = True;
*/
}

/***********************************************************************
 *
 *	Button specials
 *
 ***********************************************************************/

#define SKIPWHITE(cp) while (cp && *cp && isspace(*cp)) cp++

static void
CallAction(widget,action) 
DviPageWidget widget;
char *action;
{
  int actionLen = strlen( action );
  
  Bool doFree;
  char stringCopy[1024];
  char *allocatedStr;
  char *str;
  
  char *actionProcName;
  int num_params;
  char *params[100];	/* lets hope they don't have more than that.. */
  
  if ( actionLen > 1023 ) {
    allocatedStr = str = XtMalloc( actionLen + 2 );
    doFree = True;
  }
  else {
    str = stringCopy;
    doFree = False;
  }
  
  SKIPWHITE(str);
  strcpy(str, action);
  
  for (;;) {
    SKIPWHITE(str);
    if ( str == 0 || *str == 0 ) break;
    
    /* skip to action procedure name */
    SKIPWHITE(str);
    actionProcName = str;
    
    /* find end of action procedure name- terminate by space or paren */
    while (*str && !( isspace(*str) || *str == '(' )) str++;
    /* terminate action procedure name string */
    *(str++) = 0;

    if ( *actionProcName == 0 ) {
      return;
    }
    
    /* skip past any openning paren, e.g. in case of 'foo (bar)' */
    
    SKIPWHITE(str);
    if ( *str == '(') str++;
    
    /* now, scan to build a list of params */
    num_params = 0;
    while ( *str && *str != ')' && num_params < 100) {
      SKIPWHITE(str);
      params[num_params] = str;
      while (*str && !( isspace(*str) || *str == ',' || *str==')' )) {
	if (*str == '\\') {
	  str++;
	  if ( *str == 0 ) str--;
	}
	/* is it a quoted parameter ? */
	if( *str == '"' ){
	  /* find the closing quote, no error check here, hope for the best*/
	  while( *str && *str != '"' ) str++; 
	}
	str++;
	}

/*    SKIPWHITE(str);  this SKIPWHITE chops letters off later words */
      
      /* terminate this parameter */
      
      num_params++;
      if ( *str == 0 ) {
	break;
      } 
      else if (*str == ')') {
	*(str++) = 0;
	break;
      }
      else {
	*(str++) = 0;
      }
      
      SKIPWHITE(str);
    }
    
    SKIPWHITE(str);
    if ( *str == ')') str++;
    
    XtCallActionProc(widget, actionProcName, NULL, params, num_params);
  }
  
  if ( doFree ) {
    XtFree( allocatedStr );
  }
}

static void registerActionProc(w, cp)
DviPageWidget w;
char *cp;
{

  long dx;	/* origin displacement for width */
  long wd;	/* width */
  long dy;	/* origin displacement for height */
  long ht;	/* height */

  int rx = fastFromSpHoriz(w, w -> dviPage.dviStackPointer -> h);
  int ry = fastFromSpVert(w, w -> dviPage.dviStackPointer -> v);

  XPoint xxyy[5];

  /* get button area in scaled pixels */
  
  dx = strtol(cp, &cp, 0);
  dx = fastFromSpHoriz(w, dx);

  wd = strtol(cp, &cp, 0);
  wd = fastFromSpHoriz(w, wd);

  dy = strtol(cp, &cp, 0);
  dy = fastFromSpVert(w, dy);

  ht = strtol(cp, &cp, 0);
  ht = fastFromSpVert(w, ht);

  xxyy[0].x = rx + dx;	/* lower left */
  xxyy[0].y = ry - dy;

  xxyy[1].x = rx + wd;	/* lower right */
  xxyy[1].y = ry - dy;

  xxyy[2].x = rx + wd;	/* upper right */
  xxyy[2].y = ry - ht;

  xxyy[3].x = rx + dx;	/* upper left */
  xxyy[3].y = ry - ht;

  xxyy[4].x = rx + dx;	/* lower left */
  xxyy[4].y = ry - dy;

  XDrawLines(XtDisplay(w), XtWindow(w),
	     w -> dviPage.paintGC,
	     xxyy, 5, CoordModeOrigin);

  SKIPWHITE(cp);
  if ( *cp ) {
    DviButton *thisButton = (DviButton *) XtMalloc( sizeof(DviButton) );
    int len = strlen(cp);

    thisButton -> upperLeft  = xxyy[3];
    thisButton -> lowerRight = xxyy[1];
    if ( len < DVI_REGION_STRLEN ) {
      thisButton -> longerAction = 0;
      strcpy( thisButton -> action, cp);
    } else {
      thisButton -> longerAction = XtMalloc(len+1);
      strcpy( thisButton -> longerAction, cp);
    }

    thisButton -> next = w -> dviPage.buttons;
    w -> dviPage.buttons = thisButton;
  }
}

void actionPushButton(w, event, params, num_params)
     DviPageWidget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  Window root, child;
  int root_x, root_y, win_x, win_y;
  unsigned int buttons;
  DviButton *b;

  /* find the mouse */
  XQueryPointer(XtDisplay(w), XtWindow(w), &root, &child,
		&root_x, &root_y, &win_x, &win_y, &buttons);

  b = w -> dviPage.buttons;
  while ( b ) {

    if (   win_x >= b -> upperLeft.x
	&& win_x <= b -> lowerRight.x
	&& win_y >= b -> upperLeft.y
	&& win_y <= b -> lowerRight.y )
    {
      /* found a button, push it.. */
      char *action = b -> action;
      if ( b -> longerAction ) {
	action = b -> longerAction;
      }
      CallAction(w, action);
    }

    b = b -> next;
    
  }
}


#ifndef HAVE_DPS
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
static void
psfigBegin(w,cp)
DviPageWidget w;
char *cp;
{
  int bbllx, bblly;
  int bburx, bbury;
  int width, height;

  int rx = fastFromSpHoriz(w, w -> dviPage.dviStackPointer -> h);
  int ry = fastFromSpVert(w, w -> dviPage.dviStackPointer -> v);

  sscanf(cp, " %d %d %d %d %d %d ",
	 &width, &height,
	 &bbllx, &bblly, &bburx, &bbury);

  width = fastFromSpHoriz(w, width);
  height = fastFromSpVert(w, height);

  XDrawLine(XtDisplay(w), XtWindow(w),
	    w -> dviPage.paintGC,
	    rx, ry, rx + width, ry );
  XDrawLine(XtDisplay(w), XtWindow(w),
	    w -> dviPage.paintGC,
	    rx + width, ry, rx + width, ry + height);
  XDrawLine(XtDisplay(w), XtWindow(w),
	    w -> dviPage.paintGC,
	    rx + width, ry + height, rx, ry + height);
  XDrawLine(XtDisplay(w), XtWindow(w),
	    w -> dviPage.paintGC,
	    rx, ry + height, rx, ry);
}

/*****************************************************************************/

/*****************************************************************************/

#else

/*****************************************************************************/

static void
  HandleStatus(ctx, status)
DPSContext ctx;
int status;
{
  char *ptr, buf[1000];
  switch (status) {
  case PSRUNNING:	ptr = "[DPS status] running\n"; break;
  case PSNEEDSINPUT:	ptr = "[DPS status] needs input\n"; break;
  case PSZOMBIE:	ptr = "[DPS status] zombie\n"; break;
  case PSFROZEN:	ptr = "[DPS status] frozen\n"; break;
  default:		ptr = "[DPS status] unknown status\n"; break;
  }
  error(0,0,status);
  if (status == PSFROZEN) {
    XDPSUnfreezeContext(ctx);
  }
}

static void TextOut(ctx, buffer, count)
     DPSContext ctx;
     char *buffer;
     unsigned count;
{
  char buff[1024];
  while ( count > 0 ) {
    int n = count;
    if ( count > 1023 ) {
      n = 1023;
    }
    memcpy(buff, buffer, n);
    buff[n] = 0;
    error(0,0,buffer);
    count -= n;
  }
}

/***********************************************************************
 *
 *	Psfig/Postscript specials
 *
 ***********************************************************************/

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

/*
 * USE_GSAVE and ALWAYS_KILL were used for debugging memory leaks with
 * the Display Postscript server.
 */

/* USE_GSAVE means to wrap each user invocation in a gsave/grestore pair */

#define USE_GSAVE

/* ALWAYS_KILL means to kill the DPS context after each use, causing a
   new one to be create for the next DPS figure */

#undef ALWAYS_KILL

static void
  psfigBegin(w,cp)
DviPageWidget w;
char *cp;
{
  int bbllx, bblly;
  int bburx, bbury;
  int width, height;
  int rawWidth, rawHeight;
  
  int rx = fastFromSpHoriz(w, w -> dviPage.dviStackPointer -> h);
  int ry = fastFromSpVert(w, w -> dviPage.dviStackPointer -> v);
  
  sscanf(cp, " %d %d %d %d %d %d ",
	 &rawWidth, &rawHeight,
	 &bbllx, &bblly,
	 &bburx, &bbury);
  
  width = fastFromSpHoriz(w, rawWidth);
  height = fastFromSpVert(w, rawHeight);
  
  if ( XRectInRegion( w -> dviPage.updateRegion,
		     rx, ry, width, height) == RectangleOut) {
    w -> dviPage.dpsVisible = False;
    return;
  }
  else {
    w -> dviPage.dpsVisible = True;
  }
  
  if ( w -> dviPage.ctx == NULL ) {
    w -> dviPage.ctx = XDPSCreateSimpleContext(XtDisplay(w),
					       XtWindow(w),
					       DefaultGC(XtDisplay(w), 0),
					       rx, ry,
					       TextOut, DPSDefaultErrorProc,
					       NULL);
  }

  if ( w -> dviPage.ctx != NULL ) {
    XDPSRegisterStatusProc(w -> dviPage.ctx, HandleStatus);
    DPSPrintf(w -> dviPage.ctx, "\n resyncstart\n");
    DPSWritePostScript(w -> dviPage.ctx,
		       w -> dviPage.dpsPreamble,
		       strlen(w -> dviPage.dpsPreamble));
      

  }

  /* check if server has DPS extension */
  
  if ( w -> dviPage.ctx == NULL ) {
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w -> dviPage.paintGC,
	      rx, ry, rx + width, ry );
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w -> dviPage.paintGC,
	      rx + width, ry, rx + width, ry + height);
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w -> dviPage.paintGC,
	      rx + width, ry + height, rx, ry + height);
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w -> dviPage.paintGC,
	      rx, ry + height, rx, ry);
  }
  else
    {
      int maxWidth = XtScreen(w) -> width;
      int mmWidth = WidthMMOfScreen( XtScreen(w) );
      
      double dpi = (w -> dviPage.dpiHoriz * w -> dviPage.userMag) / 1000;
      double trueDpi;
      double scale;
      
      if ( w -> dviPage.trueDpi == 0.0 ) {
	trueDpi = (maxWidth * 25.4) / mmWidth;
      }
      else {
	trueDpi = w -> dviPage.trueDpi;
      }
      scale = dpi / trueDpi;
      

#ifdef USE_GSAVE
      DPSPrintf(w -> dviPage.ctx, "\n gsave save\n");
#else
      DPSPrintf(w -> dviPage.ctx, "\n \n");
#endif

      DPSPrintf( w -> dviPage.ctx, "%lu %lu %lu %lu setXgcdrawable\n",
		DefaultGC(XtDisplay(w),0) -> gid,
		XtWindow(w),
		rx, ry);      

      DPSPrintf(w -> dviPage.ctx,"\n%d %f @start\n",
		w -> dviPage.pixelsPerInchHoriz,
		scale);
      
      DPSPrintf(w -> dviPage.ctx, " %d %d %d %d %d %d startTexFig\n",
		rawWidth, rawHeight, bbllx, bblly, bburx, bbury);
    }
}

static void
  psfigPlotfile(w,cp)
DviPageWidget w;
char *cp;
{
  if ( w -> dviPage.ctx  && w -> dviPage.dpsVisible ) { 
    
    while ( cp && *cp && isspace(*cp) ) cp++; /* skip white */
    if (strncmp(cp,"plotfile",8) == 0 ) {
      char *filename;
      FILE *psfile;
      
      cp += 8; /* skip plotfile */
      while ( cp && *cp && isspace(*cp) ) cp++; /* skip white */
      filename = cp;
      while ( cp && *cp && !isspace(*cp) ) cp++; /* skip non-white */
      *cp = 0; /* terminate file name */
      
      if ( (psfile = fopen(filename, "r")) ) {
	char buffer[1024];
	int len;

	DPSFlushContext(w -> dviPage.ctx);
	DPSWaitContext(w -> dviPage.ctx);
	
	for (;;) { 
	  len = fread(buffer, sizeof(char), 1024, psfile);
	  if  ( len == 0 ) break;
	  DPSWritePostScript(w -> dviPage.ctx, buffer, len);
	}	  
	fclose(psfile);
	DPSFlushContext(w -> dviPage.ctx);
	DPSWaitContext(w -> dviPage.ctx);
      }
    }

    DPSPrintf( w -> dviPage.ctx, "\n endTexFig\n");

#ifdef USE_GSAVE
    DPSPrintf( w -> dviPage.ctx, "\n restore grestore\n");
    DPSFlushContext(w -> dviPage.ctx);
    DPSWaitContext(w -> dviPage.ctx);
#endif

#ifdef ALWAYS_KILL
    DPSDestroySpace( DPSSpaceFromContext( w -> dviPage.ctx ));
    w -> dviPage.ctx = 0;
#endif
  }
}

static void
  psfigEnd(w,cp)
DviPageWidget w;
char *cp;
{
  if ( w -> dviPage.ctx  && w -> dviPage.dpsVisible ) { 
    DPSFlushContext( w -> dviPage.ctx );
    DPSWaitContext( w -> dviPage.ctx );
    /* context is destroyed when widget is destroyed */
  }
}

#ifndef DECWINDOWS
int
  _DoAsyncs()
{
  /* a ruse for DEC windows specific extension to libX. */
}
#endif
#endif

void doSpecial(w, cp)
     DviPageWidget w;
     char *cp;
{
#define	COMLEN	64
  char command[COMLEN], *orig_cp;
  register int len;
  
  orig_cp = cp;
  while (isspace(*cp)) ++cp;
  len = 0;
  while (!isspace(*cp) && *cp && len < COMLEN-1) command[len++] = *cp++;
  command[len] = '\0';
  if (strcmp(command, "pn") == 0) set_pen_size(w,cp);
  else if (strcmp(command, "fp") == 0) flush_path(w, 0);
  else if (strcmp(command, "da") == 0) flush_dashed(w, cp, 0);
  else if (strcmp(command, "dt") == 0) flush_dashed(w, cp, 1);
  else if (strcmp(command, "pa") == 0) add_path(w, cp);
  else if (strcmp(command, "ar") == 0) arc(w, cp, 0);
  else if (strcmp(command, "ia") == 0) arc(w, cp, 1);
  else if (strcmp(command, "sp") == 0) flush_spline(w, cp);
  else if (strcmp(command, "sh") == 0) shade_last(w, cp);
  else if (strcmp(command, "wh") == 0) shade_last(w, "0");
  else if (strcmp(command, "bk") == 0) shade_last(w, "1");
  else if (strcmp(command, "ip") == 0) flush_path(w, 1);
  else if (strcmp(command, "tx") == 0) Texture(cp);
  else if (strcmp(command, "ps::[begin]") == 0) psfigBegin(w, cp);
#ifdef HAVE_DPS
  else if (strcmp(command, "ps::[end]") == 0) psfigEnd(w, cp);
  else if (strncmp(command, "ps:",3) == 0) psfigPlotfile(w, cp);
#else
  else if (strncmp(command, "ps:",3) == 0)		/* do nothing */;
#endif
  else if (strncmp(command, "ln03", 4) == 0) 		/* do nothing */;
  else if (strncmp(command, "xtex:", 7) == 0) registerActionProc(w, cp) ;
  else {
    error(0,0, "special \"%s\" not implemented", orig_cp);
  }
}
