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
 *	Handle printing of marked pages
 */

#include <stdio.h>
#include <X11/Xos.h>  
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <assert.h>
#include <sys/wait.h>

#include "dvi-simple.h"
#include "xtex.h"
#include "mark.h"
#include "page.h"
#include "widgets.h"

typedef struct MarkStackStruct MarkStack;

struct MarkStackStruct {
  Bool *mark;
  short pages;
  MarkStack *next;
};

MarkStack *CurrentMarks = 0;


/*
 *	Widgets for menu of marked pages
 */
typedef struct {
  Boolean valid;
  int *listOffset;
  String *listItems;
  String listStorage;
  int stringSize;
  int stringLast;
} MarkMenu;

MarkMenu TheMarkMenu = {0,0,0,0,0};

/* update the displayed marks & cause the menu to be refreshed */

static void
  TeXMarkMenuRefresh()
{
  int page;
  
  if ( TheMarkMenu.valid ) {
    
    for ( page = 0; page < DviTotalPages; page ++ ) {
      char *item = TheMarkMenu.listItems[ page ];
      
      *(item) = (CurrentMarks -> mark [ page ]) ? '*' : ' ';
    }
    
    XawListChange( markMenuList, TheMarkMenu.listItems, -1, -1, True );
  }
}

void
  TeXMarkMenuSelect(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
  XawListReturnStruct *item = (XawListReturnStruct *) call_data;
  
  if ( item != 0 ) {
    int page = item -> list_index;
    TeXMark( markToggle, page);
  }
}

void BuildMarkMenuPopUp()
{
  if ( markMenuList && DviTotalPages > 0 ) {
    int mallocSize;
    int page;
    
    /* Build the list - one item for each page, one null terminator
     * We don't know what the various count0 fields enclose, so we
     * make a guess for the string space needed and realloc it as
     * needed.
     */
    if ( TheMarkMenu.stringSize != 0 ) {
      XtFree( TheMarkMenu.listStorage );
      TheMarkMenu.listStorage = 0;
    }
    
    TheMarkMenu.stringSize = DviTotalPages * 6;
    TheMarkMenu.stringLast = TheMarkMenu.stringSize;
    TheMarkMenu.listStorage = XtMalloc( TheMarkMenu.stringSize );
    
    mallocSize = ( sizeof(String) * (DviTotalPages + 2) );
    TheMarkMenu.listItems = (String *) XtMalloc( mallocSize );
    bzero(TheMarkMenu.listItems, mallocSize);
    
    mallocSize = ( sizeof(int) * (DviTotalPages + 2) );
    TheMarkMenu.listOffset = (int *) XtMalloc( mallocSize );
    bzero(TheMarkMenu.listItems, mallocSize);
    
    for (page = 0; page < DviTotalPages; page++) {
      char thisPage[128];
      int len;
      
      sprintf(thisPage," %d", DviCount[0][page]);
      
      len = strlen(thisPage) + 1;
      if ( (TheMarkMenu.stringSize - TheMarkMenu.stringLast) <= len ) {
	TheMarkMenu.listStorage
	  = XtRealloc( TheMarkMenu.listStorage, TheMarkMenu.stringSize * 2 );
      }
      
      TheMarkMenu.listOffset[page] = TheMarkMenu.stringLast;
      bcopy(thisPage,
	    TheMarkMenu.listStorage + TheMarkMenu.listOffset[page], len);
      TheMarkMenu.stringLast += len;
    }
    
    /*
     * Convert offsets to actual addresses
     */
    for (page = 0; page < DviTotalPages; page++) {
      TheMarkMenu.listItems[ page ] =
	TheMarkMenu.listStorage + TheMarkMenu.listOffset[ page ];
    }
    
    TheMarkMenu.valid = True;
    
    TeXMarkMenuRefresh();
    XtMapWidget(markMenuViewPort);
    TeXMarkMenuRefresh();
  }
}


void
  TeXMarkPush()
{
  MarkStack *stk = (MarkStack *) malloc( sizeof(MarkStack) );
  int len = DviTotalPages * sizeof(Bool);
  stk -> mark = (Bool *) malloc( len );
  
  assert(stk && stk -> mark);
  
  if ( (int) False == 0 ) {
    bzero(stk -> mark, len);
  }
  else {
    int page;
    for ( page = 0; page< DviTotalPages; page++ ) {
      CurrentMarks -> mark [ page ] = False;
    }
  }
  stk -> next = CurrentMarks;
  CurrentMarks = stk;
  
  TeXMarkMenuRefresh();
}

