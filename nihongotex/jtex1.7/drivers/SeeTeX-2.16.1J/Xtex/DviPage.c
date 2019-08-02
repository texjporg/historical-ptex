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

#include <X11/Xos.h>  
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/Xmu/Converters.h>
#include <X11/ObjectP.h>

#include <stdio.h>
#include "dvi-simple.h"
#include "libtex/conv.h"
#include "mio.h"
#include <assert.h>

#ifdef	DEBUG_FLUSH
#  include "xtex.h"
#endif

#include "DviPageP.h"

static XtResource resources[] = {
  
#define offset(field) XtOffset(DviPageWidget, field)
  
  /* {name, class, type, size, offset, default_type, default_addr}, */
  
  {XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
     offset(dviPage.background), XtRString, "XtDefaultBackground"},
  
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     offset(dviPage.foreground), XtRString, "XtDefaultForeground"},
  
  {XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     offset(dviPage.font), XtRString, "XtDefaultFont"},
  
  {XtNpage,  XtCPage, XtRPointer, sizeof(char *),
     offset(dviPage.page), XtRPointer, 0},
  
  {XtNpai,  XtCPai, XtRPointer, sizeof(struct PostAmbleInfo *),
     offset(dviPage.pai), XtRPointer, 0 },
  
  {XtNdviFonts,  XtCDviFonts, XtRPointer, sizeof(DviFontInfo *),
     offset(dviPage.dviFonts), XtRPointer, 0 },
  
  {XtNxFonts,  XtCXFonts, XtRPointer, sizeof(XFontStruct **),
     offset(dviPage.xFonts), XtRPointer, 0 },
  
  {XtNuserMag,  XtCUserMag, XtRInt, sizeof(int),
     offset(dviPage.userMag), XtRString, "500"},
  
  {XtNdpiHoriz, XtCDpiHoriz, XtRFloat, sizeof(float),
     offset(dviPage.dpiHoriz), XtRString, "85"},
  
  {XtNdpiVert,  XtCDpiVert, XtRFloat, sizeof(float),
     offset(dviPage.dpiVert), XtRString, "-1"},
  
  {XtNreverseVideo,  XtCReverseVideo, XtRBool, sizeof(Bool),
     offset(dviPage.reverseVideo), XtRString, "False"},

  {XtNredisplay,  XtCRedisplay, XtRBool, sizeof(Bool),
     offset(dviPage.redisplay), XtRString, "False"},
  
  {XtNbackingStore, XtCBackingStore, XtRBackingStore, sizeof (int),
     offset (dviPage.useBackingStore), XtRString, "default"},
  
  {XtNpaperWidth, XtCPaperWidth, XtRFloat, sizeof (float),
     offset (dviPage.paperWidth), XtRString, "8.5"},
  
  {XtNpaperHeight, XtCPaperHeight, XtRFloat, sizeof (float),
     offset (dviPage.paperHeight), XtRString, "11"},
  
  {XtNtopOffset, XtCTopOffset, XtRFloat, sizeof (float),
     offset (dviPage.topOffset), XtRString, "1"},
  
  {XtNleftOffset, XtCLeftOffset, XtRFloat, sizeof (float),
     offset (dviPage.leftOffset), XtRString, "1"},
  
  {XtNscaleUnits, XtCScaleUnits, XtRString, sizeof (char *),
     offset (dviPage.scaleUnitsString), XtRString, "in"},
  
#ifdef HAVE_DPS
  {XtNdpsPreamble, XtCDPSpreamble, XtRString, sizeof (char *),
     offset (dviPage.dpsPreamble), XtRString, ""},

  {XtNtrueDpi, XtCTrueDpi, XtRFloat, sizeof (float),
     offset (dviPage.trueDpi), XtRString, "0.0"},
#endif

#undef offset
};

#ifdef __STDC__
static void ClassInitialize();
static void Initialize(), Destroy(), Redisplay();
static void Realize();
static Boolean SetValues( );
static void SetPageSize( DviPageWidget, Dimension *width, Dimension *heigth);
static XtGeometryResult QueryGeometry();

extern double atof(char *);
#else
static void ClassInitialize();
static void Initialize(), Destroy(), Redisplay();
static void Realize();
static Boolean SetValues();
static XtGeometryResult QueryGeometry();
static XtGeometryResult QueryGeometry();

extern double atof();
#endif

static void
  actionLineAnchor(),
  actionLineCastOff(),
  actionLineDraw(),
  actionLineUnDraw(),
  actionLineUnits();

void /* in DviPage2.c */
  actionPushButton();

static XtActionsRec dviPageActionTable[] = {
  {"dvi-line-anchor",  actionLineAnchor},
  {"dvi-line-castoff",  actionLineCastOff},
  {"dvi-line-draw",  actionLineDraw},
  {"dvi-line-undraw",  actionLineUnDraw},
  {"dvi-line-units",  actionLineUnits},
  {"dvi-push-button",  actionPushButton}
};

