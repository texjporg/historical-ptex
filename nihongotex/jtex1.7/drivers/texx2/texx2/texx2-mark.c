/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 */

/*
 *	Handle printing of marked pages
 */

#include <stdio.h>
#include <X11/Xos.h>  
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <stdio.h>
#include <assert.h>
#include "dvi-simple.h"
#include "texx2.h"
#include "texx2-mark.h"

static Bool *markedPages = 0;

void
TeXMarkNewFile()
{
  int page;

  if ( markedPages ) {
    XtFree( markedPages );
    markedPages = 0;
  }

  if ( DviFile != 0 ) {
    markedPages =  (Bool *) malloc( DviTotalPages * sizeof(Bool) );
    for ( page = 0; page< DviTotalPages; page++ ) {
      markedPages[ page ] = False;
    }
  }
}

/*
 *	toggle the mark for a given page 
 */

Bool
TeXMarkToggle(page)
int page;
{
  assert( page >=0 && page < DviTotalPages );
  markedPages[ page ]  = ~markedPages[ page ];
  return( markedPages[ page ] );
}

Bool
TeXMarkSet(page)
int page;
{
  assert( page >=0 && page < DviTotalPages );
  markedPages[ page ]  = 1;
  return( markedPages[ page ] );
}

Bool
TeXMarkUnset(page)
int page;
{
  assert( page >=0 && page < DviTotalPages );
  markedPages[ page ]  = 0;
  return( markedPages[ page ] );
}

/*
 *	Simply return the state
 */
Bool
TeXMarkState(page)
int page;
{
  assert( page >=0 && page < DviTotalPages );
  return( markedPages[ page ] );
}

void
TeXMarkAll()
{
  int page;
  for (page = 0; page < DviTotalPages; page++) {
    markedPages[ page ] = True;
  }
}

void
TeXMarkNone()
{
  int page;
  for (page = 0; page < DviTotalPages; page++) {
    markedPages[ page ] = False;
  }
}

/* Print some pages. The Menu selection dictates what pages get printed */

void
  TeXMarkPrint(pd)
PrintDirective pd;
{
  char *dvips;
  int cnt;
  int i;
  char buf[20];
  char printCommand[1024];

  char tmpFileName[1024];
  
  char *fileToPrint;
  int deletePrintedFile;
  
  if ( pd == printAll ) {
    fileToPrint = DviFileName;
    deletePrintedFile = 0;
  }
  else {
    
    int dviSelectBufferLength = 2048;
    char dviSelectBuffer[ 2048 ];

#ifdef sun
    sprintf(tmpFileName,"%s.dvi", (char *) tempnam("/tmp","_texx"));
#else
    sprintf(tmpFileName,"%s.dvi", (char *) tmpnam(0));
#endif

    fileToPrint = tmpFileName;
    deletePrintedFile = 1;
    
    /* Now run dviselect to extract the pages */
    
    sprintf(dviSelectBuffer, "dviselect -i %s -o %s ",
	    DviFileName, fileToPrint);
    
    cnt = 0;
    for (i = 0; i < DviTotalPages; i++) {
      if (   (markedPages[ i ] && pd == printMarked )
	  || (!markedPages[ i ] && pd == printUnmarked )) {
	sprintf(buf, " =%d", i + 1);
	strcat(dviSelectBuffer, buf);
	cnt++;
      }
    }
    
    if (cnt != 0) {
      error(0,0, dviSelectBuffer);
      system(dviSelectBuffer);
    }
    else {
      
      /* nothing to print? */
      
      unlink( fileToPrint );
      return;
    }
  }
  
  /* Now print pages */
  
  if (TeXxResources.printProgram == 0) {
    extern char *getenv();
    dvips = getenv("DVIPRINT");
    if (dvips == NULL) {
      dvips = "lpr -d ";	/* a good guess on BSD systems */
    }
  }
  else {
    dvips = TeXxResources.printProgram;
  }

  error(0,0, "Using default ``%s'' to print\n", dvips);
  
  sprintf(printCommand, "%s %s ", dvips, fileToPrint);
  
  error(0,0,printCommand);

  system(printCommand);
  
  if ( deletePrintedFile ) {
    unlink( fileToPrint );
  }
  
}
