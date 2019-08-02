#ifndef lint
static char Xrcsid[] =
    "$XConsortium: Valuator.c,v 1.52 89/10/09 16:20:20 jim Exp $";
#endif /* lint */

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

/* ValuatorBar.c */
/* created by weissman, Mon Jul  7 13:20:03 1986 */
/* converted by swick, Thu Aug 27 1987 */

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/Xmu/Misc.h>
#include <X11/Xaw/XawInit.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Scrollbar.h>
#include "ValuatorP.h"
#include <X11/Xaw/Cardinals.h>

static void Initialize();
static void Destroy();
static void ConstraintInitialize();
static Boolean SetValues();

static char *DEFAULT_FORMAT = "%3.2f";

#define Offset(field) XtOffset(ValuatorWidget, field)

static XtResource resources[] = {
  {XtNmin, XtCMin, XtRFloat, sizeof(float),
       Offset(valuator.min), XtRString,  (caddr_t) "0.0"},
  {XtNmax, XtCMax, XtRFloat, sizeof(float),
       Offset(valuator.max), XtRString,  (caddr_t) "1.0"},
  {XtNvalue, XtCValue, XtRFloat, sizeof(float),
       Offset(valuator.value), XtRString, (caddr_t) "0.0"},
  {XtNincrement, XtCIncrement, XtRFloat, sizeof(float),
       Offset(valuator.increment), XtRString, (caddr_t) "-1"},
  {XtNformat, XtCFormat, XtRString, sizeof(String),
       Offset(valuator.format), XtRString, (caddr_t) 0},
  {XtNvalueLabel, XtCValueLabel, XtRWidget, sizeof(Widget),
       Offset(valuator.valueWidget), XtRWidget, (caddr_t) 0}
};

ValuatorClassRec valuatorClassRec = {
  {/* core fields */
    /* superclass       */      (WidgetClass) &formClassRec,
    /* class_name       */      "Valuator",
    /* size             */      sizeof(ValuatorRec),
    /* class_initialize   */    XawInitializeWidgetSet,
    /* class_part_init  */	NULL,
    /* class_inited	*/	FALSE,
    /* initialize       */      Initialize,
    /* initialize_hook  */	NULL,
    /* realize          */      XtInheritRealize,
    /* actions          */      NULL,
    /* num_actions	*/	0,
    /* resources        */      resources,
    /* num_resources    */      XtNumber(resources),
    /* xrm_class        */      NULLQUARK,
    /* compress_motion	*/	TRUE,
    /* compress_exposure*/	TRUE,
    /* compress_enterleave*/	TRUE,
    /* visible_interest */      FALSE,
    /* destroy          */      Destroy,
    /* resize           */      XtInheritResize,
    /* expose           */      XtInheritExpose,
    /* set_values       */      SetValues,
    /* set_values_hook  */	NULL,
    /* set_values_almost */	XtInheritSetValuesAlmost,
    /* get_values_hook  */	NULL,
    /* accept_focus     */      NULL,
    /* version          */	XtVersion,
    /* callback_private */      NULL,
    /* tm_table         */      NULL,
    /* query_geometry	*/	XtInheritQueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension        */	NULL
    },
  { /* composite_class fields */
    /* geometry_manager   */   XtInheritGeometryManager,
    /* change_managed     */   XtInheritChangeManaged,
    /* insert_child       */   XtInheritInsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension          */   NULL
  },
  { /* constraint_class fields */
    /* subresourses       */   NULL,
    /* subresource_count  */   0,
    /* constraint_size    */   sizeof(ValuatorConstraintsRec),
    /* initialize         */   ConstraintInitialize,
    /* destroy            */   NULL,
    /* set_values         */   NULL,
    /* extension          */   NULL
  },
  { /* form_class fields */
    /* layout             */   XtInheritLayout
  },
  { /* valuator_class fields */
    /* empty              */   0
  }
};

WidgetClass valuatorWidgetClass = (WidgetClass)&valuatorClassRec;

static void ValuatorNormalizeValue(vw)
ValuatorWidget vw;
{
  if ( vw -> valuator.increment > 0 ) {
    float value = vw -> valuator.value;
    float inc = vw -> valuator.increment;
    int chunks = ( (value)/inc )  + 0.5;
    float newValue = (chunks * inc);

    vw -> valuator.value = newValue;
  }
}

static void ValuatorUpdateValue(vw)
ValuatorWidget vw;
{
  if ( vw -> valuator.valueWidget
      && vw -> valuator.value != vw -> valuator.lastValueShown ) {
    char formatString[128];
    char *format = DEFAULT_FORMAT;
    double value = vw -> valuator.value;
    Arg argList[20];
    Cardinal args = 0;

    if ( vw -> valuator.format ) {
      format = vw -> valuator.format;
    }
    sprintf(formatString, format, value);
    args = 0;
    XtSetArg(argList[args], XtNlabel, formatString); args++;
    XtSetValues(vw -> valuator.valueWidget, argList, args);
    vw -> valuator.lastValueShown = vw -> valuator.value;
  }
}
     
