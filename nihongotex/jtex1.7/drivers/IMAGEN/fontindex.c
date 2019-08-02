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
 * fontindex.c - get target font's index direcotry.
 */

/*
 * $Id: fontindex.c,v 1.24 90/03/27 11:00:38 void Exp $
 */

#include <stdio.h>
#include <sys/file.h>
#include "dvicom.h"
#include "config.h"
#include "fio.h"
#include "font.h"
#include "jxl4.h"

#define E_FONT_CHAR	256	/* number of charas in a font */
#define D_FONT_CHAR	(94 * 94)	/* number of charas in a font */

#define SIZEOF_EFONT_MISC   (E_FONT_CHAR * sizeof(short))
#define SIZEOF_DFONT_MISC   (D_FONT_CHAR * sizeof(short))
#define GLYPH_SIZEOF_EFONT  (E_FONT_CHAR * sizeof(GLYPH_DATA))
#define GLYPH_SIZEOF_DFONT  (D_FONT_CHAR * sizeof(GLYPH_DATA))
#define SIZEof_EFONT        (E_FONT_CHAR * SIZEof_GLYPH_DATA)
#define SIZEof_DFONT        (D_FONT_CHAR * SIZEof_GLYPH_DATA)

visible NewFontFileInfo font_node[MAXFONTFILE];

#if JXL
visible JXL4_INFO	jxl4_info[MAXFONTFILE];
#endif

extern FILE	*dvi;

extern char	*AbsPath();
extern char	*(*GetFontSuffix)();
extern char	*malloc(), *strcpy();
extern long	lseek();

