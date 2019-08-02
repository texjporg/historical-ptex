/*
 * jxl4.h - jxl4 font format structure definition.
 *
 *		Copyright (C) 1988 dit Co., Ltd.
 */

/*
 * $Id: jxl4.h,v 1.5 89/08/11 11:08:35 ditstaff Exp $
 */

#define JXL4ID	101157

struct JXL4 {
    int check_sum;      /* font file checksum */
    int magnification;  /* resolution = `dpi * 5' */
    int design_size;    /* designsize = `point-size * 2^20' */
    int fixed_pxl_size; /* pxl nums of fixed glyph */
    int fixed_tfm_size; /* tfm_width of fixed glyph */
    int tfm_ptr;        /* bytes to tfm entry */
    int tfm_size;       /* entry nums of tfm (max = 256) */
    int x_info_ptr;     /* bytes to Xinfo node */
    int x_info_size;    /* entry nums of Xinfo */
    int y_info_ptr;     /* bytes to Yinfo node */
    int y_info_size;    /* entry nums of Yinfo */
    int dir_size;       /* entry nums of PXLdir */
    int dir_ptr_0;      /* [0] -> bytes to PXLdir */
    int dir_ptr_1;      /* [1] -> Level.1 K-Char Position */
    int dir_ptr_2;      /* [2] -> Level.2 K-Char Position */
};

struct PXL_DIR {
    char tfm_index ;
    int  rat_index ;
    short x_info_index ;
    short y_info_index ;
};

struct X_INFO {
    short x_offset;
    short x_pix;
    short x_offset_p;
};

struct Y_INFO {
    short y_offset;
    short y_pix;
    short y_offset_p;
} ;

struct TFM_TABLE {
    int entry;
} ;
/* 
struct JXL4_INFO {
    struct JXL4      *jxl4;
    struct X_INFO    *xinfo;
    struct Y_INFO    *yinfo;
    struct PXL_DIR   *pxldir;
    struct TFM_TABLE *tfmtable;
} ;
*/
#define SIZEof_JXL4			60
#define SIZEof_TFM_TABLE	4
#define SIZEof_X_INFO		6
#define SIZEof_Y_INFO		6
#define SIZEof_PXL_DIR		8

#define BeginOfJis		0x2121
#define BeginOfLevel1KCode	1410
#define BeginOfLevel2KCode	4418

/*
 * config.h - Configuration file for dviimp
 *
 *		Copyright (C) 1988 dit Co., Ltd.
 */

/*
 * $Id: config.h,v 1.8 89/11/10 17:14:03 void Exp $
 */

/* #include "defs.h"
#include "site.h"
*/
#define	read4(fd, p)	(void)read((fd), (char *)&(p), 4)
#define	read2(fd, p)	(void)read((fd), (char *)&(p), 2)



