/*
 *	drive.h -- valiables definition for printer driver
 *
 *	Written by H. Nagahara on 2/10/92 08:58:06.
 *	Copyright (C) 1990 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: drive.h 10.1
 */
/* paper size */
#define	A4	1
#define	A5	2
#define	HAGAKI	3
#define	B4	4
#define	B5	5
#define	LETTER	6
#define	LIGAL	7
#define	USER	10
#define	UNKNOWN	20
/* printout pages */
#define	BOTHPAGE	0
#define	ODDPAGE		1
#define	EVENPAGE	2

/* environment variable name */
#define	CNFENV		"LIPS3DVI"

/* stack type */
typedef struct {
    int4	h, v, w, x, y, z, d;
} STACK;

#ifndef	_MAIN_
/* driver registers */
extern	int4	h, v,		/* current point */
		w, x,		/* horizontal registers */	
		y, z,		/* vertical reigisters */
		dir;		/* direction for pTeX extension */
extern	int	fno;		/* current font # */

/* font lists */
extern	struct font	*fontp;		/* pointer to current font information */
extern	struct font	*fontlist;	/* top of the font-list */
extern	struct metric	*metriclist;	/* top of the font metric infor */
extern	struct font	*g0font;	/* font assigned to G0 */
extern	struct font	*g1font;	/* font assigned to G1 */

/* caching font */
extern	struct fdir	*cachetop;	/* recently referenced bitmap for cache */
extern	struct fdir	*cachebottom;	/* last referenced bitmap for cache */
extern	struct fdir	*cachefree;	/* free element in cache */

/* drivers stack */
extern	STACK	*stack_top;
extern	STACK	*stackp;

/* global variables for identify DVI */
extern	uint2	stack_size;
extern	uint4	num, den, mag;	/* scaling values */
extern	uint4	max_h, max_w;	/* max printing size */
extern	uint2	pages;		/* number of pages */
extern	double	scalepoint;	/* dimension uint in DVI */
extern	int	current_page;	/* prcessing page # */

/* printer status variables */
extern int2	prn_max_font;	/* max assigned # of printer font */
extern int2	using_gset;	/* max assigned # of printer graphset */
extern int4	downloadcount;	/* downloaded data counter */
extern	int4	h_base;		/* horizontal base DOT */
extern	int4	v_base;		/* vertical base DOT */
extern struct jfatr *fattr;	/* font attribute list */

/* options */
extern	int	papersize;	/* paper size */
extern	int	landscape;	/* true when landscape */
extern	float	paperwidth;	/* paper width (mm) */
extern	float	paperlength;	/* paper hgiht (mm) */
extern	int	copypage;	/* copy page */
extern	int	startpage;	/* printing start page */
extern	int	endpage;	/* pringitn end page */
extern	int4	port_h_base;	/* portrait mode base */
extern	int4	port_v_base;
extern	int4	land_h_base;	/* landscape mode base */
extern	int4	land_v_base;
extern	float	h_offset;	/* horizontal offset (mm) */
extern	float	v_offset;	/* vertical offset (mm) */
extern	int	usejxlfont;	/* using JXL font flag */
extern	int4	maxdownsize;	/* max of download data size (byte) */
extern	int	mincho_syotai;	/* Minchotai syotai # */
extern	int	gothic_syotai;	/* Gothictai syotai # */
extern	int	verbose;	/* verbose flag */
extern	int	forcedownload;	/* forcedownload flag */
extern	float	magnification;	/* magnifications */
extern	int	cache_size;	/* jxl font cache size */
extern	int	prpage;		/* printout pages */

/* directory or file name */
extern char	*progname;	/* my name */
extern char	*fontdir;	/* font exist directory */
extern char	*tfmdir;	/* tfm exist directory */
extern char	*(fontpath[]);	/* grif file search path */
extern char	*(tfmpath[]);	/* tfm file search path */
extern char	*downinfo;	/* download information file */
extern char	*conffile;	/* configuration file */
extern char	*infile;	/* input file name */
extern FILE	*dvifile;	/* input file */
extern FILE	*outfile;	/* output file */

#endif	/* _MAIN_ */
