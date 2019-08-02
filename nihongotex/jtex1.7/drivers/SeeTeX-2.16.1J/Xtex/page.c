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

/*
 *
 *	This looks kind of strange because, initially, I had intended to
 *	allow you to `put away' a page without destroying it -- but
 *	I've decided not to do that.
 *
 */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Xatom.h>
#include <X11/Object.h>
#include <X11/ObjectP.h>

#include <assert.h>

#include "dvi-simple.h"

#include "xtex.h"

#include "DviPage.h"
#include "page.h"
#include "x11fonts.h"
#include "widgets.h"

#include "Valuator.h"
#include "EzMenu.h"

extern Widget TopLevelWidget;

#ifdef __STDC__
void TeXPageRealize(TeXPage *, int);
static void showPage(TeXPage *, int);
static void changeMag(TeXPage *, int );
#else
void TeXPageRealize();
static void showPage();
static void changeMag();
#endif

int TeXPages;
TeXPage *TheTeXPages[ MAX_TEX_PAGES ];

typedef struct {
  char *thePage;
  int refs;
} DviPageRef;

DviPageRef *ThePageRefs = 0;

#define BUTTONS (xtexResources.useButtons)


/*
 * free all items in the page stack of a given TeXPage
 */

void
TeXPageStackFree(tp)
TeXPage *tp;
{
  while ( tp -> pageStack ) {
    TeXPageStack *p = tp -> pageStack;
    tp -> pageStack = p -> next;
    XtFree( p );
  }
}

void
  TeXPageCloseFile()
{
  int popup;
  
  /* dispose of existing information */
  
  for ( popup = 0; popup < TeXPages; popup++ ) {
    TeXPage *tp = TheTeXPages[popup];
    if ( tp && tp -> popUpPage) {
      TeXPageDeref( tp );
      TeXFontUnref( tp -> userMag);
      TeXPageStackFree( tp );
      tp -> pageData = 0;
    }
  }
  
  if ( ThePageRefs ) {
    int i;
    for (i = 0; i < DviTotalPages; i++ ) {
      XtFree( ThePageRefs[i].thePage );
      ThePageRefs[ i ].thePage = 0;
    }
  }
  
  XtFree( ThePageRefs );
  
  ThePageRefs = 0;
}

void
  TeXPageSlamPages()
{
  int popup;
  
  /* take down pages, dispose of existing information */
  
  for ( popup = 0; popup < TeXPages; popup++ ) {
    if ( TheTeXPages[popup] && TheTeXPages[ popup ] -> popUpPage) {
      XtDestroyWidget( TheTeXPages[ popup ] -> popUpPage ); 
    }
  }
}

/*
 *	find the logial page `want'. If we want the next logical page
 *	with that number, set wantNext to True, else False (you can
 *	have multiple logical pages with the same number).
 */
static void findLogicalPage(wantNext, tp, want)
     Bool wantNext;
     TeXPage *tp;
     int want;
{
  int here = tp -> pageNumber;
  int page = here;
  
  if ( here < 0 || here >= DviTotalPages ) {
    here = 0;
  }
  
  if (wantNext) {
    page++;
    if ( page >= DviTotalPages ) {
      page = 0;
    }
  }
  
  for(;;) {
    if ( DviCount[0][page] == want ) {
      tp -> pageNumber = page;
      tp -> logicalPageNumber = want;
      return;
    }
    page++;
    if ( page >= DviTotalPages ) {
      page = 0;
    }
    if ( page == here ) 
      break;
  }
  if ( here < 0 || here >= DviTotalPages ) {
    here = 0;
  }
  tp -> pageNumber = here;
  tp -> logicalPageNumber = DviCount[0][here];
}

void
  TeXPageOpenFile()
{
  /* now rebuild it all */
  
  if ( DviFile != 0 ) {
    int i;
    int popup;
    
    ThePageRefs =
      (DviPageRef *)  XtMalloc( (DviTotalPages+1) * sizeof(DviPageRef) );
    
    for (i = 0; i < DviTotalPages; i++ ) {
      ThePageRefs[i].thePage = 0;
      ThePageRefs[i].refs = 0;
    }
    
    /* Scan existing pages, orient new pages based on the count0 field
       of the old page. This field is cached in logicalPageNumber */
    
    for ( popup = 0; popup < TeXPages; popup++ ) {
      
      TeXPage *tp = TheTeXPages[ popup ];
      
      if ( tp && tp -> popUpPage) {
	int pno;
	findLogicalPage(False, tp, tp -> logicalPageNumber);
	pno = tp -> pageNumber;
	
	if (pno >= 0 && pno < DviTotalPages) {
	  Arg argList[20];
	  int args = 0;
	  
	  TeXPageRef(tp, tp -> pageNumber);
	  
	  XtSetArg(argList[args], XtNredisplay, True); args++;
	  XtSetArg(argList[args], XtNpai, &ThePostAmbleInfo); args++;
	  XtSetArg(argList[args], XtNpage, tp -> pageData); args++;
	  XtSetArg(argList[args], XtNdviFonts, TheFontInfo); args++;
	  XtSetArg(argList[args], XtNxFonts,
		   TeXFontRef(tp -> userMag)); args++;
	  
	  XtSetValues( tp -> singleDviPage, argList, args);
	  
	  showPage( tp, False );
	}
	else {
	  XtDestroyWidget( tp -> popUpPage ); 
	}
      }
    }
  }
}