DviPageClassRec dviPageClassRec = {
  { /* core fields */
    /* superclass		*/
    (WidgetClass) &widgetClassRec,
    /* class_name		*/	"DviPage",
    /* widget_size		*/	sizeof(DviPageRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	dviPageActionTable,
    /* num_actions		*/	XtNumber(dviPageActionTable),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	NULL,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	XtInheritTranslations,
    /* query_geometry		*/	QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
    },
      { /* dviPage fields */
	/* empty			*/	0
	}
};

WidgetClass dviPageWidgetClass = (WidgetClass)&dviPageClassRec;

/****************************************************************
 *
 * Class Procedures
 *
 ****************************************************************/
     
static void
  ClassInitialize()
{
  XtAddConverter( XtRString, XtRBackingStore, XmuCvtStringToBackingStore,
		 NULL, 0 );
}

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static ScaleUnits
stringToScaleUnits(str)
char *str;
{
#define IS(a,b) if (strcmp(str,a)==0) return b
  IS("in", sIn);
  IS("pt", sPt);
  IS("cm", sCm);
  IS("mm", sMm);
  IS("sp", sSp);
  IS("pc", sPc);
  IS("bp", sBp);
  IS("dd", sDd);
  IS("cc", sCc);
  return( sSp );
}


static void releaseButtons(dw)
DviPageWidget dw;
{
  while ( dw -> dviPage.buttons ) {
    DviButton *n = dw -> dviPage.buttons -> next;
    if ( dw -> dviPage.buttons -> longerAction ) {
      XtFree(dw -> dviPage.buttons -> longerAction);
    }
    XtFree(dw -> dviPage.buttons);
    dw -> dviPage.buttons = n;
  }
}

static void
  SetPageSize(w, wd, ht)
DviPageWidget w;
Dimension *wd;
Dimension *ht;
{
  DviPagePart *pi = & (w -> dviPage);
  struct PostAmbleInfo *pai = pi -> pai;
  
  long ppi;	/* pixels per inch */
  
  extern long Numerator, Denominator;
  double dNumerator = Numerator;
  double dDenominator = Denominator;
  double dMag;
  extern double	DMagFactor();
  
  /* compute horizontial and vertical scaling factors - this code is
     largely taken from ../lib/conv.c, but we need to duplicate it
     because we've got two dpi values to deal with */
  
  if ( pi -> dpiVert < 0 ) {
    pi -> dpiVert = pi -> dpiHoriz;
  }
  
  dMag = DMagFactor( 1000) * DMagFactor(pi -> userMag);
  
  w -> dviPage.fastFromSpValueHoriz =
    (254000.0 / dNumerator) * (dDenominator / pi -> dpiHoriz)
      * (1.0 / dMag);
  
  w -> dviPage.fastFromSpValueVert =
    (254000.0 / dNumerator) * (dDenominator / pi -> dpiVert)
      * (1.0 / dMag);
  
  /* compute the screen pixels per inch at this magnification */
  
  w -> dviPage.pixelsPerInchHoriz = ( (double) pi -> dpiHoriz * dMag );
  
  w -> dviPage.pixelsPerInchVert = ( (double) pi -> dpiVert * dMag );
  
  w -> dviPage.pixelsWide
    = w -> dviPage.pixelsPerInchHoriz * w -> dviPage.paperWidth;
  
  w -> dviPage.pixelsHigh
    = w -> dviPage.pixelsPerInchVert * w -> dviPage.paperHeight;
  
  (*wd) = w -> dviPage.pixelsWide;
  (*ht) = w -> dviPage.pixelsHigh;
}

static void Initialize (request, new)
     Widget request, new;
{
  DviPageWidget want = (DviPageWidget) request;
  DviPageWidget w = (DviPageWidget) new;
  
  Dimension wd, ht;
  

  /* copy over  everything */

  w -> dviPage = want -> dviPage;

  w -> dviPage.clearGC  = 0;
  w -> dviPage.paintGC  = 0;
  w -> dviPage.invertGC  = 0;

  w -> dviPage.redisplay = False;

  w -> dviPage.anchored = False;

  w -> dviPage.units = stringToScaleUnits( w -> dviPage.scaleUnitsString);
  w -> dviPage.scalePopup = 0;
  w -> dviPage.scaleLabel = 0;

  w -> dviPage.spPerDot = INCH_TO_SP(1.0);
  w -> dviPage.spPerDot /= w -> dviPage.dpiHoriz;
  w -> dviPage.spPerDot *= 1000;
  w -> dviPage.spPerDot /= w -> dviPage.userMag;

  w -> dviPage.dviStack  = 0;
  w -> dviPage.dviStackSize  = 0;
  w -> dviPage.pathLen  = 0;
  w -> dviPage.penSize = 1;
  w -> dviPage.whiten = False;
  w -> dviPage.shade = False;
  w -> dviPage.blacken = False;
  
#ifdef HAVE_DPS
  w -> dviPage.ctx = 0;
#endif /* HAVE_DPS */

  w -> dviPage.buttons = 0;

  SetPageSize(w, &wd, &ht);
  
#define MaX(x,y) ((x) > (y) ) ? (x) : (y)
  
  w -> core.width = MaX(wd, want->core.width);
  w -> core.height = MaX(ht, want->core.height);
  
#undef MaX
  
}

static void SetGC(w)
DviPageWidget w;
{
  
  XGCValues	values;
  unsigned long fore = w->dviPage.foreground;
  unsigned long aft = w->dviPage.background;
  
  if ( w -> dviPage.reverseVideo ) {
    unsigned long tmp = aft;
    aft = fore;
    fore = tmp;
  }
  values.function	= GXcopy;
  values.foreground	= aft;
  values.background	= fore;

  XChangeGC( XtDisplay(w), w -> dviPage.clearGC,
	    (unsigned) GCFunction | GCForeground | GCBackground,
	    &values);

  /* Prepare to paint */
    
  values.function	= GXcopy;
  values.foreground	= fore;
  values.background	= aft;
  values.plane_mask	= AllPlanes;
  values.font		= w->dviPage.font->fid;
  values.line_width	= 1;
    
  XChangeGC( XtDisplay(w), w -> dviPage.paintGC,
	    (unsigned) GCFunction | GCPlaneMask
	    | GCForeground | GCBackground | GCFont | GCLineWidth,
	    &values);

  values.function	= GXxor;
  values.foreground	= (fore ^ aft);
  values.background	= (fore ^ aft);
  values.font		= w -> dviPage.font -> fid;
  values.line_width	= 0;
  
  w -> dviPage.invertGC =
    XtGetGC( w, 
	    (unsigned) GCFunction | GCForeground | GCBackground
	    | GCFont | GCLineWidth,
	    &values);
}

static void Realize (gw, valueMask, attrs)
     Widget gw;
     XtValueMask *valueMask;
     XSetWindowAttributes *attrs;
{
  
  XWindowAttributes wAttributes;
  
  DviPageWidget	w = (DviPageWidget) gw;
  
  switch (w->dviPage.useBackingStore) {
  case Always:
  case NotUseful:
  case WhenMapped:
    *valueMask |=CWBackingStore;
    attrs->backing_store = w->dviPage.useBackingStore;
    break;
  }
  
  XtCreateWindow( w, InputOutput, (Visual *)CopyFromParent,
		 *valueMask, attrs);

  XGetWindowAttributes( XtDisplay(w), XtWindow(w), &wAttributes );
  
  
  w -> dviPage.haveBackingStore
    = ( wAttributes.save_under == Always)
      || (wAttributes.save_under == WhenMapped);
  
  w->dviPage.paintGC
    = XCreateGC( XtDisplay(w), XtWindow(w), 0, NULL);

  w->dviPage.clearGC
    = XCreateGC( XtDisplay(w), XtWindow(w), 0, NULL);

  w->dviPage.invertGC
      = XCreateGC( XtDisplay(w), XtWindow(w), 0, NULL);

  SetGC( w );

}

static void Destroy (gw)
     Widget gw;
{
  DviPageWidget w = (DviPageWidget) gw;
  XtFree( w -> dviPage.dviStack );
  w -> dviPage.dviStack = 0;
  
  if ( w -> dviPage.paintGC ) {
    XFreeGC( XtDisplay(w), w -> dviPage.paintGC );
    w -> dviPage.paintGC = 0;
  }

  if ( w -> dviPage.clearGC ) {
    XFreeGC( XtDisplay(w), w -> dviPage.clearGC );
    w -> dviPage.clearGC = 0;
  }

  if ( w -> dviPage.invertGC ) {
    XtReleaseGC( w, w -> dviPage.invertGC );
    w -> dviPage.invertGC = 0;
  }

  if ( w -> dviPage.scalePopup) {
    XtDestroyWidget( w -> dviPage.scalePopup );
  }

#ifdef HAVE_DPS
  if ( w -> dviPage.ctx ) {
    DPSFlushContext( w -> dviPage.ctx );
    DPSWaitContext( w -> dviPage.ctx );
    DPSDestroySpace( DPSSpaceFromContext( w -> dviPage.ctx ));
  }
#endif

  releaseButtons(w);

}

static Boolean SetValues(current, request, new)
     DviPageWidget current, request, new;
{
  
  DviPagePart *pi = & (request -> dviPage);
  struct PostAmbleInfo *p = pi -> pai;
  Boolean replot = False;
  
#define NOT_SAME_PTR(XX) (request -> dviPage.XX && request->dviPage.XX != current -> dviPage.XX)
  
#define UPDATE_VALUE(XX) {\
	new -> dviPage.XX = request -> dviPage.XX;\
 }
  
  if ( NOT_SAME_PTR(page) ) {
    UPDATE_VALUE(page);
    replot = True;
  }
  
  if ( NOT_SAME_PTR(xFonts) ) {
    UPDATE_VALUE(xFonts);
    replot = True;
  }
  
  if ( NOT_SAME_PTR(pai) ) {
    UPDATE_VALUE(pai);
    replot = True;
  }
  
  if ( new -> dviPage.pai ) {
    
    int stackSize = new -> dviPage.pai -> pai_DVIStackSize;
    
    if (stackSize > new -> dviPage.dviStackSize ) {
      
      stackSize += 2;
      
      XtFree( new -> dviPage.dviStack );
      
      new -> dviPage.dviStack  = 0;
      new -> dviPage.dviStackSize = stackSize;
      
      if (stackSize > 0 ) {
	new -> dviPage.dviStack  = 
	  (LocalDviStack *) (XtMalloc( (stackSize+1) * sizeof(LocalDviStack) ));
      }
    }
  }
  
  if ( NOT_SAME_PTR(dpiHoriz) ) {
    UPDATE_VALUE(dpiHoriz);
    replot = True;
  }
  
  if ( NOT_SAME_PTR(dpiVert) ) {
    UPDATE_VALUE(dpiVert);
    replot = True;
  }
  
  if ( NOT_SAME_PTR(dviFonts) ) {
    UPDATE_VALUE(dviFonts);
    replot = True;
  }
  
  if ( NOT_SAME_PTR(paperWidth) ) {
    UPDATE_VALUE(paperWidth);
    replot = True;
  }
  
  if ( NOT_SAME_PTR(paperHeight) ) {
    UPDATE_VALUE(paperHeight);
    replot = True;
  }
  
  if ( NOT_SAME_PTR(topOffset) ) {
    UPDATE_VALUE(topOffset);
    replot = True;
  }
  
  if ( NOT_SAME_PTR(leftOffset) ) {
    UPDATE_VALUE(leftOffset);
    replot = True;
  }

  if ( NOT_SAME_PTR(scaleUnitsString) ) {
    UPDATE_VALUE(scaleUnitsString);
    replot = True;
  }

  if ( NOT_SAME_PTR(userMag) ) {
    UPDATE_VALUE(userMag);
    replot = True;
  }
  
  new -> dviPage.units = stringToScaleUnits( new -> dviPage.scaleUnitsString);

  new -> dviPage.spPerDot = INCH_TO_SP(1.0);
  new -> dviPage.spPerDot /=  new -> dviPage.dpiHoriz;
  new -> dviPage.spPerDot *= 1000;
  new -> dviPage.spPerDot /= new -> dviPage.userMag;

  SetGC( new );

  SetPageSize(new, &(new -> core.width), &(new -> core.height));

  /*  WHY ? new -> dviPage.anchored = True;*/
  new -> dviPage.anchored = False;
  new -> dviPage.redisplay = False;

  /* let application explicitly manage redisplay */

  replot = False;
  if ( request -> dviPage.redisplay ) {
    replot = True;
  }

  return( replot );
}

