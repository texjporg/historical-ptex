#ifndef lint
static char *font_c = "$Header: font.c,v 1.6 88/08/21 19:44:23 kagotani Locked $";
#endif 	lint

#include <sys/types.h>
#include <sys/file.h>
#include <sys/param.h>	/* MAXPATHLEN */
#include <X11/Xlib.h>
#include <stdio.h>
#include <ctype.h>
#include "dvi.h"
#include "pxl.h"
static long	mul16_20();

/* These are probably site dependent */
#define FONT_DIRECTORY	"/usr/local/lib/tex/fonts"
#define FONT_SUFFIX	".%dpxl"

#define one(fp)     num (fp, 1)
#define two(fp)     num (fp, 2)
#define stwo(fp)    snum(fp, 2)
#define four(fp)    num (fp, 4)
#define sfour(fp)   snum(fp, 4)

typedef unsigned char ubyte;

#define DBG_BITMAP	0x1
#define DBG_DVI		0x2
#define DBG_ALL		(DBG_BITMAP|DBG_DVI)

/*
 * Command line flags.
 */
extern int	debug;
extern int	no_kanji_font;
extern int	lbp_dpi;
extern int	shrink_factor;
extern FILE	*dvi_file;

extern int font_not_found;
extern struct font *current_font;	/* ptr into circular list of fonts */
#define MAX_OPEN_FONTS 12
static int n_open_fonts = 0;		/* for LRU management of fonts */

extern long	magnification;


extern unsigned long num();
extern long snum();

extern char *malloc(), *calloc(), *index(), *rindex();

#define	MAXFONTPATHS	16
static char	*fontpath[MAXFONTPATHS+1];	/* leave room for final NULL */
 

setup_fontpath()
{
	char	path[MAXPATHLEN];
	char	*pathenv;
	register char *cp;
	register char *pp;
	int	n = 0;
	char	*getenv();

	if (pathenv = getenv("TEXFONTS")) {
		pp = path;
		for (cp = pathenv;; ) {
			if (n == MAXFONTPATHS)
				error("too many fontpaths");

			if (*cp == ':' || *cp == '\0') {
				*pp = '\0';
				fontpath[n] = malloc(strlen(path)+1);
				if (fontpath[n] == NULL)
					error("malloc failed for fontpaths");
				strcpy(fontpath[n], path);
				n++;
				pp = path;
				if (*cp == '\0')
					break;
				else
					cp++;
			} else
				*pp++ = *cp++;
		}
	}

	if (n == MAXFONTPATHS)
		error("too many fontpaths");

	fontpath[n] = FONT_DIRECTORY;
	fontpath[n + 1] = NULL;
}


/*
**      define_font reads the rest of the fntdef command and then reads in
**      the specified PXL file, adding it to the global linked-list holding
**      all of the fonts used in the job.
*/
define_font(cmnd)
	ubyte cmnd;
{
	register struct font *fontp;
	int len;
	int unmodsize;
	float realsize;
	int size;
	long checksum;

	fontp = (struct font *) malloc((unsigned) sizeof(struct font));
	if (fontp == NULL)
		error("xdvi: Can't allocate memory for font");
	fontp->TeXnumber = num(dvi_file, cmnd - FNTDEF1 + 1);
	checksum = four(dvi_file);
	fontp->scale = four(dvi_file);
	fontp->design = four(dvi_file);
	len = one(dvi_file) + one(dvi_file);
	fontp->fontname = malloc(len + 10);	/* leave space for magnification */
	fread(fontp->fontname, sizeof(char), len, dvi_file);
	fontp->fontname[len] = '\0';
	fontp->file = NULL;
/*
**	In the actual implementation, scaled-size/design-size hasn't been
**	stored with sufficient precision, hence the messing around to find
**	its actual value.
*/
	realsize = (magnification/1000.)*((float) fontp->scale / fontp->design);
	unmodsize = (realsize * 1000) + 0.5;
	/* a real hack to correct for rounding in some cases */
	switch (unmodsize) {
	    case 1095:
		realsize = 1.095445;	/* stephalf */
		break;
	    case 1315:
		realsize = 1.314534;	/* stepihalf */
		break;
	    case 2074:
		realsize = 2.0736;	/* stepiv */
		break;
	    case 2488:
		realsize = 2.48832;	/* stepv */
		break;
	    case 2986:
		realsize = 2.985984;	/* stepiv */
		break;
	}
	/*
	 * the remaining magnification steps are represented
	 * with sufficient accuracy already
	 */
	size = (realsize * lbp_dpi * 5) + 0.5;
	sprintf(&fontp->fontname[len], FONT_SUFFIX, size);

	bzero(fontp->glyph, sizeof(fontp->glyph));

	fontp->xgc = NULL;

	if (current_font == NULL) {
		fontp->next = fontp;
		fontp->prev = fontp;
	} else {
		fontp->next = current_font;
		fontp->prev = current_font->prev;
		current_font->prev->next = fontp;
		current_font->prev = fontp;
	}
	current_font = fontp;
}