/*
 *	Reference and dereference a page structure -- these keep track
 *	of the memory allocated for page buffers & disposes of them
 *	when not in use.
 */

static void
  TeXPageRef(tp, pageno)
TeXPage *tp;
int pageno;
{
  assert( pageno >= 0 && pageno < DviTotalPages );
  
  ThePageRefs[ pageno ].refs++;
  if ( ThePageRefs[ pageno ].thePage == 0 ) {
    ThePageRefs[ pageno ].thePage = DviFetchPage( pageno );
  }
  
  tp -> pageNumber = pageno;
  tp -> logicalPageNumber = DviCount[0][ pageno ];
  tp -> pageData = ThePageRefs[ pageno ].thePage;
}

void
  TeXPageDeref(tp)
TeXPage *tp;
{
  int pageno = tp -> pageNumber;
  
  assert( pageno >= 0 && pageno < DviTotalPages );
  if ( ThePageRefs && ThePageRefs[ pageno ].refs > 0) {
    ThePageRefs[ pageno ].refs--;
    if ( ThePageRefs[ pageno ].refs <= 0 ) {
      XtFree( ThePageRefs[ pageno ].thePage );
      ThePageRefs[ pageno ].thePage = 0;
    }
  }
}


/*
 *	Locate page containing widget *w*
 */
#ifdef __STDC__
TeXPage *
  TeXPageLocate(Widget w)
#else
TeXPage *
  TeXPageLocate(w) Widget w;
#endif
{
  if ( w ) {
    
#define IS(x) (w == tp -> x)
    int i;
    Widget parent;
    
    for (i = 0; i < TeXPages; i++ ) {
      TeXPage *tp = TheTeXPages[i];
      if ( IS(singleDviPage)
	  || IS(popUpPage)
	  || IS(dviPageBox)
	  || IS( dviPageMenu )
	  || IS(viewPort)
	  || IS(buttonBox)
	  || IS(forewardButton)
	  || IS(backwardButton)
	  || IS(markButton)
	  || IS(dupButton)
	  || IS(dupButton)
	  || IS(tieButton)
	  || IS(largeButton)
	  || IS(smallButton)
	  || IS(quitButton)
	  || IS(pageNumberText)
	  || IS(gotoButton)
	  || IS(reopenButton) ) {
	return( tp );
      }
#undef IS
    }
    
    /* didn't find it -- attempt to find the parent and try again */
    
    parent = 0;
    if ( XtIsWidget(w) )  {
      parent = XtParent(w);
    }
    else if ( XtIsObject(w) ) {
      Object o = (Object) w;
      parent = o -> object.parent;
    }
    
    if ( parent ) {
      return( TeXPageLocate( parent ) );
    }
  }
  
  return( 0 );
}

/*
 *	Locate a free TeXPage structure. See comment at head.
 */
TeXPage *
  TeXPageLocateFree()
{
  int i;
  TeXPage *tp;
  
  for (i = 0; i < TeXPages; i++ ) {
    if ( ! TheTeXPages[ i ] -> active ) {
      return( TheTeXPages[ i ] );
    }
  }
  
  tp = (TeXPage *) XtMalloc(sizeof(TeXPage));
  
  bzero( tp, sizeof(TeXPage) );	/* happily, sets all to default */
  
  /* install the page in the page list */
  
  if ( TeXPages > MAX_TEX_PAGES ) {
    error(0,0,"No more pages available");
    return(0);
  }
  
  TheTeXPages[ TeXPages++ ] = tp;
  
  return( tp );
}

/*
 *	The callback when pages die.
 */

static void
  dviPageDied(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  TeXPage *tp = (TeXPage*) clientData;
  
  if ( tp != 0 ){
    tp -> active = False;
    tp -> realized = False;
    TeXPageDeref( tp );
    TeXFontUnref( tp -> userMag);
    TeXPageStackFree( tp );
    tp -> popUpPage = 0;
    
    if ( tp -> tiedTo != 0 && tp -> tiedTo -> active ) {
      tp -> tiedTo -> tied = 0;
    }
    
    if ( tp -> tied != 0 && tp -> tied -> active ) {
      tp -> tied -> tiedTo = 0;
    }
    
    tp -> tiedTo = 0;
    tp -> tied = 0;
    
    tp -> inputNumberValid = False;
    
  }
}

/* if accelerators haven't been installed yet, so so */

static void enableScrollBars(tp)
     TeXPage *tp;
{
  if ( ! tp -> horizBar ) {
    tp -> horizBar = XtNameToWidget(tp -> viewPort, "horizontal");
    if ( tp -> horizBar ) {
      XtInstallAccelerators(tp -> singleDviPage, tp -> horizBar);
    }
  }
  
  if ( ! tp -> vertBar ) {
    tp -> vertBar = XtNameToWidget(tp -> viewPort, "vertical");
    if ( tp -> vertBar ) {
      XtInstallAccelerators(tp -> singleDviPage, tp -> vertBar);
    }
  }
}

