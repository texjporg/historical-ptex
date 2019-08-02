/*
 * $XConsortium: ScrollP.h,v 1.20 89/08/24 16:37:46 kit Exp $
 */


/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifndef _ValuatorP_h
#define _ValuatorP_h

#include "Valuator.h"

#include <X11/Xaw/FormP.h>

typedef struct {int empty;} ValuatorClassPart;

typedef struct _ValuatorClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    ConstraintClassPart	constraint_class;
    FormClassPart	form_class;
    ValuatorClassPart		Valuator_class;
} ValuatorClassRec;

extern ValuatorClassRec valuatorClassRec;

typedef struct _ValuatorPart {
    float min;
    float max;
    float delta;
    float value;
    float lastValueShown;
    float increment;
    char *format;
    Widget valueWidget;
    Widget scrollWidget;
    Widget minLabelWidget;
    Widget maxLabelWidget;
} ValuatorPart;

typedef struct _ValuatorRec {
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    FormPart		form;
    ValuatorPart	valuator;
} ValuatorRec;

typedef struct {int empty;} ValuatorConstraintsPart;

typedef struct _ValuatorConstraintsRec {
    FormConstraintsPart	  form;
    ValuatorConstraintsPart valuator;
} ValuatorConstraintsRec, *ValuatorConstraints;


#endif /* _ValuatorP_h */
