/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 */

/*
 * Print an error message with an optional system error number, and
 * optionally quit.
 *
 */

#include <stdio.h>
#include <varargs.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/ShellP.h>
#include <X11/AsciiText.h>
#include "texx2.h"
#include "texx2-widgets.h"

#ifdef __STDC__
  extern char* malloc(int);
  extern char* realloc(char *, int);
#else
  extern char* malloc();
  extern char* realloc();
#endif

#define ERROR_BUFFER_SIZE 2048
static char errorMessageBuffer[ ERROR_BUFFER_SIZE ];
static int errorMessageBufferLength=0;

static message(str,quit)
     char *str;
     int quit;
{
  
  if ( errorText ) {
    int len = strlen(str);
    char *p;
    XtTextPosition start;
    XtTextPosition end;
    XtTextBlock tblk;

    tblk.firstPos = 0;
    tblk.length = len;
    tblk.ptr = str;
    tblk.format = FMT8BIT;

    if ( len > 0 && str[len-1] != '\n' ) {
      str[len] = '\n';
      str[len+1] = 0;
      tblk.length = len+1;
    }
    
    start = strlen( errorMessageBuffer );

    if ( start + len > ERROR_BUFFER_SIZE ) {
      start = 0;
      bzero( errorMessageBuffer, ERROR_BUFFER_SIZE );
    }

    end = start;

    XtTextDisableRedisplay(errorText);

    XtTextSetInsertionPoint(errorText, start);
    XtTextSetLastPos( errorText, start );
    
    XtTextReplace( errorText, start, end, &tblk );

    XtTextEnableRedisplay(errorText);

  }
}

static void
pushClearButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  bzero( errorMessageBuffer , ERROR_BUFFER_SIZE );

  XtTextDisableRedisplay(errorText);
  XtTextSetInsertionPoint(errorText, 0);
  XtTextSetLastPos( errorText, 0 );
  XtTextEnableRedisplay(errorText);
} 

/*
 *	If you had a real compiler, these would all be 
 *	local the next function
 */

static XtCallbackRec clearButtonCallBacks [] = {
  {pushClearButton,NULL},
  {NULL, NULL}
};

static Arg clearButtonArgs[] = {
  {XtNcallback, (XtArgVal) clearButtonCallBacks}
};

static Arg textArgs[] = {
  {XtNeditType, (XtArgVal) XttextEdit},
  {XtNstring, (XtArgVal) errorMessageBuffer},
  {XtNlength, (XtArgVal) ERROR_BUFFER_SIZE},
  {XtNtextOptions, (XtArgVal) (scrollVertical | scrollOnOverflow | wordBreak )}
};

void
BuildErrorBox()
{

  if (TeXxResources.helpText) {
    strncpy(errorMessageBuffer, TeXxResources.helpText, ERROR_BUFFER_SIZE-1);
  }

  clearButton =
    XtCreateManagedWidget("dialogClear",
			  commandWidgetClass, topPane,
			  clearButtonArgs, XtNumber(clearButtonArgs));
  errorText = 
    XtCreateManagedWidget("dialogText",
			  asciiStringWidgetClass, topPane,
			  textArgs, XtNumber(textArgs));
}


#ifdef lint

/* VARARGS3 ARGSUSED */
error(quit, e, fmt) int quit, e; char *fmt; {;}

/* VARARGS1 ARGSUSED */
panic(fmt) char *fmt; { exit(1); /* NOTREACHED */ }

#else lint

extern char *ProgName;
extern int errno;
extern char *sys_errlist[];
extern int sys_nerr;

#define ERROR_MSG_LEN 1024
static char ErrorMsgBuffer[ ERROR_MSG_LEN ];

error(va_alist)
     va_dcl
{
  va_list l;
  int quit, e;
  char *fmt;
  char *p;

  p = ErrorMsgBuffer;

  va_start(l);
  /* pick up the constant arguments: quit, errno, printf format */
  quit = va_arg(l, int);
  e = va_arg(l, int);
  if (e < 0)
    e = errno;

  fmt = va_arg(l, char *);

#if defined(sun)
  (void) vsprintf(p, fmt, l);
#else
  /*	Does someone know how to do this properly? */

  {
    struct _iobuf ungodlyhack;
    ungodlyhack._cnt = ERROR_MSG_LEN;
    ungodlyhack._ptr = p;
    ungodlyhack._base = p;
    ungodlyhack._bufsiz = ERROR_MSG_LEN;
    ungodlyhack._flag = _IOSTRG;
    ungodlyhack._file = 255;
    _doprnt(fmt, l, &ungodlyhack);
  }
#endif

  va_end(l);

  message( ErrorMsgBuffer, quit);
}

panic(va_alist)
     va_dcl
{
  va_list l;
  char *fmt;
  char *p;

  p = ErrorMsgBuffer;
  sprintf(p, "panic: ");
  p += strlen(p);

  va_start(l);
  /* pick up the constant arguments: quit, errno, printf format */
  fmt = va_arg(l, char *);

#if defined(sun)
  (void) vsprintf(p, fmt, l);
#else
  /*	Does someone know how to do this properly? */

  {
    struct _iobuf ungodlyhack;
    ungodlyhack._cnt = ERROR_MSG_LEN;
    ungodlyhack._ptr = p;
    ungodlyhack._base = p;
    ungodlyhack._bufsiz = ERROR_MSG_LEN;
    ungodlyhack._flag = _IOSTRG;
    ungodlyhack._file = 255;
    _doprnt(fmt, l, &ungodlyhack);
  }
#endif

  va_end(l);

  message(ErrorMsgBuffer, 1);
}

#endif /* lint */
