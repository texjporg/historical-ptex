/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 */

/*
 *
 *	This looks kind of strange because, initially, I had intended to
 *	allow you to `put away' a page without destroying it -- but
 *	I've decided not to do that.
 *
 */

#include <stdio.h>
#include <X11/Xos.h>  
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <stdio.h>
#include <assert.h>

#include "dvi-simple.h"

#include "texx2.h"

#include "DviPage.h"
#include "texx2-page.h"
#include "texx2-font.h"
#include "texx2-widgets.h"
#include <X11/ViewportP.h>	/* needed to get scrollbar sizes? */

extern Widget TopLevelWidget;

#ifdef __STDC__
  void TeXPageRealize(TeXPage *, int);
  static void showPage(TeXPage *, int);
#else
  void TeXPageRealize();
  static void showPage();
#endif

int TeXPages;
TeXPage *TheTeXPages[ MAX_TEX_PAGES ];

typedef struct {
  char *thePage;
  int refs;
} DviPageRef;

DviPageRef *ThePageRefs = 0;

#define BUTTONS (TeXxResources.useButtons)

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
    for (i = 0; i < TeXPages; i++ ) {
      TeXPage *tp = TheTeXPages[i];
      if ( IS(singleDviPage) || IS(popUpPage) || IS(dviPageBox)
	  || IS(viewPort) || IS(buttonBox) || IS(forewardButton)
	  || IS(backwardButton) || IS(markButton) || IS(dupButton)
	  || IS(dupButton) || IS(tieButton) || IS(largeButton)
	  || IS(smallButton) || IS(quitButton) || IS(pageNumberText)
	  || IS(gotoButton) || IS(reopenButton) ) {
	return( tp );
      }
#undef IS
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

  bzero( tp, sizeof(TeXPage) );

  tp -> realized = False;
  tp -> tied = 0;
  tp -> tiedTo = 0;

  tp -> inputNumberValid = False;

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
    TeXPageRef(tp,  tp -> pageNumber );
    args = 0;
    XtSetArg(argList[args], XtNpage, tp -> pageData); args++;
    XtSetValues(tp -> singleDviPage, argList, args);
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
  
  maxWd = TeXxResources.pageMaxWd;
  maxHt = TeXxResources.pageMaxHt;
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
  
  if ( TeXxResources.autoSize ) {
    ViewportWidget vp = (ViewportWidget) (tp -> viewPort);
    
    tp -> horizBar = vp -> viewport.horiz_bar;
    if ( tp -> horizBar ) {
      XtInstallAccelerators(tp -> singleDviPage, tp -> horizBar);
    }
    
    tp -> vertBar = vp -> viewport.vert_bar;
    if (tp -> vertBar) {
      XtInstallAccelerators(tp -> singleDviPage, tp -> vertBar);
    }
    
    args = 0; XtSetArg(argList[args], XtNwidth, &dviWd); args++;
    XtSetArg(argList[args], XtNheight, &dviHt); args++;
    XtGetValues( tp -> singleDviPage, argList, args);
    
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

/*************** PAGE MANIPULATION ROUTINES ***************/

static void texxPageQuit(tp)
TeXPage *tp;
{
  XtDestroyWidget( tp -> popUpPage);
}

static void texxPageForward(tp)
TeXPage *tp;
{
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
	texxPageForward( tp -> tied );
    }
  }
}