static XtGeometryResult QueryGeometry(w, intended, preferred)
     Widget w;
     XtWidgetGeometry *intended, *preferred;
{
  register DviPageWidget lw = (DviPageWidget) w;
  Dimension wd, ht;
  
  /* I begin to think that this procedure doesn't do much of worth */

  return XtGeometryYes;
/***********************************************************************
#if 0

  SetPageSize((DviPageWidget) w, &wd, &ht);
  
  preferred -> request_mode = CWWidth | CWHeight;
  preferred -> width = wd ;
  preferred -> height =  ht ;

#define NEAR(X,Y) (abs(X-Y) < 5)

  if (  ((intended->request_mode & (CWWidth | CWHeight))
	  == (CWWidth | CWHeight)))
    {
      if (intended->width == w->core.width &&
	  intended->height == w->core.height) {
	return XtGeometryYes;
      }
      else if (NEAR(intended->width, preferred->width) &&
	  NEAR(intended->height, preferred->height)) {
	return XtGeometryAlmost;
      }
      else {
	return XtGeometryNo;
      }
  }
  else {
    return XtGeometryYes;
  }
#endif
***********************************************************************/
}

/*
 *	Some very DVI related things
 */

#define	ABS(X)	( (X) < 0 ? -(X) : (X) )

static int selectFont(w, ptr, index, font)
     DviPageWidget w;
     DviFontInfo **ptr;
     int *index;
     int font;
{
  if ( *ptr != 0 && (*ptr) -> paf.paf_DVIFontIndex == font ) {
    return 0;
  }
  else {
    int offset;
    DviFontInfo *fonts = w -> dviPage.dviFonts;
    
    *ptr = 0;
    for ( offset = 0; offset < RegisteredFonts; offset++ ) {
      if ( fonts[offset].paf.paf_DVIFontIndex == font ) {
	*ptr = &(fonts[offset]);
	*index = offset;
	break;
      }
    }
    
    if ( *ptr == 0 ) {
      error(0,0,"Can't find font %d", font);
      return 1;
    }
    else {
      return(0);
    }
  }
}

