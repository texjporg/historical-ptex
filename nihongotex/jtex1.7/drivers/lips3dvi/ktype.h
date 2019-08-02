/*
 *	ktype.h -- include file for jxlfont.c cache.c
 *		   define Kanji level checking macros
 *
 *	Written by H. Nagahara on 2/10/92 08:58:41.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: ktype.h 10.1
 */
#define	iskanji0(k)	((((k) >= 0x2121) && ((k) <= 0x2771))?1:0)
#define	iskanji1(k)	((((k) >= 0x3021) && ((k) <= 0x4f53))?1:0)
#define	iskanji2(k)	((((k) >= 0x5021) && ((k) <= 0x737e))?1:0)
