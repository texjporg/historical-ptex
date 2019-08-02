/*
 *	read_v_nwf.c for jxdvi/dvi2nwp
 *	$Header: read_v_nwf.c,v 3.1 89/02/25 23:02:02 atsuo Exp $
 */

#include "atsuo-copyright.h"  

#include "config.h"

#ifdef V_NWF

#include <stdio.h>
#include "font.h"

int m_v_font_fd = -1, g_v_font_fd = -1;

/*
 * open_v_file() opens SONY NWF-605 (Outline Font) and returns
 * file descriptor.
 * We don't close it until jxdvi/dvi2nwp exits.
 */
open_v_file(font)
  struct k_font *font;
{
    char vfname[256];

    switch ((font->fontname[0])) {
      case 'm':
	if (m_v_font_fd > 0) {
	    return m_v_font_fd;
	} else {
	    sprintf(vfname, "%s/%s", V_NWF_FONT_DIR, M_V_FONT);
	    if ((m_v_font_fd=FS_open_font(vfname)) < 0) {
		return -1;	/* Not found */
	    } else {
		return m_v_font_fd;
	    }
	}
	/* break not needed */
      case 'g':
	if (g_v_font_fd > 0) {
	    return g_v_font_fd;
	} else {
	    sprintf(vfname,"%s/%s",V_NWF_FONT_DIR,G_V_FONT);
	    if ((g_v_font_fd=FS_open_font(vfname)) < 0) {
		return -1;	/* Not found */
	    } else {
		return g_v_font_fd;
	    }
	}
	/* break not needed */
      default:
	return -1;
    }
}

read_v_bits(kf,kb,code)
  struct k_font *kf;
  struct k_bitmap	*kb;
  unsigned int code;
{
    int i, j;
/*    int byte_paddings; */
    struct bitmap b;
    unsigned char *ptr;
    int fd;

/*
    byte_paddings = 
      ((kf->file_bytes_wide*BITS_PER_BYTE)%BITS_PER_SHORT) ? 1 : 0;
*/

    /* Here, we can calculate & set shrunken bitmap data, and
     * get already shrunken bitmap from Vector font.
     * However, it will produce a little bit thick character.
     * So we use shrink_bitmap because it makes the better character.
     */
    b.h = kf->h = kf->file_h;
    b.w = kf->w = kf->file_w;
    b.bytes_wide = kf->bytes_wide =
#ifdef JXDVI
#  ifndef X11 /* X10 */
	  ROUNDUP(b.w, BITS_PER_SHORT) * BYTES_PER_SHORT;
#  else /* X11 */
	  ROUNDUP(b.w, BITS_PER_BYTE);
#  endif /* X Version */
#endif /* JXDVI */
#ifdef DVI2NWP
	  ROUNDUP(b.w, BITS_PER_BYTE);
#endif /* DVI2NWP */

    ptr = b.bits = kb->bits = (unsigned char *)
      calloc( (sizeof(char) * b.h * b.bytes_wide), sizeof(char) );
    if (ptr == NULL)
      error("jxdvi: Can't allocate bitmap for char %d, font %s (%d by %d)",
	    code, kf->fontname, b.h, b.w);
    
    if ((fd=open_v_file(kf)) < 0)
      error("jxdvi: Can't find font file %s", kf->fontname);

    if ((FS_get_bitmap(c2s(code), fd, b.w, b.h, 
		       b.bytes_wide, 0, (char *) ptr, 
		       0, 0, 25)) < 0)
      error("jxdvi: Can't get bitmap from vfont. font: %s", kf->fontname);

#ifdef JXDVI
#  ifndef X11 /* X10 only */
    for (i = 0; i < b.h; i += 1) {
	for (j = 0; j < b.bytes_wide; j += 1) {
	    int f;
	    f = (int) *ptr;
	    *ptr = (unsigned char) reverse_byte[f];
	    ptr++;
	}
    }
#  endif /* X10 only */
#endif /* JXDVI */

#ifdef JXDVI
    if (shrink_factor != 1) {
	shrink_bitmap(&b, shrink_factor, shrink_factor);
    }

    kf->h = b.h;
    kf->w = b.w;
    kf->bytes_wide = b.bytes_wide;

    kb->bits = b.bits;
#  ifndef X11 /* X10 only */
    if (backwards) reverse_bytes(&b);
#  endif /* X10 only */
#endif JXDVI
/*
#ifdef STATISTICS
    kf->load += 1;
    kf->load_bytes += (kf->file_h * kf->file_bytes_wide);
#endif
*/
}

c2s(jis)		/* Calculate Serial Index */
  int jis;
{
    return ( ((jis>>8)-0x21)*94 + (jis&0xff)-0x21 );
}

print_v_bitmap(bitmap)
  struct bitmap *bitmap;
{
	register unsigned char *ptr;
	register int x, y, i;

	ptr = bitmap->bits;
	if (ptr == NULL)
		return;
	printf("w = %d, h = %d, bytes wide = %d\n",
		bitmap->w, bitmap->h, bitmap->bytes_wide);
	for (y = 0; y < bitmap->h; y += 1) {
		for (x = 0; x < bitmap->bytes_wide; x += 1) {
			for (i = 1; i <= BITS_PER_BYTE; i += 1)
				if (*ptr & (1 << (BITS_PER_BYTE - i)))
					putchar('*');
				else
					putchar(' ');
			ptr += 1;
		}
		putchar('\n');
	}
}
#endif /* V_NWF */
