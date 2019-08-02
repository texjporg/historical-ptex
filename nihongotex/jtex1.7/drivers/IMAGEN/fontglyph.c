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
 * fontglyph.c - get target font's glyph data, depending on suffix.
 */

/*
 * $Id: fontglyph.c,v 1.28 90/03/27 11:00:34 void Exp $
 */

#include <stdio.h>
#include <sys/file.h>
#include "dvicom.h"
#include "config.h"
#include "fio.h"
#include "font.h"
#include "jxl4.h"

#if TATE
#include "stack.h"
extern STACK Curr;
GLYPH_DATA  gf_rot;
#endif

#if JXL
extern JXL4_INFO    jxl4_info[];
extern		    jxlexp();
#endif

extern NewFontFileInfo font_node[];
extern int             use_landscapemode;

extern long     lseek();
extern char     *malloc();
extern Sig4Byte	SPtoDEV(), AdvanceWidth();


hidden	char *
Rotation(glyph, glyph_data)
char	*glyph;
GLYPH_DATA *glyph_data;
{
    register char	*new_rast;  /* new raster */
    register char	*old_rast;  /* old raster */
    register Sig2Byte	old_height; /* old raster height, new raster width */
    register Sig2Byte	old_width;  /* old raster width, new raster height */
    unsigned int	size;	    /* size of new raster (in bytes) */
    int			new_plus;   /* offset between rows in new_rast */

    /*
     * First, get a new raster. 
     */
    {	register int    t;

	old_height = glyph_data->height;
	old_width  = glyph_data->width;

	/*
	 * Offset is (new width) rounded to bytes. 
	 */
	new_plus = (old_height + 7) / 8;

	/*
	 * Size of new raster is as follows.
	 * (new height) * (new rounded width, in bytes). 
	 */
	size = new_plus * old_width;

	if ((new_rast = (char *) malloc(size)) == NULL)
	    cant_alloc("rotation");	/* exit */
#if MSDOS
	(void) memset(new_rast, 0, size);
#else
	bzero(new_rast, (int) size);
#endif

	/*
	 * new y origin is old x origin; new x origin is old height minus
	 * old y origin - 1. 
	 */
	t = glyph_data->yoffset;
#if TATE
	gf_rot.yoffset = glyph_data->xoffset;
	gf_rot.xoffset = old_height - t - 1;
#else
	glyph_data->yoffset = glyph_data->xoffset;
	glyph_data->xoffset = old_height - t - 1;
#endif

	/*
	 * While we are at it, exchange height & width...
	 */
#if TATE
	gf_rot.height = old_width;
	gf_rot.width  = old_height;
#else
	glyph_data->height = old_width;
	glyph_data->width  = old_height;
#endif

	/*
	 * ... and grab a pointer to the old raster.
	 */
	old_rast = glyph;
    }

    /*
     * Now copy bits from the old raster to the new one.  The mapping
     * function is 
     *
     * for i in [0..height]
     *     for j in [0..width]
     *	       new[j, height - i - 1] = old[i, j] 
     *
     * Thus i maps to height - i - 1 and (since we have to do our own 2
     * dimensional array indexing) j to new_plus * j.  We call the mapped
     * variables i and j, and, since we scan sequentially through the old
     * raster, can discard the original i and j. 
     */
    {
	register int    i, j, c, k;

	i = old_height;
	old_width *= new_plus;

	while (--i >= 0) {
	    k = 7;
	    for (j = 0; j < old_width; j += new_plus) {
		/*
		 * get another byte
		 */
		if (++k == 8)
		    c = *old_rast++, k = 0;
		/*
		 * old[i, j] was set
		 */
		if (c & 0x80)
		    new_rast[j + (i >> 3)] |= 1 << (7 - (i & 7));

		c <<= 1;
	    }
#if 0	/* for word boundary */
	    old_rast += mod;
#endif
	}
    } /* scope */

    /*
     * Finally, free the storage associated with the original raster. 
     */
    (void) free(glyph);
    return (new_rast);
}

