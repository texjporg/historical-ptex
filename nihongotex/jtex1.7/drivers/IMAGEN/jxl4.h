/*
 * Copyright 1988 ASCII Corporation.   
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of ASCII Corporation not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * ASCII Corporation makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * ASCII Corporation DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL DIT CO., LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
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
 * jxl4.h - jxl4 font format structure definition.
 */

/*
 * $Id: jxl4.h,v 1.7 90/03/23 17:28:25 void Exp $
 */

#define JXL4ID	101157

typedef struct {
    Sig4Byte check_sum;      /* font file checksum */
    Sig4Byte magnification;  /* resolution = `dpi * 5' */
    Sig4Byte design_size;    /* designsize = `point-size * 2^20' */
    Sig4Byte fixed_pxl_size; /* pxl nums of fixed glyph */
    Sig4Byte fixed_tfm_size; /* tfm_width of fixed glyph */
    Sig4Byte tfm_ptr;        /* bytes to tfm entry */
    Sig4Byte tfm_size;       /* entry nums of tfm (max = 256) */
    Sig4Byte x_info_ptr;     /* bytes to Xinfo node */
    Sig4Byte x_info_size;    /* entry nums of Xinfo */
    Sig4Byte y_info_ptr;     /* bytes to Yinfo node */
    Sig4Byte y_info_size;    /* entry nums of Yinfo */
    Sig4Byte dir_size;       /* entry nums of PXLdir */
    Sig4Byte dir_ptr_0;      /* [0] -> bytes to PXLdir */
    Sig4Byte dir_ptr_1;      /* [1] -> Level.1 K-Char Position */
    Sig4Byte dir_ptr_2;      /* [2] -> Level.2 K-Char Position */
} JXL4;

#if	BYTESWAP
typedef struct {
    Sig1Byte tfm_index;
    Sig3Byte rat_index;
    Sig2Byte x_info_index;
    Sig2Byte y_info_index;
} PXL_DIR;
#else
typedef struct {
    Sig1Byte tfm_index : 8;
    Sig3Byte rat_index : 24;
    Sig2Byte x_info_index : 16;
    Sig2Byte y_info_index : 16;
} PXL_DIR;
#endif

typedef struct {
    Sig2Byte x_offset;
    Sig2Byte x_pix;
    Sig2Byte x_offset_p;
} X_INFO;

typedef struct {
    Sig2Byte y_offset;
    Sig2Byte y_pix;
    Sig2Byte y_offset_p;
} Y_INFO;

typedef struct {
    Sig4Byte entry;
} TFM_TABLE;

typedef struct {
    JXL4      jxl4;
    X_INFO    *xinfo;
    Y_INFO    *yinfo;
    PXL_DIR   *pxldir;
    TFM_TABLE *tfmtable;
} JXL4_INFO;

X_INFO    x_info;
Y_INFO    y_info;
PXL_DIR   pxl_dir;
TFM_TABLE tfm_table;

#define SIZEof_JXL4			60
#define SIZEof_TFM_TABLE	4
#define SIZEof_X_INFO		6
#define SIZEof_Y_INFO		6
#define SIZEof_PXL_DIR		8
