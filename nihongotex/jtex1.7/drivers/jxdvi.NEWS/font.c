/*
 *	Font Manager for jxdvi/dvi2nwp
 *	$Header: font.c,v 1.3 88/02/29 15:24:51 atsuo Exp $
 *		written by Atsuo, K.
 *		on 25/12/87
 */

#include "atsuo-copyright.h"  

#include <sys/types.h>
#include <X/Xlib.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include "jxdvi.h"
#include "font.h"
#include "dvi.h"
#include "pxl.h"

extern double conv;
extern int	debug;

#define pixel_round(x)      ((long) (conv * (double) (x) + 0.5))

extern char *malloc();

extern long	magnification, denominator, numerator;
extern int	pixels_per_inch;
extern struct font *current_font;
extern int shrink_factor;
extern int list_fonts;
extern char reverse_byte[];
extern int font_not_found;
extern int n_open_fonts;
extern unsigned long num();

#define MAX_OPEN_K_FONTS 5
int n_open_k_fonts = 0;                /* for LRU management of kanji fonts */

struct glyph current_kanji_glyph;

/* Font Kind Database */
int font_kind[0xff];
int	current_font_kind;

/* kanji font ring list and bitmap table */
struct k_font	*kanji_font_head;
struct k_font	*current_kanji_font;

struct k_bitmap *kanji_font_bitmap[7000];

/*
 *	font_manager(command,arg1,arg2,arg3,arg4,arg5,arg6,arg7)
 *		int	command;
 *		Ptr	arg1, arg2, arg3;
 *
 *		commands are:
 *		DEFINE_FONT		Define font with
 *			arg1	font_def command (int)
 *			arg2	font number ( int )
 *			arg3	check sum   ( int )
 *			arg4	scale       ( int )
 *			arg5	design size ( int )
 *			arg6	name length	( int )
 *			arg7	name        ( *char )
 *		GET_GLYPH		Get glyph with
 *			arg1	char code   ( int )
 *			arg2    struct glyph( *(struct gryph) )
 *		CHANGE_FONT		Change current font
 *			arg1	new font number ( int )
 *		CLOSE_FONT_FILE: Close all files
 *
 *		font_manager returns TRUE if succeeded else return FALUSE.
 *
 *		Note: Arguments not used are ignored.
 */

