/*
 *		read_dnf.c for jxdvi/dvi2nwp
 *		written by Setsu Suzuki, setsu@aml.co.jp
 */

/*
 *	The functions jxlexp(), expand[1-3](), getnyb(), pcknum(),
 *	and part of jxl4info() followed the same functions in the
 *	IMAGEN's driver jxl4.c and fontglyph.c by the dit Co., Ltd.
 *
 *	Followings are the Copy Right Notice of ASCII Co. and 
 *	dit Co., Ltd.  They have the first priority of these functions.
*/

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

#include "config.h"

#ifdef DNF

#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include "jxdvi.h"
#include "font.h"

#define E_FONT_CHAR	256	/* number of charas in a font */
#define D_FONT_CHAR	(94 * 94)	/* number of charas in a font */

#define SIZEOF_EFONT_MISC   (E_FONT_CHAR * sizeof(int))
#define SIZEOF_DFONT_MISC   (D_FONT_CHAR * sizeof(int))
#define GLYPH_SIZEOF_EFONT  (E_FONT_CHAR * sizeof(GLYPH_DATA))
#define GLYPH_SIZEOF_DFONT  (D_FONT_CHAR * sizeof(GLYPH_DATA))
#define SIZEof_EFONT        (E_FONT_CHAR * SIZEof_GLYPH_DATA)
#define SIZEof_DFONT        (D_FONT_CHAR * SIZEof_GLYPH_DATA)

extern char	*malloc(), *strcpy();
extern long	lseek();

static	int  rep;
static	int  peek;
static  int  lastrep;
static	char bitset[] = { 128, 64, 32, 16, 8, 4, 2, 1};
static	char expf;
static	unsigned char *bufp, *maxbuf;

static	unsigned char *bufp_res = NULL;
static	int	buf_size_res = 0;

open_dnf_file(font)
struct k_font *font;
{
	char filename[300];
	extern int errno;
	int	len;

	if (font->file == NULL) {
	    sprintf(filename, "%s/%s",
			DNF_FONT_DIR, font->fontname);
	    len = strlen( filename );
	    strcpy ( &filename[len-3], DNF_FONT_EXT );

            if (n_open_k_fonts == MAX_OPEN_K_FONTS)
               close_k_lru();
            font->file = fopen(filename, "r");
	    if (font->file == NULL) {
		return (0);
	    }
            n_open_k_fonts += 1;
	    len = strlen(font->fontname);
	    strcpy(&(font->fontname[len-3]), DNF_FONT_EXT);
    	}
	return (1);
}

init_dnf_index (kf)
     struct k_font *kf;
{
  struct JXL4_INFO	*jxl4_infop;
  int	fd;
  int	id_of_pxl;

  jxl4_infop = kf->jxl4_info = 
    	(struct JXL4_INFO *)malloc(sizeof(struct JXL4_INFO) );
  if ( jxl4_infop == NULL )
    error ( "jxdvi: cannot allocate jxl4_info" );

  fd = fileno( kf->file );
  read4(fd, id_of_pxl);
  
  if (id_of_pxl == (int)JXL4ID) {
    jxl4info(fd, jxl4_infop);
  }
  else {
    (void) fprintf(stderr, "! I don't know pxl id (%d) of %s.\n",
		   id_of_pxl, kf->fontname);
    error("Some fonts can not be use in this program");	/* exit */
  }

}

