/*
 *	lbp.h -- header file for the routines that call priter dependent routines
 *
 *	Written by H. Nagahara on 2/10/92 08:58:49.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: lbp.h 10.1
 */
/* priter dependent constant */
#define	PRINTER_RES	300	/* printer resolution (dpi) */
#define	MINPAPERLEN	148.0	/* min paper length */
#define	MAXPAPERLEN	364.0	/* max paper length */
#define	MINPAPERWID	100.0	/* min paper width */
#define	MAXPAPERWID	257.0	/* max paper width */
#define	MINUSERGSET	3840	/* min graphset # for user use */
#define	MAXUSERGSET	4095	/* max graphset # for user use */
#define	MAXDOWNWIDTH	128	/* max width (dots) of download font cel */
#define	MAXDOWNHIGHT	128	/* max hight (dots) of download font cel */
#define	MAXASSIGN	32	/* max # of assigned font */
#if	ESCP
#define	H_INCHPOINT	240	/* horizontal base point (dots) for TeX */
#define	V_INCHPOINT	240	/* vertical base point (dots) for TeX */
#else
#define	H_INCHPOINT	240	/* horizontal base point (dots) for TeX */
#define	V_INCHPOINT	177	/* vertical base point (dots) for TeX */
#endif
#define	JIS_GSET	0xc2	/* graphic set # for Kanji
				 * JIS83 --> 0xc2, JIS78 --> 0xc0 */
#if	ESCP
#define	MINCHO_FACE	10	/* Mincho type face # */
#define	GOTHIC_FACE	9	/* Gothic type face # */
#define	DEFAULT_WEIGHT	0	/* no mean on ESC/Page -> normal */
#else
#define	MINCHO_FACE	80	/* Mincho type face # */
#define	KAKU_GOTHIC	81	/* Kaku-Gothic --> 81 */
#define	MARU_GOTHIC	82	/* Maru-Gothic --> 82 */
#define	GOTHIC_FACE	KAKU_GOTHIC	/* Gothic type face # */
#define	MIDIUM_WEIGHT	22	/* stroke weight # */
#define	LIGHT_WEIGHT	2
#define	BOLD_WEIGHT	1
#define	DEFAULT_WEIGHT	MIDIUM_WEIGHT	/* default stroke weight -> midium */
#endif

/* conversion expressions */
#define	m2dot(a)	((a) * 11811.021)	/* meter to dots */
#define	mm2dot(a)	((a) * 11.811021)
#define	WIDTH_ADJ	0.95			/* internal font adjustment */
#define	HIGHT_ADJ	0.95
#define	DEPTH_ADJ	0.5

#define	SHIFTIN		0x0f
#define	SHIFTOUT	0x0e

#ifndef		_LBP_
extern void	LBP_sendSOJ();		/* printer initialize */
extern void	LBP_sendMSG();		/* show message onto printer console */
extern void	LBP_sendEOJ();		/* close printer */
extern void	LBP_selectJfont();	/* select Japanese font */
extern void	LBP_selectAfont();	/* select alphabet font */
extern int	LBP_aFont();		/* download a font set */
extern void	LBP_putchar();		/* print a character */
extern void	LBP_rasterAchar();	/* print alphabet char as raster */
extern void	LBP_raster20();		/* print 0x20 char as raster */
extern void	LBP_move();		/* move priting point */
extern void	LBP_rectangle();	/* print a rectangle */
extern void	LBP_raster();		/* print a raster */
extern void	LBP_numJfont();		/* select Japanese font as its # */
extern void	LBP_numAfont();		/* select alphabet font as its # */
extern void	LBP_flushpage();	/* flush page data */
extern void	LBP_attrib();		/* set attribute */
extern void	LBP_clearattr();	/* clear attribute */
#if	ESCP
extern int	LBP_assignAfont();	/* set font # to alphabet font */
extern int	LBP_assignJfont();	/* set font # to Japanese font */
extern void	set_fontname();		/* set font name decripter */
extern int	get_fontname();		/* get font name descipter */
#else
extern void	LBP_assignAfont();	/* set font # to alphabet font */
extern void	LBP_assignJfont();	/* set font # to Japanese font */
#endif	/* ESCP */
#endif	/* _LBP_ */

