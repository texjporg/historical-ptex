#include <X11/copyright.h>

/* $XConsortium: DviPageP.h,v 1.2 88/10/25 17:37:59 swick Exp $ */
/* Copyright	Massachusetts Institute of Technology	1987, 1988 */

#ifndef _DviPageP_h
#define _DviPageP_h

#include "DviPage.h"
/* include superclass private header file */

#include <X11/CoreP.h>

typedef struct {
    int empty;
} DviPageClassPart;

typedef struct _DviPageClassRec {
    CoreClassPart	core_class;
    DviPageClassPart	dviPage_class;
} DviPageClassRec;

extern DviPageClassRec dviPageClassRec;

typedef struct {
	i32	stack_hh;
	i32	stack_vv;
	i32	h;		/* the saved h */
	i32	v;		/* the saved v */
	i32	w;		/* etc */
	i32	x;
	i32	y;
	i32	z;
} LocalDviStack;

typedef struct {
    Pixel	background;
    Pixel	foreground;

    int useBackingStore;	/* backing store type */
    Bool haveBackingStore;

    Bool reverseVideo;

    XFontStruct *font;

    /* private state */
    
    GC globalGC;

    char *page;
    Dimension pixelsWide;
    Dimension pixelsHigh;
    Dimension pixelsPerInch;

    char *paperWidthString;
    char *paperHeightString;
    float paperWidth;
    float paperHeight;

    char *topOffsetString;
    char *leftOffsetString;
    float topOffset;
    float leftOffset;

    int maxDrift;	/* in points */

    XFontStruct **xFonts;

    struct PostAmbleInfo *pai;
    DviFontInfo *dviFonts;

    int userMag;
    int dpi;

    Conv Conversion;
    int fastFromSpValue;

    Region updateRegion;

    LocalDviStack *dviStack;
    LocalDviStack *dviStackPointer;
    int dviStackSize;

    /* for tpic specials */
#define	MAXPOINTS	300	/* Max points in a path */
    int texPicScale;
    int xx[MAXPOINTS];
    int yy[MAXPOINTS];
    int pathLen;
    int penSize;
    Bool whiten;
    Bool shade;
    Bool blacken;
} DviPagePart;

typedef struct _DviPageRec {
    CorePart		core;
    DviPagePart	dviPage;
} DviPageRec;

#ifdef __STDC__
static inline long fastFromSP(DviPageWidget w, long x)
{
  return( x / ( w -> dviPage.fastFromSpValue ) );
}
#else

#define fastFromSP(w, x) ((x) / (w -> dviPage.fastFromSpValue))

#endif

#endif  _DviPageP_h
