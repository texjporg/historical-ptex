extern Widget topPane;

/*
 *	Widgets related to file I/O
 */
extern Widget topFileBox;
extern Widget fileNameText;
extern Widget openButton;
extern Widget closeButton;
extern Widget copyButton;
extern Widget cdButton;
extern Widget quitButton;

extern Widget directoryLabel;
extern Widget fileLabel;

/*
 *	Widgets related to the pages table
 */
extern Widget pageButton;
extern Widget printAllButton;
extern Widget printMarkedButton;
extern Widget printUnmarkedButton;
extern Widget clearMarksButton;

/*
 *	Error dialog widgets
 */

extern Widget errorMsgBox;
extern Widget errorText;
extern Widget clearButton;

/*
 *	Things that build widgets
 */

extern void BuildErrorBox();
extern void BuildFileLevel();
extern void BuildPageLevel();