typedef struct {
  int spDrift;
  long spPerDot;
  DviPagePart *page; 
  DviFontInfo *dviCurrentFont;
  int dviCurrentFontIndex;
  XFontStruct *currentFontStruct;
  
  /* following are used to reduce the number of XDrawStrings we do */
  
#define MAX_DVI_STRING 64
#ifdef ASCIIJTEX
  XChar2b dviString[ MAX_DVI_STRING ];
#else /* !ASCIIJTEX */
  char dviString[ MAX_DVI_STRING ];
#endif /* !ASCIIJTEX */
  int dviStringLength;
  
#ifdef STATS
  int longestString;
#endif
  
  int startHH, startVV;
  int endHH, endVV;
#if defined(ASCIIJTEX) && defined(FAKEKANJI)
  int offHH; /* left offset of a Kanji character in a fixed width font */
#endif /* ASCIIJTEX && FAKEKANJI */
  
  LocalDviStack *dviStackP;	/* current stack item */
  
} DviDisplayInfo;

static void DviDisplayFlush(widget, info)
     DviPageWidget widget;
     DviDisplayInfo *info;
{
#ifdef DEBUG_FLUSH
  if (Debug) {
    fprintf(stderr,"Flush string of %d bytes at %d,%d\n",
	    info -> dviStringLength ,  info -> startHH ,  info -> startVV );
  }
#endif

  if (  info -> dviStringLength  > 0 ) {
    
    i32 atHH = fastFromSpHoriz(widget,  info -> startHH );
    i32 atVV = fastFromSpVert(widget,  info -> startVV );
    Bool doit = True;
    
#ifdef STATS
    if (info -> dviStringLength > info -> longestString) {
      info -> longestString = info -> dviStringLength;
    }
#endif
    
    if ( widget -> dviPage.haveBackingStore ) {
      i32 width = fastFromSpHoriz(widget,  info -> endHH );
      i32 height = fastFromSpVert(widget,  info -> endVV );
      
      doit =
	XRectInRegion( widget -> dviPage.updateRegion, atHH, atVV,
		      width, height) != RectangleOut;
    }
    
    if (doit) {
#ifdef ASCIIJTEX
      XDrawString16( XtDisplay( widget ), XtWindow( widget ),
#ifdef FAKEKANJI
		  info -> page  -> paintGC, atHH - info -> offHH, atVV,
#else /* !FAKEKANJI */
		  info -> page  -> paintGC, atHH, atVV,
#endif /* !FAKEKANJI */
		  info -> dviString ,  info -> dviStringLength  );
#else /* !ASCIIJTEX */
      XDrawString( XtDisplay( widget ), XtWindow( widget ),
		  info -> page  -> paintGC, atHH, atVV,
		  info -> dviString ,  info -> dviStringLength  );
#endif /* !ASCIIJTEX */
    }
  }
  
  info -> dviStringLength = 0;
  info -> startHH = info -> dviStackP -> h;
  info -> startVV = info -> dviStackP -> v ;
  info -> endHH = info -> startHH ;
}

