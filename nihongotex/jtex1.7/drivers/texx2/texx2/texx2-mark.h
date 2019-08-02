/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 */

typedef enum {printAll, printMarked, printUnmarked } PrintDirective;

extern char *printProgram;

#ifdef _STDC_
  void TeXMarkNewFile();
  Bool TeXMarkToggle(int);
  Bool TeXMarkUnset(int);
  Bool TeXMarkSet(int);
  Bool TeXMarkState(int);
  void TeXMarkAll();
  void TeXMarkNone();
  void TeXMarkPrint(PrintDirective);
#else
  void TeXMarkNewFile();
  Bool TeXMarkToggle();
  Bool TeXMarkState();
  void TeXMarkAll();
  void TeXMarkNone();
  void TeXMarkPrint();
#endif
