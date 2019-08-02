/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 */

#include "texx2.h"

#include <errno.h>
#include <sys/param.h>
#include <assert.h>
#include <X11/Label.h>
#include "dvi-simple.h"
#include "texx2-page.h"
#include "texx2-mark.h"
#include "texx2-widgets.h"

char TeXInputFileNameString[MAXPATHLEN+40];
char *TeXInputFileName;

char TeXInputDirNameString[MAXPATHLEN+40];
char *TeXInputDirName;

char fileNameInput[MAXPATHLEN];

/*
 *	If it's a file name, set to the directory of that file,
 *	otherwise, set to the directory
 */
void
setDirectory(dir)
char *dir;
{
  Arg argList[20];
  Cardinal args;

  char pDir[MAXPATHLEN];
  char *endPDir;
  int len;

  strncpy(pDir,dir,MAXPATHLEN);
  len = strlen(pDir);
  endPDir = &pDir[len];

  while (chdir(pDir) < 0) {
    /* skip back to last slash */
    while (endPDir != pDir && *endPDir != '/') endPDir--;

    if (endPDir == pDir) {
      getwd(pDir);
    }
    else {
      *endPDir = 0;
    }

  }

  getwd(pDir);

  strcpy(TeXInputDirNameString, "Directory: ");
  TeXInputDirName = &TeXInputDirNameString[strlen(TeXInputDirNameString)];
  strncpy(TeXInputDirName, pDir, MAXPATHLEN);

  if ( directoryLabel != NULL ) {
    args = 0;
    XtSetArg(argList[args], XtNlabel, TeXInputDirNameString); args++;
    XtSetValues( directoryLabel, argList, args);
  }
}

void setFileName(fileName)
char *fileName;
{
  if ( fileNameText && XtIsRealized( fileNameText ) ) {
    XtTextBlock tblk;
    int len = (fileName == 0) ? 0 : strlen(fileName);
    
    if ( len > 0 ) {
      XtTextPosition start = 0;
      XtTextPosition end = strlen(fileNameInput);;

      tblk.firstPos = 0;
      tblk.length = len;
      tblk.ptr = fileName;
      tblk.format = FMT8BIT;
      
      XtTextDisableRedisplay(fileNameText);

      XtTextReplace( fileNameText, start, end, &tblk );

      XtTextEnableRedisplay(fileNameText);
    }
  }
  else {
    int len = (fileName == 0) ? 0 : strlen(fileName);
    bzero(fileNameInput, MAXPATHLEN);
    if ( len > 0 ) {
      strncpy(fileNameInput, fileName, MAXPATHLEN);
    }
  }
}

void
setFile(fileName, copy)
char *fileName;
int copy;
{
  DviDpi = TeXxResources.dpi;

  TeXPageCloseFile();	/* close open pages stuff */

  if( DviInit(fileName, copy )) {
    disablePageLevel();
    TeXPageSlamPages();
    error(0, 0,"[fopen] Unable to open ``%s''", fileName);
  }
  else {
    enablePageLevel();

    error(0, 0, "%s the %d page file ``%s''",
	  (copy) ? "Copied" : "Opened",
	  DviTotalPages, DviFileName);

    strcpy(TeXInputFileNameString, "File: ");
    TeXInputFileName = &TeXInputFileNameString[strlen(TeXInputFileNameString)];
    strcpy(TeXInputFileName, DviFileName, MAXPATHLEN);

    if ( fileLabel != NULL) {
      Arg argList[20];
      Cardinal args;
      
      args = 0;
      XtSetArg(argList[args], XtNlabel, TeXInputFileNameString); args++;
      XtSetValues( fileLabel, argList, args );
    }
  }
  TeXFontNewFile();
  TeXMarkNewFile();
  TeXPageOpenFile();
}

static lastOpenKind = 0;

static void
pushOpenFileButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  char *fileName = fileNameInput;
  
  if ( fileName == NULL ) {
    error(0, 0, "No file specified?");
  } 
  else {
    if ( w == cdButton ) {
      setDirectory( fileName );
    }
    else if (w == openButton) {
      lastOpenKind = 0;
      setFile( fileName, 0);
    }
    else if (w == copyButton) {
      lastOpenKind = 1;
      setFile( fileName, 1);
    }
  }
}

void
pushReopenButton(w, clientData, callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  char *fileName = fileNameInput;
  
  if ( fileName == NULL ) {
    error(0, 0, "No file specified?");
  } 
  else {
    setFile( fileName, lastOpenKind);	/* open it same way they did */
  }
}

static void
pushCloseFileButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  DviFini();
  TeXFontNewFile();
  TeXMarkNewFile();
  TeXPageCloseFile();
  TeXPageSlamPages();
  disablePageLevel();
}

/*
 *	Page related callbacks & routines
 */

enablePageLevel()
{
  Arg argList[20];
  Cardinal args;
      
  args = 0;
  XtSetArg(argList[args], XtNsensitive, True); args++;

  XtSetValues(pageButton, argList, args);
  XtSetValues(printAllButton, argList, args);
  XtSetValues(printMarkedButton, argList, args);
  XtSetValues(printUnmarkedButton, argList, args);
  XtSetValues(clearMarksButton, argList, args);
}

disablePageLevel()
{
  Arg argList[20];
  Cardinal args;
      
  args = 0;
  XtSetArg(argList[args], XtNsensitive, False); args++;

  XtSetValues(pageButton, argList, args);
  XtSetValues(printAllButton, argList, args);
  XtSetValues(printMarkedButton, argList, args);
  XtSetValues(printUnmarkedButton, argList, args);
  XtSetValues(clearMarksButton, argList, args);
}