font_manager(com,arg1,arg2,arg3,arg4,arg5,arg6,arg7)
int	com;
Ptr arg1, arg2, arg3, arg4, arg5, arg6, arg7;
{
	switch(com) {
	case DEFINE_FONT:
	{
	/* arg1:font_def command, arg2:font num, arg3:check sum, arg4:scale,
	 * arg5:design, arg6:filename len, arg7:font name
	 */
		int		s;
		
		s = kanji_font_q((char *)arg7);
		if (s == TRUE) {
			struct jfm j;
			struct k_font *kf, *add_k_font();
			struct k_font *search_k_font_by_name();

			kf = search_k_font_by_name((char *)arg7, (int)arg4, (int)arg5);
			if (kf != NULL) {			/* font is pre-loaded */
				kf->TeXnumber = (int)arg2;
			} else {
				read_jfm((char *)arg7,&j);

				kf = add_k_font((int)arg2,(int)arg4,(int)arg5,&j);
				make_k_glyph(kf,&j);

				if (!open_nwf_file(kf)) {
#ifdef JXDVI
					printf("%s [not found]\n", kf->fontname);
#endif
#ifdef DVI2NWP
					fprintf(stderr,"%s [not found]\n", kf->fontname);
#endif
					return;
				} else {
					if (list_fonts)
						printf("%s\n", kf->fontname);
				}

				if (!font_not_found) {
					read_special_k_bitmap(&j,kf);
				}
			}
			/* Register font kind */
			font_kind[(int)arg2] = F_KANJI;
		} else 
		if (s == FALSE) {
			struct font *f;
			struct font *search_font_by_name();

			f = search_font_by_name((char *)arg7, (int)arg4, (int)arg5);
			if (f != NULL) {			/* font is pre-loaded */
				f->TeXnumber = (int)arg2;
			} else {
				define_font((int)arg1,(int)arg2,(int)arg3,(int)arg4,(int)arg5,
    	                (int)arg6,(char *)arg7);
			}
			/* Register font kind */
			font_kind[(int)arg2] = F_ALPHA;
		} else {
			return FALSE;
		}

		break;
	}
	case CLOSE_FONT_FILE:
	{
		close_all_file();
		close_all_k_file();

		break;
	}
	case PRE_DEFINE_FONT:
	{
		/* arg1:number (<0) arg2:scale, arg3:design size, arg4:font name */

		int		s=TRUE;
/*
**		s = kanji_font_q((char *)arg7);
*/
		if (s == TRUE) {
			struct jfm j;
			struct k_font *kf, *add_k_font();

printf("Pre-defining font:%s, scale:%d, design size:%d\n",
			(char *)arg4, (int)arg2, (int)arg3);

			read_jfm((char *)arg4,&j);

			kf = add_k_font((int)arg1,(int)arg2,(int)arg3,&j);
			make_k_glyph(kf,&j);
			if (!font_not_found) {
printf("Pre-loading special chars of font:%s\n",kf->fontname);
				read_special_k_bitmap(&j,kf);
				/* Do not register font kind yet 
				** We are pre-defining font. But current_font_kind is F_KANJI
				*/
				/* font_kind[(int)arg2] = F_KANJI;*/
				current_font_kind = F_KANJI;
			}
		} else 
		if (s == FALSE) {
			define_font((int)arg1,(int)arg2,(int)arg3,(int)arg4,(int)arg5,
                    (int)arg6,(char *)arg7);

			/* Register font kind */
			font_kind[(int)arg2] = F_ALPHA;
		} else {
			return FALSE;
		}

		break;
	}
	case GET_GLYPH:
	{
		int cc;
		struct glyph *set_glyph(), *set_kanji_glyph();
		struct glyph ** g;

/*printf("%x:",arg1); fflush(stdout);*/
		cc = (int)arg1;
		g = (struct glyph **)arg2;
/*printf("%x ",cc); fflush(stdout);*/
		if (current_font_kind == F_ALPHA) {
			*g = set_glyph(cc);
		} else {
			*g = set_kanji_glyph(cc);
		}
		break;
	}
	case CHANGE_FONT:
		if ( (current_font_kind=font_kind[(int)arg1]) == F_ALPHA) {
			change_font((unsigned long)arg1);
		} else {
			change_k_font((unsigned long)arg1);
		}
		break;
	default:
		break;
	}
}

struct font *
search_font_by_name(name, sc, d_sc)
char	*name;				/* font name */
int		sc, d_sc;			/* scaled size, design size */
{
	double realsize;
	char	fontname[128];
	register struct font *fontp;

#ifdef DEBUG
	printf("search_font_by_name name:%s, sc:%d, d_sc:%d\n",name,sc,d_sc);
#endif

	strcpy(fontname,name);
	realsize = (magnification/1000.)*((float)sc / d_sc);
	make_font_name((double)realsize,&(fontname[strlen(name)]),PXLFONT_SUFFIX);

	if ( (fontp=current_font)==NULL ) return NULL;
	for (;;) {
		if (strcmp(fontp->fontname,fontname)==0) return fontp;
		fontp = fontp->next;
		if (fontp == current_font) return NULL;
	}
}

struct k_font *
search_k_font_by_name(name, sc, d_sc)
char	*name;				/* font name */
int		sc, d_sc;			/* scaled size, design size */
{
	double realsize;
	char	fontname[128];
	register struct k_font *fontp;

	strcpy(fontname,name);
	realsize = (magnification/1000.)*((float)sc / d_sc);
	make_font_name((double)realsize,&(fontname[strlen(name)]),NWFFONT_SUFFIX);
#ifdef DEBUG
	printf("search_k_font_by_name fontname:%s\n",fontname);
#endif
	if ( (fontp=current_kanji_font)==NULL ) return NULL;
	for (;;) {
		if (strcmp(fontp->fontname,fontname)==0) return fontp;
		fontp = fontp->next;
		if (fontp == current_kanji_font) return NULL;
	}
}

close_all_file()
{
	struct font *fontp;

	if ( (fontp=current_font)==NULL ) return NULL;
	for (;;) {
		if (fontp->file != NULL) {
			fclose(fontp->file);
			fontp->file = NULL;
		}
		fontp = fontp->next;
		if (fontp == current_font) break;
	}
	n_open_fonts = 0;
}