void
  TeXMarkPop()
{
  if ( CurrentMarks != 0 ) {
    MarkStack *stk = CurrentMarks;
    CurrentMarks = stk -> next;
    free( stk -> mark );
    free( stk );
    
    TeXMarkMenuRefresh();
    
  }
}

static void
  insureMarks()
{
  if ( CurrentMarks == 0 ) TeXMarkPush();
}

void
  TeXMarkNewFile()
{
  int page;
  
  TheMarkMenu.valid = False;
  
  while (CurrentMarks != 0) {
    TeXMarkPop();
  }
  
  if ( DviFile != 0 ) {
    TeXMarkPush();
    BuildMarkMenuPopUp();
  }
}

/*
 *	Simply return the state
 */
Bool
  TeXMarkState(page)
int page;
{
  if ( page >=0 && page < DviTotalPages ) {
    insureMarks();
    return( CurrentMarks -> mark[  page  ] );
  }
  else {
    return( 0 );
  }
}

Bool
  TeXMark( md, page )
MarkDirective md;
int page;
{
  if ( page >=0 && page < DviTotalPages ) {
    insureMarks();
    
    switch ( md ) {
    case markSet:
      CurrentMarks -> mark[  page  ]  = True;
      break;
    case markClear:
      CurrentMarks -> mark[  page  ]  = False;
      break;
    case markToggle:
      CurrentMarks -> mark[  page  ]  = ~ CurrentMarks -> mark[ page ];
      break;
    default:
      error(0,0,"Unknown mark directive in TeXMark: %d\n", (int) md);
    }
    TeXMarkMenuRefresh();
    return( CurrentMarks -> mark[  page  ] );
  }
  else {
    return( False );
  }
}

void
  TeXMarkAll( md )
MarkDirective md;
{
  int page;
  
  insureMarks();
  
  switch ( md ) {
    
  case markSet:
    for (page = 0; page < DviTotalPages; page++) {
      CurrentMarks -> mark[  page  ] = True;
    }
    break;
    
  case markClear:
    for (page = 0; page < DviTotalPages; page++) {
      CurrentMarks -> mark[  page  ] = False;
    }
    break;
    
  case markToggle:
    for (page = 0; page < DviTotalPages; page++) {
      CurrentMarks -> mark[  page  ] = ~CurrentMarks -> mark[ page ];
    }
    break;
    
  default:
    error(0,0,"Unknown mark directive in TeXMark: %d\n", (int) md);
  }
  
  TeXMarkMenuRefresh();
}

/*
 *	Do a system call, while handling X events. This is used to print
 *	marked pages while still doing other X things.
 */

static Boolean TeXSystemChildDied;
static int TeXSystemChild;

static void
  TeXSystemCallRead( f )
caddr_t f;
{
  int fd = (int) f;
  char buffer[1024];
  int bytes;
  int pid;
  union wait status;
  
  /* call ye not upon the dead */
  
  int rfds = 1 << fd;
  int wfds = 0;
  int efds = 0;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  
  if ( select(fd+1, &rfds, &wfds, &efds, &tv) ) {
    bytes = read(fd, buffer, 1024);
    if ( bytes > 0 ) {
      buffer[bytes] = 0;
      DialogMessage(buffer,0);
    }
  }
}

void
  TeXSystemCall(str)
char *str;
{
  int fildes[2];
  
  if ( pipe(fildes) < 0 ) {
    perror("pipe");
    exit(1);
  }
  
  if (TeXSystemChild = fork()) {
    /* ye olde parent, who will read from fildes[0] */
    
    int filemask = (1 << fildes[0]);
    
    XtAppContext context = 
      XtWidgetToApplicationContext( TopLevelWidget );
    
    XtInputId inputId;
    
    TeXSystemChildDied = False;
    inputId = XtAppAddInput(context, fildes[0],
			    XtInputReadMask,
			    TeXSystemCallRead,
			    (caddr_t) fildes[0]);
    
    while ( ! TeXSystemChildDied ) {
      
      int pid;
      union wait status;
      
      /*
       * This is not desirable, since it causes the X application to
       * spin while the child does its stuff, but the X application
       * appears to be stuck on some event if I do the more normal
       * AppNextEvent/DispatchEvent loop - as if we're blocked waiting
       * for something to happen.
       */
      
      XEvent event;
      if ( XtAppPeekEvent(context, &event) ) {
	XtDispatchEvent(&event);
      }
      pid = wait3(&status, WNOHANG, 0);
      if ( pid == TeXSystemChild ) {
	TeXSystemChildDied = True;
      }
    }
    
    TeXSystemCallRead(fildes[0]);	/* catch any pending output */
    close(fildes[0]);
    XtRemoveInput( inputId );
  }
  else {
    /* ye newe child, who will write to fildes[1] */
    dup2(fildes[1],1);
    dup2(fildes[1],2);
    fprintf(stderr,"[ exec ] %s\n", str);
    system( str );
    
    /* terminate the line */
    fclose(stderr);
    fclose(stdout);
    close(fildes[1]);
    close(1);
    close(2);
    exit(0);
  }
}