static void
pushPageButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  if (DviFile != 0 ) {
    TeXPageBuild();
  }
} 

static void
pushPrintButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  PrintDirective what = (PrintDirective) clientData;
  assert( what == printAll || what == printMarked || what == printUnmarked );

  if (DviFile != 0) {
    TeXMarkPrint( what );
  }

} 

static void
pushClearMarksButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  if (DviFile != 0 ) {
    TeXMarkNone();
  }
} 

static void
pushQuitButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  exit(0);
} 

static void actionQuit(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  exit(0);
}

static void actionNoop(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  XBell(XtDisplay(w), 100);
}

static void actionFileOpen(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  pushOpenFileButton( openButton, 0, 0 );
}

static void actionFileCopy(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  pushOpenFileButton( copyButton, 0, 0 );
}

static void actionFileReopen(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  pushReopenButton( w, 0, 0 );
}

static XtActionsRec fileInputActionsTable[] = {
  {"texx2-file-open",   actionFileOpen},
  {"texx2-file-copy",   actionFileCopy},
  {"texx2-file-reopen",   actionFileReopen},
  {"texx2-quit",   actionQuit},
  {"texx2-noop",   actionNoop}
};

void
TeXFileInstallActions()
{
  XtAddActions(fileInputActionsTable, XtNumber(fileInputActionsTable));
}


void
  BuildFileLevel()
{
  Arg argList[20];
  Cardinal args;
  int len = strlen(fileNameInput);

  args = 0;
  XtSetArg(argList[args], XtNeditType,XttextEdit); args++;
  XtSetArg(argList[args], XtNstring, fileNameInput); args++;
  XtSetArg(argList[args], XtNlength, MAXPATHLEN); args++;
  XtSetArg(argList[args], XtNtextOptions, resizeWidth); args++;
  fileNameText =
    XtCreateManagedWidget("fileName",
			  asciiStringWidgetClass, topPane,
			  argList, args);

  quitButton =
    XtCreateManagedWidget("quitButton",
			  commandWidgetClass, topPane,
			  0,0);
  XtAddCallback( quitButton, XtNcallback, pushQuitButton, NULL);

  openButton =
    XtCreateManagedWidget("openButton",
			  commandWidgetClass, topPane,
			  0,0);
  XtAddCallback( openButton, XtNcallback, pushOpenFileButton, NULL);

  copyButton =
    XtCreateManagedWidget("copyButton",
			  commandWidgetClass, topPane,
			  0,0);
  XtAddCallback( copyButton, XtNcallback, pushOpenFileButton, NULL);

  closeButton =
    XtCreateManagedWidget("closeButton",
			  commandWidgetClass, topPane,
			  0,0);
  XtAddCallback( closeButton, XtNcallback, pushCloseFileButton, NULL);

  cdButton =
    XtCreateManagedWidget("cdButton",
			  commandWidgetClass, topPane,
			  0,0);
  XtAddCallback( cdButton, XtNcallback, pushOpenFileButton, NULL);


  pageButton
    = XtCreateManagedWidget("pageButton",
			    commandWidgetClass, topPane,
			    0,0);
  XtAddCallback( pageButton, XtNcallback, pushPageButton, NULL);
  
  printAllButton
    = XtCreateManagedWidget("printAllButton",
			    commandWidgetClass, topPane,
			    0,0);
  XtAddCallback( printAllButton, XtNcallback, pushPrintButton, printAll);
  
  printMarkedButton
    = XtCreateManagedWidget("printMarkedButton",
			    commandWidgetClass, topPane,
			    0,0);
  XtAddCallback( printMarkedButton, XtNcallback, pushPrintButton, printMarked);
  
  printUnmarkedButton
    = XtCreateManagedWidget("printUnmarkedButton",
			    commandWidgetClass, topPane,
			    0,0);
  XtAddCallback( printUnmarkedButton, XtNcallback,
		pushPrintButton, printUnmarked);


  clearMarksButton
    = XtCreateManagedWidget("clearMarksButton",
			    commandWidgetClass, topPane,
			    0,0);
  XtAddCallback( clearMarksButton, XtNcallback,
		pushClearMarksButton, 0);
 
  XtInstallAccelerators( fileNameText, openButton );
  XtInstallAccelerators( fileNameText, copyButton );
  XtInstallAccelerators( fileNameText, closeButton );
  XtInstallAccelerators( fileNameText, cdButton );
  XtInstallAccelerators( fileNameText, pageButton );
  XtInstallAccelerators( fileNameText, printAllButton );
  XtInstallAccelerators( fileNameText, printMarkedButton );
  XtInstallAccelerators( fileNameText, printUnmarkedButton );
  XtInstallAccelerators( fileNameText, clearMarksButton );

  disablePageLevel();
  
  args = 0;
  XtSetArg(argList[args], XtNlabel, TeXInputDirNameString); args++;
  XtSetArg(argList[args], XtNresize, True); args++;
  directoryLabel = 
    XtCreateManagedWidget("directoryLabel",
			  labelWidgetClass, topPane,
			  argList, args);

  args = 0;
  XtSetArg(argList[args], XtNlabel, TeXInputFileNameString); args++;
  XtSetArg(argList[args], XtNresize, True); args++;
  fileLabel = 
    XtCreateManagedWidget("fileLabel",
			  labelWidgetClass, topPane,
			  argList, args);
}