read_dnf_bits(kf,kb,code)
  struct k_font *kf;
  struct k_bitmap	*kb;
  unsigned int code;
{
  register struct JXL4_INFO *jxl4_infop;
  int i, j;
  struct bitmap b;

  int	    fd;
  int	    b_width;
  int    long_ptr;
  register int    size, h;
  register char   *op, *np;
  int	xoffset_l, yoffset_l, tfmwidth_l;


  if (!open_dnf_file(kf))
    error("jxdvi: Can't find font file %s", kf->fontname);

  fd = fileno( kf->file );
  jxl4_infop = kf->jxl4_info;

  size = i = jis_to_dnf_index( code );

  if (size < 1410) {  /* non kanji */
    long_ptr = jxl4_infop->jxl4->dir_ptr_0;
  }
  else if (size < 4418) {	/* level-1 kanji */
    size -= 1410;
    long_ptr = jxl4_infop->jxl4->dir_ptr_1;
  }
  else {	/* level-2 kanji */
    size -= 4418;
    long_ptr = jxl4_infop->jxl4->dir_ptr_2;
  }

  (void)lseek(fd, (long)(long_ptr + (SIZEof_PXL_DIR * size)), 0);
  (void)read(fd, (char *)&(jxl4_infop->pxldir[i]), SIZEof_PXL_DIR);

  size = (int) jxl4_infop->pxldir[i].x_info_index;
  b.w = kb->w = jxl4_infop->xinfo[size].x_pix;
  b_width = b.bytes_wide = kb->bytes_width = (b.w + 7) / 8;
  kb->x = jxl4_infop->xinfo[size].x_offset;

  size = (int) jxl4_infop->pxldir[i].y_info_index;
  b.h = kb->h = jxl4_infop->yinfo[size].y_pix;
  kb->y = jxl4_infop->yinfo[size].y_offset;

  size = (int) jxl4_infop->pxldir[i].tfm_index;
  tfmwidth_l = jxl4_infop->tfmtable[size].entry;

  if ((b.bits = kb->bits = (unsigned char *)
				malloc((unsigned int)(b_width * b.h)))
				 == NULL)
    error("jxdvi: Can't allocate bitmap for char %d, font %s (%d by %d)",
	  code, kf->fontname, b.h, b.w);

  (void)lseek(fd, (long)jxl4_infop->pxldir[i].rat_index, 0);

  jxlexp(fd, b.bits, b_width, b.w, b.h);

#ifdef JXDVI
#ifndef X11 /* X10 */
  	size = b_width * b.h; 
	reverse_x10(b.bits, size );
#endif /* X10 */
#endif /* JXDVI */

/*
  font_node[num].TFMwidth[i] =
    AdvanceWidth(font_node[num].GlyphData[i].tfmwidth,
		 font_node[num].font_mag);
*/

#ifdef JXDVI
	if (shrink_factor != 1) {
	  shrink_bitmap(&b, shrink_factor, shrink_factor);
	}

	kb->h = b.h;
	kb->w = b.w;
	kb->bytes_width = b.bytes_wide;
	kb->bits = b.bits;
	kb->x /= shrink_factor;
	kb->y /= shrink_factor;

#  ifndef X11 /* X10 */
	if (backwards) reverse_bytes(&b);
#  endif
#endif JXDVI

#ifdef STATISTICS
	kf->load += 1;
	kf->load_bytes += (kf->file_h * kf->file_bytes_wide);
#endif
}

jxlexp(fp, raster, hbytes, width, height)
int  fp;
char *raster;
int  hbytes;
int  width, height;
{
    unsigned char	x;
    unsigned int	size;
    int	flag;
    int	t;

    (void)read(fp, (char *)&x, 1);
    flag = x;

    if ((flag & 0x0f) == 14) {
	size = ((width * height) + 7) / 8;
    	if ( size > buf_size_res ) {
		if ( bufp_res != NULL )
			free ( bufp_res );
		if ((bufp = bufp_res = 
				(unsigned char *)malloc(size)) == NULL)
	    		error("expand");	/* exit */
		buf_size_res = size;
    	}
    	else {
		bufp = bufp_res;
	}
	
	(void)read(fp, (char *)bufp, size);
	expand0(raster, hbytes, width, height);
    }
    else {
	t = ((flag >> 4) & 3) + 1;

	size = 0;
	while (t--) {
	    (void)read(fp, (char *)&x, 1);
	    size = (size << 8) + x;
	}
	if ( size > buf_size_res ) {
		if ( bufp_res != NULL )
			free ( bufp_res );
		if ((bufp = bufp_res = 
				(unsigned char *)malloc(size)) == NULL)
	    		error("expand");	/* exit */
		buf_size_res = size;
	}
	else {
		bufp = bufp_res;
	}

	maxbuf = bufp + size;
	(void)read(fp, (char *)bufp, size);

	if ((flag & 0x40) == 0)
	    expand1(flag, raster, hbytes, width, height);
	else
	    expand2(flag, raster, hbytes, width, height);
    }
}

