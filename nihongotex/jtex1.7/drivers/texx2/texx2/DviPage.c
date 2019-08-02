/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 */
  
#include <X11/Xos.h>  
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include "dvi-simple.h"
#include "mio.h"
#include "conv.h"
#include <assert.h>

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

    {XtNdpi,  XtCDpi, XtRInt, sizeof(int),
       offset(dviPage.dpi), XtRString, "300"},

    {XtNreverseVideo,  XtCReverseVideo, XtRBool, sizeof(Bool),
       offset(dviPage.reverseVideo), XtRString, "False"},

    {XtNbackingStore, XtCBackingStore, XtRBackingStore, sizeof (int),
    	offset (dviPage.useBackingStore), XtRString, "default"},

    {XtNpaperWidth, XtCPaperWidth, XtRString, sizeof (char *),
    	offset (dviPage.paperWidthString), XtRString, "8.5"},

    {XtNpaperHeight, XtCPaperHeight, XtRString, sizeof (char *),
    	offset (dviPage.paperHeightString), XtRString, "11"},

    {XtNtopOffset, XtCTopOffset, XtRString, sizeof (char *),
    	offset (dviPage.topOffsetString), XtRString, "1"},

    {XtNleftOffset, XtCLeftOffset, XtRString, sizeof (char *),
    	offset (dviPage.leftOffsetString), XtRString, "1"},

    {XtNmaxDrift, XtCMaxDrift, XtRInt, sizeof (int),
    	offset (dviPage.maxDrift), XtRString, "3"}

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

static void DviPageAction(/* Widget, XEvent*, String*, Cardinal* */);

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
    /* actions			*/	NULL,
    /* num_actions		*/	0,
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

/* Sun cc complains about the following line, but I'll be damned if
   I know why. It's decld a static void above */

static void ClassInitialize()
{
  XtAddConverter( XtRString, XtRBackingStore, XmuCvtStringToBackingStore,
		 NULL, 0 );
}

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void DviPageAction(/* Widget, XEvent*, String*, Cardinal* */)
{
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

  if ( pai != 0 ) {
    CSetConversion(&(pi -> Conversion),
		   pi -> dpi,
		   pi -> userMag,
		   pai -> pai_Numerator,
		   pai -> pai_Denominator,
		   pai -> pai_DVIMag);
  }
  else {
    long Numerator = 25400000;
    long Denominator = 473628272;

    CSetConversion(&(pi -> Conversion),
		   pi -> dpi,
		   pi -> userMag,
		   Numerator,
		   Denominator,
		   1000);
  }
    
  w -> dviPage.fastFromSpValue
    = (long) ( w -> dviPage.Conversion.c_tosp );

  w -> dviPage.pixelsPerInch
    = cfromSP(&(pi -> Conversion), SP_PER_INCH( POINTS_PER_INCH ));

  ppi = w -> dviPage.pixelsPerInch;

  w -> dviPage.texPicScale = pi -> userMag * ppi;
  
  w -> dviPage.pixelsWide = ppi * w -> dviPage.paperWidth;
  w -> dviPage.pixelsHigh = ppi * w -> dviPage.paperHeight;

  (*wd) = w -> dviPage.pixelsWide;
  (*ht) = w -> dviPage.pixelsHigh;
}

