/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 */

#include "types.h"
#include "dviclass.h"
#include "dvicodes.h"
#include "tfm.h"
#include "fio.h"
#include "postamble.h"

/*****************************************************************************
 *	Things we read from the DVI file
 */

extern struct PostAmbleInfo ThePostAmbleInfo;

typedef struct {
	struct PostAmbleFont paf;
#ifdef ASCIIJTEX /* kagotai */
	int nt;				/* number of types */
	struct char_type_word *ct;	/* copy of char_type table */
#endif /* ASCIIJTEX */
	int *widths;
#if defined(ASCIIJTEX) && defined(FAKEKANJI) /* kagotani */
	int *glues;
#endif /* ASCIIJTEX && FAKEKANJI */
	i32 pspace;		/* boundary between `small & `large spaces
				   (for positive horizontal motion) */
	i32 nspace;		/* -4 * pspace, for negative motion */
	i32 vspace;		/* 5 * pspace, for vertical motion */
} DviFontInfo;

#define MAX_POSTAMBLE_FONTS 256
extern int RegisteredFonts;
extern DviFontInfo TheFontInfo[ MAX_POSTAMBLE_FONTS ];

/*****************************************************************************
/*
 *	Defaults and configuration for dvi-simple
 */

#define DEFAULT_DPI	300
#define DEFAULT_HHMARGIN	DEFAULT_DPI
#define DEFAULT_VVMARGIN	DEFAULT_DPI
#define DEFAULT_MAX_DRIFT	3

#define DEFAULT_BLACKNESS 3

#define DVI_COUNT	10	/* count0 .. count9 */

#define SP_PER_INCH(dpi) ( (dpi) * 65536 )
#define POINTS_PER_INCH 72.27

extern long	*DviCount[DVI_COUNT];
extern long	*DviPageOffsets;

extern int	DviHHMargin;
extern int	DviVVMargin;
extern int	DviDpi;
extern int	DviTallestPage;
extern int	DviWidestPage;
extern int	DviTotalPages;
extern int	DviCurrentPage;
extern int	DviMaxDrift;
extern char 	*DviPrintEngine;
extern int      DviBlackness;
extern int	DviUserMag;

extern FILE	*DviFile;
extern char	*DviFileName;

#ifdef __STDC__
  extern int	DviInit( char *, int );
  extern void	DviFini();
  extern char * DviFetchPage( int );
#else
  extern int	DviInit();
  extern void	DviFini();
  extern char * DviFetchPage();
#endif
