/*
* $XConsortium: Valuator.h,v 1.24 89/08/24 16:37:44 kit Exp $
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

#ifndef _Valuator_h
#define _Valuator_h

/****************************************************************
 *
 * Valuatorbar Widget
 *
 ****************************************************************/

#include <X11/Xmu/Converters.h>

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		White
 border		     BorderColor	Pixel		Black
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Function		NULL
 foreground	     Color		Pixel		Black
 height		     Height		Dimension	length or thickness
 jumpProc	     Callback		Function	NULL
 length		     Length		Dimension	1
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 orientation	     Orientation	XtOrientation	XtorientVertical
 reverseVideo	     ReverseVideo	Boolean		False
 ValuatorDCursor	     Cursor		Cursor		XC_sb_down_arrow
 ValuatorHCursor	     Cursor		Cursor		XC_sb_h_double_arrow
 ValuatorLCursor	     Cursor		Cursor		XC_sb_left_arrow
 ValuatorProc	     Callback		Function	NULL
 ValuatorRCursor	     Cursor		Cursor		XC_sb_right_arrow
 ValuatorUCursor	     Cursor		Cursor		XC_sb_up_arrow
 ValuatorVCursor	     Cursor		Cursor		XC_sb_v_double_arrow
 sensitive	     Sensitive		Boolean		True
 shown		     Shown		float		0.0
 thickness	     Thickness		Dimension	14
 thumb		     Thumb		Pixmap		Grey
 thumbProc	     Callback		Function	NULL
 top		     Top		float		0.0
 width		     Width		Dimension	thickness or length
 x		     Position		Position	0
 y		     Position		Position	0

*/

#define XtCMin "Min"
#define XtNmin "min"

#define XtCMax "Max"
#define XtNmax "max"

#ifndef XtCValue
#  define XtCValue "Value"
#  define XtNvalue "value"
#endif

#define XtCIncrement "Increment"
#define XtNincrement "increment"

#define XtCFormat "Format"
#define XtNformat "format"

#define XtCValueLabel "ValueLabel"
#define XtNvalueLabel "valueLabel"

typedef struct _ValuatorRec	  *ValuatorWidget;
typedef struct _ValuatorClassRec *ValuatorWidgetClass;

extern WidgetClass valuatorWidgetClass;

typedef union
{
  int i;
  float f;
} IntFloatUnion;

#endif /* _Valuator_h */