static void DviDisplay(widget)
     DviPageWidget widget;
{
  int c;
#ifdef ASCIIJTEX
  unsigned char byte1, byte2;
  int i; /* index for char_info. (e.g. widths, glues ...) */
#endif /* ASCIIJTEX */
  i32 p;
  int advance;
  int wuz;
  DviDisplayInfo info;
  double spDriftTmp;
  long tmp;
  
  struct PostAmbleInfo *pai;
  char *thisPage;
  LocalDviStack *dviStack;
  
  /* initialize info structure */
  
  info.page = &( widget -> dviPage); 
  info.dviCurrentFont = 0;
  info.dviCurrentFontIndex = 0;
  info.currentFontStruct = widget -> dviPage.font;
  info.dviStringLength = 0;
  info.startHH = info.startVV = -1;
  info.endHH = info.endVV = -1;
#ifdef STATS
  info.longestString = 0;
#endif
  
  /* compute  spPerDot  -- the number of Sp per actual *dot* on the display */
  
  info.spPerDot = INCH_TO_SP(1.0);
  info.spPerDot /= info.page -> pixelsPerInchHoriz;
  
#ifdef STATS
  fprintf(stderr,"spPerDot		= %f\n", info.spPerDot);
#endif
  
  /* initialize things dependent on info structure */
  
  pai = info.page  -> pai;
  thisPage = info.page->page ;
  dviStack = info.page  -> dviStack;
  
  if ( pai == 0 || thisPage == 0 || dviStack == 0) return;
  
  assert( dviStack != 0 );
  info.dviStackP = dviStack;
  
  /* insure that first byte is a BOP mark */
  
  c = (*thisPage) & 0xff;
  if ( DVI_DT(c) != DT_BOP ) {
    error(0,0,"No BOP found");
    goto cleanup;
  }
  
  for(;;) {
    
    c = mGetByte(&thisPage);
    
    if (DVI_IsChar(c)) {
      
      int offset;
      int thisCharWidthInSp;
      int thisCharTfmWidthInSp;
      int thisWidth;
      int driftInSp;
      
      advance = 1;
      
    doDviChar:
      
#ifdef ASCIIJTEX
      byte1 = (unsigned)c >> 8;
      byte2 = (unsigned)c & 0xff;
      /* for normal font (i.e. non kanji font), i equals to c. */
      i = codetoinfo(info.dviCurrentFont->nt, info.dviCurrentFont->ct, c);
#endif /* ASCIIJTEX */
      
      /*
       *	The following code makes use of the FontStruct information;
       *	each time we cache a glyph, we update the stack to point to
       *	the location where we should be. endHH, on the other hand,
       *	is the *actual* location, based on the width of the displayed
       *	glyph.
       */
      
#ifdef ASCIIJTEX
#define b1 (info.currentFontStruct -> min_byte1)
#define B1 (info.currentFontStruct -> max_byte1)
#define cb2 (info.currentFontStruct -> min_char_or_byte2)
#define CB2 (info.currentFontStruct -> max_char_or_byte2)
      if (byte1 < b1 || byte1 > B1 || byte2 < cb2 || byte2 > CB2) {
	byte1 = info.currentFontStruct->default_char >> 8;
	byte2 = info.currentFontStruct->default_char & 0xff;
      }
      offset = (byte1 - b1) * (CB2 - cb2 + 1) + (byte2 - cb2);
#undef b1
#undef B1
#undef cb2
#undef CB2
#else /* !ASCIIJTEX */
      offset = c - ( info.currentFontStruct  -> min_char_or_byte2);
#endif /* !ASCIIJTEX */
      if (  info.currentFontStruct  -> per_char ) {
	XCharStruct *cs = &( info.currentFontStruct  -> per_char[offset]);
	thisWidth = cs -> width;
      }
      else {
	thisWidth = info.currentFontStruct  -> max_bounds.width;
      }
      thisCharWidthInSp = (thisWidth *  info.spPerDot );
      
      if ( info.dviCurrentFont  &&  info.dviCurrentFont  -> widths) { 
#ifdef ASCIIJTEX
	thisCharTfmWidthInSp =  ( info.dviCurrentFont  -> widths[ i ]);
#else /* ASCIIJTEX */
	thisCharTfmWidthInSp =  ( info.dviCurrentFont  -> widths[ c ]);
#endif /* ASCIIJTEX */
      }
      
      /*
       * driftInSp is the number of scaled-points we'll be drifting
       * from the true location if we lay down the current glyph.
       *
       * we used to check if the current string when over the drift
       * limit, but that gives choppier output.
       *
       */
      
      driftInSp = ABS((info.endHH + thisCharWidthInSp)
		      - ( info.dviStackP  -> h + thisCharTfmWidthInSp));
      
#ifdef DEBUG_FLUSH
      if (Debug) {
	fprintf(stderr,"Flush decision:\n");
	fprintf(stderr,"%-20s=%d\n", "advance", advance);
	fprintf(stderr,"%-20s=%d\n", "thisWidth", thisWidth);
	fprintf(stderr,"%-20s=%d\n", "spPerDot", info.spPerDot);
	fprintf(stderr,"%-20s=%d\n", "thisCharWidthInSp", thisCharWidthInSp);
	fprintf(stderr,"%-20s=%d\n", "driftInSp", driftInSp);
	fprintf(stderr,"%-20s=%d\n", " info.endHH ",
		info.endHH );
	fprintf(stderr,"%-20s=%d\n", " info.endVV ",
		info.endVV );
	fprintf(stderr,"%-20s=%d\n", " info.dviStackP ->v",
		info.dviStackP  -> v);
	fprintf(stderr,"%-20s=%d\n", " info.dviStackP ->h",
		info.dviStackP  -> h);
      }
#endif

/*
 * should we flush the existing string before adding the current
 * character?
 */

      if ( !advance
	   ||  driftInSp > info.spPerDot
	   ||  info.endVV  != ( info.dviStackP  -> v)
	   ||  info.dviStringLength  >= MAX_DVI_STRING )
	{
	  DviDisplayFlush(widget, &info);
	}
      
      if (info.dviStringLength == 0 ) {
	info.startHH = info.dviStackP -> h;
	info.startVV = info.dviStackP -> v ;
	info.endHH = info.startHH ;
#if defined(ASCIIJTEX) && defined(FAKEKANJI)
	if (info.dviCurrentFont -> glues) {
	  info.offHH = fastFromSpHoriz(widget, info.dviCurrentFont -> glues[i]);
	} else {
	  info.offHH = 0;
	}
#endif /* ASCIIJTEX && FAKEKANJI */
      }
#ifdef ASCIIJTEX
      info.dviString [  info.dviStringLength ].byte1 = byte1;
      info.dviString [  info.dviStringLength ].byte2 = byte2;
      info.dviStringLength ++;
#else /* !ASCIIJTEX */
      info.dviString [  info.dviStringLength ++ ] = c; 
#endif /* !ASCIIJTEX */
      
      if ( advance ) {
	if ( info.dviCurrentFont  &&  info.dviCurrentFont  -> widths) { 
	  info.dviStackP  -> h += thisCharTfmWidthInSp;
	}
	info.endHH  += thisCharWidthInSp;
      }
      else {
	info.endHH = info.dviStackP  -> h;
      }
      
      info.endVV = info.dviStackP  -> v;
      
    } else {
      
      switch (DVI_OpLen(c)) {
	
      case DPL_NONE:
	break;
	
      case DPL_SGN1:
	p = mGetByte(&thisPage);
	p = Sign8(p);
	break;
	
      case DPL_SGN2:
	mGetWord(&thisPage, &p);
	p = Sign16(p);
	break;
	
      case DPL_SGN3:
	mGet3Byte(&thisPage, &p);
	p = Sign24(p);
	break;
	
      case DPL_SGN4:
	mGetLong(&thisPage, &p);
	break;
	
      case DPL_UNS1:
	p = UnSign8(mGetByte(&thisPage));
	break;
	
      case DPL_UNS2:
	mGetWord(&thisPage, &p);
	p = UnSign16(p);
	break;
	
      case DPL_UNS3:
	mGet3Byte(&thisPage, &p);
	p = UnSign24(p);
	break;
	
      default:
	panic("DVI_OpLen(%d) = %d", c, DVI_OpLen(c));
	goto cleanup;
	/* NOTREACHED */
      }
      
      switch (DVI_DT(c)) {
	
      case DT_SET:
	c = p;
	advance = 1;
#ifdef DEBUG_FLUSH
	if (Debug) {
	  fprintf(stderr,"SET character %d (advance)\n", c);
	}
#endif
	goto doDviChar;
	break;
	
      case DT_PUT:
	c = p;
	advance = 0;
#ifdef DEBUG_FLUSH
	if (Debug) {
	  fprintf(stderr,"PUT character %d (no advance)\n", c);
	}
#endif
	goto doDviChar;
	break;
	
      case DT_SETRULE:
      case DT_PUTRULE:
	DviDisplayFlush(widget, &info);
	{
	  i32 cHH, cVV;
	  double dhh, dvv;
	  i32 rhh, rvv;
	  
	  cHH = fastFromSpHoriz(widget,  info.dviStackP  -> h);
	  cVV = fastFromSpVert(widget,  info.dviStackP  -> v);
	  
	  mGetLong(&thisPage, &rvv);
	  mGetLong(&thisPage, &rhh);
	  
	  if (DVI_DT(c) == DT_SETRULE) {
	    info.dviStackP  -> h += rhh;
	  }
	  
	  dhh = rhh;
	  dvv = rvv;
	  
	  rhh = CEIL( fastFromSpHoriz(widget, dhh) );
	  rvv = CEIL( fastFromSpVert(widget, dvv) );
	  
	  XFillRectangle(XtDisplay(widget), XtWindow(widget),
			 info.page  -> paintGC, cHH, cVV - rvv, rhh, rvv);
	  
	}
	break;
	
      case DT_NOP:
	break;
	
      case DT_BOP:
	/*
	 *	Each beginning of  info.page  has 11 4-byte words telling us things
	 *	about the  info.page. We ignore them.
	 */
	thisPage += (11 * 4);
	
	info.dviStackP = dviStack;
	info.dviStackP  -> h = INCH_TO_SP( (double)  info.page  -> leftOffset);
	info.dviStackP  -> v = INCH_TO_SP( (double)  info.page  -> topOffset);
	info.startHH = info.dviStackP  -> h;
	info.startVV = info.dviStackP  -> v ;
	info.endHH = info.startHH ;
	info.endVV = info.startVV ;
	info.dviStackP  -> w = 0;
	info.dviStackP  -> x = 0;
	info.dviStackP  -> y = 0;
	info.dviStackP  -> z = 0;
	
	break;
	
      case DT_EOP:
	goto cleanup;
	/* NOTREACHED */
	
      case DT_PUSH:
	DviDisplayFlush(widget, &info);
	{
	  /* copy the old stack to the new one */
	  
	  LocalDviStack *old = info.dviStackP ;
	  info.dviStackP ++;
	  * info.dviStackP = *old;
	}
	break;
	
      case DT_POP:
	/* just use the old stack */
	DviDisplayFlush(widget, &info);
	info.dviStackP --;
	break;
	
      case DT_W0:	/* there should be a way to make these pretty */
	p = info.dviStackP  -> w;
	goto move_right;
	
      case DT_W:
	info.dviStackP  -> w = p;
	goto move_right;
	
      case DT_X0:
	p = info.dviStackP  -> x;
	goto move_right;
	
      case DT_X:
	info.dviStackP  -> x = p;
	goto move_right;
	
      case DT_RIGHT:
      move_right:
	DviDisplayFlush(widget, &info);
#ifdef DEBUG_FLUSH
	if (Debug) {
	  fprintf(stderr,"RIGHT %d\n", p);
	}
#endif
	info.dviStackP  -> h += p;
	break;
	
      case DT_Y0:
	p = info.dviStackP  -> y;
	goto move_down;
	
      case DT_Y:
	info.dviStackP  -> y = p;
	goto move_down;
	
      case DT_Z0:
	p = info.dviStackP  -> z;
	goto move_down;
	
      case DT_Z:
	info.dviStackP  -> z = p;
	goto move_down;
	
      case DT_DOWN:
      move_down:
	DviDisplayFlush(widget, &info);
#ifdef DEBUG_FLUSH
	if (Debug) {
	  fprintf(stderr,"DOWN %d\n", p);
	}
#endif
	info.dviStackP  -> v += p;
	break;
	
      case DT_FNTNUM:
      case DT_FNT:
	{
	  int font;
	  
	  /* flush existing text */
	  
	  DviDisplayFlush(widget, &info);
	  
	  /* switch to new font */
	  
	  font = ( DVI_DT(c) == DT_FNTNUM )
	    ? font = (c - DVI_FNTNUM0) : p;
	  
	  if ( selectFont(widget,
			  & info.dviCurrentFont , & info.dviCurrentFontIndex , font) ) {
	    goto cleanup;
	  }
	  else {
	    /* load the font */
	    int useFont =
	      (widget -> dviPage.xFonts
	       && widget -> dviPage.xFonts[  info.dviCurrentFontIndex  ]);
	    
	    Font font ;
	    
	    info.currentFontStruct = 
	      (useFont)
		? (widget->dviPage.xFonts[  info.dviCurrentFontIndex  ])
		  : widget -> dviPage.font ;
	    
	    font = info.currentFontStruct  -> fid;
	    
	    XSetFont( XtDisplay(widget), widget -> dviPage.paintGC, font );
	  }
	}
	break;
	
      case DT_XXX:
	{
	  char specialBuffer [2048];
	  register char *cp;
	  int sweetp = 0;
	  
	  DviDisplayFlush(widget, &info);
	  if (p > 2047) {
	    sweetp = p - 2047;
	    p = 2047;
	  }
	  
	  for (cp = specialBuffer ; p > 0; p--) {
	    *cp = mGetByte(&thisPage);
	    cp++;
	  }
	  *(cp) = 0;
	  
	  while(sweetp > 0) {
	    mGetByte(&thisPage);
	  }
	  
	  widget -> dviPage.dviStackPointer = info.dviStackP ;
	  doSpecial(widget, specialBuffer);
	}
	break;
	
      case DT_FNTDEF:
	{
	  int i;
	  long j;
	  mGetLong(&thisPage, &j);
	  mGetLong(&thisPage, &j);
	  mGetLong(&thisPage, &j);
	  i = UnSign8(mGetByte(&thisPage));
	  i += UnSign8(mGetByte(&thisPage));
	  while (--i >= 0) {
	    (void) mGetByte(&thisPage);
	  }
	}
	break;
	
      case DT_PRE:
	GripeUnexpectedOp("PRE");
	goto cleanup;
	/* NOTREACHED */
	
      case DT_POST:
	GripeUnexpectedOp("POST");
	goto cleanup;
	/* NOTREACHED */
	
      case DT_POSTPOST:
	GripeUnexpectedOp("POSTPOST");
	goto cleanup;
	/* NOTREACHED */
	
      case DT_UNDEF:
	GripeUndefinedOp(c);
	goto cleanup;
	/* NOTREACHED */
	
      default:
	panic("DVI_DT(%d) = %d", c, DVI_DT(c));
	goto cleanup;
	/* NOTREACHED */
      }
    }
  }
  
 cleanup:
  /* clean up laststring if there was one */
  DviDisplayFlush(widget, &info);
  
#ifdef STATS
  fprintf(stderr,"Longest string was %d\n", info.longestString);
#endif
  
  return;
}


