/*
 *	config.h -- configuration header file for lips3dvi & escpdvi
 *
 *	This file containes compiler & machine dependent macros.
 *	Please edit this file to fit your machine.
 *
 *	Written by H. Nagahara on 2/10/92 08:57:41
 *	Copyright (C) 1990-1992 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: config.h 10.1
 */
/* select machine */
#define	SUN		1	/* select only one of them */
#define	NEWS		0
#define	PC98		0
#define	J3100		0
#define	DOSV		0

/* select printer */
#define	LIPS3		1	/* select only one of them */
#define	ESCP		0	/* LIPS3 is not used in source file but ESCP */

/* select EMS use : DOS version ONLY!! */
#define	USEEMS		1	/* if you need EMS on DOS version */


/* ----	DON'T TOUCH BELOW LINES IN NORMAL USE ---- */
#ifdef	MSDOS		/* MS-C defines MSDOS */
# undef	MSDOS
#endif

# define	MSDOS	(PC98 || J3100 || DOSV)
# define	CPU32	!(PC98 || J3100 || DOSV)
# define	CPU16	(PC98 || J3100 || DOSV)
# define	LITTLE	(PC98 || J3100 || DOSV)	/* little endian */
# define	BIG	!(PC98 || J3100 || DOSV)
#if	!MSDOS
# undef		USEEMS
# define	USEEMS	0
#endif
/* ---- DON'T TOUCHE ABOVE LINES IN NORMAL USE ---- */

/* needed file path name */
#if	MSDOS
#define	FONTDIR		"c:/texfont/pxl"	/* font existing dir */
#define	TFMDIR		"c:/texfont/tfm"	/* TFM existing dir */
#define	DOWNINFO	"c:/tmp/download.fnt"	/* downloaded file name file */
#define	CONFFILE	"c:/tex/lips3dvi.cnf"	/* default configuration file */
#else
#define	FONTDIR		"/usr/local/lib/tex/fonts"
#define	TFMDIR		"/usr/local/lib/tex/fonts"
#define	DOWNINFO	"/var/tmp/.fontinfo"
#define	CONFFILE	"/usr/local/lib/tex/lips3dvi.cnf"
/* below 3 symboles are defined for 'dvipr' & 'dviback' (UNIX only) */
#if	ESCP
#define	MAINPROG	"/usr/local/lib/tex/escpdvi"	/* body program path */
#else
#define	MAINPROG	"/usr/local/lib/tex/lips3dvi"	/* body program path */
#endif
#define	LPRCMD		"/usr/ucb/lpr"		/* lpr command */
#define	LPRDEFOPT	"-d"			/* lpr necessarily option */
#define	TMPFNAME	"/var/tmp/lips3dvi"	/* tmp file base name */
#endif

/* support font selection */
#define	PXLFONT		1		/* PXL font support */
#define	PKFONT		1		/* PK font support */
#define	FARFONT		0		/* FAR font support (DOS only) */
	/* search order : FAR -> PXL -> PK */
#define	MICROTEX	0		/* micro TeX output support */

/* JXL font cache size (depend on memory) */
#if	MSDOS
# define CACHE_SIZE	1000	/* font cache size (# of char in cache) */
#else
# define CACHE_SIZE	10000
#endif

/* font search path */
#define	MAXPATH		10	/* max # of search path */

/* ----	DON'T TOUCH BELOW LINES IN NORMAL USE ---- */
/* variable size depending on CPU */
#if	CPU16
typedef	signed char	int1;		/* 1 byte signed */
typedef signed int	int2;		/* 2 bytes signed */
typedef	signed long	int4;		/* 4 bytes signed */
typedef	unsigned char	uint1;		/* 1 byte unsigned */
typedef	unsigned int	uint2;		/* 2 bytes unsigned */
typedef	unsigned long	uint4;		/* 4 bytes unsigned */
#define	MAXINT		32767		/* integer max */
#define	MININT		-32768		/* integer min */
#define	MAXINT2		32767		/* 2 bytes integer max */
#define	MININT2		-32768		/* 2 bytes integer min */
#endif
#if	CPU32
typedef	char		int1;		/* 1 byte signed */
typedef short		int2;		/* 2 bytes signed */
typedef	int		int4;		/* 4 bytes signed */
typedef	unsigned char	uint1;		/* 1 byte unsigned */
typedef	unsigned short	uint2;		/* 2 bytes unsigned */
typedef	unsigned int	uint4;		/* 4 bytes unsigned */
#define	MAXINT		2147483647	/* integer max */
#define	MININT		-2147483648	/* integer min */
#define	MAXINT2		32767		/* 2 bytes integer max */
#define	MININT2		-32768		/* 2 bytes integer min */
#endif

/* CPU byte order */
#if	LITTLE
# define	SWAP		1	/* 1 when CPU is a little endian */
#endif
#if	BIG
# define	SWAP		0	/* 0 when CPU is a big endian */
#endif

/* basic constant : DON'T CHANGE THEM */
#define	PROGID		('l' + 'i' + 'p' + 's' + '3' + 'd' + 'v' + 'i')
#define	TRUE		1
#define	FALSE		0

/* for ANSI prototype declaration (DOS only) */
#if	MSDOS
# define	PP(x)	x	/* enable prototype declaration */
# ifndef	MAKEDECL
#  include	"declears.h"
# endif
#else
# define	PP(x)	()	/* disable prototype declaration */
#endif
/* ---- DON'T TOUCHE ABOVE LINES IN NORMAL USE ---- */
