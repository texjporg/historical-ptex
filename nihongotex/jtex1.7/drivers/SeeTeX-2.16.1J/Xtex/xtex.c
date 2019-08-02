/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

#include "xtex.h"
#include "mark.h"
#include <string.h>
#include "dvi-simple.h"

char *ProgName = "xtex";
char *ClassName = "Xtex";

extern void BuildTopLevelWidget();

xtexResourceStruct xtexResources;

#define Offset(field) (XtOffset(xtexResourceStruct *, field))

static XtResource resources[] = {

/*
 * Some variables for sanity checking
 */

  {"incorrectlyInstalled", XtCString, XtRBool, sizeof(Bool),
     Offset(incorrectlyInstalled), XtRString, "True"},

  {"programVersion", XtCString, XtRFloat, sizeof(float),
     Offset(programVersion), XtRString, XTEX_VERSION},

  {"resourcesVersion", XtCString, XtRFloat, sizeof(float),
     Offset(resourcesVersion), XtRString, "0.0"},

  {"print", XtCString, XtRString, sizeof(char *),
     Offset(printProgram), XtRString, "lpr -d"},

  {"tfmPath", XtCString, XtRString, sizeof(char *),
     Offset(tfmPath), XtRString, ""},

  {"xFontPath", XtCString, XtRString, sizeof(char *),
     Offset(xFontPath), XtRString, ""},

  {"mag", XtCString, XtRInt, sizeof(int),
     Offset(userMag), XtRString, "500"},

  {"topOffset", XtCString, XtRFloat, sizeof(float),
     Offset(topOffset), XtRString, "1"},

  {"voffset", XtCString, XtRFloat, sizeof(float),
     Offset(topOffset), XtRString, "1"},

  {"leftOffset", XtCString, XtRFloat, sizeof(float),
     Offset(leftOffset), XtRString, "1"},

  {"hoffset", XtCString, XtRFloat, sizeof(float),
     Offset(leftOffset), XtRString, "1"},

  {"paperWidth", XtCString, XtRFloat, sizeof(float),
     Offset(paperWidth), XtRString, "8.5"},

  {"paperHeight", XtCString, XtRFloat, sizeof(float),
     Offset(paperHeight), XtRString, "11"},

  {"smallMag", XtCString, XtRInt, sizeof(int),
     Offset(smallMag), XtRString, "333"},

  {"largeMag", XtCString, XtRInt, sizeof(int),
     Offset(largeMag), XtRString, "500"},

  {"dpiHoriz", XtCString, XtRFloat, sizeof(float),
     Offset(dpiHoriz), XtRString, "300"},

  {"dpiVert", XtCString, XtRFloat, sizeof(float),
     Offset(dpiVert), XtRString, "-1"},

  {"debug", XtCString, XtRBool, sizeof(Bool),
     Offset(debug), XtRString, "True"},

  {"pageMaxWd", XtCString, XtRInt, sizeof(int),
     Offset(pageMaxWd), XtRString, "0"},

  {"pageMaxHt", XtCString, XtRInt, sizeof(int),
     Offset(pageMaxHt), XtRString, "0"},

  {"autoSize", XtCString, XtRBool, sizeof(Bool),
     Offset(autoSize), XtRString, "True"},

  {"useButtons", XtCString, XtRBool, sizeof(Bool),
     Offset(useButtons), XtRString, "True"},

  {"updateNames", XtCString, XtRBool, sizeof(Bool),
     Offset(updateNames), XtRString, "True"},

  {"pushOnStart", XtCString, XtRString, sizeof(char *),
     Offset(pushOnStart), XtRString, "open"},

  {"divertByPush", XtCString, XtRBool, sizeof(Bool),
     Offset(divertByPush), XtRString, "true"},

/*
 *	the following options overide various globals, but aren't set
 *	via command line switches.
 */
  {"helpText", "HelpText",
     XtRString, sizeof(String),
     Offset(helpText),
     XtRString, "Warning -- Someone forgot to install APPDEFAULTSDIR/Xtex;\n\
(You can also set the XAPPLRESDIR environment variable to point to it)"}
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
  {"-dpi", "*.dpiHoriz", XrmoptionSepArg, (caddr_t) NULL},
  {"-dpiVert", "*.dpiVert", XrmoptionSepArg, (caddr_t) NULL},
  {"-debug", "*.debug", XrmoptionSepArg, (caddr_t) NULL},
  {"-pageMaxWd", "*.pageMaxWd", XrmoptionSepArg, (caddr_t) NULL},
  {"-pageMaxHt", "*.pageMaxHt", XrmoptionSepArg, (caddr_t) NULL},
  {"-autoSize", "*.autoSize", XrmoptionSepArg, (caddr_t) NULL},
  {"-useButtons", "*.useButtons", XrmoptionSepArg, (caddr_t) NULL},
  {"-updateNames", "*.updateNames", XrmoptionSepArg, (caddr_t) NULL},
  {"-tfmPath", "*.tfmPath", XrmoptionSepArg, (caddr_t) NULL},
  {"-xFontPath", "*.xFontPath", XrmoptionSepArg, (caddr_t) NULL},
  {"-pushOnStart", "*.pushOnStart", XrmoptionSepArg, (caddr_t) NULL},
  {"-divertByPush", "*.divertByPush", XrmoptionSepArg, (caddr_t) NULL}
};

