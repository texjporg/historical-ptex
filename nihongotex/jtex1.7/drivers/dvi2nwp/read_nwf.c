/*
 *	read_nwf.c for jxdvi/dvi2nwp
 *	$Header: read_nwf.c,v 3.1 89/02/25 23:01:53 atsuo Exp $
 *		written by Atsuo, K.
 */

#include "atsuo-copyright.h"  

#include "config.h"

#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include "jxdvi.h"
#include "font.h"

open_nwf_file(font)
struct k_font *font;
{
	char filename[300];
	extern int errno;

	if (font->file == NULL) {
	    sprintf(filename, "%s/%s",
			NWF_FONT_DIR, font->fontname);

            if (n_open_k_fonts == MAX_OPEN_K_FONTS)
               close_k_lru();
            font->file = fopen(filename, "r");
	    if (font->file == NULL) {
		return (0);
	    }
            n_open_k_fonts += 1;
    	}
	return (1);
}

read_nwf_bits(kf,kb,code)
  struct k_font *kf;
  struct k_bitmap	*kb;
  unsigned int code;
{
	int i, j;
	int byte_paddings;
	struct bitmap b;
	unsigned char	*ptr;

	byte_paddings = 
		((kf->file_bytes_wide*BITS_PER_BYTE)%BITS_PER_SHORT) ? 1 : 0;

	b.h = kf->file_h;
	b.w = kf->file_w;
	b.bytes_wide = kf->bytes_wide = kf->file_bytes_wide + byte_paddings;

	ptr = b.bits = kb->bits = (unsigned char *)
		malloc( ( sizeof(char) * kf->file_h * kf->bytes_wide ) );
	if (ptr == NULL)
	  error("jxdvi: Can't allocate bitmap for char %d, font %s (%d by %d)",
		code, kf->fontname, b.h, b.w);

	if (!open_nwf_file(kf))
	  error("jxdvi: Can't find font file %s", kf->fontname);

	fseek(kf->file,
	      (long)(jis_to_index(code)*kf->file_bytes_wide*kf->file_h),0);

	for (i = 0; i < b.h; i += 1) {
	    for (j = 0; j < kf->file_bytes_wide; j += 1) {
		unsigned long f;
		f = one(kf->file);
#ifdef JXDVI
# ifndef X11 /* X10 */
			*ptr++ = (unsigned char) reverse_byte[f];
# else /* X11 */
			*ptr++ = (unsigned char) f;
# endif /* X Version */
#endif /* JXDVI */
# ifdef DVI2NWP
			*ptr++ = (unsigned char) f;
# endif DVI2NWP
	    }
	    if (byte_paddings) {
		*ptr++ = 0x00;
	    }
	}

#ifdef JXDVI
	if (shrink_factor != 1) {
	  shrink_bitmap(&b, shrink_factor, shrink_factor);
	}

	kf->h = b.h;
	kf->w = b.w;
	kf->bytes_wide = b.bytes_wide;

	kb->bits = b.bits;
#  ifndef X11 /* X10 */
	if (backwards) reverse_bytes(&b);
#  endif
#endif JXDVI

#ifdef STATISTICS
	kf->load += 1;
	kf->load_bytes += (kf->file_h * kf->file_bytes_wide);
#endif
}