/* close_all_k_file() closes all files currently opened.
** It's called at the end of pre-loading.
*/
close_all_k_file()
{
	register struct k_font *fontp;

	if ( (fontp=current_kanji_font)==NULL ) return NULL;
	for (;;) {
#	ifdef DEBUG
		printf("Closing font preFontNum:%d\n",fontp->preFontNum);
#	endif
		if (fontp->file != NULL) {
			fclose(fontp->file);
			fontp->file = NULL;
		}
		fontp = fontp->next;
		if (fontp == current_kanji_font) break;;
	}
	n_open_k_fonts = 0;
}

struct k_font *
add_k_font(n,sc,d_sc,j)
int	n,sc,d_sc;			/* font number, scaled size, design size */
struct jfm *j;
{
	struct k_font *kf;
	float	realsize;
	int		len;

	kf = (struct k_font *)calloc(1,(sizeof(struct k_font)));
	if (kf == NULL)
		error("xdvi: Can't allocate memory for font");
	kf->TeXnumber = n;
	kf->preFontNum = n;
	kf->scale = sc;
	kf->design = d_sc;

	len = strlen(j->fn);
	kf->fontname = malloc((unsigned)(len+10)); /* leave space for mag */
	strcpy(kf->fontname,j->fn);

	realsize = (magnification/1000.)*((float) kf->scale / kf->design);
	make_font_name((double)realsize,&kf->fontname[len],NWFFONT_SUFFIX);
	/* NWF format does not contain font information.
	 * We refer internal database and set
	 * h, w, and bytes_wide.
	 */
	set_nwf_data(kf);

/* for debugging */
/*
 *print_k_font(kf);
 */

	if (kanji_font_head == NULL) {
		kanji_font_head = kf;
		current_kanji_font = kf;
		kf->next = kf;
		kf->prev = kf;
	} else {
		current_kanji_font->prev->next = kf;
		kf->prev = current_kanji_font->prev;
		current_kanji_font->prev = kf;
		kf->next = current_kanji_font;
		current_kanji_font = kf;
	}

	return kf;
}

set_nwf_data(kf)
struct k_font *kf;
{
	int	i;

	for (i=0; ; i++) {
		if (kanji_font_db[i].fontname == NULL) break;
		if ( strcmp(kf->fontname,kanji_font_db[i]) == 0 ) {
			kf->file_h = kf->h = kanji_font_db[i].h;
			kf->file_w = kf->w = kanji_font_db[i].w;
			kf->file_bytes_wide = kf->bytes_wide = 
					kanji_font_db[i].bytes_wide;
			break;
		}
	}
}

/*
**	Make Kanji glyph from jfm
*/
#define GET_HEIGHT_INDEX(h_d) ( ((h_d)>>4)&0x0f )
#define GET_DEPTH_INDEX(h_d)  ( (h_d)&0x0f )

make_k_glyph(kf,j)
struct k_font	*kf;
struct jfm		*j;
{
	int	i;

	kf->glyph = (struct k_glyph *) malloc( 
					(unsigned)sizeof(struct k_glyph)*(j->table[J_EC]+1));

	for (i=0; i<= j->table[J_EC]; i++) {
		kf->glyph[i].dvi_adv =
			( (double) kf->scale * j->width[(j->info[i].width_ix)] / (1<<20) );
		kf->glyph[i].x = 0;
		kf->glyph[i].y = pixel_round(
			( (double) kf->scale *
			  (j->height[GET_HEIGHT_INDEX(j->info[i].height_depth_ix)]) /
			  (1<<20) ) );
		kf->glyph[i].pxl_adv = pixel_round(kf->glyph[i].dvi_adv);
	}
}

/*
**      define_font reads in
**      the specified PXL file, adding it to the global linked-list holding
**      all of the fonts used in the job.
*/