static int ValuatorJumpProc(w, vw, call_data)
     Widget w;
     ValuatorWidget vw;
     float *call_data;
{
  float top = *call_data;
  float min = vw -> valuator.min;
  float max = vw -> valuator.max;
  float delta = vw -> valuator.delta;
  float value =  top * delta + min;

  vw -> valuator.value = value;
  ValuatorNormalizeValue(vw);
  ValuatorUpdateValue(vw);
}

static int ValuatorScrollProc(w, vw, call_data)
     Widget w;
     ValuatorWidget vw;
     int call_data;
{
  float inc = vw -> valuator.increment;
  float min = vw -> valuator.min;
  float max = vw -> valuator.max;
  float delta = vw -> valuator.delta;
  float value;
  float newValue;
  double top;

  ValuatorNormalizeValue(vw);

  value = vw -> valuator.value;
  newValue = (call_data < 0) ? (value + inc) : (value - inc);

  if ( newValue < min ) newValue = min;
  if ( newValue > max ) newValue = max;
  top = (newValue - min) / delta;

  XawScrollbarSetThumb(vw -> valuator.scrollWidget, top, -1.0);

  vw -> valuator.value = newValue;
  ValuatorUpdateValue(vw);
}

/* ARGSUSED */
static void Initialize( request, new )
     Widget request;		/* what the client asked for */
     Widget new;		/* what we're going to give him */
{
  ValuatorWidget rw = (ValuatorWidget) request;
  ValuatorWidget vw = (ValuatorWidget) new;
  Arg argList[20];
  Cardinal args = 0;
  char formatString[128];
  double top;
  double thumb;
  char *format;
  
  static XtCallbackRec jumpCallbacks[] = {
    {(XtCallbackProc) ValuatorJumpProc, NULL},
    {(XtCallbackProc) NULL, NULL}};

  static XtCallbackRec scrollCallbacks[] = {
    {(XtCallbackProc) ValuatorScrollProc, NULL},
    {(XtCallbackProc) NULL, NULL}};

  jumpCallbacks[0].closure = (XtPointer) new;
  scrollCallbacks[0].closure = (XtPointer) new;

  /*
   * Copy & normalize values
   */

  vw -> valuator.min = rw -> valuator.min;
  vw -> valuator.max = rw -> valuator.max;
  vw -> valuator.value = rw -> valuator.value;
  vw -> valuator.increment = rw -> valuator.increment;

  if ( rw -> valuator.format ) {
    int len = strlen(rw -> valuator.format);
    vw -> valuator.format = XtMalloc(len+1);
    strcpy(vw -> valuator.format, rw -> valuator.format);
  }

  format = DEFAULT_FORMAT;
  if ( vw -> valuator.format ) {
    format = vw -> valuator.format;
  }

  if ( vw ->  valuator.min > vw ->  valuator.max ) {
    float t = vw ->  valuator.max;
    vw ->  valuator.max = vw ->  valuator.min;
    vw ->  valuator.min = t;
  }

  vw -> valuator.delta = vw -> valuator.max - vw -> valuator.min;
  
  if ( vw ->  valuator.value < vw ->  valuator.min ) {
    vw ->  valuator.value = vw ->  valuator.min;
  }
  
  if ( vw ->  valuator.value > vw ->  valuator.max ) {
    vw ->  valuator.value = vw ->  valuator.max;
  }

  ValuatorNormalizeValue(vw);

  if ( vw -> valuator.valueWidget ) {
    args = 0;
    sprintf( formatString, format, vw -> valuator.value );
    XtSetArg(argList[args], XtNlabel, formatString); args++;
    XtSetValues( vw -> valuator.valueWidget, argList, args);
  }

  args = 0;
  XtSetArg(argList[args], XtNborderWidth, 0); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetArg(argList[args], XtNjustify, XtJustifyRight); args++;
  sprintf( formatString, format, vw -> valuator.min );
  XtSetArg(argList[args], XtNlabel, formatString); args++;
  vw -> valuator.minLabelWidget =
    XtCreateManagedWidget("minLabel", labelWidgetClass,
			  new, argList, args);
  args = 0;
  XtSetArg(argList[args], XtNborderWidth, 0); args++;
  XtSetArg(argList[args], XtNfromHoriz, vw -> valuator.minLabelWidget); args++;
  XtSetArg(argList[args], XtNleft, XtRubber); args++;
  XtSetArg(argList[args], XtNright, XtRubber); args++;
  XtSetArg(argList[args], XtNjumpProc, jumpCallbacks); args++;
  XtSetArg(argList[args], XtNscrollProc, scrollCallbacks); args++;
  XtSetArg(argList[args], XtNorientation, XtorientHorizontal);  args++;
  XtSetArg(argList[args], XtNlength, 100);  args++;
  vw -> valuator.scrollWidget =
    XtCreateManagedWidget("scrollbar", scrollbarWidgetClass,
			  new, argList, args);
  
  if (vw -> valuator.max == vw -> valuator.min) {
    top = 0;
  } else {
    top = (vw -> valuator.value - vw -> valuator.min)
      / (vw -> valuator.max - vw -> valuator.min);
  }

  thumb = -1.0;

  if ( vw -> valuator.increment > 0 && vw -> valuator.delta != 0) {
    thumb = vw -> valuator.increment / vw -> valuator.delta;
  }
  XawScrollbarSetThumb( vw -> valuator.scrollWidget, top, thumb);

  args = 0;
  XtSetArg(argList[args], XtNborderWidth, 0); args++;
  XtSetArg(argList[args], XtNfromHoriz, vw -> valuator.scrollWidget); args++;
  XtSetArg(argList[args], XtNjustify, XtJustifyRight); args++;
  XtSetArg(argList[args], XtNright, XtChainRight); args++;
  sprintf( formatString, format, vw -> valuator.max );
  XtSetArg(argList[args], XtNlabel, formatString); args++;
  vw -> valuator.maxLabelWidget =
    XtCreateManagedWidget("maxLabel", labelWidgetClass,
			  new, argList, args);
  
  ValuatorUpdateValue(vw);
}