int Argc;
char **Argv;
Widget TopLevelWidget;

static void
ArgError(argc, argv)
int argc;
char **argv;
{
  int i;
  fprintf(stderr,"Following %s not understood:\n",
	  (argc == 2) ? "arg" : "args");
  for (i = 1; i < argc; i++ ) {
    fprintf(stderr,"%s ", argv[i]);
  }
  fprintf(stderr,"\n The following are valid arguments\n");

  fprintf(stderr,"%25s%20s\n",
	  "Command Line Option", "Resource Name");
  for (i = 0; i < XtNumber(Options); i++ ) {
    fprintf(stderr,"%25s%20s\n",
	    Options[i].option, Options[i].specifier);
  }
  exit(1);
}


void 
main(argc,argv)
char ** argv;
int argc;
{

  Argc = argc;
  Argv = argv;

  if( (ProgName = index(argv[0], '/')) == NULL )
	  ProgName = argv[0];

  TopLevelWidget = XtInitialize(ProgName, ClassName,
				Options, XtNumber(Options),
				&Argc, Argv);

  XtGetApplicationResources(TopLevelWidget,
			    (caddr_t) &xtexResources, 
			    resources, XtNumber(resources),
			    NULL, (Cardinal) 0);

  /* barf if anything other than program name & file name is left */

  if ( Argc > 2 ) { 
    ArgError(Argc,Argv);
  }

  if ( xtexResources.incorrectlyInstalled ) {
    fprintf(stderr,"ERROR: xtex has been incorrectly installed.\n");
    fprintf(stderr,"Tell your local installation guru that the file\n");
    fprintf(stderr,"``Xtex.ad'' must be installed in the system application\n");
    fprintf(stderr,"defaults directory, e.g. ``/usr/lib/X11/app-defaults/Xtex''\n");
    exit(0);
  }

  TeXFileInstallActions();
  TeXPageInstallActions();
  TeXMarkInstallActions();

  BuildTopLevelWidget(TopLevelWidget);
  XtRealizeWidget(TopLevelWidget);

  if ( xtexResources.programVersion != xtexResources.resourcesVersion ) {
    fprintf(stderr,"WARNING: program version does not match resource file version\n");
    error(0,0, "WARNING: program version does not match resource file version\n");
  }


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

    if ( strcmp( xtexResources.pushOnStart, "open") == 0) {
      setFile( p , False );
      if ( DviFile != 0 ) {
	TeXPageBuild();
      }
    }
    else if ( strcmp( xtexResources.pushOnStart, "copy") == 0 ) {
      setFile( p , True );
      if ( DviFile != 0 ) {
	TeXPageBuild();
      }
    }
  }
  else {
    setDirectory(".");
  }

  if ( Argc > 2 ) {
    error(0,0,"Additional arguments ignored");
  }

  XtMainLoop();
  exit(0);
}