define_font(cmnd,num,cksm,sc,d_sc,fn_len,fn)
int cmnd;
int num, cksm, sc, d_sc,fn_len;
char *fn;
{
    register struct font *fontp;
	int len;
	int unmodsize;
	float realsize;
	int size;
        long checksum;

	fontp = (struct font *) malloc(sizeof(struct font));
	if (fontp == NULL)
		error("xdvi: Can't allocate memory for font");
	fontp->TeXnumber = num;
	checksum = cksm;
	fontp->scale = sc;
	fontp->design = d_sc;
	len = fn_len;
	 /* leave space for magnification */
	fontp->fontname = malloc((unsigned)(len+10));
	strncpy(fontp->fontname,fn,len);
	fontp->fontname[len] = '\0';
	fontp->file = NULL;
/*
**	In the actual implementation, scaled-size/design-size hasn't been
**	stored with sufficient precision, hence the messing around to find
**	its actual value.
*/
	realsize = (magnification/1000.)*((float) fontp->scale / fontp->design);

	make_font_name((double)realsize,&fontp->fontname[len],PXLFONT_SUFFIX);

	if (!open_pxl_file(fontp))
		return;
	read_glyphs(fontp);
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

make_font_name(realsize,name,suffix)
double realsize;
char *name, *suffix;
{
	int unmodsize;
	int size;
/*
**	In the actual implementation, scaled-size/design-size hasn't been
**	stored with sufficient precision, hence the messing around to find
**	its actual value.
*/
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
	size = (realsize * pixels_per_inch * 5) + 0.5;
	sprintf(name, suffix, size);
}

extern int backwards;

struct glyph *
set_glyph(cc)
unsigned int	cc;
{
    struct glyph *g;

	g = &current_font->glyph[cc];
#ifdef JXDVI
	if (g->bitmap.bits == NULL) {
		read_pxl_bitmap(cc, g);
		if (backwards) reverse_bytes(&g->bitmap);
    }
#endif JXDVI
#ifdef DVI2NWP
	if (g->bitmap.nbits == NULL) {
		read_pxl_bitmap(cc, g);
    }
#endif DVI2NWP

	return (g);
}

add_k_bitmap(kb,code)
struct k_bitmap *kb;
unsigned int	code;
{
	struct k_bitmap *k;
	int	ix;

	ix = jis_to_index(code);
	if ( kanji_font_bitmap[ix] == NULL ) {
		kanji_font_bitmap[ix] = kb;
	} else {
		k = kanji_font_bitmap[ix];
		while ( k->next != NULL) {
			k = k->next;
		}
		k->next = kb;
		kb->prev = k;
	}
	return;
}

struct k_bitmap *
get_k_bitmap(code,number)
int	code;
int	number;
{
	struct k_bitmap *k;
	int	ix;

	ix = jis_to_index(code);
	if ( ix == -1 ) {
		printf("Illeagal code: %d, number: %d\n",code,number);
		ix = jis_to_index(JIS_QUESTION_MARK);
	}
	k = kanji_font_bitmap[ix];
	if ( k == NULL ) {
		return NULL;
	} else {
		do {
			if (k->TeXnumber == number) {
				break;
			}
			k = k->next;
		} while (k != NULL);
		return k;
	}
}

/*
** read_nwf_bitmap reads in
** the specified NWF file, adding it to the global linked-list holding
** all of the kanji font bitmap used in the job.
*/

struct k_bitmap *
read_nwf_bitmap(kf,code,info)
	struct k_font *kf;
	unsigned int code, info;
{
	struct k_bitmap	*kb;
	int	byte_paddings, total_bytes;
	struct bitmap b;
	int		i, j, k;
	short	row;
#ifdef JXDVI
	char	*ptr;
#endif JXDVI
#ifdef DVI2NWP
	char	*n_ptr;
#endif DVI2NWP

	/* In NWF file, bitmap rows are multiples of 8 bit wide */
	byte_paddings = 
		((kf->file_bytes_wide*BITS_PER_BYTE)%BITS_PER_SHORT) ? 1 : 0;
	total_bytes = kf->file_bytes_wide + byte_paddings;

	kb = (struct k_bitmap *) malloc(sizeof(struct k_bitmap));
	kb->next = kb->prev = NULL;
	kb->uc = 0;
	/* kf->preFontNum and TeXnumber are always same when font is
	** demand-loaded. If font is pre-loaded, preFontNum gives you
	** real font number (in memory).
	*/
	kb->TeXnumber = kf->preFontNum;
	kb->info = info;
#ifdef JXDVI
	ptr = b.bits = kb->bits = malloc( ( sizeof(char) * kf->file_h *
						(kf->file_bytes_wide + byte_paddings) ) );
	if (ptr == NULL)
		error("jxdvi: Can't allocate bitmap for char. %d, font %s (%d by %d)",
				code, kf->fontname, b.h, b.w);
#endif JXDVI
#ifdef DVI2NWP
	n_ptr = b.nbits = kb->nbits = 
		malloc( ( sizeof(char) * kf->file_h *
						(kf->file_bytes_wide + byte_paddings) ) );
	if (n_ptr == NULL)
		error("jxdvi: Can't allocate nbitmap for char. %d, font %s",
			code, kf->fontname);
#endif DVI2NWP

	b.h = kf->file_h;
	b.w = kf->file_w;
	b.bytes_wide = kf->bytes_wide = total_bytes;

	if (!open_nwf_file(kf))
		error("xdvi: Can't find font file %s", kf->fontname);
	fseek(kf->file,
			(long)(jis_to_index(code)*kf->file_bytes_wide*kf->file_h),0);

	for (i = 0; i < b.h; i += 1) {
		for (j = 0; j < kf->file_bytes_wide; j += 1) {
			unsigned long f;
			f = one(kf->file);
#		ifdef JXDVI
			*ptr++ = (char) reverse_byte[f];
#		endif JXDVI
#		ifdef DVI2NWP
			*n_ptr++ = (unsigned char) f;
#		endif DVI2NWP
		}
		if (byte_paddings) {
#		ifdef JXDVI
			*ptr++ = 0x00;
#		endif JXDVI
#		ifdef DVI2NWP
			*n_ptr++ = 0x00;
#		endif DVI2NWP
		}
	}

#ifdef JXDVI
	if (shrink_factor != 1) {
		shrink_bitmap(&b, shrink_factor, shrink_factor);
		kf->h = b.h;
		kf->w = b.w;
		kf->bytes_wide = b.bytes_wide;
		kb->bits = b.bits;
	}
	if (backwards) reverse_bytes(&b);
#endif JXDVI

	add_k_bitmap(kb,code);

#ifdef STATISTICS
	kf->load += 1;
	kf->load_bytes += (kf->file_h * kf->file_bytes_wide);
#endif

	return kb;
}

struct glyph *
set_kanji_glyph(cc)
int cc;
{
	struct k_font	*k;
	struct k_bitmap *kb;

	k = current_kanji_font;
#ifdef STATISTICS
	k->use += 1;
#endif
	/* kf->preFontNum and TeXnumber are always same when font is
	** demand-loaded. If font is pre-loaded, preFontNum gives you
	** real font number.
	*/
	kb = get_k_bitmap(cc, current_kanji_font->preFontNum);
	if (kb == NULL)
		kb = read_nwf_bitmap(k,cc,0);	/* demand-loaded fonts 
										are always type 0 */

	current_kanji_glyph.dvi_adv = k->glyph[kb->info].dvi_adv;
	current_kanji_glyph.pxl_adv = k->glyph[kb->info].pxl_adv;
	current_kanji_glyph.x = k->glyph[kb->info].x;
	current_kanji_glyph.y = k->glyph[kb->info].y;
	current_kanji_glyph.bitmap.h = k->h;
	current_kanji_glyph.bitmap.w = k->w;
	current_kanji_glyph.bitmap.bytes_wide = k->bytes_wide;
#ifdef JXDVI
	current_kanji_glyph.bitmap.bits = kb->bits;
#endif JXDVI
#ifdef DVI2NWP
	current_kanji_glyph.bitmap.nbits = kb->nbits;
#endif DVI2NWP

	return (&current_kanji_glyph);
}

read_pxl_bitmap(ch, g)
	ubyte ch;
	register struct glyph *g;
{
	register struct bitmap *bitmap;
	register int file_bytes_wide;
	register int i, j;
#ifdef JXDVI
	register char *ptr;
#endif JXDVI
#ifdef DVI2NWP
	register char *n_ptr;
#endif DVI2NWP

	bitmap = &g->bitmap;

	/* in file, bitmap rows are multiples of 32 bits wide */
	file_bytes_wide = ROUNDUP(bitmap->w, BITS_PER_LONG)*BYTES_PER_LONG;

	/* width must be multiple of 16 bits for raster_op */
	bitmap->bytes_wide = ROUNDUP(bitmap->w, BITS_PER_SHORT)*BYTES_PER_SHORT;
#ifdef JXDVI
	ptr = bitmap->bits = malloc((unsigned) bitmap->h * bitmap->bytes_wide);
	if (ptr == NULL)
		error("jxdvi: Can't allocate bitmap for char. %d, font %s (%d by %d)",
			ch, current_font->fontname, bitmap->h, bitmap->w);
#endif JXDVI
#ifdef DVI2NWP
	n_ptr = bitmap->nbits = malloc((unsigned) bitmap->h * bitmap->bytes_wide);
	if (n_ptr == NULL)
		error("jxdvi: Can't allocate n_bitmap for char. %d, font %s",
				ch, current_font->fontname);
#endif DVI2NWP

	if (!open_pxl_file(current_font))
		error("xdvi: Can't find font file %s", current_font->fontname);
	fseek(current_font->file, g->addr, 0);

	for (i = 0; i < bitmap->h; i += 1)
		for (j = 0; j < file_bytes_wide; j += 1)
			if (j < bitmap->bytes_wide) {
				unsigned long f;
				f = one(current_font->file);
#			ifdef JXDVI
				*ptr++ = reverse_byte[f];
#			endif JXDVI
#			ifdef DVI2NWP
				*n_ptr++ = (unsigned char) f;
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

change_k_font(n)
unsigned long n;
{
	register struct k_font *fontp;

	fontp = current_kanji_font;

	/* Note thae kf->TeXnumber always gives you font number in
	** dvi file
	*/
	for (;;) {
		if (fontp->TeXnumber == n)
                        break;
		fontp = fontp->next;
		if (fontp == current_kanji_font)
			error("xdvi: Non-existent font #%d", n);
	}
	if (current_kanji_font == fontp)
		return;
	fontp->prev->next = fontp->next;
	fontp->next->prev = fontp->prev;
	fontp->next = current_kanji_font;
	fontp->prev = current_kanji_font->prev;
	current_kanji_font->prev->next = fontp;
	current_kanji_font->prev = fontp;
	current_kanji_font = fontp;
}

/*
 * Close the NWF file for the least recently used font.
 */
close_k_lru()
{
    register struct k_font *f;
    
    f = current_kanji_font->prev;
    for (;;) {
        if (f->file != NULL)
            break;
        f = f->prev;
        if (f == current_kanji_font->prev)
            error("xdvi: Can't find an open NWF file to close");
    }
printf("Closing K_font:%d\n",f->TeXnumber);
    fclose(f->file);
    f->file = NULL;
    n_open_k_fonts -= 1;
}
                    
kanji_font_q(fn)
char *fn;
{
	char	tfmname[256];
	FILE	*tfm;
	int		id;

/*
 *	if (n_open_k_fonts == MAX_OPEN_K_FONTS)
 *        	close_k_lru();
 */
	sprintf(tfmname,"%s/%s%s",TFM_DIRECTORY, fn, TFM_SUFFIX);
	tfm = fopen(tfmname,"r");
	if (tfm == NULL) {
		font_not_found = 1;
		printf("%s%s [not found]\n", fn, TFM_SUFFIX);
		return(ERROR);
	}

	/* read jfm id field. */
	id = (int) two(tfm);

	fclose(tfm);

	if ( id == JFM_ID ) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*
** read japanise font metric file since NWF files do not have
** font information.
*/
read_jfm(fn,j)
char *fn;
struct jfm *j;
{
	char	tfmname[256];
	FILE	*tfm;
	int		i;

	sprintf(tfmname,"%s/%s%s",TFM_DIRECTORY, fn, TFM_SUFFIX);
	tfm = fopen(tfmname,"r");
	if (tfm == NULL) {
		font_not_found = 1;
		printf("%s%s [not found]\n", fn, TFM_SUFFIX);
		return(ERROR);
	}

	/* set font name */
	strcpy(j->fn,fn);

	/* read jfm table field. */
	for (i=0; i<=J_NP; i++) {
		j->table[i]= (short) two(tfm);
	}

	/* read jfm header */
	j->check_sum = four(tfm);
	j->design_size = four(tfm);
	fseek(tfm,(long)(sizeof(long)*(j->table[J_LH]-2)),1);

	/* read jfm char_type */
	j->type = (struct jfm_char_type *)
				malloc( (sizeof(struct jfm_char_type)*j->table[J_NT]) );
	for (i=0; i<j->table[J_NT]; i++) {
		j->type[i].code = (short) two(tfm);
		j->type[i].index = (short) two(tfm);
	}

	/* read jfm char_info */
	j->info = (struct jfm_char_info *)
				malloc( (sizeof(struct jfm_char_info)*(j->table[J_EC]+1)) );
	for (i=0; i<=j->table[J_EC]; i++) {
		j->info[i].width_ix = (unsigned char) one(tfm);
		j->info[i].height_depth_ix = (unsigned char) one(tfm);
		j->info[i].italic_ix_tag = (unsigned char) one(tfm);
		j->info[i].remainder_ix = (unsigned char) one(tfm);
	}

	/* read jfm width */
	j->width = (unsigned long *)
				malloc((sizeof(unsigned long)*(j->table[J_NW])));
	for (i=0; i<j->table[J_NW]; i++) {
		j->width[i] = four(tfm);
	}

	/* read jfm height */
	j->height = (unsigned long *)
				malloc((sizeof(unsigned long)*(j->table[J_NH])));
	for (i=0; i<j->table[J_NH]; i++) {
		j->height[i] = four(tfm);
	}

	/* read jfm depth */
	j->depth = (unsigned long *)
				malloc((sizeof(unsigned long)*(j->table[J_ND])));
	for (i=0; i<j->table[J_ND]; i++) {
		j->depth[i] = four(tfm);
	}

	/* read jfm italic */
	j->italic = (unsigned long *)
				malloc((sizeof(unsigned long)*(j->table[J_NI])));
	for (i=0; i<j->table[J_NI]; i++) {
		j->italic[i] = four(tfm);
	}
/* for debugging */
/* print_jfm(j); */
}

open_nwf_file(font)
struct k_font *font;
{
	char filename[300];
	extern int errno;

	if (font->file == NULL) {
	    sprintf(filename, "%s/%s",
			FONT_DIRECTORY, font->fontname);

            if (n_open_k_fonts == MAX_OPEN_K_FONTS)
               close_k_lru();
            font->file = fopen(filename, "r");
	    if (font->file == NULL) {
		font_not_found = 1;
		return (0);
	    }
            n_open_k_fonts += 1;
    	}
	return (1);
}

/*
 * We read some kanji fonts since they need special treatment.
 */
read_special_k_bitmap(j,kf)
struct jfm *j;
struct k_font *kf;
{
	int	i;

	for (i=1; i < j->table[J_NT]; i++) {
		read_nwf_bitmap(kf,(unsigned int) j->type[i].code,
						 (unsigned int) j->type[i].index);
	}		
}

print_jfm(j)
struct jfm *j;
{
	int	i;

	for (i=0; i<=J_NP; i++) {
		printf("%d ",j->table[i]);
	}
	printf("\n");
	printf("check_sum:%lx, designe_size:%lx\n",
			j->check_sum, j->design_size);
	for (i=0; i<j->table[J_NT]; i++) {
		printf("code:%x, index:%x\n",j->type[i].code,j->type[i].index);
	}
	for (i=0; i<=j->table[J_EC]; i++) {
		printf("w_ix:%x, hd_ix:%x, i_ix:%x, rm_ix:%x\n",
				j->info[i].width_ix, j->info[i].height_depth_ix,
				j->info[i].italic_ix_tag, j->info[i].remainder_ix);
	}
	for (i=0; i<j->table[J_NW]; i++) {
		printf("width:%#8x\n",j->width[i]);
	}
	for (i=0; i<j->table[J_NH]; i++) {
		printf("height:%#8x\n",j->height[i]);
	}
	for (i=0; i<j->table[J_ND]; i++) {
		printf("depth:%#8x\n",j->depth[i]);
	}
	for (i=0; i<j->table[J_NI]; i++) {
		printf("italic:%#8x\n",j->italic[i]);
	}
}

print_k_font(kf)
struct k_font *kf;
{
	printf("TeXnumber:%d, scale:%d, design:%d\n",
			kf->TeXnumber, kf->scale, kf->design);
	printf("fontname:%s\n",kf->fontname);
	printf("file_h:%d, file_w:%d, file_bytes_wide:%d\n",
			kf->file_h, kf->file_w, kf->file_bytes_wide);
	printf("h:%d, w:%d, bytes_wide:%d\n",
			kf->h, kf->w, kf->bytes_wide);
}

#ifdef STATISTICS
report_statistics()
{
	struct k_font	*k_st;
	struct k_font	*k;
	int	use, load, bytes;

	use = load = bytes = 0;	
	printf("Kanji Font Usage\n");

	k = k_st = kanji_font_head;
	if (k==NULL) {
		printf("No Kanji used\n");
	} else {
		do {
			use += k->use;
			load += k->load;
			bytes += k->load_bytes;
			printf("%s use:%d, load:%d, bytes:%d\n",
					k->fontname, k->use, k->load, k->load_bytes);
			k = k->next;
		} while( k != k_st );			/* Kanji Font List is ring list */
		printf("Total use:%d, load:%d, bytes:%d\n", use, load, bytes);
	}
}
#endif