static void texxPageBackward(tp)
TeXPage *tp;
{
  char *p;
  int increment = texxPageNumber(tp, 1);
  int nextPageNumber = tp -> pageNumber - increment;

  if ( tp -> tied ) {
    texxPageBackward( tp -> tied );
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

static void texxPageMarkToggle( tp )
TeXPage *tp;
{
  if ( tp ) {
    TeXMarkToggle( tp -> pageNumber );
    showPage(tp, False);
  }
} 

static void texxPageMarkSet( tp )
TeXPage *tp;
{
  if ( tp ) {
    TeXMarkSet( tp -> pageNumber );
    showPage(tp, False);
  }
} 

static void texxPageMarkUnset( tp )
TeXPage *tp;
{
  if ( tp ) {
    TeXMarkUnset( tp -> pageNumber );
    showPage(tp, False);
  }
} 

static void texxPageDup(tp)
TeXPage *tp;
{
  TeXPage *newtp = TeXPageLocateFree();
  TeXPageRealize(newtp, tp -> pageNumber);
} 

static void texxPageTie(tp)
TeXPage *tp;
{
  if ( tp -> tied == 0 ) {
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
 */

static void texxPageGoto(tp, want)
TeXPage *tp;
int want;
{
  if (tp) {
    TeXPageDeref( tp );
    findLogicalPage( True, tp, want );
    TeXPageRef(tp, tp -> pageNumber);
    showPage(tp, True);
  }
}

static void texxPageLarge(tp)
TeXPage *tp;
{
  if (tp)
    changeMag( tp, TeXxResources.largeMag );
} 

static void texxPageSmall(tp)
TeXPage *tp;
{
  if (tp)
    changeMag( tp, TeXxResources.smallMag );
} 

/************************ CALLBACKS ******************************/

static void pushQuitButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  TeXPage *tp = (TeXPage*) clientData;
  if ( tp ) 
    texxPageQuit(tp);
} 

static void pushForewardButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  TeXPage *tp = (TeXPage*) clientData;
  texxPageForward(tp);
} 

static void pushBackwardButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  TeXPage *tp = (TeXPage*) clientData;
  texxPageBackward(tp);
} 

static void pushMarkButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  TeXPage *tp = (TeXPage*) clientData;
  texxPageMarkToggle(tp);
} 

static void pushDupButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  TeXPage *tp = (TeXPage *) clientData;
  if ( tp )
    texxPageDup(tp);
} 

static void pushTieButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  TeXPage *tp = (TeXPage *) clientData;
  if ( tp )
    texxPageTie(tp);
} 

static void pushGotoButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  TeXPage *tp = (TeXPage*) clientData;
  if ( tp ) {
    char *pageNumber = tp -> pageNumberBuffer;
    if ( pageNumber != 0 && strlen( pageNumber ) > 0 ) {
      int want = atoi( pageNumber );
      texxPageGoto(tp, want);
    }
  }
} 

static void pushLargeButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  TeXPage *tp = (TeXPage *) clientData;
  texxPageLarge(tp);
} 

