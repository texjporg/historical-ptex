/*
 * $XConsortium: EzMenP.h,v 1.12 89/12/11 15:01:39 kit Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/*
 * EzMenuP.h - Private Header file for EzMenu widget.
 *
 * Date:    April 3, 1989
 *
 * By:      Chris D. Peterson
 *          MIT X Consortium
 *          kit@expo.lcs.mit.edu
 */

#ifndef _EzMenuP_h
#define _EzMenuP_h

#include <X11/Xaw/SimpleMenP.h>
#include "EzMenu.h"

typedef struct {
    XtPointer extension;		/* For future needs. */
} EzMenuClassPart;

typedef struct _EzMenuClassRec {
  CoreClassPart	          core_class;
  CompositeClassPart      composite_class;
  ShellClassPart          shell_class;
  OverrideShellClassPart  override_shell_class;
  SimpleMenuClassPart	  simpleMenu_class;
  EzMenuClassPart	  ezMenu_class;
} EzMenuClassRec;

extern EzMenuClassRec ezMenuClassRec;

typedef struct _EzMenuPart {
    char *menu;
} EzMenuPart;

typedef struct _EzMenuRec {
  CorePart		core;
  CompositePart 	composite;
  ShellPart 	        shell;
  OverrideShellPart     override;
  SimpleMenuPart	simple_menu;
  EzMenuPart	ez_menu;
} EzMenuRec;

#endif /* _EzMenuP_h */