/* Print some pages. The mark selection dictates what pages get printed */
void
  TeXMarkPrintUsing(inWidget, pd, dvips)
Widget inWidget;
char *dvips;
PrintDirective pd;
{
  int cnt;
  int i;
  char buf[20];
  
  char printCommand[1024];
  char *pc;
  char tmpFileName[1024];
  
  char *fileToPrint;
  int deletePrintedFile;
  
  
  if ( pd == printAll ) {
    fileToPrint = DVIFileName;
    deletePrintedFile = 0;
  }
  else {
    
    int dviSelectBufferLength = 2048;
    char dviSelectBuffer[ 2048 ];
    
    /* create a temporary file name in a more portable way */
    sprintf(tmpFileName,"/tmp/xtex%u.dvi", getpid());
    
    fileToPrint = tmpFileName;
    deletePrintedFile = 1;
    
    /* Now run dviselect to extract the pages */
    
    sprintf(dviSelectBuffer, "dviselect -i \"%s\" -o \"%s\" ",
	    DVIFileName, fileToPrint);
    
    cnt = 0;
    for (i = 0; i < DviTotalPages; i++) {
      if (   (CurrentMarks -> mark[  i  ] && pd == printMarked )
	  || (!CurrentMarks -> mark[  i  ] && pd == printUnmarked )) {
	sprintf(buf, " =%d", i + 1);
	strcat(dviSelectBuffer, buf);
	cnt++;
      }
    }
    
    if (cnt != 0) {
      TeXSystemCall(dviSelectBuffer);
    }
    else {
      unlink( fileToPrint );
      return;
    }
  }
  
  /* Now print pages */
  
  sprintf(printCommand, "%s %s ", dvips, fileToPrint);
  
  pc = TeXConfirmString(inWidget, "Print using command:", printCommand);
  
  if ( pc ) { 
    TeXSystemCall(pc);
  }
  
  if ( deletePrintedFile ) {
    unlink( fileToPrint );
  }
}

void
  TeXMarkPrint(inWidget, pd)
Widget inWidget;
PrintDirective pd;
{
  char *dvips = xtexResources.printProgram;
  
  if ( dvips == 0 ) {
    extern char *getenv();
    dvips = getenv("DVIPRINT");
    if (dvips == NULL) {
      dvips = "lpr -d ";	/* a good guess on BSD systems */
    }
    error(0,0, "Using default ``%s'' to print\n", dvips);
  }
  
  TeXMarkPrintUsing( inWidget, pd, dvips );
}

/**********************************************************************/
/**	Confirm String **/

#define MAX_STRING 1024
static char TeXString[MAX_STRING];

static Widget PopUpWidget;
static Widget StringWidget;
static Boolean ConfirmOrCancel;
static Boolean ConfirmP;

static void
  confirmString()
{
  ConfirmOrCancel = True;
  ConfirmP = True;
  if ( PopUpWidget && XtIsRealized(PopUpWidget)) {
    XtPopdown( PopUpWidget );
  }
}

static void
  cancelString()
{
  ConfirmOrCancel = True;
  ConfirmP = False;
  if ( PopUpWidget && XtIsRealized(PopUpWidget)) {
    XtPopdown( PopUpWidget );
  }
}

char *
  TeXConfirmString(inWidget, title, defaultString)
