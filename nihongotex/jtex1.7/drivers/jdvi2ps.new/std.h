/*
 * Standard definitions.
 *
 * Neil Hunt (Neil%Teleos.com@ai.sri.com).
 *
 * Copyright (c) 1989 Teleos Research, Inc 1989.
 * Copyright (c) 1988 Schlumberger Technologies, Inc 1988.
 *
 * Anyone can use this software in any manner they choose,
 * including modification and redistribution, provided they make
 * no charge for it, and these conditions remain unchanged.
 *
 * This program is distributed as is, with all faults (if any), and
 * without any wrranty.  No author or distributor accepts responsibility
 * to anyone for the consequences of using it, or for whether it serves any
 * particular purpose at all, or any other reason.
 *
 * $Log:	std.h,v $
 * Revision 1.1  89/02/10  18:41:57  neil
 * Initial revision
 * 
 */

#ifndef _TYPES_
typedef unsigned int		uint;
typedef short			ushort;
#endif _TYPES_
typedef unsigned char		uchar;
#ifdef bool
#undef bool
#endif bool
typedef unsigned int		bool;

#define DEL			'\177'
#define ESC			'\033'
#define Control(c)		((c) - 0x40)

#define forward			extern

#define Range(min, x, max)	(((x) < (min))? \
				 (min) : (((x) > (max))? (max) : (x)) \
				)

#define MAXINT			2147483647
#define NEGMAXINT		(-2147483648)

#ifndef TRUE
#define TRUE			1
#define FALSE			0
#endif  TRUE

#define Abs(n)			(((n) >= 0)? (n) : -(n))
#define Min(a, b)		((a) > (b)? (b) : (a))
#define Max(a, b)		((a) < (b)? (b) : (a))