/*
 *	Set all the buttons for this page, possibly also
 *	sending the page information
 */

static   Arg sensArgs[] = {
  {XtNsensitive, (XtArgVal) True}
};

static   Arg insensArgs[] = {
  {XtNsensitive, (XtArgVal) False}
};

static void showPage(tp, displayPage)
     TeXPage *tp;
     Bool displayPage;
{
  Arg argList[20];
  Cardinal args;
  
  Pixel fore;
  Pixel back;
  
  if ( displayPage ) {
    
#if 0
    args = 0;
    XtSetArg(argList[args], XtNresizable, False); args++;
    XtSetValues( tp -> viewPort, argList, args);
#endif
    
    TeXPageRef(tp,  tp -> pageNumber );
    args = 0;
    XtSetArg(argList[args], XtNpage, tp -> pageData); args++;
    XtSetArg(argList[args], XtNredisplay, True); args++;
    XtSetValues(tp -> singleDviPage, argList, args);
    
#if 0
    args = 0;
    XtSetArg(argList[args], XtNresizable, True); args++;
    XtSetValues( tp -> viewPort, argList, args);
#endif
    
  }
  
  if ( xtexResources.updateNames ) {
    char spbuffer[1024];
    
    /* physical pages are zero based internally, one based externally */
    
    sprintf(spbuffer,"%s (%s): Logical Page %d, Physical Page %d",
	    ProgName, DVIFileName,
	    DviCount[0][tp -> pageNumber],
	    tp -> pageNumber + 1);
    
    XChangeProperty(XtDisplay(tp -> popUpPage),
		    XtWindow(tp -> popUpPage),
		    XA_WM_NAME, XA_STRING, 8, PropModeReplace,
		    (unsigned char*) spbuffer, strlen(spbuffer));
    
    sprintf(spbuffer,"%s (%s, page %d)",
	    ProgName, DVIFileName, DviCount[0][tp -> pageNumber]);
    
    XChangeProperty(XtDisplay(tp -> popUpPage),
		    XtWindow(tp -> popUpPage),
		    XA_WM_ICON_NAME, XA_STRING, 8, PropModeReplace,
		    (unsigned char*) spbuffer, strlen(spbuffer));
  }
  
  if ( BUTTONS ) {
    if ( tp -> pageNumber == 0 ) {
      XtSetValues( tp -> backwardButton, insensArgs, XtNumber(insensArgs));
    }
    else {
      XtSetValues( tp -> backwardButton, sensArgs, XtNumber(sensArgs));
    }
    
    if ( tp -> pageNumber >= (DviTotalPages-1)) {
      XtSetValues( tp -> forewardButton, insensArgs, XtNumber(insensArgs));
    }
    else {
      XtSetValues( tp -> forewardButton, sensArgs, XtNumber(sensArgs));
    }
    
    /* Take normal values from another button */
    
    args = 0;
    XtSetArg(argList[args], XtNforeground, &fore); args++;
    XtSetArg(argList[args], XtNbackground, &back); args++;
    XtGetValues( tp -> forewardButton, argList, args);
    
    if (TeXMarkState( tp -> pageNumber ) ) {
      Arg argList[20];
      Cardinal args;
      
      args = 0;
      XtSetArg(argList[args], XtNforeground, back); args++;
      XtSetArg(argList[args], XtNbackground, fore); args++;
      
      XtSetValues( tp -> markButton, argList, args );
    }
    else {
      Arg argList[20];
      Cardinal args;
      
      args = 0;
      XtSetArg(argList[args], XtNforeground, fore); args++;
      XtSetArg(argList[args], XtNbackground, back); args++;
      
      XtSetValues( tp -> markButton, argList, args );
    }
  }
  
  enableScrollBars(tp);
}