Widget inWidget;
char *title;
char *defaultString;
{
  XawTextBlock tblk;
  int len = (defaultString == 0) ? 0 : strlen(defaultString);
  char *string;
  Arg argList[20];
  int args = 0;
  Window root, child;
  static Boolean inited = False;
  int root_x, root_y, child_x, child_y;
  unsigned int buttons;
  static Widget titleWidget, formWidget, cancelWidget, confirmWidget;
  
  Dimension width;
  Dimension height;
  Dimension confirmHeight;
  Dimension confirmWidth;
  Dimension titleHeight;
  
  static XtCallbackRec confirmCB[] = {
    {(XtCallbackProc) confirmString, NULL},
    {(XtCallbackProc) NULL, NULL}};
  
  static XtCallbackRec cancelCB[] = {
    {(XtCallbackProc) cancelString, NULL},
    {(XtCallbackProc) NULL, NULL}};
  
  XtAppContext context = 
    XtWidgetToApplicationContext( inWidget );
  
  if ( ! inited ) {
    /*
     * The confirmation box will be a pop-up widget.
     */
    PopUpWidget =
      XtCreatePopupShell("stringQuery",
			 POPUP_WINDOW_TYPE, TopLevelWidget,
			 NULL, 0);
    
    /*
     * Make a form to put the buttons in.
     */
    formWidget = XtCreateWidget("form", formWidgetClass,
				PopUpWidget, NULL, 0);
    
    args = 0;
    XtSetArg(argList[args], XtNlabel, title); args++;
    titleWidget = XtCreateManagedWidget("title",labelWidgetClass,
					formWidget, argList, args);
    
    /*
     * Confirmation & cancellation buttons.
     */
    args = 0;
    XtSetArg(argList[args], XtNcallback, confirmCB); args++;
    confirmWidget = XtCreateManagedWidget("confirm", commandWidgetClass,
					  formWidget, argList, args);
    
    args = 0;
    XtSetArg(argList[args], XtNcallback, cancelCB); args++;
    cancelWidget = XtCreateManagedWidget("cancel", commandWidgetClass,
					 formWidget, argList, args);
    
    args = 0;
    XtSetArg(argList[args], XtNeditType,XawtextEdit); args++;
    XtSetArg(argList[args], XtRAsciiType, XawAsciiString); args++;
    XtSetArg(argList[args], XtNstring, TeXString); args++;
    XtSetArg(argList[args], XtNlength, MAX_STRING); args++;
    StringWidget = XtCreateManagedWidget("string",
					 asciiTextWidgetClass,
					 formWidget, argList, args);
    /*
     * Let the shell widget know we're here.
     */
    XtManageChild(formWidget);
    XtRealizeWidget(PopUpWidget);
    inited = True;
  }
  
  /*
   * Find out where the mouse is, so we can put the confirmation
   * box right there.
   */
  XQueryPointer(XtDisplay(inWidget),
		XtWindow(inWidget), &root, &child,
		&root_x, &root_y, &child_x, &child_y, &buttons);
  
  /* get the box size */
  
  args = 0;
  XtSetArg(argList[args], XtNwidth, &width); args++;
  XtSetArg(argList[args], XtNheight, &height); args++;
  XtGetValues(PopUpWidget,argList,args);
  
  args = 0;
  XtSetArg(argList[args], XtNwidth, &confirmWidth); args++;
  XtSetArg(argList[args], XtNheight, &confirmHeight); args++;
  XtGetValues(confirmWidget,argList,args);
  
  args = 0;
  XtSetArg(argList[args], XtNheight, &titleHeight); args++;
  XtGetValues(titleWidget,argList,args);
  
  /* reset the confirmation box position -- try to get it lined
     up on the confirm widget */
  
  root_x -= (confirmWidth/2);
  root_y -= (titleHeight + confirmHeight);
  
  args = 0;
  XtSetArg(argList[args], XtNx, root_x); args++;
  XtSetArg(argList[args], XtNy, root_y); args++;
  XtSetValues(PopUpWidget,argList,args);
  
  /* set the default string */
  
  if ( len > 0 ) {
    Arg argList[20];
    Cardinal args;
    char *currentString;
    XawTextPosition start;
    XawTextPosition end;
    
    args = 0;
    XtSetArg(argList[args], XtNstring, &currentString); args++;
    XtGetValues(StringWidget, argList, args);
    start = 0; end = strlen(currentString);
    
    tblk.firstPos = 0;
    tblk.length = len;
    tblk.ptr = defaultString;
    tblk.format = FMT8BIT;
    
    XawTextDisableRedisplay(StringWidget);
    XawTextReplace( StringWidget, start, end, &tblk );
    XawTextEnableRedisplay(StringWidget);
  }
  
  /* pop it up */
  ConfirmP = False;
  ConfirmOrCancel = False;
  XtPopup(PopUpWidget, XtGrabExclusive);
  
  while ( ! ConfirmOrCancel ) {
    XEvent event;
    XtAppNextEvent(context, &event);
    XtDispatchEvent(&event);
  }
  
  if ( ConfirmP ) {
    args = 0;
    XtSetArg(argList[args], XtNstring, &string); args++;
    XtGetValues(StringWidget, argList, args);
  }
  else {
    string = 0;
  }
  return(string);
}