visible
PXLIndex(def_byte)
Sig4Byte    def_byte;
{
    char junk[128];
    int	 fd;
    register int i;
    static NewFontFileInfo	*newfont;   /* usagi */
    Sig4Byte	IdOfPxl;

    char *strcat();
    char *GetString();
    int	 FontOpen();

    if ((newfont = (NewFontFileInfo *)malloc(sizeof(NewFontFileInfo))) == NULL)
	error("new font information");	/* exit */

    newfont->CheckSum = Get4Byte(dvi);
    newfont->font_mag = Get4Byte(dvi);
    newfont->font_dsz = Get4Byte(dvi);

    (void)strcpy(junk, strcat(GetString(dvi, (int)(Get1Byte(dvi) +
     Get1Byte(dvi))), (*GetFontSuffix)(newfont->font_mag, newfont->font_dsz)));

    newfont->fontfilename = AbsPath(junk);
    font_node[(int)def_byte] = *newfont;

    if ((fd = FontOpen((int)def_byte)) == ERR) {
	(void) fprintf(stderr, "! Can't open the font file(%d)(%s).\n", def_byte, junk);
	exit(1);
    }

    read4(fd, IdOfPxl);
    newfont->pxl_data.pxl_id =  IdOfPxl;

    if (IdOfPxl == (Sig4Byte)PXLID) {
	if ((newfont->GlyphData = (GLYPH_DATA *)malloc(GLYPH_SIZEOF_EFONT)) == NULL)
	    cant_alloc("Glyph Data");	/* exit */
#if TATE
	if ((newfont->DownLoad = (short *)malloc(SIZEOF_EFONT_MISC * 2)) == NULL)
	    cant_alloc("Download flag");	/* exit */
#else
	if ((newfont->DownLoad = (short *)malloc(SIZEOF_EFONT_MISC)) == NULL)
	    cant_alloc("Download flag");	/* exit */
#endif

	/*
	 * Initialize the font
	 */
	for (i = 0; i < E_FONT_CHAR; ++i)
#if TATE
{
	    ((short (*)[2])(newfont->DownLoad))[i][0] = UNLOAD;
	    ((short (*)[2])(newfont->DownLoad))[i][1] = UNLOAD;
}
#else
	    newfont->DownLoad[i] = UNLOAD;
#endif

	if ((newfont->TFMwidth = (Sig4Byte *)malloc(SIZEOF_EFONT_MISC)) == NULL)
	    cant_alloc("tfm_width");	/* exit */

	(void)lseek(fd, (long)(-SIZEof_PXL), 2);
#if	BYTESWAP
	(void)read(fd, (char *)&(newfont->pxl_data), SIZEof_PXL);
	newfont->pxl_data.pxl_sum = SWAP4(newfont->pxl_data.pxl_sum);
	newfont->pxl_data.pxl_mag = SWAP4(newfont->pxl_data.pxl_mag);
	newfont->pxl_data.pxl_dsn = SWAP4(newfont->pxl_data.pxl_dsn);
	newfont->pxl_data.pxl_dir = SWAP4(newfont->pxl_data.pxl_dir);
	newfont->pxl_data.pxl_id = SWAP4(newfont->pxl_data.pxl_id);
#else
	(void)read(fd, (char *)&(newfont->pxl_data), SIZEof_PXL);
#endif

	(void)lseek(fd, (long) newfont->pxl_data.pxl_dir * 4, 0);
#if	BYTESWAP
	(void)read(fd, (char *)newfont->GlyphData, SIZEof_EFONT);
	{ register int i;
	register GLYPH_DATA	*gp;
	for (i = 0; i < E_FONT_CHAR; i++) {
	gp = &(newfont->GlyphData[i]);
	gp->width = SWAP2(gp->width);
	gp->height = SWAP2(gp->height);
	gp->xoffset = SWAP2(gp->xoffset);
	gp->yoffset = SWAP2(gp->yoffset);
	gp->dataoffset = SWAP4(gp->dataoffset);
	gp->tfmwidth = SWAP4(gp->tfmwidth);
	}
	}
#else
	(void)read(fd, (char *)newfont->GlyphData, SIZEof_EFONT);
#endif
    }
#if DIT_LOCAL
    else if (IdOfPxl == (Sig4Byte)DITID) {
	if ((newfont->GlyphData = (GLYPH_DATA *)malloc(GLYPH_SIZEOF_DFONT)) == NULL)
	    cant_alloc("Glyph Data area for Japanese font");	/* exit */
	if ((newfont->DownLoad = (short *)malloc(SIZEOF_DFONT_MISC)) == NULL)
	    cant_alloc("Down flag for Japanese font");	/* exit */

	/*
	 * Initialize the font
	 */
	for (i = 0; i < D_FONT_CHAR; ++i)
	    newfont->DownLoad[i] = UNLOAD;

	if ((newfont->TFMwidth = (Sig4Byte *)malloc(SIZEOF_DFONT_MISC)) == NULL)
	    cant_alloc("for tfm_width for Japanese font");	/* exit */

	(void)lseek(fd, (long)(-SIZEof_PXL), 2);
#if	BYTESWAP
	(void)read(fd, (char *)&(newfont->pxl_data), SIZEof_PXL);
	newfont->pxl_data.pxl_sum = SWAP4(newfont->pxl_data.pxl_sum);
	newfont->pxl_data.pxl_mag = SWAP4(newfont->pxl_data.pxl_mag);
	newfont->pxl_data.pxl_dsn = SWAP4(newfont->pxl_data.pxl_dsn);
	newfont->pxl_data.pxl_dir = SWAP4(newfont->pxl_data.pxl_dir);
	newfont->pxl_data.pxl_id = SWAP4(newfont->pxl_data.pxl_id);
#else
	(void)read(fd, (char *)&(newfont->pxl_data), SIZEof_PXL);
#endif

	(void)lseek(fd, (long) newfont->pxl_data.pxl_dir * 4, 0);
#if	BYTESWAP
	(void)read(fd, (char *)newfont->GlyphData, SIZEof_EFONT);
	newfont->GlyphData->width = SWAP2(newfont->GlyphData->width);
	newfont->GlyphData->height = SWAP2(newfont->GlyphData->height);
	newfont->GlyphData->xoffset = SWAP2(newfont->GlyphData->xoffset);
	newfont->GlyphData->yoffset = SWAP2(newfont->GlyphData->yoffset);
	newfont->GlyphData->dataoffset = SWAP4(newfont->GlyphData->dataoffset);
	newfont->GlyphData->tfmwidth = SWAP4(newfont->GlyphData->tfmwidth);
#else
	(void)read(fd, (char *)newfont->GlyphData, SIZEof_EFONT);
#endif
    }
#endif
#if JXL
    else if (IdOfPxl == (Sig4Byte)JXL4ID) {

	jxl4info(fd, (int)def_byte);

	if ((newfont->GlyphData = (GLYPH_DATA *)malloc(GLYPH_SIZEOF_DFONT)) == NULL)
	    cant_alloc("Glyph Data area for Japanese font");	/* exit */
/*
	if ((newfont->DownLoad = (int *)malloc(jxl4_info[(int)def_byte].jxl4.dir_size * sizeof(int))) == NULL)
*/
	if ((newfont->DownLoad = (short *)malloc(SIZEOF_DFONT_MISC)) == NULL)
	    cant_alloc("Down flag for Japanese font");	/* exit */

	/*
	 * Initialize the font.
	 */
	for (i = 0; i < D_FONT_CHAR; ++i)
	    newfont->DownLoad[i] = UNLOAD;

	if ((newfont->TFMwidth = (Sig4Byte *)malloc(SIZEOF_DFONT_MISC)) == NULL)
	    cant_alloc("tfm_width for Japanese font");	/* exit */
	
    }
#endif
    else {
	(void) fprintf(stderr, "! I don't know pxl id (%d) of %s.\n",
		IdOfPxl, newfont->fontfilename);
	error("Some font can not use in this program");	/* exit */
    }

    font_node[(int)def_byte] = *newfont;
}