/* ARGSUSED */
static void Redisplay (gw, event, region)
     Widget gw;
     XEvent *event;		/* unused */
     Region region;		/* unused */
{
  DviPageWidget w = (DviPageWidget) gw;

  if (XtIsRealized(w)) {
    XSetRegion( XtDisplay(w), w -> dviPage.clearGC, region );
    
    XFillRectangle(XtDisplay(w), XtWindow(w),
		   w -> dviPage.clearGC,
		   0, 0,
		   w -> core.width, w -> core.height);
    
    XSetRegion( XtDisplay(w), w -> dviPage.paintGC, region );
    
    if (w -> dviPage.pai && w -> dviPage.page  && w -> dviPage.dviFonts ) {
      w -> dviPage.updateRegion = region;
      releaseButtons(w);
      DviDisplay(w);
    }
  }
}

/************************************************************************
 *		Action Procedures					*
 ***********************************************************************/

static void actionLineAnchor(w, event, params, num_params)
     DviPageWidget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  XGCValues	values;
  unsigned long fore = w -> dviPage.foreground;
  unsigned long aft = w -> dviPage.background;
  
  if ( ! w -> dviPage.scalePopup ) {
    w -> dviPage.scalePopup =
      XtCreatePopupShell("scale",
			 POPUP_WINDOW_TYPE, w,
			 NULL, 0);

    w -> dviPage.scaleLabel = (LabelWidget)
      XtCreateManagedWidget("label",
			    labelWidgetClass, w -> dviPage.scalePopup,
			    NULL, 0);
  }
  XtPopup(w -> dviPage.scalePopup, XtGrabNone);

  if ( !event ) return ;

  if ( event -> type == ButtonPress || event -> type == ButtonRelease ) {
    XButtonEvent *xb = (XButtonEvent *) event;
    w -> dviPage.anchorFromX = xb -> x;
    w -> dviPage.anchorFromY = xb -> y;
    w -> dviPage.anchored = True;
  }
  
} 