static void changeMag( tp, mag )
     TeXPage *tp;
     int mag;
{
  Arg argList[20];
  Cardinal args;
  
  XFontStruct **x;
  Dimension maxHt;
  Dimension maxWd;
  Dimension dviHt;
  Dimension dviWd;
  Dimension dviBWd;
  Dimension vpWd;
  
  maxWd = xtexResources.pageMaxWd;
  maxHt = xtexResources.pageMaxHt;
  if (maxWd < 1) {
    maxWd = (int) ((WidthOfScreen(XtScreen(TopLevelWidget))) * 0.95);
  }
  if (maxHt < 1) {
    maxHt = (int) ((HeightOfScreen(XtScreen(TopLevelWidget))) * 0.90);
  }
  
  /* allow this to be called simply to auto-resize the window */
  
  if ( tp -> userMag != mag ) {
    TeXFontUnref( tp -> userMag);
    tp -> userMag = mag; x = TeXFontRef(tp -> userMag);
    args = 0;
    XtSetArg(argList[args], XtNuserMag, tp -> userMag); args++;
    XtSetArg(argList[args], XtNxFonts, x); args++;
    XtSetValues( tp -> singleDviPage, argList, args);
  }
  
  /* get the size of the dvi page */
  
  if ( xtexResources.autoSize ) {
    ViewportWidget vp = (ViewportWidget) (tp -> viewPort);
    
    if ( ! tp -> horizBar ) {
      tp -> horizBar = XtNameToWidget(vp, "horizontal");
      if ( tp -> horizBar ) {
	XtInstallAccelerators(tp -> singleDviPage, tp -> horizBar);
      }
    }
    
    if ( ! tp -> vertBar ) {
      tp -> vertBar = XtNameToWidget(vp, "vertical");
      if (tp -> vertBar) {
	XtInstallAccelerators(tp -> singleDviPage, tp -> vertBar);
      }
    }
    
    args = 0;
    XtSetArg(argList[args], XtNwidth, &dviWd); args++;
    XtSetArg(argList[args], XtNborderWidth, &dviBWd); args++;
    XtSetArg(argList[args], XtNheight, &dviHt); args++;
    XtGetValues( tp -> singleDviPage, argList, args);
    
    dviWd += (2 * dviBWd);
    
    if ( dviWd <= maxWd ){
      maxWd = dviWd;
      
      /* will we need scrollbars? */
      
      if ( dviWd <= maxWd ){
	Widget vb = tp -> vertBar;
	maxWd = dviWd;
	if ( vb && dviHt > maxHt ) {
	  maxWd = maxWd + vb -> core.width + vb -> core.border_width;
	}
      }
      if ( dviHt <= maxHt ) {
	Widget hb = tp -> horizBar;
	maxHt = dviHt;
	if ( hb && dviWd > maxWd ) {
	  maxHt = maxHt + hb -> core.width + hb -> core.border_width;
	}
      }
    }
    
    args = 0;
    XtSetArg(argList[args], XtNwidth, maxWd); args++;
    XtSetArg(argList[args], XtNheight, maxHt); args++;
    XtSetValues( tp -> viewPort, argList, args);
  }
}

/**************** NUMBER FETCHING ****************/
static int texxPageNumber(tp, def)
     TeXPage *tp;
     int def;
{
  int value;
  if ( tp  && tp -> inputNumberValid ) {
    
    if ( tp -> inputNumberMinus ) {
      value = -( tp -> inputNumber);
    }
    else {
      value = ( tp -> inputNumber );
    }
  }
  else {
    value = def;
  }
  return(value);
}

static void texxPageNumberReset(tp)
     TeXPage *tp;
{
  if ( tp ) {
    tp -> inputNumberValid = False;
    tp -> inputNumberMinus = False;
    tp -> inputNumber = 0;
  }
}

/*
 *	Realize the TeXPage structure and pop up the corresponding window
 */
