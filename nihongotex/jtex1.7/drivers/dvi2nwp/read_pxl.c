/*
 *	readpxl.c for jxdvi/dvi2nwp
 *	$Header: read_pxl.c,v 3.1 89/02/25 23:01:42 atsuo Exp $
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

extern double conv;
extern int	debug;

#define pixel_round(x)      ((long) (conv * (double) (x) + 0.5))

extern char *malloc();

extern long	magnification, denominator, numerator;
extern int	pixels_per_inch;
extern struct font *current_font;
extern int shrink_factor;
extern int list_fonts;
extern unsigned char reverse_byte[];
extern int font_not_found;
extern int n_open_fonts;
extern unsigned long num();
extern int backwards;

extern char *calloc();

open_pxl_file(font)
	struct font *font;
{
	char filename[300];
	extern int errno;

	if (font->file == NULL) {
		sprintf(filename, "%s/%s",
				PXL_FONT_DIR, font->fontname);
		if (n_open_fonts == MAX_OPEN_FONTS)
			close_lru();
		font->file = fopen(filename, "r");
		if (font->file == NULL) {
/*			font_not_found = 1;*/
/*
#ifdef JXDVI
			printf("%s [not found]\n", font->fontname);
#endif
#ifdef DVI2NWP
			fprintf(stderr,"%s [not found]\n", font->fontname);
#endif
*/
			return (0);
		}
		n_open_fonts += 1;
	}
/*
	if (list_fonts)
		printf("%s\n", font->fontname);
*/
	return (1);
}

read_pxl_glyphs (fontp)
        register struct font *fontp;
{
	register struct glyph *g;
	long checksum, magnify, design_size, font_dir_ptr, pxl_id_word;

	/* seek to trailer info */
	fseek(fontp->file, (long) -(5 * BYTES_PER_LONG), 2);
        checksum = four(fontp->file);
        magnify = four(fontp->file);
        design_size = four(fontp->file);
        font_dir_ptr = sfour(fontp->file) * 4;
        pxl_id_word = four(fontp->file);
#ifdef lint
	magnify = design_size = pxl_id_word = magnify;
#endif
	/* seek to font directory */
	fseek(fontp->file, font_dir_ptr, 0);
	for (g = &fontp->glyph[0]; g < &fontp->glyph[MAXCHARS]; g += 1) {
#	ifdef JXDVI
		g->bitmap.bits = NULL;
#	endif JXDVI
#	ifdef DVI2NWP
		g->bitmap.bits = NULL;
#	endif DVI2NWP
		g->bitmap.w = two(fontp->file);	/* leave this for shrink_bitmap */
		g->bitmap.h = two(fontp->file);	/* leave this for shrink_bitmap */
		g->x = stwo(fontp->file) / shrink_factor;
		g->y = stwo(fontp->file) / shrink_factor;
		g->addr = four(fontp->file) * 4;
		/*
		**  The TFM-width word is kind of funny in the units
		**  it is expressed in.  It works this way:
		**
		**  If a glyph has width 'w' in a font with design-size
		**  'd' (both in same units), the TFM-width word is
		**
		**                    (w/d) * 2^20
		**
		**  Therefore, in order to find the glyph width in
		**  DVI units (1 / 2^16 points), we take the design-size
		**  'd' (in DVI's), the magnification 'm' of the PXL file
		**  and the TFM-width word 't' to the width (in DVI's)
		**  as follows:
		**
		**                     dmt
		**                w = -----
		**                    2^20
		**
		**  But the magnification of the PXL file is just the
		**  scaled size 's' over the design size, so the final
		**  expression for the width is
		**
		**                     st
		**                w = ----
		**                    2^20
		**
		*/      
		g->dvi_adv =
			((double) fontp->scale * four(fontp->file)) / (1 << 20);
		g->pxl_adv = pixel_round(g->dvi_adv);
	}
}

read_pxl_bitmap(ch, g)
	ubyte ch;
	register struct glyph *g;
{
	register struct bitmap *bitmap;
	register int file_bytes_wide;
	register int i, j;
	register unsigned char *ptr;

	bitmap = &g->bitmap;

	/* in file, bitmap rows are multiples of 32 bits wide */
	file_bytes_wide = ROUNDUP(bitmap->w, BITS_PER_LONG)*BYTES_PER_LONG;

	/* width must be multiple of 16 bits for raster_op */
	bitmap->bytes_wide = ROUNDUP(bitmap->w, BITS_PER_SHORT)*BYTES_PER_SHORT;
	ptr = bitmap->bits = (unsigned char *)
	  malloc((unsigned) bitmap->h * bitmap->bytes_wide);
	if (ptr == NULL)
	  error("jxdvi: Can't allocate bitmap for char. %d, font %s (%d by %d)",
		ch, current_font->fontname, bitmap->h, bitmap->w);

	if (!open_pxl_file(current_font))
		error("xdvi: Can't find font file %s", current_font->fontname);
	fseek(current_font->file, g->addr, 0);

	for (i = 0; i < bitmap->h; i += 1)
		for (j = 0; j < file_bytes_wide; j += 1)
			if (j < bitmap->bytes_wide) {
				unsigned long f;
				f = one(current_font->file);
#ifdef JXDVI
# ifndef X11 /* X10 */
				*ptr++ = reverse_byte[f];
# else /* X11 */
				*ptr++ = (char) f;
# endif /* X Version */
#endif /* JXDVI */
#			ifdef DVI2NWP
				*ptr++ = (unsigned char) f;
#			endif DVI2NWP
			} else {
				one(current_font->file);
			}
#ifdef JXDVI
	if (shrink_factor != 1)
		shrink_bitmap(bitmap, shrink_factor, shrink_factor);
	if (debug & DBG_BITMAP)
		print_char(ch, g);
#endif JXDVI
}