static void actionLineCastOff(w, event, params, num_params)
     DviPageWidget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  if ( w -> dviPage.scalePopup ) XtPopdown( w -> dviPage.scalePopup );
  /* un-anchor line ? */
  w -> dviPage.anchored = False;
}

static void actionLineCommon(draw, w, event, params, num_params)
     int draw;
     DviPageWidget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  int fx = w -> dviPage.anchorFromX;
  int fy = w -> dviPage.anchorFromY;
  int tx;
  int ty;
  int st;
  double xValue, yValue;
  char *units;

  char buff[128];

  Arg argList[20];
  int args = 0;
  
  if ( !event || ! w -> dviPage.anchored ) return ;
  
  switch( event -> type ) {
  case ButtonPress:
  case ButtonRelease:
    {
      XButtonEvent *xb = (XButtonEvent *) event;
      tx = xb -> x; ty = xb -> y;
    }
    break;
    
  case MotionNotify:
    {
      XMotionEvent *xb = (XMotionEvent *) event;
      tx = xb -> x; ty = xb -> y;
    }
    break;
    
    default:
    return;
  }
  
  
  if ( draw ) {
    w -> dviPage.anchorToX = tx;
    w -> dviPage.anchorToY = ty;
  }
  
  tx = w -> dviPage.anchorToX;
  ty = w -> dviPage.anchorToY;
  
  XDrawLine(XtDisplay(w), XtWindow(w),
	    w -> dviPage.invertGC, fx, fy, tx, ty);
  XDrawLine(XtDisplay(w), XtWindow(w),
	    w -> dviPage.invertGC, tx, 0, tx, w -> core.height);
  XDrawLine(XtDisplay(w), XtWindow(w),
	    w -> dviPage.invertGC, 0, ty, w -> core.width, ty);

  st = (int) w -> dviPage.units;
  units = ScaleUnitsString[ st ];

  xValue = ABS(tx-fx);
  xValue *= w -> dviPage.spPerDot;
  xValue /= ScaleUnitsInSp[ st ];

  yValue = ABS(ty-fy);
  yValue *= w -> dviPage.spPerDot;
  yValue /= ScaleUnitsInSp[ st ];


  sprintf(buff, "%4.2f %s, %4.2f %s",
	  xValue, units, yValue, units);

  args = 0;
  XtSetArg(argList[args], XtNlabel, buff); args++;
  XtSetValues( w -> dviPage.scaleLabel, argList, args);
} 

static void actionLineDraw(w, event, params, num_params)
     DviPageWidget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  actionLineCommon(1, w, event, params, num_params);
} 

static void actionLineUnDraw(w, event, params, num_params)
     DviPageWidget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  actionLineCommon(0, w, event, params, num_params);
} 

static void actionLineUnits(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  if ( *num_params == 1 ) {

    DviPageWidget ww;

    while ( XtClass(w) != dviPageWidgetClass ) {
      if ( XtIsWidget(w) ) {
	w = XtParent(w);
      } else if ( XtIsObject(w) ) {
	w = ( (Object) w ) -> object.parent; 
      }
      if ( ! w ) return;
    }

    ww = (DviPageWidget) w;

    ww -> dviPage.units = stringToScaleUnits( *params );
  }
  else {
    error(0,0,"Bogus number of parameters specified to dvi-line-units: %d",
	  *num_params);
  }
} 