/*********************************************************/
/*	Action Interface				 */
/*********************************************************/


static void actionPrintUsing(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  char *directive = (char *) params[0];
  PrintDirective pd;
  
  if (*num_params < 1 ) {
    return;
  }
  
  if ( strcmp(directive,"all") == 0 ) {
    pd = printAll;
  }
  else if (strcmp(directive,"set") == 0) {
    pd = printMarked;
  }
  else if (strcmp(directive,"clear") == 0) {
    pd = printUnmarked;
  }
  else {
    return;
  }
  
  if (*num_params == 1) {
    TeXMarkPrint(w, pd);
  }
  else if (*num_params >= 2) {
    int totalLen;
    int i;
    char *printCommand;
    
    totalLen = 0;
    for ( i = 1; i < *num_params; i++ ) {
      if ( params[i] ) {
	totalLen += strlen( params[i] );
      }
      else {
	break;
      }
    }
    
    totalLen += (*num_params = 10);
    
    printCommand = (char *) malloc( sizeof(char) * totalLen + 1);
    bzero(printCommand, totalLen + 1);
    
    for ( i = 1; i < *num_params; i++ ) {
      if ( params[i] ) {
	strncat(printCommand, params[i], totalLen);
	strncat(printCommand, " ", totalLen);
      }
      else {
	break;
      }
    }
    TeXMarkPrintUsing(w, pd, printCommand);
    
    free( printCommand );
  }
}

static void actionMarkPage(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  MarkDirective d;
  
  d = markNone;
  
  if (*num_params < 1) {
    error(0,0,"Must specify set/clear/toggle to texx-mark-all");
  }
  else if ( strcmp(params[0],"set") == 0 ) {
    d = markSet;
  }
  else if ( strcmp(params[0],"clear") == 0 ) {
    d = markClear;
  }
  else if ( strcmp(params[0],"toggle") == 0 ) {
    d = markToggle;
  }
  else {
    error(0,0,"Must specify set/clear/toggle to texx-mark-all");
  }
  
  if (markNone) {
    return;
  }
  else {
    TeXPage *tp = TeXPageLocate(w);
    int num = tp -> pageNumber;
    TeXMark( d, num );
  }
} 

static void actionMarkAll(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  MarkDirective d;
  
  d = markNone;
  
  if (*num_params < 1) {
    error(0,0,"Must specify set/clear/toggle to texx-mark-all");
  }
  else if ( strcmp(params[0],"set") == 0 ) {
    d = markSet;
  }
  else if ( strcmp(params[0],"clear") == 0 ) {
    d = markClear;
  }
  else if ( strcmp(params[0],"toggle") == 0 ) {
    d = markToggle;
  }
  else {
    error(0,0,"Must specify set/clear/toggle to texx-mark-all");
  }
  
  if  ( d != markNone ) {
    TeXMarkAll( d );
  }
}

static void actionMarkPush(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXMarkPush();
} 

static void actionMarkPop(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXMarkPop();
} 

static void actionSystem(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  if ( num_params ) {
    int p;
    char buffer[1024];
    char *buf = buffer;
    int buflen = 0;
    char *b = buf;
    int space = 1024;
    Bool malloced = False;

    for (p = 0; p < *num_params ; p++) {
      int len = strlen( params[p] );
      if (space < (len+1)) {
	char *new = (char *) XtMalloc( buflen * 2);
	space = buflen * 2;
	strcpy(new,buf);
	if ( malloced ) XtFree(buf);
	malloced = True;
	buf = new;
	b = buf + buflen;
      }

      strcpy(b,params[p]);

      b += len;
      *(b++) = ' ';

      buflen += (len + 1);
      space -= (len + 1);
    }

    *b = 0;

    TeXSystemCall(buf);

    if ( malloced ) XtFree(buf);
  }
} 

static XtActionsRec texxMarkActionsTable[] = {
  {"xtex-mark-all",  actionMarkAll },
  {"xtex-mark-page",  actionMarkPage },
  {"xtex-mark-push",  actionMarkPush },
  {"xtex-mark-pop",  actionMarkPop },
  {"xtex-system",  actionSystem},
  {"xtex-string-confirm",  confirmString},
  {"xtex-string-cancel",  cancelString},
  {"xtex-print-using",  actionPrintUsing }
};

void
  TeXMarkInstallActions()
{
  XtAddActions(texxMarkActionsTable, XtNumber(texxMarkActionsTable));
}