void TeXPageRealize(tp, pageNumber)
     TeXPage *tp;
     int pageNumber;
{
  Arg argList[40];
  int args;
  
  Dimension maxHt;
  Dimension maxWd;
  Dimension dviHt;
  Dimension dviWd;
  extern pushReopenButton();	/* from file */
  Widget scrollBar;
  
  bzero(tp, sizeof(TeXPage));
  
  maxWd = xtexResources.pageMaxWd;
  maxHt = xtexResources.pageMaxHt;
  if (maxWd < 1) {
    maxWd = (int) ((WidthOfScreen(XtScreen(TopLevelWidget))) * 0.95);
  }
  if (maxHt < 1) {
    maxHt = (int) ((HeightOfScreen(XtScreen(TopLevelWidget))) * 0.90);
  }
  
  if ( tp == NULL ) {
    return;
  }
  
  assert(TopLevelWidget != 0);
  
  args = 0;
  tp -> popUpPage = 
    XtCreatePopupShell("xtex-page",
		       applicationShellWidgetClass, TopLevelWidget,
		       argList,args);
  
  args = 0;
  tp -> dviPageBox =
    XtCreateManagedWidget("form",
			  formWidgetClass, tp -> popUpPage,
			  argList, args);
  
  if ( BUTTONS ) {
    args = 0;
    XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
    tp -> quitButton =
      XtCreateManagedWidget("quitButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    
    args = 0;
    XtSetArg(argList[args], XtNfromHoriz, tp -> quitButton); args++;
    tp -> forewardButton =
      XtCreateManagedWidget("forwardButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    args = 0;
    XtSetArg(argList[args], XtNfromHoriz, tp -> forewardButton); args++;
    tp -> backwardButton =
      XtCreateManagedWidget("backwardButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    args = 0;
    XtSetArg(argList[args], XtNfromHoriz, tp -> backwardButton); args++;
    tp -> markButton =
      XtCreateManagedWidget("markButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    
    args = 0;
    XtSetArg(argList[args], XtNfromHoriz, tp -> markButton); args++;
    tp -> dupButton =
      XtCreateManagedWidget("dupButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    
    args = 0;
    XtSetArg(argList[args], XtNfromHoriz, tp -> dupButton); args++;
    tp -> tieButton =
      XtCreateManagedWidget("tieButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    
    args = 0;
    XtSetArg(argList[args], XtNfromHoriz, tp -> tieButton); args++;
    tp -> largeButton =
      XtCreateManagedWidget("largeButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    
    args = 0;
    XtSetArg(argList[args], XtNfromHoriz, tp -> largeButton); args++;
    tp -> smallButton =
      XtCreateManagedWidget("smallButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    args = 0;
    XtSetArg(argList[args], XtNfromHoriz, tp -> smallButton); args++;
    tp -> reopenButton = 
      XtCreateManagedWidget("reopenButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    args = 0;
    XtSetArg(argList[args], XtNfromHoriz, tp -> reopenButton); args++;
    XtSetArg(argList[args], XtNeditType, XawtextEdit); args++;
    XtSetArg(argList[args], XtNstring, tp -> pageNumberBuffer); args++;
    XtSetArg(argList[args], XtNlength, MAXPAGENUMBERBUFFER); args++;
    tp -> pageNumberText =
      XtCreateManagedWidget("numberText",
			    asciiTextWidgetClass, tp -> dviPageBox,
			    argList, args);
    
    args = 0;
    XtSetArg(argList[args], XtNfromHoriz, tp -> pageNumberText); args++;
    tp -> gotoButton = 
      XtCreateManagedWidget("gotoButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
  }
  
  args = 0;
  if (BUTTONS) {
    XtSetArg(argList[args], XtNfromVert, tp -> pageNumberText); args++;
  }
  tp -> viewPort = 
    XtCreateWidget("viewPort",
		   viewportWidgetClass, tp -> dviPageBox,
		   argList, args);
  
  bzero(tp -> pageNumberBuffer, MAXPAGENUMBERBUFFER);
  tp -> realized = True;
  TeXPageRef(tp, pageNumber);
  tp -> userMag = xtexResources.userMag;
  tp -> active = True;
  
  args = 0;
  XtSetArg(argList[args], XtNpai, &ThePostAmbleInfo); args++;
  
  XtSetArg(argList[args], XtNdviFonts, TheFontInfo); args++;
  
  {
    IntFloatUnion x;

    x.f = xtexResources.dpiHoriz;
    XtSetArg(argList[args], XtNdpiHoriz, x.i); args++;

    x.f = xtexResources.dpiVert;
    XtSetArg(argList[args], XtNdpiVert, x.i); args++;

    x.f = xtexResources.paperWidth;
    XtSetArg(argList[args], XtNpaperWidth, x.i); args++;

    x.f = xtexResources.paperHeight;
    XtSetArg(argList[args], XtNpaperHeight, x.i); args++;

    x.f = xtexResources.topOffset;
    XtSetArg(argList[args], XtNtopOffset, x.i); args++;

    x.f = xtexResources.leftOffset;
    XtSetArg(argList[args], XtNleftOffset, x.i); args++;
  }
  
  XtSetArg(argList[args], XtNpage, tp -> pageData); args++;
  XtSetArg(argList[args], XtNuserMag, tp -> userMag); args++;
  XtSetArg(argList[args], XtNxFonts, TeXFontRef(tp -> userMag)); args++;
  
  tp -> singleDviPage = 
    XtCreateWidget("dviPage",
		   dviPageWidgetClass, tp -> viewPort,
		   argList, args);
  
  XtAddCallback( tp -> singleDviPage, XtNdestroyCallback, dviPageDied, tp);
  
  tp -> dviPageMenu = XtCreatePopupShell("menu",
					 ezMenuWidgetClass, tp -> singleDviPage,
					 NULL, 0);
  
  /* get the size of the dvi page. This is an attempt to provide
     a nice default window size */
  
  /* sadly, this appears to be impossible to keep unless we redisplay
     several times each time, so lets just blow it off */
  
#if 0
  args = 0;
  XtSetArg(argList[args], XtNwidth, &dviWd); args++;
  XtSetArg(argList[args], XtNheight, &dviHt); args++;
  XtGetValues( tp -> singleDviPage, argList, args);
  
  scrollBar = XtNameToWidget( tp -> viewPort, "vertical");
  if ( ! scrollBar ) {
    scrollBar = XtNameToWidget( tp -> viewPort, "horizontial");
  }
  
  if ( scrollBar ) {
    Dimension sWd;
    Dimension sHt;
    
    args = 0;
    XtSetArg(argList[args], XtNwidth, &sWd); args++;
    XtSetArg(argList[args], XtNheight, &sHt); args++;
    XtGetValues( scrollBar, argList, args);
    
    dviWd += sWd;
    dviHt += sHt;
  }
  
  if ( dviWd <= maxWd ){
    maxWd = dviWd;
  }
  if ( dviHt <= maxHt ) {
    maxHt = dviHt;
  }
  args = 0;
  XtSetArg(argList[args], XtNwidth, maxWd); args++;
  XtSetArg(argList[args], XtNheight, maxHt); args++;
  XtSetValues( tp -> viewPort, argList, args);
#endif
  
  /* Now manage the viewport & single page */
  XtManageChild( tp -> singleDviPage );
  XtManageChild( tp -> viewPort );
  
  XtRealizeWidget(tp -> popUpPage);
  
  /* some bug somewhere makes the first page not appear unless I reset page */
  args = 0;
  XtSetArg(argList[args], XtNpage, tp -> pageData); args++;
  XtSetValues(tp -> singleDviPage, argList, args);
  
  showPage( tp, False );	/* set remaining buttons */
  
  changeMag( tp, tp -> userMag );
  
  XtPopup( tp -> popUpPage, XtGrabNone);
  
  enableScrollBars(tp);
  
}

void TeXPageBuild()
{
  int i;
  
  TeXPage *tp = TeXPageLocateFree();
  
  if ( ! tp -> realized  ) {
    TeXPageRealize(tp, 0);
  }
}

/*************** ACTIONS ***************/

static int getActionPageNumber(tp, params, num_params)
     TeXPage *tp;
     String *params;
     Cardinal *num_params;
{
  int want;

  if ( *num_params == 1 ) {
    if ( tp && strcmp(params[0], "box") == 0) {
      
      if ( ! tp -> pageNumberText ) {

	/* do nothing if box doesn't exist... */

	want = tp -> logicalPageNumber;
      }
      else {
	Arg argList[2];
	int args = 0;
	char *currentString;
	
	XtSetArg(argList[args], XtNstring, &currentString); args++;
	XtGetValues(tp -> pageNumberText, argList, args);
	
	want = atoi( currentString );
      }
    }
    else {
      want = atoi( params[0] );
    }
  }
  else {
    int bogus = -99999;
    want = texxPageNumber(tp,bogus);
  }

  return( want );

}

static void actionRealize(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  int want = getActionPageNumber( tp, params, num_params ); 
  TeXPage *newtp = TeXPageLocateFree();

  if ( ! newtp -> realized  ) {
    TeXPageRealize(newtp, want);
  }
} 

static void actionQuit(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp ) {
    XtDestroyWidget( tp -> popUpPage);
  }
} 

static void actionEcho(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  int totalLen;
  int i;
  char *echoCommand;
  
  totalLen = 0;
  for ( i = 1; i < *num_params; i++ ) {
    
    /* some error in the toolkit returns num_params==10 when I only have 2 */
    
    if ( params[i] ) {
      totalLen += strlen( params[i] );
    }
    else {
      break;
    }
  }
  
  totalLen += (*num_params = 10);
  
  echoCommand = (char *) XtMalloc( sizeof(char) * totalLen + 1);
  bzero(echoCommand, totalLen+1);
  
  for ( i = 0; i < *num_params; i++ ) {
    if ( params[i] ) {
      strncat(echoCommand, params[i], totalLen);
      strncat(echoCommand, " ", totalLen);
    }
    else {
      break;
    }
  }
  error(0,0, echoCommand );
  
  free( echoCommand );
} 

static void actionForeward(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp ) {
    char *p;
    int increment = texxPageNumber(tp, 1);
    int nextPageNumber = tp -> pageNumber + increment;
    
    /* wants to go to a non-valid page? */
    
    if (tp -> pageNumber >= DviTotalPages || nextPageNumber >= DviTotalPages) {
      nextPageNumber = DviTotalPages-1;
    }
    else  if (nextPageNumber < 0) {
      nextPageNumber = 0;
    }
    
    if (tp -> pageData && nextPageNumber != tp -> pageNumber ) {
      if ( tp -> pageData )
	TeXPageDeref( tp );
      
      tp -> pageNumber = nextPageNumber;
      showPage(tp, True);
      
      if ( tp -> tied )
	actionForeward(tp -> tied -> popUpPage, 0, 0, 0);
    }
  }
} 

static void actionBackward(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp ) {
    char *p;
    int increment = texxPageNumber(tp, 1);
    int nextPageNumber = tp -> pageNumber - increment;
    
    if ( tp -> tied ) {
      actionBackward( tp -> tied -> popUpPage, 0, 0, 0);
    }
    
    if ( tp -> pageNumber < 0 || nextPageNumber < 0 ) {
      nextPageNumber = 0;
    }
    else if (nextPageNumber >= DviTotalPages) {
      nextPageNumber = DviTotalPages - 1 ;
    }
    
    if ( tp -> pageData && nextPageNumber != tp -> pageNumber )  {
      TeXPageDeref( tp );
      tp -> pageNumber = nextPageNumber;
      showPage(tp, True);
    }
  }
} 

static void actionDup(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp ) {
    TeXPage *newtp = TeXPageLocateFree();
    TeXPageRealize(newtp, tp -> pageNumber);
  }
} 

static void actionTie(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp && tp -> tied == 0 ) {
    TeXPage *newtp = TeXPageLocateFree();
    tp -> tied = newtp;
    newtp -> tiedTo = tp;
    TeXPageRealize(newtp, tp -> pageNumber);
  }
} 

/*
 *	The goto button. This scans, starting at the current page number,
 *	until a value of ``DviCount[0]'' meets the goto destination.
 *	This allows us to hit the multiple ``page 1'' refs.
 *
 *	recall that we're going to the LOGICAL page specified. Thus, we
 *	must map the LOGICAL page number to a PHYSICAL page.
 *
 */
#define GOTO_KIND 0
#define PUSH_KIND 1

static void commonGoto(kind, w, event, params, num_params)
     int kind;
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp ) {
    int was = tp -> pageNumber;
    int want = getActionPageNumber( tp, params, num_params );

    findLogicalPage(True, tp, want);

    if ( tp -> pageNumber != was ) {

      if ( kind == PUSH_KIND ) {
	TeXPageStack *item = (TeXPageStack *) XtMalloc( sizeof(TeXPageStack) );
	item -> physicalPageNumber = was;
	item -> next = tp -> pageStack;
	tp -> pageStack = item;
      }

      TeXPageDeref( tp );
      TeXPageRef(tp, tp -> pageNumber);
      showPage(tp, True);
    }
  }
}

static void actionGoto(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  commonGoto(GOTO_KIND, w, event, params, num_params);
}

static void actionPush(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  commonGoto(PUSH_KIND, w, event, params, num_params);
}

static void actionPop(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp ) {
    TeXPageStack *item;
    int toPop = getActionPageNumber(tp, params, num_params);
    int pageNumber = -1;

    if ( toPop < 1 ) {
      toPop = 1;
    }
    
    item = tp -> pageStack;
    while( item ) {

      tp -> pageStack = item -> next;
      pageNumber = item -> physicalPageNumber;
      XtFree( item );
      item = tp -> pageStack;

      toPop--;
      if ( toPop < 1) break;
    };
    
    if ( pageNumber > -1 ) {
      if ( pageNumber != tp -> pageNumber ) {
	TeXPageDeref( tp );
	TeXPageRef(tp, pageNumber);
      }
      showPage(tp, True);
    }
  }
}

static void actionDivert(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  if ( xtexResources.divertByPush ) {
    actionPush(w, event, params, num_params);
  } else {
    actionRealize(w, event, params, num_params);
  }
}

static void actionUnDivert(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  if ( xtexResources.divertByPush ) {
    actionPop(w, event, params, num_params);
  }
}

static void actionRedisplay(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp ) {
    Arg argList[20];
    int args = 0;

    XtSetArg(argList[args], XtNredisplay, True); args++;
    XtSetValues( tp -> singleDviPage, argList, args);
  }
}

static Widget PopUpWidget;
static Widget GotoValuatorWidget;
static TeXPage *GotoTeXPage;

static void
  actionGotoPopupConfirm(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
  float value;
  int want;
  Arg argList[20];
  int args = 0;
  
  TeXPage *tp = GotoTeXPage;
  
  args = 0;
  XtSetArg(argList[args], XtNvalue, &value); args++;
  XtGetValues(GotoValuatorWidget, argList, args );
  want = value - 1;
  if ( want < 0 ) want = 0;
  if ( want >= DviTotalPages ) want = DviTotalPages - 1;
  
  if ( want != tp -> pageNumber ) {
    TeXPageDeref(tp);
    TeXPageRef(tp, want);
    showPage(tp, True);
  }
  XtPopdown( PopUpWidget );
}

static void
  actionGotoPopupCancel(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  XtPopdown( PopUpWidget );
}

static void
  actionGotoPopup(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  float value;
  Arg argList[20];
  int args = 0;
  
  Window root, child;
  static Widget labelWidget, formWidget, cancelWidget, confirmWidget,
  valueWidget;
  
  static Boolean inited = False;
  int root_x, root_y, child_x, child_y;
  unsigned int buttons;
  
  IntFloatUnion max;
  IntFloatUnion one;
  IntFloatUnion here;
  
  Dimension width;
  Dimension height;
  Dimension px,py,pw,ph;
  
  /*
   * Find out where the mouse is, so we can put the confirmation
   * box right there.
   */
  XQueryPointer(XtDisplay(tp -> popUpPage),
		XtWindow(tp -> popUpPage), &root, &child,
		&root_x, &root_y, &child_x, &child_y, &buttons);
  /*
   * If we need to construct the confirmation box do that,
   * otherwise just reset the position and callbacks and
   * put it up again.
   */
  if (!inited) {
    /*
     * The confirmation box will be a pop-up widget.
     */
    PopUpWidget =
      XtCreatePopupShell("Goto", POPUP_WINDOW_TYPE,
			 TopLevelWidget, NULL, 0);
    
    /*
     * Make a form to put the buttons in.
     */
    formWidget = XtCreateWidget("form", formWidgetClass,
				PopUpWidget, NULL, 0);
    
    /*
     * Confirmation button.
     */
    labelWidget = XtCreateManagedWidget("valueLabel", labelWidgetClass,
					formWidget, NULL, 0);
    
    /*
     * Confirmation button.
     */
    confirmWidget = XtCreateManagedWidget("confirm", commandWidgetClass,
					  formWidget, NULL, 0);
    
    /*
     * Valuator.
     */
    
    one.f = 1.0;
    args = 0;
    XtSetArg(argList[args], XtNmin, one.i); args++;
    XtSetArg(argList[args], XtNincrement, one.i); args++;
    GotoValuatorWidget = XtCreateManagedWidget("valuator", valuatorWidgetClass,
					       formWidget, argList, args);
    
    valueWidget = XtCreateManagedWidget("value", labelWidgetClass,
					formWidget, NULL, 0);
    
    args = 0;
    XtSetArg(argList[args], XtNvalueLabel, valueWidget); args++;
    XtSetValues(GotoValuatorWidget, argList, args);
    
    /*
     * Cancellation button.
     */
    cancelWidget = XtCreateManagedWidget("cancel", commandWidgetClass,
					 formWidget, NULL, 0);
    
    /*
     * Let the shell widget know we're here.
     */
    XtManageChild(formWidget);
    XtRealizeWidget(PopUpWidget);
    inited = True;
  }
  
  /*
   * Take some pains to center the popup on the pointer, but be certain
   * the thing is visible, else they can never exit
   */
  args = 0;
  XtSetArg(argList[args], XtNx, &px); args++;
  XtSetArg(argList[args], XtNy, &py); args++;
  XtSetArg(argList[args], XtNwidth, &pw); args++;
  XtSetArg(argList[args], XtNheight, &ph); args++;
  XtGetValues(tp -> popUpPage, argList,args);
  
  args = 0;
  XtSetArg(argList[args], XtNwidth, &width); args++;
  XtSetArg(argList[args], XtNheight, &height); args++;
  XtGetValues(PopUpWidget,argList,args);
  
  root_x -= (width/2);
  root_y -= (height/2);
  
  if ( root_x < 0 ) root_x = 0;
  if ( root_y < 0 ) root_y = 0;
  
  args = 0;
  XtSetArg(argList[args], XtNx, root_x); args++;
  XtSetArg(argList[args], XtNy, root_y); args++;
  XtSetValues(PopUpWidget,argList,args);
  
  /* reset the valuator */
  
  max.f = DviTotalPages;
  here.f = tp -> pageNumber + 1;
  
  args = 0;
  XtSetArg(argList[args], XtNmax, max.i); args++;
  XtSetArg(argList[args], XtNvalue, here.i); args++;
  XtSetValues(GotoValuatorWidget, argList, args);
  
  GotoTeXPage = tp;
  
  /* pop it up */
  XtPopup(PopUpWidget, XtGrabExclusive);
}

static void actionMagnify(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  int mag;
  
  if ( *num_params > 0 && *params ) {
    
    if ( strcmp(params[0], "large") == 0) {
      mag = xtexResources.largeMag;
    }
    else if ( strcmp(params[0], "small") == 0) {
      mag = xtexResources.smallMag;
    }
    else {
      mag = atoi( params[0] );
    }
  }
  else {
    mag = texxPageNumber( tp, tp -> userMag );
  }
  
  changeMag( tp, mag );
} 

static void actionNumber(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp && params && *params) {
    int digits = strlen(*params);
    int value = atoi(*params);
    
    if ( tp -> inputNumberValid ) {
      while ( digits > 0 ) {
	tp -> inputNumber *= 10;
	digits--;
      }
    }
    else {
      tp -> inputNumber = 0;
      tp -> inputNumberValid = True;
      tp -> inputNumberMinus = False;
    }
    tp -> inputNumber +=value;
  }
} 

