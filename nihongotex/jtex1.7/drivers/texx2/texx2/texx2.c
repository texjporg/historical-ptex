/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 */

#include "texx2.h"
#include "texx2-mark.h"

char *ProgName, *ClassName;

extern void BuildTopLevelWidget();

TeXxResourceStruct TeXxResources;

#define Offset(field) (XtOffset(TeXxResourceStruct *, field))

static XtResource resources[] = {
  {"print", XtCString, XtRString, sizeof(char *),
     Offset(printProgram), XtRString, "lpr -d"},

  {"mag", XtCString, XtRInt, sizeof(int),
     Offset(userMag), XtRString, "500"},

  {"topOffset", XtCString, XtRString, sizeof(char *),
     Offset(topOffsetString), XtRString, "1"},

  {"leftOffset", XtCString, XtRString, sizeof(char *),
     Offset(leftOffsetString), XtRString, "1"},

  {"paperWidth", XtCString, XtRString, sizeof(char *),
     Offset(paperWidthString), XtRString, "8.5"},

  {"paperHeight", XtCString, XtRString, sizeof(char *),
     Offset(paperHeightString), XtRString, "11.5"},

  {"smallMag", XtCString, XtRInt, sizeof(int),
     Offset(smallMag), XtRString, "333"},

  {"largeMag", XtCString, XtRInt, sizeof(int),
     Offset(largeMag), XtRString, "500"},

  {"dpi", XtCString, XtRInt, sizeof(int),
     Offset(dpi), XtRString, "300"},

  {"pageMaxWd", XtCString, XtRInt, sizeof(int),
     Offset(pageMaxWd), XtRString, "0"},

  {"pageMaxHt", XtCString, XtRInt, sizeof(int),
     Offset(pageMaxHt), XtRString, "0"},

  {"autoSize", XtCString, XtRBool, sizeof(Bool),
     Offset(autoSize), XtRString, "True"},

  {"useButtons", XtCString, XtRBool, sizeof(Bool),
     Offset(useButtons), XtRString, "True"},

/*
 *	the following options overide various globals, but aren't set
 *	via command line switches.
 */
  {"helpText", "HelpText",
     XtRString, sizeof(String),
     Offset(helpText),
     XtRString, "Warning -- Someone forgot to install APPDEFAULTSDIR/Texx2"}
};

static XrmOptionDescRec Options[] = {
  {"-print", "*.print", XrmoptionSepArg, (caddr_t) NULL},
  {"-mag", "*.mag", XrmoptionSepArg, (caddr_t) NULL},
  {"-topOffset", "*.topOffset", XrmoptionSepArg, (caddr_t) NULL},
  {"-leftOffset", "*.leftOffset", XrmoptionSepArg, (caddr_t) NULL},
  {"-paperWidth", "*.paperWidth", XrmoptionSepArg, (caddr_t) NULL},
  {"-paperHeight", "*.paperHeight", XrmoptionSepArg, (caddr_t) NULL},
  {"-smallMag", "*.smallMag", XrmoptionSepArg, (caddr_t) NULL},
  {"-largeMag", "*.largeMag", XrmoptionSepArg, (caddr_t) NULL},
  {"-dpi", "*.dpi", XrmoptionSepArg, (caddr_t) NULL},
  {"-pageMaxWd", "*.pageMaxWd", XrmoptionSepArg, (caddr_t) NULL},
  {"-pageMaxHt", "*.pageMaxHt", XrmoptionSepArg, (caddr_t) NULL},
  {"-autoSize", "*.autoSize", XrmoptionSepArg, (caddr_t) NULL},
  {"-useButtons", "*.useButtons", XrmoptionSepArg, (caddr_t) NULL}
};

int Argc;
char **Argv;

Widget TopLevelWidget;

void 
main(argc,argv)
char ** argv;
int argc;
{

  Argc = argc;
  Argv = argv;

  ClassName = "Texx2";

  if( (ProgName = index(argv[0], '/')) == NULL )
	  ProgName = argv[0];
  
  TopLevelWidget = XtInitialize(ProgName, ClassName,
				Options, XtNumber(Options),
				&Argc, Argv);

  XtGetApplicationResources(TopLevelWidget,
			    (caddr_t) &TeXxResources, 
			    resources, XtNumber(resources),
			    NULL, (Cardinal) 0);

  TeXFileInstallActions();
  TeXPageInstallActions();

  BuildTopLevelWidget(TopLevelWidget);
  XtRealizeWidget(TopLevelWidget);

  if ( Argc > 1 ) {
    register char *p;

    /* set the directory name */
    setDirectory( Argv[1] );

    /* peel off directory names & set the file name with the remainder */

    p = Argv[1];
    p += strlen(Argv[1]) - 1;
    while( p != Argv[1] && *p != '/' )
      --p;
    if( *p == '/' ) p++;
    setFileName( p );
  }
  else {
    setDirectory(".");
  }

  if ( Argc > 2 ) {
    error(0,0,"Additional arguments ignored");
  }

  XtMainLoop();
}