/* ARGSUSED */
static void ConstraintInitialize(request, new)
    Widget request, new;
{
}

/* ARGSUSED */
static void Destroy(current)
ValuatorWidget current;
{
  ValuatorPart *v = &(current -> valuator);
  if ( v -> scrollWidget ) XtDestroyWidget( v -> scrollWidget );
  if ( v -> minLabelWidget ) XtDestroyWidget( v -> minLabelWidget );
  if ( v -> maxLabelWidget ) XtDestroyWidget( v -> maxLabelWidget );
  if ( v -> format ) XtFree( v -> format );
}

/* ARGSUSED */
static Boolean SetValues( current, request, desired )
     Widget current,		/* what I am */
       request,		/* what he wants me to be */
       desired;		/* what I will become */
{
  ValuatorWidget w = (ValuatorWidget) current;
  ValuatorWidget rw = (ValuatorWidget) request;
  ValuatorWidget vw = (ValuatorWidget) desired;
  Boolean redraw = FALSE;
  Boolean realized = XtIsRealized (desired);
  Arg argList[5];
  Cardinal args;
  char formatString[128];
  char *format;

  vw -> valuator.value = rw -> valuator.value;
  vw -> valuator.min = rw -> valuator.min;
  vw -> valuator.max = rw -> valuator.max;
  vw -> valuator.increment = rw -> valuator.increment;

  if ( !vw -> valuator.format
      || ! strcmp(rw -> valuator.format, vw -> valuator.format) ) {

    int len = strlen(rw -> valuator.format);
    XtFree( vw -> valuator.format );
    vw -> valuator.format = XtMalloc(len+1);
    strcpy(vw -> valuator.format, rw -> valuator.format);
  }
  
  if ( vw -> valuator.min > vw -> valuator.max ) {
    double t = vw -> valuator.max;
    vw -> valuator.max = vw -> valuator.min;
    vw -> valuator.min = t;
  }

  vw -> valuator.delta = vw -> valuator.max - vw -> valuator.min;
  
  if ( vw -> valuator.value < vw -> valuator.min ) {
    vw -> valuator.value = vw -> valuator.min;
  }
  
  if ( vw -> valuator.value > vw -> valuator.max ) {
    vw -> valuator.value = vw -> valuator.max;
  }
  
  /*
   * repaint labels & scroll bar if values changed
   */

  format = DEFAULT_FORMAT;
  if ( vw -> valuator.format ) {
    format = vw -> valuator.format;
  }
  
  if ( vw -> valuator.min != w -> valuator.min ) {
    sprintf( formatString, format, vw -> valuator.min );
    args = 0;
    XtSetArg(argList[args], XtNlabel, formatString); args++;
    XtSetValues( w -> valuator.minLabelWidget, argList, args);
    redraw = TRUE;
  }
  
  if ( vw -> valuator.max != w -> valuator.max ) {
    sprintf( formatString, format, vw -> valuator.max );
    args = 0;
    XtSetArg(argList[args], XtNlabel, formatString); args++;
    XtSetValues( w -> valuator.maxLabelWidget, argList, args);
    redraw = TRUE;
  }
  
  if ( vw -> valuator.value != w -> valuator.value
      || vw -> valuator.increment != w -> valuator.increment
      || vw -> valuator.delta != w -> valuator.delta ) {

    double top = (vw -> valuator.value - vw -> valuator.min)
      / (vw -> valuator.max - vw -> valuator.min);

    double thumb = -1.0;
    if ( vw -> valuator.increment > 0 ) {
      thumb = vw -> valuator.increment / vw -> valuator.delta;
    }
    XawScrollbarSetThumb( w -> valuator.scrollWidget, top, thumb);
    ValuatorNormalizeValue(w);
    ValuatorUpdateValue(vw);
    redraw = TRUE;
  }
  
  return( redraw );
}
