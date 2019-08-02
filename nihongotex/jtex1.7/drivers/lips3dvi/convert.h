/*
 *	convert.h -- unit conversion macros.
 *
 *	Written by H. Nagahara on 2/10/92 08:57:45.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: convert.h 10.1
 */
#define	INCH_BY_METER	39.37007
#define	INCH_BY_MMETER	0.03937007
#define	INCH_IN_METER	0.025400006
#define	m2inch(a)	((a) * INCH_BY_METER)
#define	mm2inch(a)	((a) * INCH_BY_MMETER)
#define	mm2point(a)	((a) * 2.84527559055118)
#define	m2point(a)	((a) * 2845.27559055118)