static void actionNumberReset(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp ) {
    texxPageNumberReset(tp);
  }
} 

static void actionNumberMinus(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp ) {
    if ( ! tp -> inputNumberValid ) {
      tp -> inputNumberValid = True;
      tp -> inputNumber = 0;
      tp -> inputNumberMinus = True;
    }
    else {
      tp -> inputNumberMinus = ~(tp->inputNumberMinus);
    }
  }
} 

static XtActionsRec texxPageActionsTable[] = {
  {"xtex-echo",  actionEcho },
  {"xtex-page-realize",  actionRealize },
  {"xtex-page-quit",  actionQuit },
  {"xtex-page-forward",  actionForeward },
  {"xtex-page-foreward",  actionForeward },
  {"xtex-page-backward",  actionBackward },
  {"xtex-page-duplicate",  actionDup },
  {"xtex-page-tie",  actionTie },
  {"xtex-page-goto",  actionGoto },
  {"xtex-page-push",  actionPush },
  {"xtex-page-pop",  actionPop },
  {"xtex-page-divert",  actionDivert },
  {"xtex-page-undivert",  actionUnDivert },
  {"xtex-page-redisplay",  actionRedisplay },
  {"xtex-page-goto-popup",  actionGotoPopup },
  {"xtex-page-goto-popup-confirm",  actionGotoPopupConfirm },
  {"xtex-page-goto-popup-cancel",  actionGotoPopupCancel },
  {"xtex-page-magnify",  actionMagnify },
  {"xtex-page-number",  actionNumber },
  {"xtex-page-number-minus",  actionNumberMinus },
  {"xtex-page-number-reset",  actionNumberReset }
};

void
  TeXPageInstallActions()
{
  XtAddActions(texxPageActionsTable, XtNumber(texxPageActionsTable));
}