static void Initialize (request, new)
    Widget request, new;
{
  DviPageWidget want = (DviPageWidget) request;
  DviPageWidget w = (DviPageWidget) new;

  XFontStruct *f;
  int dir, as, ds;
  XCharStruct os;

  Dimension wd, ht;

  w -> dviPage.page = want -> dviPage.page;
  w -> dviPage.pai  = want -> dviPage.pai ;
  w -> dviPage.xFonts  = want -> dviPage.xFonts ;
  w -> dviPage.dviFonts  = want -> dviPage.dviFonts ;

  w -> dviPage.paperWidth = atof(want -> dviPage.paperWidthString);
  w -> dviPage.paperHeight = atof(want -> dviPage.paperHeightString);

  w -> dviPage.topOffset = atof(want -> dviPage.topOffsetString);
  w -> dviPage.leftOffset = atof(want -> dviPage.leftOffsetString);

  w -> dviPage.globalGC  = 0;

  w -> dviPage.dviStack  = 0;
  w -> dviPage.dviStackSize  = 0;
  w -> dviPage.pathLen  = 0;
  w -> dviPage.penSize = 1;
  w -> dviPage.whiten = False;
  w -> dviPage.shade = False;
  w -> dviPage.blacken = False;

  SetPageSize(w, &wd, &ht);

#define MaX(x,y) ((x) > (y) ) ? (x) : (y)

  w -> core.width = MaX(wd, want->core.width);
  w -> core.height = MaX(ht, want->core.height);

#undef MaX

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

  w->dviPage.globalGC
    = XCreateGC( XtDisplay(w), XtWindow(w), 0, NULL);
}

static void Destroy (gw)
     Widget gw;
{
  DviPageWidget w = (DviPageWidget) gw;
  XtFree( w -> dviPage.dviStack );
  w -> dviPage.dviStack = 0;

  if ( w -> dviPage.globalGC ) {
    XFreeGC( XtDisplay(w), w -> dviPage.globalGC );
    w -> dviPage.globalGC = 0;
  }
}

static Boolean SetValues(current, request, new)
    DviPageWidget current, request, new;
{

  DviPagePart *pi = & (request -> dviPage);
  struct PostAmbleInfo *p = pi -> pai;

  new -> dviPage.page =
    (request -> dviPage.page)
      ? (request -> dviPage.page) : current -> dviPage.page ;

  new -> dviPage.xFonts = 
    (request -> dviPage.xFonts)
      ? (request -> dviPage.xFonts) : current -> dviPage.xFonts;;

  new -> dviPage.pai =
    (request -> dviPage.pai)
      ? (request -> dviPage.pai) : current -> dviPage.pai;

  if ( new -> dviPage.pai ) {

    int stackSize = new -> dviPage.pai -> pai_DVIStackSize;

    if (stackSize > new -> dviPage.dviStackSize ) {

      stackSize += 2;
      
      XtFree( new -> dviPage.dviStack );
      
      new -> dviPage.dviStack  = 0;
      new -> dviPage.dviStackSize = stackSize;
      
      if (stackSize > 0 ) {
	new -> dviPage.dviStack  = 
	  (LocalDviStack *) (malloc( stackSize * sizeof(LocalDviStack) ));
      }
    }
  }

  new -> dviPage.dviFonts =
    (request -> dviPage.dviFonts)
      ? (request -> dviPage.dviFonts) :  current -> dviPage.dviFonts;

  new -> dviPage.paperWidth = 
    (request -> dviPage.paperWidthString)
      ? atof(request -> dviPage.paperWidthString)
	: current -> dviPage.paperWidth;

  new -> dviPage.paperHeight = 
    (request -> dviPage.paperHeightString)
      ? atof(request -> dviPage.paperHeightString)
	: current -> dviPage.paperHeight;

  new -> dviPage.topOffset = 
    (request -> dviPage.topOffsetString)
      ? atof(request -> dviPage.topOffsetString)
	: current -> dviPage.topOffset;

  new -> dviPage.leftOffset = 
    (request -> dviPage.leftOffsetString)
      ? atof(request -> dviPage.leftOffsetString)
	: current -> dviPage.leftOffset;

  SetPageSize(new, &(new -> core.width), &(new -> core.height));

  return( True );	 /* resize */
}