static void pushSmallButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  TeXPage *tp = (TeXPage *) clientData;
  texxPageSmall(tp);
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
  extern pushReopenButton();	/* from texx2-file */

  maxWd = TeXxResources.pageMaxWd;
  maxHt = TeXxResources.pageMaxHt;
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
  XtSetArg(argList[args], XtNresize, True); args++;
  XtSetArg(argList[args], XtNallowShellResize, True); args++;
  tp -> popUpPage = 
    XtCreatePopupShell("TeXx2-Page-Display",
		       topLevelShellWidgetClass, TopLevelWidget,
		       argList,args);

  args = 0;
  XtSetArg(argList[args], XtNresize, True); args++;
  tp -> dviPageBox =
    XtCreateManagedWidget("page",
			  formWidgetClass, tp -> popUpPage,
			  argList, args);

  if ( BUTTONS ) {
    args = 0;
    XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
    XtSetArg(argList[args], XtNtop, XtChainTop); args++;
    tp -> quitButton =
      XtCreateManagedWidget("quitButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    XtAddCallback(tp -> quitButton, XtNcallback, pushQuitButton, tp);
    
    args = 0;
    XtSetArg(argList[args], XtNtop, XtChainTop); args++;
    XtSetArg(argList[args], XtNfromHoriz, tp -> quitButton); args++;
    tp -> forewardButton =
      XtCreateManagedWidget("forwardButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    XtAddCallback(tp -> forewardButton, XtNcallback, pushForewardButton, tp);
    
    args = 0;
    XtSetArg(argList[args], XtNtop, XtChainTop); args++;
    XtSetArg(argList[args], XtNfromHoriz, tp -> forewardButton); args++;
    tp -> backwardButton =
      XtCreateManagedWidget("backwardButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    XtAddCallback(tp -> backwardButton, XtNcallback, pushBackwardButton, tp);
    
    args = 0;
    XtSetArg(argList[args], XtNtop, XtChainTop); args++;
    XtSetArg(argList[args], XtNfromHoriz, tp -> backwardButton); args++;
    tp -> markButton =
      XtCreateManagedWidget("markButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    XtAddCallback(tp -> markButton, XtNcallback, pushMarkButton, tp);
    
    args = 0;
    XtSetArg(argList[args], XtNtop, XtChainTop); args++;
    XtSetArg(argList[args], XtNfromHoriz, tp -> markButton); args++;
    tp -> dupButton =
      XtCreateManagedWidget("dupButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    XtAddCallback(tp -> dupButton, XtNcallback, pushDupButton, tp);
    
    
    args = 0;
    XtSetArg(argList[args], XtNtop, XtChainTop); args++;
    XtSetArg(argList[args], XtNfromHoriz, tp -> dupButton); args++;
    tp -> tieButton =
      XtCreateManagedWidget("tieButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    XtAddCallback(tp -> tieButton, XtNcallback, pushTieButton, tp);
    
    args = 0;
    XtSetArg(argList[args], XtNtop, XtChainTop); args++;
    XtSetArg(argList[args], XtNfromHoriz, tp -> tieButton); args++;
    tp -> largeButton =
      XtCreateManagedWidget("largeButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    XtAddCallback(tp -> largeButton, XtNcallback, pushLargeButton, tp);
    
    args = 0;
    XtSetArg(argList[args], XtNtop, XtChainTop); args++;
    XtSetArg(argList[args], XtNfromHoriz, tp -> largeButton); args++;
    tp -> smallButton =
      XtCreateManagedWidget("smallButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    XtAddCallback(tp -> smallButton, XtNcallback, pushSmallButton, tp);
    
    args = 0;
    XtSetArg(argList[args], XtNtop, XtChainTop); args++;
    XtSetArg(argList[args], XtNfromHoriz, tp -> smallButton); args++;
    tp -> reopenButton = 
      XtCreateManagedWidget("reopenButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    XtAddCallback(tp -> reopenButton, XtNcallback, pushReopenButton, tp);
    
    args = 0;
    XtSetArg(argList[args], XtNtop, XtChainTop); args++;
    XtSetArg(argList[args], XtNfromHoriz, tp -> reopenButton); args++;
    XtSetArg(argList[args], XtNeditType, XttextEdit); args++;
    XtSetArg(argList[args], XtNtextOptions, resizeWidth); args++;
    XtSetArg(argList[args], XtNstring, tp -> pageNumberBuffer); args++;
    XtSetArg(argList[args], XtNlength, MAXPAGENUMBERBUFFER); args++;
    tp -> pageNumberText =
      XtCreateManagedWidget("numberText",
			    asciiStringWidgetClass, tp -> dviPageBox,
			    argList, args);
    
    args = 0;
    XtSetArg(argList[args], XtNtop, XtChainTop); args++;
    XtSetArg(argList[args], XtNfromHoriz, tp -> pageNumberText); args++;
    tp -> gotoButton = 
      XtCreateManagedWidget("gotoButton",
			    commandWidgetClass, tp -> dviPageBox,
			    argList, args);
    XtAddCallback(tp -> gotoButton, XtNcallback, pushGotoButton, tp);
    
    XtInstallAccelerators( tp -> pageNumberText, tp -> gotoButton);
  }

  args = 0;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
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
  tp -> userMag = TeXxResources.userMag;
  tp -> active = True;
  
  args = 0;
  XtSetArg(argList[args], XtNpai, &ThePostAmbleInfo); args++;
  XtSetArg(argList[args], XtNdviFonts, TheFontInfo); args++;

  XtSetArg(argList[args], XtNdpi, TeXxResources.dpi ); args++;
  XtSetArg(argList[args], XtNpaperWidth,
	   TeXxResources.paperWidthString ); args++;
  XtSetArg(argList[args], XtNpaperHeight,
	   TeXxResources.paperHeightString ); args++;

  XtSetArg(argList[args], XtNtopOffset,
	   TeXxResources.topOffsetString ); args++;
  XtSetArg(argList[args], XtNleftOffset,
	   TeXxResources.leftOffsetString ); args++;

  XtSetArg(argList[args], XtNpage, tp -> pageData); args++;
  XtSetArg(argList[args], XtNuserMag, tp -> userMag); args++;
  XtSetArg(argList[args], XtNxFonts, TeXFontRef(tp -> userMag)); args++;

  tp -> singleDviPage = 
    XtCreateWidget("dviPage",
			  dviPageWidgetClass, tp -> viewPort,
			  argList, args);

  XtAddCallback( tp -> singleDviPage, XtNdestroyCallback, dviPageDied, tp);

  /* install accelerators -- note that we also wire the toplevel Quit
     button to every dviPage, allowing them to press Q to exit
     entirely */

  XtInstallAccelerators( tp -> singleDviPage, quitButton );
  XtInstallAccelerators( tp -> singleDviPage, printAllButton );
  XtInstallAccelerators( tp -> singleDviPage, printMarkedButton );
  XtInstallAccelerators( tp -> singleDviPage, printUnmarkedButton );
  XtInstallAccelerators( tp -> singleDviPage, clearMarksButton );

  if ( BUTTONS ) {
    XtInstallAccelerators( tp -> singleDviPage, tp -> quitButton);
    XtInstallAccelerators( tp -> singleDviPage, tp -> backwardButton);
    XtInstallAccelerators( tp -> singleDviPage, tp -> forewardButton);
    XtInstallAccelerators( tp -> singleDviPage, tp -> markButton);
    XtInstallAccelerators( tp -> singleDviPage, tp -> dupButton);
    XtInstallAccelerators( tp -> singleDviPage, tp -> largeButton);
    XtInstallAccelerators( tp -> singleDviPage, tp -> smallButton);
    XtInstallAccelerators( tp -> singleDviPage, tp -> reopenButton);
  }

  /* get the size of the dvi page. This is an attempt to provide
     a nice default window size */

  args = 0;
  XtSetArg(argList[args], XtNwidth, &dviWd); args++;
  XtSetArg(argList[args], XtNheight, &dviHt); args++;
  XtGetValues( tp -> singleDviPage, argList, args);
  if ( dviWd < maxWd ){
    maxWd = dviWd;
  }
  if ( dviHt < maxHt ) {
    maxHt = dviHt;
  }
  args = 0;
  XtSetArg(argList[args], XtNwidth, maxWd); args++;
  XtSetArg(argList[args], XtNheight, maxHt); args++;
  XtSetValues( tp -> viewPort, argList, args);

  /* Now manage the viewport & single page */
  XtManageChild( tp -> singleDviPage );
  XtManageChild( tp -> viewPort );

  XtRealizeWidget(tp -> popUpPage);

  changeMag( tp, tp -> userMag );

  /* some bug somewhere makes the first page not appear unless I reset page */
  args = 0;
  XtSetArg(argList[args], XtNpage, tp -> pageData); args++;
  XtSetValues(tp -> singleDviPage, argList, args);

  showPage( tp, False );	/* set remaining buttons */

  XtPopup( tp -> popUpPage, XtGrabNone);
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

static void actionQuit(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp ) 
    texxPageQuit(tp);
} 

static void actionForeward(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  texxPageForward(tp);
} 

static void actionBackward(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  texxPageBackward(tp);
} 

static void actionMarkToggle(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  texxPageMarkToggle(tp);
} 

static void actionMarkSet(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  texxPageMarkSet(tp);
} 

static void actionMarkUnset(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  texxPageMarkUnset(tp);
} 

static void actionDup(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp )
    texxPageDup(tp);
} 

static void actionTie(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp )
    texxPageTie(tp);
} 

static void actionGoto(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( tp ) {
    int bogus = -99999;
    int want = texxPageNumber(tp,bogus);
    if ( want != bogus ) {
      texxPageGoto(tp, want);
    }
  }
} 

static void actionLarge(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  texxPageLarge(tp);
} 

static void actionSmall(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  texxPageSmall(tp);
} 

static void actionMag(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXPage *tp = TeXPageLocate(w);
  if ( num_params > 0 && *params ) {
    int mag = atoi( *params );
    changeMag( tp, mag );
  }
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
  {"texx2-page-quit",  actionQuit },
  {"texx2-page-forward",  actionForeward },
  {"texx2-page-foreward",  actionForeward },
  {"texx2-page-backward",  actionBackward },
  {"texx2-page-mark-toggle",  actionMarkToggle },
  {"texx2-page-mark-set",  actionMarkSet },
  {"texx2-page-mark-unset",  actionMarkUnset },
  {"texx2-page-duplicate",  actionDup },
  {"texx2-page-tie",  actionTie },
  {"texx2-page-goto",  actionGoto },
  {"texx2-page-large",  actionLarge },
  {"texx2-page-small",  actionSmall },
  {"texx2-page-mag",  actionMag },
  {"texx2-page-number",  actionNumber },
  {"texx2-page-number-minus",  actionNumberMinus },
  {"texx2-page-number-reset",  actionNumberReset }
};

void
TeXPageInstallActions()
{
  XtAddActions(texxPageActionsTable, XtNumber(texxPageActionsTable));
}