open_pxl_file(font)
	struct font *font;
{
	char filename[300];
	extern int errno;
	char **fpv;

	if (font->file == NULL) {
		if (index(font->fontname, '/') == NULL) {
		  for (fpv = fontpath; *fpv; fpv++) {
			  sprintf(filename, "%s/%s", *fpv, font->fontname);
			  if (n_open_fonts == MAX_OPEN_FONTS)
				  close_lru();
			  font->file = fopen(filename, "r");
			  if (font->file)
				  break;
		  }
		}
		else {
/* file name is given as a full path name. */
		  font->file = fopen(font->fontname, "r");
		}

		if (font->file == NULL) {
			if (!check_jfm(font, filename)){
				font_not_found = 1;
				printf("%s [not found]\n", font->fontname);
				return (0);
			}
			else return(0);
		}
#if 1
		else if (check_jfm(font, filename)) {
			(void)fclose(font->file);
			font->file = NULL;
			return(0);
		}
#endif
		n_open_fonts += 1;
	}

	return (1);
}

#define	ID	para[0]
#define	NT	para[1]
#define	LF	para[2]
#define	LH	para[3]
#define	BC	para[4]
#define	EC	para[5]
#define	NW	para[6]
#define	NH	para[7]
#define	ND	para[8]
#define	NI	para[9]
#define	NL	para[10]
#define	NK	para[11]
#define	NG	para[12]
#define	NP	para[13]

check_jfm(f, pxl_name)
struct font *f;
char	*pxl_name;
{
	char	jfm_name[100];
	char	fontname[100];
	int	id;
	char	*p, *q;
	FILE	*jfm;
	int	i, offset, size;
	int	para[14];
	long	info[256], wid[256], glue[256];
	unsigned long gluekern[256];
	register unsigned long *u;
	register CHAR_TYPE	*s;
	register FONT_CH	*t;
	register char		**fpv;

	strcpy(fontname, f->fontname);
	if ((p = rindex(fontname, '/')) != NULL)
		p++;
	else
		p = fontname;
	while(*p){
		if(*p == '.')
			q = p;
		p++;
	}
	strcpy(q, ".tfm");
        for (fpv = fontpath; *fpv; fpv++) {
        	sprintf(jfm_name, "%s/%s", *fpv, fontname);
                jfm = fopen(jfm_name, "r");
                if (jfm)
                	break;
        }

	if(jfm == NULL)
		return(0);
	for(i = 0; i < 14; i++){
		para[i] = two(jfm);
	}
	if(ID != 11){
		fclose(jfm);
		return(0);
	}
	fseek(jfm, (long)((LH)*4), L_INCR);
	if((s = f->ctype = (CHAR_TYPE*)malloc((NT + 1) * 4))==NULL)
		error("can't allocate memory for font table.\n");
	(s++)->type = NT;
	for(i = 0; i < NT; i++, s++){
		s->code = two(jfm);
		s->type = two(jfm);
	}
	for(i = BC; i <= EC; i++) 
		info[i] = four(jfm);
	if((t = f->ch = (FONT_CH *)malloc(sizeof(FONT_CH)*(EC-BC+1)))==NULL)
		error("can't allocate memory for font table.\n");
	for(i = 0; i < NW; i++) {
		wid[i] = mul16_20(f->scale, four(jfm));
	}
	fseek(jfm, (long)((NH+ND+NI)*4), L_INCR);
	for(i = 0; i < NL; i++)
		gluekern[i] = four(jfm);
	fseek(jfm, (long)(NK*4), L_INCR);
	for(i = 0; i < NG; i++){
		glue[i] = four(jfm);
	}
	size = wid[(unsigned)info[0] >> 24];
	f->size = calc_pxl_size(size);

	if ( no_kanji_font ) {
		f->xf = NULL;
		f->xgc = NULL;
	} else {
		load_kfont( f, ROUNDUP(f->size,shrink_factor) );
	}

	offset = (size - f->size*0x10000*72.27/lbp_dpi)/2;
	for(i = 0; i <= EC; i++, t++){
		t->tfmw = wid[(unsigned)info[i] >> 24];
		t->yoff = - f->size*0x10000*72.27/lbp_dpi * 15/100;
		t->xoff = offset;
		if(((info[0] >> 8) & 3) == 1){
			u = &gluekern[info[0] & 0xff];
			do{
				if((((*u >> 16) & 0xff) == i) &&
						(((*u >> 8) & 0xff) < 128)){
					t->xoff += mul16_20(f->scale,
							glue[(*u & 0xff)*3]);
					break;
				}
			}while((*u++ >> 24) < 128);
		}
	}
	fclose(jfm);
	return(1);
}

