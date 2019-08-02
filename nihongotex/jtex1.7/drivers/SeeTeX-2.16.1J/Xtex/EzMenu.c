#if ( !defined(lint) && !defined(SABER) )
static char Xrcsid[] = "$XConsortium: SimpleMenu.c,v 1.32 89/12/11 15:01:50 kit Exp $";
#endif 

/*
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
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>  
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include "EzMenuP.h"
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h>
#include "EzME.h"
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>

#include <X11/Xaw/Cardinals.h>

#include <ctype.h>

#define offset(field) XtOffset(EzMenuWidget, ez_menu.field)

static XtResource resources[] = { 
  {XtNmenu,  XtCMenu, XtRString, sizeof(String),
     offset(menu), XtRString, NULL},
};  
#undef offset


/*
 * Semi Public function definitions. 
 */

static void Initialize();
static void Destroy();
static void Realize();
static Boolean SetValues();

#define superclass (&simpleMenuClassRec)
    
EzMenuClassRec ezMenuClassRec = {
  {
    /* superclass         */    (WidgetClass) superclass,
    /* class_name         */    "EzMenu",
    /* size               */    sizeof(EzMenuRec),
    /* class_initialize   */	NULL,
    /* class_part_initialize*/	NULL,
    /* Class init'ed      */	FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */	NULL,
    /* realize            */    Realize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    resources,
    /* resource_count     */	XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    TRUE, 
    /* compress_exposure  */    TRUE,
    /* compress_enterleave*/ 	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    Destroy,
    /* resize             */    XtInheritResize,
    /* expose             */    XtInheritExpose,
    /* set_values         */    SetValues,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */	XtInheritSetValuesAlmost,  
    /* get_values_hook    */	NULL,			
    /* accept_focus       */    NULL,
    /* intrinsics version */	XtVersion,
    /* callback offsets   */    NULL,
    /* tm_table		  */    XtInheritTranslations,
    /* query_geometry	  */    XtInheritQueryGeometry,
    /* display_accelerator*/    XtInheritDisplayAccelerator,
    /* extension	  */    NULL
  },{
    /* geometry_manager   */    XtInheritGeometryManager,
    /* change_managed     */    XtInheritChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension	  */    NULL
  },{
    /* Shell extension	  */    NULL
  },{
    /* Override extension */    NULL
  },{
    /* Simple Menu extension*/  NULL
  },{
    /* Ez Menu extension*/  NULL
  }
};

WidgetClass ezMenuWidgetClass = (WidgetClass)&ezMenuClassRec;

/************************************************************
 *
 * Semi-Public Functions.
 *
 ************************************************************/

/*
 * routines to process menus
 */

#define QUOTE '\"'
#define BACKSLASH '\\'

#define SKIPWHITE(cp) while (cp && *cp && isspace(*cp)) cp++
#define FINDPAREN(cp) while (cp && *cp && *cp != '(') cp++
#define FINDQUOTE(cp) while (cp && *cp && *cp != QUOTE) cp++
#define KEY_LEN 32
#define NAME_LEN 128
#define LABEL_LEN 128

/*
 * Parse a main level menu decl. and add all children to that parent
 */

char *
EzMenuParseString(from, to, maxlen)
char *from;
char *to;
int maxlen;
{
  char *oldto = to;
  SKIPWHITE(from);
  if ( from && *from == QUOTE ) {
    maxlen--;
    from++;
    while (*from && *from != QUOTE && maxlen > 1) {
      if ( *from == BACKSLASH) {
	from++;
      }
      *to++ = *from++;
      maxlen--;
    }
    if ( *from == QUOTE ) {
      from++;
    }
  }
  *to = 0;
  return(from);
}  

