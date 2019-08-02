/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 */

/* Std system and C header files */

#ifndef _tex2h_
#define _tex2h_

#include <stdio.h>
#include <X11/Xos.h>
#include <sys/dir.h>
#include <sys/stat.h>

/* X include files */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

/* X toolkit header files */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

/* Widget header files. */

#include <X11/AsciiText.h>
#include <X11/Box.h>
#include <X11/Command.h>
#include <X11/Scroll.h>
#include <X11/Shell.h>
#include <X11/Viewport.h>

#include "texx2-defs.h"

extern char *ProgName;

typedef struct  {
    char *printProgram;
    int userMag;
    int dpi;
    Bool autoSize;
    int largeMag;
    int smallMag;

    int pageMaxWd;
    int pageMaxHt;

    char *paperWidthString;
    char *paperHeightString;

    char *leftOffsetString;
    char *topOffsetString;

    char *helpText;

    Bool useButtons;

} TeXxResourceStruct;

extern TeXxResourceStruct TeXxResources;

#endif
