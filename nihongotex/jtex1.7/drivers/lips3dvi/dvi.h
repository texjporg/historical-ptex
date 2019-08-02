/*
 *	dvi.h -- Header file for lips3dvi
 *
 *	This file containes constants in dvi file and declears
 *	structure for accessing dvi.
 *
 *	Written by H. Nagahara on 2/10/92 08:58:10.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: dvi.h 10.1
 */
/* nimonic codes */
#define	SET_CHAR	0
#define	SET1		128
#define	SET2		129
#define	SET3		130
#define	SET4		131
#define	SET_RULE	132
#define	PUT1		133
#define	PUT2		134
#define	PUT3		135
#define	PUT4		136
#define	PUT_RULE	137
#define	NOP		138
#define	BOP		139
#define	EOP		140
#define	PUSH		141
#define	POP		142
#define	RIGHT1		143
#define	RIGHT2		144
#define	RIGHT3		145
#define	RIGHT4		146
#define	W0		147
#define	W1		148
#define	W2		149
#define	W3		150
#define	W4		151
#define	X0		152
#define	X1		153
#define	X2		154
#define	X3		155
#define	X4		156
#define	DOWN1		157
#define	DOWN2		158
#define	DOWN3		159
#define	DOWN4		160
#define	Y0		161
#define	Y1		162
#define	Y2		163
#define	Y3		164
#define	Y4		165
#define	Z0		166
#define	Z1		167
#define	Z2		168
#define	Z3		169
#define	Z4		170
#define	FNT_NUM		171

#define	FNT1		235
#define	FNT2		236
#define	FNT3		237
#define	FNT4		238
#define	XXX1		239
#define	XXX2		240
#define	XXX3		241
#define	XXX4		242
#define	FNT_DEF1	243
#define	FNT_DEF2	244
#define	FNT_DEF3	245
#define	FNT_DEF4	246
#define	PRE		247
#define	POST		248
#define	POST_POST	249

#define	DIR		255	/* pTeX extension */

/* pointers to the command handling functions */
#ifndef	DVIBODY
extern int	(*set_char_func)(),
		(*fnt_num_func)(),
		(*pre_func)(),
		(*post_func)(),
		(*post_post_func)(),
		(*fnt_def_func)(),
		(*bop_func)(),
		(*eop_func)(),
		(*fnt_func)(),
		(*set_func)(),
		(*put_func)(),
		(*set_rule_func)(),
		(*put_rule_func)(),
		(*right_func)(),
		(*down_func)(),
		(*w0_func)(),
		(*w_func)(),
		(*x0_func)(),
		(*x_func)(),
		(*y0_func)(),
		(*y_func)(),
		(*z0_func)(),
		(*z_func)(),
		(*push_func)(),
		(*pop_func)(),
		(*xxx1_func)(),
		(*xxx2_func)(),
		(*xxx3_func)(),
		(*xxx4_func)(),
		(*nop_func)(),
		(*dir_func)(),
		(*illegal_func)();
#endif

