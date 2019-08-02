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
 * fio.h - Non machine dependent file I/O header
 */

/*
 * $Id: fio.h,v 1.7 90/03/27 11:00:20 void Exp $
 */

#ifndef O_RDONLY
#include <fcntl.h>	/* for O_BINARY */
#endif

#if defined(lint) || MSDOS
#define Get1Byte(fp)	    fgetc(fp)
#else
#define Get1Byte(fp)	    getc(fp)
#endif
#define GetUs1Byte(fp)	    (UnSig1Byte) Get1Byte(fp)
#define GetUs2Byte(fp)	    (UnSig2Byte) Get2Byte(fp)
#ifdef lint
#define GetUs3Byte(fp)	    (UnSig3Byte) Get3Byte(fp)
#else
#define GetUs3Byte(fp)	    (UnSig3Byte) (Get3Byte(fp) & (UnSig3Byte) 0xffffff)
#endif
#define GetUs4Byte(fp)	    (UnSig4Byte) Get4Byte(fp)

#if defined(lint) || MSDOS
#define Put1Byte(n, fp)	    fputc((int) (n), fp)
#else
#define Put1Byte(n, fp)	    putc((int) (n), fp)
#endif
#define PutUs1Byte(n, fp)   Put1Byte((n), fp)
#define PutUs2Byte(n, fp)   Put2Byte((Sig2Byte) (n), fp)
#define PutUs3Byte(n, fp)   Put3Byte((Sig3Byte) (n), fp)
#define PutUs4Byte(n, fp)   Put4Byte((Sig4Byte) (n), fp)

extern Sig2Byte		Get2Byte();
extern Sig3Byte		Get3Byte();
extern Sig4Byte		Get4Byte();

/*
 * MAXFILES	maximum number of open files at the same time
 * READMODE	open() flags to open file for binary read
 * FREADMODE	fopen() flags to open file for binary read
 * FWRITEMODE	fopen() flags to open file for binary output
 */

#if MSDOS
#define MAXFILES    10
#define READMODE    O_RDONLY | O_BINARY
#define FREADMODE   "rb"
#define FWRITEMODE  "wb"
#else
#define MAXFILES    20
#define READMODE    O_RDONLY
#define FREADMODE   "r"
#define FWRITEMODE  "w"
#endif