static XtGeometryResult QueryGeometry(w, intended, preferred)
    Widget w;
    XtWidgetGeometry *intended, *preferred;
{
    register DviPageWidget lw = (DviPageWidget) w;
    Dimension wd, ht;

    SetPageSize((DviPageWidget) w, &wd, &ht);

    preferred -> request_mode = CWWidth | CWHeight;

    preferred -> width = wd ;
    preferred -> height =  ht ;

    if (  ((intended->request_mode & (CWWidth | CWHeight))
	   	== (CWWidth | CWHeight)) &&
	  intended->width == preferred->width &&
	  intended->height == preferred->height)
	return XtGeometryYes;
    else if (preferred->width == w->core.width &&
	     preferred->height == w->core.height)
	return XtGeometryNo;
    else
	return XtGeometryAlmost;
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

static void DviDisplay(widget)
     DviPageWidget widget;
{
  int c;
  i32 p;
  int advance;
  int wuz;
  int spDrift;

  DviPagePart *page = &( widget -> dviPage); 

  struct PostAmbleInfo *pai = page -> pai;

  char *thisPage = page -> page;
  Conv *conv = &(page -> Conversion);

  DviFontInfo *dviCurrentFont = 0;
  int dviCurrentFontIndex = 0;
  XFontStruct *currentFontStruct = widget -> dviPage.font;

  /* this is the maximum width over all glyphs in the current font;
     it's used to increase the accuracy of glyph placement, compensating
     for round-off due to font shrinking. */

  int maxWidthThisFont = 0;

  /* following are used to reduce the number of XDrawStrings we do */

#define MAX_DVI_STRING 64
#ifdef ASCIIJTEX /* kagotani */
#ifdef FAKEKANJI
  int offHH;
#endif /* FAKEKANJI */
  XChar2b dviString[ MAX_DVI_STRING ];
#else /* !ASCIIJTEX */
  char dviString[ MAX_DVI_STRING ];
#endif /* !ASCIIJTEX */
  int dviStringLength;
  int startHH, startVV;
  int endHH, endVV;

  LocalDviStack *dviStack = page -> dviStack;
  LocalDviStack *dviStackP;	/* current stack item */

  if ( pai == 0 || thisPage == 0 || dviStack == 0) {
    return;
  }

  assert( dviStack != 0 );

  dviStackP = dviStack;

  dviStringLength = 0;
  startHH = startVV = -1;
  endHH = endVV = -1;

  /* insure that first byte is a BOP mark */

  c = (*thisPage) & 0xff;
  if ( DVI_DT(c) != DT_BOP ) {
    error(0,0,"No BOP found");
    goto cleanup;
  }

  /* initialize spDrift to be a single pixel width */
  spDrift = ctoSP( conv,
		  ((float) (widget -> dviPage.maxDrift * POINTS_PER_INCH))
		  / DviDpi );

  for(;;) {
    
    c = mGetByte(&thisPage);
    
    if (DVI_IsChar(c)) {

      advance = 1;

    doDviChar:
      if ( !advance
	  || ABS(endHH - (dviStackP -> h)) > spDrift
	  || ABS(endVV - (dviStackP -> v)) > spDrift
	  || dviStringLength >= MAX_DVI_STRING )
	{

	  if ( dviStringLength > 0 ) {
	    i32 atHH = fastFromSP(widget, startHH);
	    i32 atVV = fastFromSP(widget, startVV);
	    Bool doit = True;

	    if ( widget -> dviPage.haveBackingStore ) {
	      i32 width = fastFromSP(widget, endHH);
	      i32 height = fastFromSP(widget, endVV);
	      
	      doit =
		XRectInRegion( widget -> dviPage.updateRegion, atHH, atVV,
			      width, height) != RectangleOut;
	    }

	    if (doit) {
#ifdef ASCIIJTEX /* kagotani */
	      XDrawString16( XtDisplay( widget ), XtWindow( widget ),
#ifdef FAKEKANJI
			  page -> globalGC, atHH - offHH, atVV,
#else /* !FAKEKANJI */
			  page -> globalGC, atHH, atVV,
#endif /* !FAKEKANJI */
			  dviString, dviStringLength );
#else /* !ASCIIJTEX */
	      XDrawString( XtDisplay( widget ), XtWindow( widget ),
			  page -> globalGC, atHH, atVV,
			  dviString, dviStringLength );
#endif /* !ASCIIJTEX */
	    }
	  }
	  dviStringLength = 0;
	  startHH = dviStackP -> h;
	  startVV = dviStackP -> v ;
	  endHH = startHH;
	}

#ifdef ASCIIJTEX /* kagotani */
#ifdef FAKEKANJI
      if (dviStringLength == 0 && dviCurrentFont->glues) {
	offHH = fastFromSP(widget, dviCurrentFont -> glues[ codetoinfo(dviCurrentFont->nt, dviCurrentFont->ct, c) ]);
      } else {
	offHH = 0;
      }
#endif FAKEKANJI
      dviString[ dviStringLength ].byte1 = (unsigned)c / 0x100; 
      dviString[ dviStringLength++ ].byte2 = (unsigned)c % 0x100; 
#else /* !ASCIIJTEX */
      dviString[ dviStringLength++ ] = c; 
#endif /* !ASCIIJTEX */

      if ( advance ) {
	if (dviCurrentFont && dviCurrentFont -> widths) { 
#ifdef ASCIIJTEX /* kagotani */
	  dviStackP -> h += (dviCurrentFont -> widths[ codetoinfo(dviCurrentFont->nt, dviCurrentFont->ct, c) ]) ;
#else /* !ASCIIJTEX */
	  dviStackP -> h += (dviCurrentFont -> widths[ c ]) ;
#endif /* !ASCIIJTEX */
	}

/*
 *	The following code makes use of the FontStruct information;
 *	each time we cache a glyph, we update the stack to point to
 *	the location where we should be. endHH, on the other hand,
 *	is the *actual* location, based on the width of the displayed
 *	glyph.
 */
	{
#ifdef ASCIIJTEX /* kagotani */
	  register unsigned char b1 = c / 0x100;
	  register unsigned char b2 = c % 0x100;
	  register unsigned min_b1 = currentFontStruct -> min_byte1;
	  register unsigned max_b1 = currentFontStruct -> max_byte1;
	  register unsigned min_cb2 = currentFontStruct -> min_char_or_byte2;
	  register unsigned max_cb2 = currentFontStruct -> max_char_or_byte2;
	  int offset;
	  XCharStruct *cs;
	  int thisWidth;
	  if ( b1 < min_b1 || b1 > max_b1 || b2 < min_cb2 || b2 > max_cb2 ) {
		b1 = currentFontStruct -> default_char / 0x100;
		b2 = currentFontStruct -> default_char % 0x100;
	  }
	  offset = (b1 - min_b1) * (max_cb2 - min_cb2 + 1) + b2 - min_cb2;
	  cs = currentFontStruct -> per_char != 0
				? &(currentFontStruct -> per_char[offset])
				: &(currentFontStruct -> min_bounds);
	  thisWidth = cs -> width;
#else /* !ASCIIJTEX */
	  int offset = c - (currentFontStruct -> min_char_or_byte2);
#ifdef BUGGY /* kagotani */
	  XCharStruct *cs = &(currentFontStruct -> per_char[c]);
#else /* !BUGGY */
	  XCharStruct *cs = currentFontStruct -> per_char != 0
				? &(currentFontStruct -> per_char[offset])
				: &(currentFontStruct -> min_bounds);
#endif /* !BUGGY */
	  int thisWidth = cs -> width;
#endif /* !ASCIIJTEX */
	  endHH += ctoSP(conv, thisWidth);
	}
      }
      else {
	endHH = dviStackP -> h;
      }
      endVV = dviStackP -> v;

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
	goto doDviChar;
	break;
	
      case DT_PUT:
	c = p;
	advance = 0;
	goto doDviChar;
	break;;
	
      case DT_SETRULE:
      case DT_PUTRULE:
	
	{
	  i32 cHH, cVV;
	  i32 rhh, rvv;

	  cHH = fastFromSP(widget, dviStackP -> h);
	  cVV = fastFromSP(widget, dviStackP -> v);
	  
	  mGetLong(&thisPage, &rvv);
	  mGetLong(&thisPage, &rhh);

	  if (DVI_DT(c) == DT_SETRULE) {
	    dviStackP -> h += rhh;
	  }
	  
	  rhh = CConvRule(conv, rhh);
	  rvv = CConvRule(conv, rvv);

	  XFillRectangle(XtDisplay(widget), XtWindow(widget),
			 page -> globalGC, cHH, cVV - rvv, rhh, rvv);

	}
	break;
	
      case DT_NOP:
	break;
	
      case DT_BOP:
	/*
	 *	Each beginning of page has 11 4-byte words telling us things
	 *	about the page. We ignore them.
	 */
	thisPage += (11 * 4);

	dviStackP = dviStack;

	dviStackP -> h = SP_PER_INCH( POINTS_PER_INCH * page -> leftOffset);
	dviStackP -> v = SP_PER_INCH( POINTS_PER_INCH * page -> topOffset);
	startHH = dviStackP -> h;
	startVV = dviStackP -> v ;
	endHH = startHH;
	endVV = startVV;
	dviStackP -> w = 0;
	dviStackP -> x = 0;
	dviStackP -> y = 0;
	dviStackP -> z = 0;

	break;
	
      case DT_EOP:
	goto cleanup;
	/* NOTREACHED */
	
      case DT_PUSH:
	{
	  /* copy the old stack to the new one */

	  LocalDviStack *old = dviStackP;
	  dviStackP++;
	  *dviStackP = *old;
	}
	break;
	
      case DT_POP:
	/* just use the old stack */
	dviStackP--;
	break;
	
      case DT_W0:	/* there should be a way to make these pretty */
	p = dviStackP -> w;
	goto move_right;
	
      case DT_W:
	dviStackP -> w = p;
	goto move_right;
	
      case DT_X0:
	p = dviStackP -> x;
	goto move_right;
	
      case DT_X:
	dviStackP -> x = p;
	goto move_right;
	
      case DT_RIGHT:
      move_right:
	dviStackP -> h += p;
	break;
	
      case DT_Y0:
	p = dviStackP -> y;
	goto move_down;
	
      case DT_Y:
	dviStackP -> y = p;
	goto move_down;
	
      case DT_Z0:
	p = dviStackP -> z;
	goto move_down;
	
      case DT_Z:
	dviStackP -> z = p;
	goto move_down;
	
      case DT_DOWN:
      move_down:
	dviStackP -> v += p;
	break;
	
      case DT_FNTNUM:
      case DT_FNT:
	{
	  int font;

	  /* flush existing text */

	  if ( dviStringLength > 0 ) {
	    i32 atHH = fastFromSP(widget, startHH);
	    i32 atVV = fastFromSP(widget, startVV);
	    Bool doit = True;

	    if ( widget -> dviPage.haveBackingStore ) {
	      i32 width = fastFromSP(widget, endHH);
	      i32 height = fastFromSP(widget, endVV);
	      
	      doit =
		XRectInRegion( widget -> dviPage.updateRegion, atHH, atVV,
			      width, height) != RectangleOut;
	    }

	    if (doit) {
#ifdef ASCIIJTEX /* kagotani */
	      XDrawString16( XtDisplay( widget ), XtWindow( widget ),
#ifdef FAKEKANJI
			  page -> globalGC, atHH - offHH, atVV,
#else /* !FAKEKANJI */
			  page -> globalGC, atHH, atVV,
#endif /* !FAKEKANJI */
			  dviString, dviStringLength );
#else /* !ASCIIJTEX */
	      XDrawString( XtDisplay( widget ), XtWindow( widget ),
			  page -> globalGC, atHH, atVV,
			  dviString, dviStringLength );
#endif /* !ASCIIJTEX */
	    }
	    dviStringLength = 0;
	    startHH = dviStackP -> h;
	    startVV = dviStackP -> v ;
	    endHH = startHH;
	  }

	  /* switch to new font */

	  font = ( DVI_DT(c) == DT_FNTNUM )
	    ? font = (c - DVI_FNTNUM0) : p;

	  if ( selectFont(widget,
			  &dviCurrentFont, &dviCurrentFontIndex, font) ) {
	    goto cleanup;
	  }
	  else {
	    /* load the font */
	    int useFont =
	      (widget -> dviPage.xFonts
	       && widget -> dviPage.xFonts[ dviCurrentFontIndex ]);

	    Font font ;

	    currentFontStruct = 
	      (useFont)
		? (widget->dviPage.xFonts[ dviCurrentFontIndex ])
		: widget -> dviPage.font ;

	    font = currentFontStruct -> fid;
		
	    XSetFont( XtDisplay(widget), widget -> dviPage.globalGC, font );

	    maxWidthThisFont = currentFontStruct -> max_bounds.width;
	    maxWidthThisFont = ctoSP(conv, maxWidthThisFont);
	  }
	}
	break;
	
      case DT_XXX:
	{
	  char specialBuffer [2048];
	  register char *cp;
	  int sweetp = 0;
	  
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

	  widget -> dviPage.dviStackPointer = dviStackP;
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
	  i  = UnSign8(mGetByte(&thisPage));
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

  if ( dviStringLength > 0 ) {
    i32 atHH = fastFromSP(widget, startHH);
    i32 atVV = fastFromSP(widget, startVV);
    Bool doit = True;
    
    if ( widget -> dviPage.haveBackingStore ) {
      i32 width = fastFromSP(widget, endHH);
      i32 height = fastFromSP(widget, endVV);
      
      doit =
	XRectInRegion( widget -> dviPage.updateRegion, atHH, atVV,
		      width, height) != RectangleOut;
    }
    
    if (doit) {
#ifdef ASCIIJTEX /* kagotani */
      XDrawString16( XtDisplay( widget ), XtWindow( widget ),
#ifdef FAKEKANJI
		  page -> globalGC, atHH - offHH, atVV,
#else /* !FAKEKANJI */
		  page -> globalGC, atHH, atVV,
#endif /* !FAKEKANJI */
		  dviString, dviStringLength );
#else /* !ASCIIJTEX */
      XDrawString( XtDisplay( widget ), XtWindow( widget ),
		  page -> globalGC, atHH, atVV,
		  dviString, dviStringLength );
#endif /* !ASCIIJTEX */
    }
    dviStringLength = 0;
  }

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

    XGCValues	values;
    unsigned long fore = w->dviPage.foreground;
    unsigned long aft = w->dviPage.background;
    
    if ( w -> dviPage.reverseVideo ) {
      unsigned long tmp = aft;
      aft = fore;
      fore = tmp;
    }
    
    /* Prepare to clear the main screen */
    
    values.function	= GXcopy;
    values.foreground	= aft;
    values.background	= fore;
    
    XChangeGC( XtDisplay(gw), w -> dviPage.globalGC,
	      (unsigned) GCFunction | GCForeground | GCBackground,
	      &values);

    XSetRegion( XtDisplay(w), w -> dviPage.globalGC, region );

    XFillRectangle(XtDisplay(w), XtWindow(w),
		   w -> dviPage.globalGC,
		   0, 0,
		   w -> core.width, w -> core.height);

    /* Prepare to paint */
    
    values.foreground	= fore;
    values.background	= aft;
    values.plane_mask	= AllPlanes;
    values.font		= w->dviPage.font->fid;
    
    if ( aft ) {
      values.function = GXand;
    }
    else {
      values.function = GXor;
    }
    
    XChangeGC( XtDisplay(gw), w -> dviPage.globalGC,
	      (unsigned) GCFunction | GCPlaneMask |
	      GCForeground | GCBackground | GCFont,
	      &values);

    XSetRegion( XtDisplay(w), w -> dviPage.globalGC, region );

    if (w -> dviPage.pai && w -> dviPage.page && w -> dviPage.dviFonts ) {

      w -> dviPage.updateRegion = region;

      DviDisplay(w);

    }
  }
}
