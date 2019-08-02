/*
 * Copyright 1988 dit Co., Ltd.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of dit Co., Ltd. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * Dit Co., Ltd. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * DIT CO., LTD. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL DIT CO., LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/*
 * config.h - Configuration file for dviimp
 */

/*
 * $Id: config.h,v 1.9 90/03/27 10:51:35 void Exp $
 */

#include "defs.h"
#include "site.h"

/*
 * Determine byte order
 */

#if sun
#define BYTESWAP	FALSE
#endif
#if vax || MSDOS
#define BYTESWAP	TRUE
#endif

#ifndef BYTESWAP
#define BYTESWAP	FALSE		    /* defalut is FALSE */
#endif

#if MSDOS
#define IntIsLong	FALSE		    /* int is 16 bits */
#else
#define IntIsLong	TRUE		    /* int is 32 bits */
#endif

#define MAXPATHLEN	256		    /* maximum pathname length */

typedef char		Sig1Byte;	    /* Signed Byte */
typedef unsigned char   UnSig1Byte;	    /* Unsigned Byte */
typedef short		Sig2Byte;	    /* Signed 2 Bytes */
typedef unsigned short  UnSig2Byte;	    /* Unsigned 2 Bytes */
#if IntIsLong
typedef int		Sig3Byte;	    /* Signed 3 Bytes */
typedef unsigned int	UnSig3Byte;	    /* Unsigned 3 Bytes */
typedef int		Sig4Byte;	    /* Signed 4 Bytes */
typedef unsigned int	UnSig4Byte;	    /* Unsigned 4 Bytes */
#else
typedef long		Sig3Byte;	    /* Signed 3 Bytes */
typedef unsigned long	UnSig3Byte;	    /* Unsigned 3 Bytes */
typedef long		Sig4Byte;	    /* Signed 4 Bytes */
typedef unsigned long	UnSig4Byte;	    /* Unsigned 4 Bytes */
#endif /* IntIsLong */

#if BYTESWAP
#define SWAP2(n)	(((CharMask((n) >> 8) | CharMask(n) << 8)) & 0xffff)
#define SWAP4(n)	(CharMask((n) >> 24) | (((n) >> 8) & 0xff00) | \
			(((n) & 0xff00) << 8) | (n) << 24)
/*
#else
#define SWAP2(n)	(n)
#define SWAP4(n)	(n)
*/
#endif

extern void	error();
extern void	cant_alloc();
extern void	cant_open();

#ifdef	BYTESWAP
#if	BYTESWAP
#define	read4(fd, p)	{(void)read((fd), (char *)&(p), 4); (p) = SWAP4(p);}
#define	read2(fd, p)	{(void)read((fd), (char *)&(p), 2); (p) = SWAP2(p);}
#else
#define	read4(fd, p)	(void)read((fd), (char *)&(p), 4)
#define	read2(fd, p)	(void)read((fd), (char *)&(p), 2)
#endif	/* of if */
#else	/* if def */
#define	read4(fd, p)	read_2byte((fd), &(p))
#define	read2(fd, p)	read_4byte((fd), &(p))
extern void	read_4byte();
extern void	read_2byte();
#endif	/* if def */