read_pxl_bitmap(ch, g)
	ubyte ch;
	register struct glyph *g;
{
	register struct bitmap *bitmap;
	register int file_bytes_wide;
	register char *ptr;
	register int i, j;

	bitmap = &g->bitmap;

	/* in file, bitmap rows are multiples of 32 bits wide */
	file_bytes_wide = ROUNDUP(bitmap->w, BITS_PER_LONG)*BYTES_PER_LONG;
	/* width must be multiple of 16 bits for raster_op */
	bitmap->bytes_wide = ROUNDUP(bitmap->w, BITS_PER_BYTE);
	ptr = bitmap->bits = malloc((unsigned) bitmap->h * bitmap->bytes_wide);
	if (ptr == NULL)
		error("xdvi: Can't allocate bitmap for character %d of font %s (%d by %d)",
			ch, current_font->fontname, bitmap->h, bitmap->w);
	if (!open_pxl_file(current_font))
		error("xdvi: Can't find font file %s", current_font->fontname);
	fseek(current_font->file, g->addr, L_SET);
	for (i = 0; i < bitmap->h; i += 1)
		for (j = 0; j < file_bytes_wide; j += 1)
			if (j < bitmap->bytes_wide)
				*ptr++ = one(current_font->file);
			else
				one(current_font->file);
	if (shrink_factor != 1)
		shrink_bitmap(bitmap, shrink_factor, shrink_factor);
}

/*
 * Find font #n and move it to the head of the list.
 */
change_font(n)
	unsigned long n;
{
	register struct font *fontp;

	fontp = current_font;
	for (;;) {
		if (fontp->TeXnumber == n)
			break;
		fontp = fontp->next;
		if (fontp == current_font)
			error("xdvi: Non-existent font #%d", n);
	}
	if (current_font == fontp)
		return;
	fontp->prev->next = fontp->next;
	fontp->next->prev = fontp->prev;
	fontp->next = current_font;
	fontp->prev = current_font->prev;
	current_font->prev->next = fontp;
	current_font->prev = fontp;
	current_font = fontp;
}

/*
 * Close the PXL file for the least recently used font.
 */
close_lru()
{
	register struct font *f;

	f = current_font->prev;
	for (;;) {
		if (f->file != NULL)
			break;
		f = f->prev;
		if (f == current_font->prev)
			error("xdvi: Can't find an open PXL file to close");
	}
	fclose(f->file);
	f->file = NULL;
	n_open_fonts -= 1;
}

reset_fonts()
{
	/*register*/ struct font *f;
	register struct glyph *g;

	f = current_font;
	for (;;) {
	    if ( open_pxl_file(f) ) {
		    for (g = &f->glyph[0]; g < &f->glyph[MAXCHARS]; g += 1) {
			if (g->bitmap.bits) free(g->bitmap.bits);
		    }
		    read_glyphs(f);
	    }
	    f = f->next;
	    if (f == current_font) break;
	}
}

read_glyphs (fontp)
	/*register*/ struct font *fontp;
{
	register struct glyph *g;
	long checksum, magnify, design_size, font_dir_ptr, pxl_id_word;

	/* seek to trailer info */
	fseek(fontp->file, (long) -(5 * BYTES_PER_LONG), L_XTND);
	checksum = four(fontp->file);
	magnify = four(fontp->file);
	design_size = four(fontp->file);
	font_dir_ptr = sfour(fontp->file) * 4;
	pxl_id_word = four(fontp->file);
#ifdef lint
	magnify = design_size = pxl_id_word = magnify;
#endif
	/* seek to font directory */
	fseek(fontp->file, font_dir_ptr, L_SET);
	for (g = &fontp->glyph[0]; g < &fontp->glyph[MAXCHARS]; g += 1) {
		g->bitmap.bits = NULL;
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
		g->adv = mul16_20( fontp->scale, four(fontp->file) );
	}
}


