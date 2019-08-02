/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 */

/* This is everything that's in a TeXPage */

#include "texx2.h"

#include <X11/Shell.h>

typedef struct TeXPageStruct
{
  int pageNumber;
  int logicalPageNumber;
  char * pageData;
  int userMag;
  Bool active;
  Bool realized;
#define MAXPAGENUMBERBUFFER 10
  char pageNumberBuffer[MAXPAGENUMBERBUFFER];

  /* action entered numbers */

  Bool inputNumberValid;
  Bool inputNumberMinus;
  int inputNumber;

  /* if you add any widgets, change texx2-page.c::TeXPageLocate */

  Widget popUpPage;
  Widget dviPageBox;
  Widget singleDviPage;
  Widget viewPort;
  Widget buttonBox;
  Widget forewardButton;
  Widget backwardButton;
  Widget markButton;
  Widget dupButton;
  struct TeXPageStruct *tied;
  struct TeXPageStruct *tiedTo;
  Widget tieButton;
  Widget largeButton;
  Widget smallButton;
  Widget quitButton;
  Widget pageNumberText;
  Widget gotoButton;
  Widget reopenButton;

  /* these are pulled out of the ViewPort widget */
  Widget vertBar;
  Widget horizBar;
} TeXPage;

extern int TeXPages;
#define MAX_TEX_PAGES 10
extern TeXPage *TheTeXPages [ MAX_TEX_PAGES ];

#ifdef _STDC_
 extern void TeXPageBuild();
 extern void TeXPageCloseFile();
 extern void TeXPageSlamPages();
 extern void TeXPageOpenFile();
 extern TeXPage *TeXPageLocate();
 extern void TeXPageRef(TeXPage *, int);
 extern void TeXPageDeref(TeXPage *);
 extern void TeXPageRealize( TeXPage *);

 extern void TeXPageInstallActions();

#else
 extern void TeXPageBuild();
 extern void TeXPageCloseFile();
 extern void TeXPageSlamPages();
 extern void TeXPageOpenFile();
 extern TeXPage *TeXPageLocate();
 extern void TeXPageRef();
 extern void TeXPageDeref();

 extern void TeXPageInstallActions();
#endif