static
expand0(raster, hbytes, width, height)
unsigned char *raster;
int hbytes;
int width, height;
{
    int	i;
    int	shift;
    int	mod;

    if ((mod = width % 8) == 0) {
	for (i = 0; i < hbytes * height; i++)
	    *raster++ = *bufp++;
    }
    else {
	for (shift = 0; height-- > 0; shift = (shift + mod) % 8) {
	    for (i = 0; i < hbytes - 1; i++) {
		*raster = *bufp << shift;
		*raster |= *(++bufp) >> (8 - shift);
		++raster;
	    }
	    *raster = *bufp << shift;
	    if (mod + shift >= 8)
		*raster |= *(++bufp) >> (8 - shift);
	    *raster &= 0xff << (8 - mod);
	    ++raster;
	}
    }
}

static
expand1(flag, raster, hbytes, width, height)
int flag;
register char *raster;
int hbytes;
int width, height;
{
    register int    n;
    register int    hpos, bit;
    register char  *lastrow;
    int             black;
    int             dynf;

    expf = 0;
    black = (flag & 0x80) != 0;
    dynf = flag & 0x0f;

    rep = bit = peek = hpos = 0;
    lastrow = raster;

    while (height > 0) {
	n = pcknum(dynf);
	while (n--) {
	    if (black)
		bit |= bitset[hpos & 7];

	    hpos++;

	    if (hpos == width) {
		*raster++ = bit;

		if (--height == 0)
		    return;

		while (rep > 0) {
		    for (hpos = 0; hpos < hbytes; hpos++)
			*raster++ = *lastrow++;

		    if (--height == 0)
			break;

		    rep--;
		}
		lastrow += hbytes;
		bit = hpos = 0;
	    }
	    else if ((hpos & 7) == 0) {
		*raster++ = bit;
		bit = 0;
	    }
	}
	black = !black;
    }
}

static
expand2(flag, raster, hbytes, width, height)
register char *raster;
{
    register int    n, i;
    register int    hpos;
    register char  *lastrow;

    expf = 1;
    rep = peek = 0;
    hpos = -1;
    lastrow = raster;

    for (i = 0; i < hbytes; ++i)
	lastrow[i] = 0;

    while (height > 0) {
	n = pcknum(flag & 0xf);

	if (n < 0) {
	    while (--height) {
		raster = lastrow + hbytes;

		for (i = 0; i < hbytes; i++)
		    *raster++ = *lastrow++;

	    }
	    return;
	}

	while (n--) {
	    hpos++;

	    if (!(hpos & 7) && hpos)
		raster++;

	    if (hpos == width) {
		if (!(--height))
		    return;
		if (hpos & 7)
		    raster++;

		for (i = 0; i < hbytes; i++)
		    *raster++ = *lastrow++;

		raster = lastrow;
		hpos = 0;
	    }
	}

	rep++;

	while (rep) {
	    *raster ^= bitset[hpos & 7];
	    hpos++;

	    if (!(hpos & 7) && hpos)
		raster++;

	    if (hpos == width) {
		if (!(--height))
		    return;
		if (hpos & 7)
		    raster++;

		for (i = 0; i < hbytes; i++)
		    *raster++ = *lastrow++;

		raster = lastrow;
		hpos = 0;
	    }
	    rep--;
	}
    }
}

