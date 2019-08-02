#include "texx2.h"
#include <errno.h>
#include <sys/param.h>
#include <assert.h>
#include "texx2-widgets.h"

Widget topPane;
Widget topFile;
Widget topPages;
Widget topError;

/*
 *	Widgets related to file I/O
 */
Widget topFileBox;
Widget fileNameText;
Widget openButton;
Widget copyButton;
Widget cdButton;
Widget closeButton;
Widget quitButton;
Widget directoryLabel;
Widget fileLabel;

/*
 *	Widgets related to the pages table
 */
Widget pageButton;
Widget printAllButton;
Widget printMarkedButton;
Widget printUnmarkedButton;
Widget clearMarksButton;

/*
 *	Error dialog widgets
 */

Widget errorMsgBox;
Widget errorText;
Widget clearButton;

void
  BuildTopLevelWidget(topLevel)
Widget topLevel;
{

  Arg argList[20];
  Cardinal args;

  args = 0;
  XtSetArg(argList[args], XtNresize, True); args++;
  topPane = 
    XtCreateManagedWidget("topLevel",
			  formWidgetClass, topLevel,
			  argList, args);

  BuildFileLevel(topPane);
  BuildErrorBox(topPane);

  args = 0;
  XtSetArg(argList[args], XtNtop, XtChainTop); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetArg(argList[args], XtNresizable, True); args++;
  XtSetValues(fileNameText, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, fileNameText); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(quitButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, fileNameText); args++;
  XtSetArg(argList[args], XtNfromHoriz, quitButton); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(openButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, fileNameText); args++;
  XtSetArg(argList[args], XtNfromHoriz, openButton); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(copyButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, fileNameText); args++;
  XtSetArg(argList[args], XtNfromHoriz, copyButton); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(closeButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, fileNameText); args++;
  XtSetArg(argList[args], XtNfromHoriz, closeButton); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(cdButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, openButton); args++;
  XtSetArg(argList[args], XtNfromHoriz, quitButton); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(pageButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, quitButton); args++;
  XtSetArg(argList[args], XtNfromHoriz, pageButton); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(printAllButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, quitButton); args++;
  XtSetArg(argList[args], XtNfromHoriz, printAllButton); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(printMarkedButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, quitButton); args++;
  XtSetArg(argList[args], XtNfromHoriz, printMarkedButton); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(printUnmarkedButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, quitButton); args++;
  XtSetArg(argList[args], XtNfromHoriz, printUnmarkedButton); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(clearMarksButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, pageButton); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetArg(argList[args], XtNresizable, True); args++;
  XtSetArg(argList[args], XtNresize, True); args++;
  XtSetValues(directoryLabel, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, directoryLabel); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetArg(argList[args], XtNresizable, True); args++;
  XtSetArg(argList[args], XtNresize, True); args++;
  XtSetValues(fileLabel, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, fileLabel); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(clearButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, clearButton); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetArg(argList[args], XtNresizable, True); args++;
  XtSetValues(errorText, argList, args);
}