sample(bitmap, x, y, w, h)
	register struct bitmap *bitmap;
	int x, y, w, h;
{
	register char byte, *p1, *ptr, *endp;
	register int b, i, j, m, n;

	ptr = bitmap->bits + (y * bitmap->bytes_wide) + (x / BITS_PER_BYTE);
	endp = bitmap->bits + (bitmap->h * bitmap->bytes_wide);
	b = (1 << (BITS_PER_BYTE-1 - x % BITS_PER_BYTE));
	n = 0;
	for (i = 0; i < h && ptr < endp; i += 1) {
		p1 = ptr;
		byte = *p1;
		for (m = b, j = 0; j < w; j += 1, m >>= 1) {
			if (m == 0 && x+j < bitmap->w) {
				m = (1 << BITS_PER_BYTE-1);
				byte = *++p1;
			}
			if (byte & m)
				n += 1;
		}
		ptr += bitmap->bytes_wide;
	}
	if ( ptr >= endp )
		i ++;
	return (n >= i*w / 3);
}

shrink_bitmap(bitmap, x_factor, y_factor)
	register struct bitmap *bitmap;
	int x_factor, y_factor;
{
	char *shrunk_bits;
	int shrunk_height, shrunk_width, shrunk_bytes_wide;
	register char *ptr;
	char *cp;
	register int x, y, b, m;

	shrunk_height = ROUNDUP(bitmap->h, y_factor);
	shrunk_width = ROUNDUP(bitmap->w, x_factor);
	shrunk_bytes_wide = ROUNDUP(shrunk_width, BITS_PER_BYTE);
	ptr = shrunk_bits = calloc((unsigned) shrunk_height * shrunk_bytes_wide, 1);
	if (ptr == NULL)
		error("Can't allocate shrunken bitmap (%d by %d)",
			shrunk_height, shrunk_width);
	for (y = 0; y < bitmap->h; y += y_factor) {
		b = 0;
		m = (1 << BITS_PER_BYTE-1);
		cp = ptr;
		for (x = 0; x < bitmap->w; x += x_factor) {
			if (sample(bitmap, x, y, x_factor, y_factor))
				*ptr |= m;
			else
				*ptr &= ~m;
			b += 1;
			m >>= 1;
			if (b % BITS_PER_BYTE == 0) {
				b = 0;
				m = (1 << BITS_PER_BYTE-1);
				ptr += 1;
			}
		}
		ptr = cp + shrunk_bytes_wide;
	}
	free(bitmap->bits);
	bitmap->bits = shrunk_bits;
	bitmap->h = shrunk_height;
	bitmap->w = shrunk_width;
	bitmap->bytes_wide = shrunk_bytes_wide;
}


calc_pxl_size(size)
long size;
{
	int	font = size*240/(int)(0x10000*72.27);
	int	font12 = 12;
	int	font16 = 16;
	int	font24 = 24;
	int	font32 = 32;
	int	font48 = 48;
	int	font64 = 64;

	if      (font < (font12 + font16)/2)
		font = font12;
	else if (font < (font16 + font24)/2)
		font = font16;
	else if (font < (font24 + font32)/2)
		font = font24;
	else if (font < (font32 + font48)/2)
		font = font32;
	else if (font < (font48 + font64)/2)
		font = font48;
	else
		font = font64;

	return font*lbp_dpi/240;
}

char get_char_type(jc)
register unsigned short jc;
{
	register int	n, r;
	register unsigned half;
	register CHAR_TYPE	*dp, *sp, *ep, *mp;

	dp = current_font->ctype;
	n = dp->type;
	dp++;
	sp = dp + 1;
	ep = dp + --n;
	n--;
	if(ep->code < jc)
		return((char)dp->type);
	while(sp <= ep){
		if(half = n >> 1){
			mp = sp + (n & 1 ? half : (half - 1));
			if(!(r = jc - mp->code)){
				return((char)mp->type);
			}else{
				if(r < 0){
					ep = mp - 1;
					n = n & 1 ? half : half - 1;
				}else{
					sp = mp + 1;
					n = half;
				}
			}
		}else{
			if(jc == sp->code)
				return((char)sp->type);
			return((char)dp->type);
		}
	}
	return((char)dp->type);
}


static long
mul16_20(a, b)
unsigned long	a;
unsigned long	b;
{
	register unsigned long	ah, al, bh, bl;
	if ( b & 0xF >= 8 ) b += 0x10;
	ah = a >> 16;
	al = a & 0xFFFF;
	bh = (b >> 4) >> 16;
	bl = (b >> 4) & 0xFFFF;
	return (ah*bh << 16) + ah*bl + al*bh + (al*bl >> 16);
}