#if JXL

jxl4info(font_fd, num)
int font_fd;
int num;
{
    Sig4Byte	jxl4_id;
    unsigned int	size;
    int	rsize;
    register JXL4	*jxl4p;

    jxl4p = &(jxl4_info[num].jxl4);

    (void)lseek(font_fd, 0L, L_SET); /* rewind */
    read4(font_fd, jxl4_id);

    (void)lseek(font_fd, -4L, L_XTND);
    read4(font_fd, jxl4_id);

    (void)lseek(font_fd, -64L, L_XTND);
#if	BYTESWAP
    (void)read(font_fd, (char *)jxl4p, SIZEof_JXL4);
    jxl4p->check_sum = SWAP4(jxl4p->check_sum);
    jxl4p->magnification = SWAP4(jxl4p->magnification);
    jxl4p->design_size = SWAP4(jxl4p->design_size);
    jxl4p->fixed_pxl_size = SWAP4(jxl4p->fixed_pxl_size);
    jxl4p->fixed_tfm_size = SWAP4(jxl4p->fixed_tfm_size);
    jxl4p->tfm_ptr = SWAP4(jxl4p->tfm_ptr);
    jxl4p->tfm_size = SWAP4(jxl4p->tfm_size);
    jxl4p->x_info_ptr = SWAP4(jxl4p->x_info_ptr);
    jxl4p->x_info_size = SWAP4(jxl4p->x_info_size);
    jxl4p->y_info_ptr = SWAP4(jxl4p->y_info_ptr);
    jxl4p->y_info_size = SWAP4(jxl4p->y_info_size);
    jxl4p->dir_size = SWAP4(jxl4p->dir_size);
    jxl4p->dir_ptr_0 = SWAP4(jxl4p->dir_ptr_0);
    jxl4p->dir_ptr_1 = SWAP4(jxl4p->dir_ptr_1);
    jxl4p->dir_ptr_2 = SWAP4(jxl4p->dir_ptr_2);
#else
    (void)read(font_fd, (char *)jxl4p, SIZEof_JXL4);
#endif

    read4(font_fd, jxl4_id);

    /*
     * Get all tfm-width table entry.
     */

    size = sizeof(tfm_table) * jxl4p->tfm_size;
    rsize = SIZEof_TFM_TABLE * jxl4p->tfm_size;

    if ((jxl4_info[num].tfmtable = (TFM_TABLE *)malloc(size)) == NULL)
	cant_alloc("tfm_width");	/* exit */

    (void) lseek(font_fd, (long)jxl4p->tfm_ptr, L_SET);
#if	BYTESWAP
    (void)read(font_fd, (char *)jxl4_info[num].tfmtable, rsize);
    {
    register int	i;
    register TFM_TABLE	*tp;

    for (i = 0; i < jxl4p->tfm_size; i++) {
	tp = &(jxl4_info[num].tfmtable[i]);
	tp->entry = SWAP4(tp->entry);
    }
    }
#else
    (void)read(font_fd, (char *)jxl4_info[num].tfmtable, rsize);
#endif

    /*
     * Get X_info & Y_info.
     */

    size = sizeof(x_info) * jxl4p->x_info_size;
    rsize = SIZEof_X_INFO * jxl4p->x_info_size;

    if ((jxl4_info[num].xinfo = (X_INFO *)malloc(size)) == NULL)
	cant_alloc("x_info");	/* exit */

    (void)lseek(font_fd, (long)jxl4p->x_info_ptr, L_SET);
#if	BYTESWAP
    (void)read(font_fd, (char *)jxl4_info[num].xinfo, rsize);
    {
    register int	i;
    register X_INFO	*xp;

    for  (i = 0; i < jxl4p->x_info_size; i++) {
	xp = &(jxl4_info[num].xinfo[i]);
	xp->x_offset = SWAP2(xp->x_offset);
	xp->x_pix = SWAP2(xp->x_pix);
	xp->x_offset_p = SWAP2(xp->x_offset_p);
    }
    }
#else
#ifdef PYRAMID
    {
    register int	i;
    register short	*pp, *ppp;
    register X_INFO	*xp;

    pp = (short *)malloc(rsize);
    if (pp == NULL) {
	perror("malloc");
	exit(1);
    }
    (void)read(font_fd, (char *)pp, rsize);
    xp = jxl4_info[num].xinfo;
    ppp = pp;
    for  (i = 0; i < jxl4p->x_info_size; i++, xp++) {
	xp->x_offset = *ppp++;
	xp->x_pix = *ppp++;
	xp->x_offset_p = *ppp++;
    }
    free(pp);
    }
#else
    (void)read(font_fd, (char *)jxl4_info[num].xinfo, rsize);
#endif
#endif

    size = sizeof(y_info) * jxl4p->y_info_size;
    rsize = SIZEof_Y_INFO * jxl4p->y_info_size;

    if ((jxl4_info[num].yinfo = (Y_INFO *)malloc(size)) == NULL)
	cant_alloc("y_info");	/* exit */

    (void)lseek(font_fd, (long)jxl4p->y_info_ptr, L_SET);
#if	BYTESWAP
    (void)read(font_fd, (char *)jxl4_info[num].yinfo, rsize);
    {
    register int	i;
    register Y_INFO	*yp;

    for  (i = 0; i < jxl4p->y_info_size; i++) {
	yp = &(jxl4_info[num].yinfo[i]);
	yp->y_offset = SWAP2(yp->y_offset);
	yp->y_pix = SWAP2(yp->y_pix);
	yp->y_offset_p = SWAP2(yp->y_offset_p);
    }
    }
#else
#ifdef PYRAMID
    {
    register int	i;
    register short	*pp, *ppp;
    register Y_INFO	*yp;

    pp = (short *)malloc(rsize);
    if (pp == NULL) {
	perror("malloc");
	exit(1);
    }
    (void)read(font_fd, (char *)pp, rsize);
    yp = jxl4_info[num].yinfo;
    ppp = pp;
    for  (i = 0; i < jxl4p->y_info_size; i++, yp++) {
	yp->y_offset = *ppp++;
	yp->y_pix = *ppp++;
	yp->y_offset_p = *ppp++;
    }
    free(pp);
    }
#else
    (void)read(font_fd, (char *)jxl4_info[num].yinfo, rsize);
#endif
#endif

    /*
     * Get pxl_dir information.
     */

    size = sizeof(pxl_dir) * jxl4p->dir_size;

    if ((jxl4_info[num].pxldir = (PXL_DIR *)malloc(size)) == NULL)
	cant_alloc("pxl_dir");	/* exit */
}
#endif