static
getnyb()
{
    if (bufp >= maxbuf)
	return (-1);

    if (peek) {
	peek = 0;
	return (*bufp++ & 15);
    }
    else {
	peek = 1;
	return (*bufp >> 4);
    }
}

static
pcknum(dynf)
int dynf;
{
    register int    i, j;

    if ((i = getnyb()) < 0)
	return (-1);

    if (i == 0) {
	do {
	    j = getnyb();
	    i++;
	} while (j == 0);

	while (i > 0) {
	    j = j * 16 + getnyb();
	    i--;
	}
	return (j - 15 + (13 - dynf) * 16 + dynf);
    }
    else if (i <= dynf)
	return (i);
    else if (i < 14)
	return ((i - dynf - 1) * 16 + getnyb() + dynf + 1);
    else {
	if (rep != 0)
	    error("Extra repeat count");	/* exit */
	if (i == 14)
	    lastrep = rep = pcknum(dynf);
	else {
	    if (expf)
		rep = lastrep;
	    else
		rep = 1;
	}
	return (pcknum(dynf));
    }
}

jxl4info(font_fd, jxl4_infop)
int font_fd;
struct	JXL4_INFO *jxl4_infop;
{
    int	jxl4_id;
    unsigned int	size;
    int	rsize;
    register struct JXL4	*jxl4p;

    jxl4p = jxl4_infop->jxl4 = (struct JXL4 *)malloc(sizeof(struct JXL4));

    (void)lseek(font_fd, 0L, 0); /* rewind */
    read4(font_fd, jxl4_id);

    (void)lseek(font_fd, -4L, 2);
    read4(font_fd, jxl4_id);

    (void)lseek(font_fd, -64L, 2);
    (void)read(font_fd, (char *)jxl4p, SIZEof_JXL4);

    read4(font_fd, jxl4_id);

    /*
     * Get all tfm-width table entry.
     */

    size = sizeof(struct TFM_TABLE ) * jxl4p->tfm_size;
    rsize = SIZEof_TFM_TABLE * jxl4p->tfm_size;

    if ((jxl4_infop->tfmtable = (struct TFM_TABLE *)malloc(size)) == NULL)
	error("tfm_width");	/* exit */

    (void) lseek(font_fd, (long)jxl4p->tfm_ptr, 0);
    (void)read(font_fd, (char *)jxl4_infop->tfmtable, rsize);

    /*
     * Get X_info & Y_info.
     */

    size = sizeof(struct X_INFO) * jxl4p->x_info_size;
    rsize = SIZEof_X_INFO * jxl4p->x_info_size;

    if ((jxl4_infop->xinfo = (struct X_INFO *)malloc(size)) == NULL)
	error("x_info");	/* exit */

    (void)lseek(font_fd, (long)jxl4p->x_info_ptr, 0);
    (void)read(font_fd, (char *)jxl4_infop->xinfo, rsize);

    size = sizeof(struct Y_INFO) * jxl4p->y_info_size;
    rsize = SIZEof_Y_INFO * jxl4p->y_info_size;

    if ((jxl4_infop->yinfo = (struct Y_INFO *)malloc(size)) == NULL)
	error("y_info");	/* exit */

    (void)lseek(font_fd, (long)jxl4p->y_info_ptr, 0);
    (void)read(font_fd, (char *)jxl4_infop->yinfo, rsize);

    /*
     * Get pxl_dir information.
     */

    size = sizeof(struct PXL_DIR) * jxl4p->dir_size;

    if ((jxl4_infop->pxldir = (struct PXL_DIR *)malloc(size)) == NULL)
	error("pxl_dir");	/* exit */
}

#ifdef JXDVI
#ifndef X11
reverse_x10 ( buf, size )
register unsigned char	*buf;
register int	size;
{
	register int i;
	for ( i = 0; i < size; i++ ) {
		*buf = (unsigned char )reverse_byte[*buf];
		buf++;
	}
}
#endif /* X10 */
#endif /* JXDVI */

#endif /* DNF */