char *
EzMenuParseMenu(parent,menu,subMenuOk)
Widget parent;
char *menu;
Bool subMenuOk;
{
  char *thisWidgetName[NAME_LEN];
  char *thisWidgetLabel[LABEL_LEN];

  *thisWidgetName = 0;
  *thisWidgetLabel = 0;
  while(menu && *menu) {
    SKIPWHITE(menu);

    if (!menu || *menu != '(') {
      return( menu );
    }
    else {
      menu++;
    }

    SKIPWHITE(menu);

    if (isalpha(*menu)) {
      char key[KEY_LEN+1];
      char *p = key;
      int keylen = 0;

      while(isalpha(*menu) && keylen < KEY_LEN) {
	*p++ = *menu++;
	keylen++;
      }
      *p = 0;

      if (strcmp(key, "action") == 0 ) {
	char name[NAME_LEN];
	Arg arglist[1];
	Cardinal num_args = 0;

	menu = EzMenuParseString(menu, name, NAME_LEN);

	XtSetArg(arglist[num_args], XtNaction, name); num_args++;
	XtSetValues(parent, arglist, num_args);
      }
      else if (strcmp(key, "blank") == 0 ) {
	char name[NAME_LEN];
	Widget entry;

	*name = 0;
	menu = EzMenuParseString(menu, name, NAME_LEN);
	if ( *name == 0 ) {
	  static int counter = 0;
	  sprintf(name, "blank%d", counter);
	}
	entry = XtCreateManagedWidget(name, smeObjectClass,
				      parent, NULL, ZERO);
      }
      else if (strcmp(key, "line") == 0 ) {
	char name[NAME_LEN];
	Widget entry;

	*name = 0;
	menu = EzMenuParseString(menu, name, NAME_LEN);
	if ( *name == 0 ) {
	  static int counter = 0;
	  sprintf(name, "line%d", counter);
	}
	entry = XtCreateManagedWidget(name, smeLineObjectClass, parent,
				      NULL, ZERO);
      }
      else if (strcmp(key, "item") == 0 || strcmp(key, "menu") == 0 ) {
	char name[NAME_LEN];
	char label[LABEL_LEN];
	Widget entry;
	Bool isMenu;
	extern void MenuSelect();
	Arg arglist[10];
	Cardinal num_args = 0;


	isMenu = (strcmp(key, "menu") == 0);
	*name = 0;

	menu = EzMenuParseString(menu, label, LABEL_LEN);
	menu = EzMenuParseString(menu, name, NAME_LEN);

	if ( *name == 0 ) {
	  static int counter = 0;
	  sprintf(name, "%s%d", key, counter);
	}

	if ( *label == 0 ) {
	  strcpy(label, name);
	}

	if ( isMenu ) {
	  char menu_name[NAME_LEN];
	  Widget menu_entry;

	  sprintf(menu_name,"%s-menu", name);

	  XtSetArg(arglist[num_args], XtNlabel, label); num_args++;

	  menu_entry = XtCreatePopupShell(menu_name, ezMenuWidgetClass,
			     parent, arglist, num_args);

	  /* now, parse their menu entry. This is gross, but beats
	   scanning ahead to find the stupid balanced parens */
	  menu = EzMenuParseMenu(menu_entry, menu, True);

	  num_args = 0;
	  XtSetArg(arglist[num_args], XtNlabel, label); num_args++;
	  XtSetArg(arglist[num_args], XtNmenu, menu_entry); num_args++;
	  entry = XtCreateManagedWidget(name, ezMEObjectClass, parent, 
					arglist, num_args);
	}
	else {
	  num_args = 0;
	  XtSetArg(arglist[num_args], XtNlabel, label); num_args++;
	  XtSetArg(arglist[num_args], XtNmenu, 0); num_args++;
	  entry = XtCreateManagedWidget(name, ezMEObjectClass, parent, 
					arglist, num_args);
	}

	menu = EzMenuParseMenu(entry, menu, isMenu);
      }

      SKIPWHITE(menu);
      if (*menu != ')') {
	char buffer[2048];
	XtAppContext context = 
	  XtWidgetToApplicationContext( parent );

	sprintf(buffer, "unbalanced parens or bogus format: %s\n", menu);

	XtAppError(context, buffer);
	exit(1);
      }
      menu++;
    }
  }
  return(menu);
}

/*      Function Name: Initialize
 *      Description: Initializes the simple menu widget
 *      Arguments: request - the widget requested by the argument list.
 *                 new     - the new widget with both resource and non
 *                           resource values.
 *      Returns: none.
 */

/* ARGSUSED */
static void
Initialize(request, new)
Widget request, new;
{
  EzMenuWidget newez = (EzMenuWidget) new;
  EzMenuWidget requestez = (EzMenuWidget) request;

#ifdef DEBUG
  fprintf(stderr,"Initialize %s\n",
	  XtWidgetName(request);
#endif

  /* just copy the string that will be used to initialize the menu */
  
  newez -> ez_menu.menu = 0;

  if (requestez -> ez_menu.menu ) {
    int len = strlen(requestez -> ez_menu.menu);
    newez -> ez_menu.menu = XtMalloc( len + 1 );
    strcpy( newez -> ez_menu.menu, requestez -> ez_menu.menu);
  }

  if ( newez -> ez_menu.menu ) {
    EzMenuParseMenu( newez, newez -> ez_menu.menu, True );
  }
}

/*      Function Name: Realize
 *      Description: Realizes the widget.
 *      Arguments: w - the simple menu widget.
 *                 mask - value mask for the window to create.
 *                 attrs - attributes for the window to create.
 *      Returns: none
 */

static void
Realize(w, mask, attrs)
Widget w;
XtValueMask * mask;
XSetWindowAttributes * attrs;
{
  EzMenuWidget newez = (EzMenuWidget) w;
  (*superclass->core_class.realize) (w, mask, attrs);
}


/*      Function Name: SetValues
 *      Description: Relayout the menu when one of the resources is changed.
 *      Arguments: current - current state of the widget.
 *                 request - what was requested.
 *                 new - what the widget will become.
 *      Returns: none
 */

/* ARGSUSED */
static Boolean
SetValues(current, request, new)
     Widget current, request, new;
{
  EzMenuWidget ez_old = (EzMenuWidget) current;
  EzMenuWidget ez_want = (EzMenuWidget) request;
  EzMenuWidget ez_new = (EzMenuWidget) new;
  Boolean ret_val = FALSE;
  if (

      /* no old menu, want one now */

      ( !ez_old -> ez_menu.menu && ez_want -> ez_menu.menu )
      ||

      /* old menu, new one is different */

      (ez_want -> ez_menu.menu
       && ez_old -> ez_menu.menu
       && ( strcmp( ez_old -> ez_menu.menu,
		   ez_want -> ez_menu.menu) != 0 ))
      ) {

    int len = strlen(ez_old -> ez_menu.menu);

    ez_new -> ez_menu.menu = XtMalloc( len + 1 );
    strcpy( ez_new -> ez_menu.menu,
	   ez_old -> ez_menu.menu);
    
    if ( XtIsRealized(ez_old) ) {
      SimpleMenuWidget smw = (SimpleMenuWidget) ez_old;
      SmeObject *entry;
      
      ForAllChildren(smw, entry) {
	XtDestroyWidget(entry);
      }

      EzMenuParseMenu(ez_new, ez_new -> ez_menu.menu, True);
      ret_val = TRUE;
    }
  }
  return(ret_val);
}

static void
Destroy(who)
EzMenuWidget who;
{
  XtFree( who -> ez_menu.menu );
}