visible char *
PXLGlyph(i, num)
int i;
int num;
{

    char    *byte_map, *word_map;
    int	    fd;
    int	    b_width;
#if JXL
    Sig4Byte    long_ptr;
#endif
    register int    size, h;
    register char   *op, *np;

    char    *Rotation();
    int	    FontOpen();

    /*
     * Open Font File.
     */
    fd = FontOpen(num);

#if JXL
    if (font_node[num].pxl_data.pxl_id != JXL4ID) {
#endif

    size = ((font_node[num].GlyphData[i].width + 31) / 32) * sizeof(long);

    (void)lseek(fd, (long)font_node[num].GlyphData[i].dataoffset * sizeof(long), 0);

    if ((word_map = (char *) malloc((unsigned int) (size *
	font_node[num].GlyphData[i].height))) == NULL)
	cant_alloc("map");	/* exit */

    (void)read(fd, word_map, size * font_node[num].GlyphData[i].height);

    /*
     * convert word boundary to byte boundary
     */

    b_width  = (font_node[num].GlyphData[i].width + 7) / 8;

    if ((byte_map = (char *) malloc((unsigned)b_width *
	(font_node[num].GlyphData[i].height))) == NULL)
	cant_alloc("map");	/* exit */

    if (size == b_width)
	byte_map = word_map;
    else {
	np = byte_map;
	op = word_map;
	for (h = 0; h < font_node[num].GlyphData[i].height; h++) {
#if MSDOS
	    (void) memcpy(np, op, b_width);
#else
	    (void) bcopy(op, np, b_width);
#endif
	    op += size;
	    np += b_width;
	}
    }

    font_node[num].TFMwidth[i] =
	AdvanceWidth(font_node[num].GlyphData[i].tfmwidth, font_node[num].font_mag);

#if JXL
    }
    else {
	size = i;

	if (size < 1410) {  /* non kanji */
	    long_ptr = jxl4_info[num].jxl4.dir_ptr_0;
	}
	else if (size < 4418) {	/* level-1 kanji */
	    size -= 1410;
	    long_ptr = jxl4_info[num].jxl4.dir_ptr_1;
	}
	else {	/* level-2 kanji */
	    size -= 4418;
	    long_ptr = jxl4_info[num].jxl4.dir_ptr_2;
	}
	(void)lseek(fd, (long)(long_ptr + (SIZEof_PXL_DIR * size)), L_SET);
#if	BYTESWAP
	{
	char tbuf[SIZEof_PXL_DIR];
	PXL_DIR	*pp;

	(void)read(fd, tbuf, SIZEof_PXL_DIR);
	pp = &(jxl4_info[num].pxldir[i]);

	pp->tfm_index = tbuf[0] & 0xff;

	pp->rat_index = tbuf[1] & 0xff;
	pp->rat_index = pp->rat_index << 8 | (tbuf[2] & 0xff);
	pp->rat_index = pp->rat_index << 8 | (tbuf[3] & 0xff);

	pp->x_info_index = tbuf[4] & 0xff;
	pp->x_info_index = pp->x_info_index << 8 | (tbuf[5] & 0xff);

	pp->y_info_index = tbuf[6] & 0xff;
	pp->y_info_index = pp->y_info_index << 8 | (tbuf[7] & 0xff);
	}
#else
	(void)read(fd, (char *)&(jxl4_info[num].pxldir[i]), SIZEof_PXL_DIR);
#endif

	size = (int) jxl4_info[num].pxldir[i].x_info_index;
	font_node[num].GlyphData[i].width = jxl4_info[num].xinfo[size].x_pix;
	b_width = (font_node[num].GlyphData[i].width + 7) / 8;
	font_node[num].GlyphData[i].xoffset = jxl4_info[num].xinfo[size].x_offset;

	size = (int) jxl4_info[num].pxldir[i].y_info_index;
	font_node[num].GlyphData[i].height = jxl4_info[num].yinfo[size].y_pix;
	font_node[num].GlyphData[i].yoffset = jxl4_info[num].yinfo[size].y_offset;

	size = (int) jxl4_info[num].pxldir[i].tfm_index;
	font_node[num].GlyphData[i].tfmwidth = jxl4_info[num].tfmtable[size].entry;

	if ((byte_map = (char *)malloc(
	(unsigned int)(b_width * font_node[num].GlyphData[i].height))
	) == NULL)
	    cant_alloc("raster");	/* exit */

	(void)lseek(fd, (long)jxl4_info[num].pxldir[i].rat_index, L_SET);

	jxlexp(fd, byte_map, b_width,
	    font_node[num].GlyphData[i].width, font_node[num].GlyphData[i].height);

	font_node[num].TFMwidth[i] =
	    AdvanceWidth(font_node[num].GlyphData[i].tfmwidth,
		font_node[num].font_mag);
    }
#endif

    /*
     * landscape
     */

#if TATE

    gf_rot.width = font_node[num].GlyphData[i].width;
    gf_rot.height = font_node[num].GlyphData[i].height;
    gf_rot.xoffset = font_node[num].GlyphData[i].xoffset;
    gf_rot.yoffset = font_node[num].GlyphData[i].yoffset;
    gf_rot.dataoffset = font_node[num].GlyphData[i].dataoffset;
    gf_rot.tfmwidth = font_node[num].GlyphData[i].tfmwidth;

    if (use_landscapemode || Curr.Direction == TATE_DIR)
#else
    if (use_landscapemode)
#endif
	byte_map = Rotation(byte_map, &(font_node[num].GlyphData[i]));

    return(byte_map);
}
