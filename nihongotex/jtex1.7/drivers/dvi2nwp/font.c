/*
 *	Font Manager for jxdvi/dvi2nwp
 *	$Header: font.c,v 3.1 89/02/25 22:59:07 atsuo Exp $
 *		written by Atsuo, K.
 *		on 25/12/87
 */

#include "atsuo-copyright.h"  

#include "config.h"

#include <sys/types.h>

#ifndef X11 /* X10 */
#  include <X/Xlib.h>
#else /* X11 */
#  include <X11/Xlib.h>
#endif /* X Version */

#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include "jxdvi.h"
#include "font.h"
#include "dvi.h"

extern double conv;
extern int	debug;

#define pixel_round(x)      ((long) (conv * (double) (x) + 0.5))

extern char *malloc();

int n_open_k_fonts = 0;                /* for LRU management of kanji fonts */

struct glyph current_kanji_glyph;

/* Font Kind Database */
int font_kind[0xff];
int	current_font_kind;

/* kanji font ring list and bitmap table */
struct k_font	*kanji_font_head;
struct k_font	*current_kanji_font;

struct k_bitmap *kanji_font_bitmap[MAX_KANJI_CHAR];
extern struct k_bitmap *read_nwf_bitmap();
extern struct k_bitmap *read_dnf_bitmap();

extern char *calloc();

/*
 *	font_manager(command,arg1,arg2,arg3,arg4,arg5,arg6,arg7)
 *		int	command;
 *		Ptr	arg1, arg2, arg3, arg4, arg5, arg6, arg7;
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
      if (s == TRUE) {		/* Kanji font */
	struct jfm *j;
	struct k_font *kf, *add_k_font();
	struct k_font *search_k_font_by_name();
	
	kf = search_k_font_by_name((char *)arg7, (int)arg4, (int)arg5);
	if (kf != NULL) {			/* font is pre-loaded */
	  kf->TeXnumber = (int)arg2;
	} else {
	  define_k_font((int)arg1,(int)arg2,(int)arg3,(int)arg4,(int)arg5,
			(int)arg6,(char *)arg7);
	}
	/* Register font kind */
	font_kind[(int)arg2] = F_KANJI;
      } else if (s == FALSE) {		/* Alphabet, etc. font */
	struct font *f;
	struct font *search_font_by_name();
	
	f = search_font_by_name((char *)arg7, (int)arg4, (int)arg5);
	if (f != NULL) {		/* font is pre-loaded */
	  f->TeXnumber = (int)arg2;
	} else {
	  define_font((int)arg1,(int)arg2,(int)arg3,(int)arg4,(int)arg5,
		      (int)arg6,(char *)arg7);
	}
	/* Register font kind */
	font_kind[(int)arg2] = F_ALPHA;
      } else {			/* TFM file not found */
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
	struct jfm *j;
	struct k_font *kf, *add_k_font();
	
	printf("Pre-defining font:%s, scale:%d, design size:%d\n",
	       (char *)arg4, (int)arg2, (int)arg3);
	
	define_k_font((int)arg1,(int)arg2,(int)arg3,(int)arg4,(int)arg5,
		      (int)arg6,(char *)arg7);

	/* Do not register font kind yet 
	 * We are pre-defining font. But current_font_kind is F_KANJI
	 */
	current_font_kind = F_KANJI;
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
#ifdef JXDVI /* jxdvi only */
  case RESET_FONTS:
    {
      register struct font *f;
      register struct k_font *kf;
      register struct k_bitmap *kb;
      register struct glyph *g;
      int	i;
      
      if ( (f=current_font) != NULL ) {
	  for (;;) {
	      for (g = &f->glyph[0]; g < &f->glyph[MAXCHARS]; g += 1) {
		  if (g->bitmap.bits) free(g->bitmap.bits);
	      }
	      if (f->fonttype == FT_PXL) {
		  open_pxl_file(f);
		  read_pxl_glyphs(f);
	      } else if (f->fonttype == FT_PK) {
		  open_PK_file(f);
		  read_PK_file(f);
	      }
	      
	      f = f->next;
	      if (f == current_font) break;
	  }
      }

      for (i=0; i<MAX_KANJI_CHAR; i++) {
	kb = kanji_font_bitmap[i];
	if (kb == NULL) continue;
	do {
	  if (kb->bits != NULL) {
	    free(kb->bits);
	    /*			printf("Freeing %d %d\n",i,kb->TeXnumber);*/
	    kb->bits = NULL;
	  }
	} while ((kb=kb->next) != NULL);
      }
      kf = current_kanji_font;
      if ( kf != NULL ) {
	for (;;) {
	  if (kf->file != NULL) {
	    fclose(kf->file);
	    kf->file = NULL;
	  }
	  make_k_glyph(kf,kf->j);
	  read_nwfadj(kf,kf->j);
	  
	  kf = kf->next;
	  if (kf == current_kanji_font) break;
	}
      }
      n_open_k_fonts = 0;
    }
#endif /* jxdvi only */
  default:
    break;
  }
}

struct font *
search_font_by_name(name, sc, d_sc)
     char *name;		/* font name */
     int sc, d_sc;		/* scaled size, design size */
{
  char	fontname[128];
  register struct font *fontp;

#ifdef DEBUG
  printf("search_font_by_name name:%s, sc:%d, d_sc:%d\n",name,sc,d_sc);
#endif

  strcpy(fontname,name);

  if ( (fontp=current_font)==NULL ) return NULL;
  for (;;) {
    if (strncmp(fontp->fontname,fontname,fontp->fn_len) == 0 &&
		fontp->scale == sc && 
		fontp->design == d_sc) return fontp;
    fontp = fontp->next;
    if (fontp == current_font) return NULL;
  }
}

struct k_font *
search_k_font_by_name(name, sc, d_sc)
  char *name;			/* font name */
  int  sc, d_sc;		/* scaled size, design size */
{
  char	fontname[128];
  register struct k_font *fontp;

  strcpy(fontname,name);

#ifdef DEBUG
  printf("search_k_font_by_name fontname:%s\n",fontname);
#endif

  if ( (fontp=current_kanji_font)==NULL ) return NULL;
  for (;;) {
    if (strncmp(fontp->fontname,fontname,fontp->fn_len) == 0 &&
		fontp->scale == sc && 
		fontp->design == d_sc) return fontp;
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

#ifdef V_NWF
	if (m_v_font_fd >= 0) {
	    FS_close_font(m_v_font_fd);
	    m_v_font_fd = -1;
	}
	if (g_v_font_fd >= 0) {
	    FS_close_font(g_v_font_fd);
	    g_v_font_fd = -1;
	}
#endif /* V_NWF */
}

struct k_font *
add_k_font(n,sc,d_sc,j)
int	n,sc,d_sc;		/* font number, scaled size, design size */
struct jfm *j;
{
	struct k_font *kf;
	float	realsize;
	int		len;
#ifdef NWF_605
	int bit_width;
#endif /* NWF_605 */

	kf = (struct k_font *)calloc(1,(sizeof(struct k_font)));
	if (kf == NULL)
		error("xdvi: Can't allocate memory for font");
	kf->j = j;
	kf->TeXnumber = n;
	kf->preFontNum = n;
	kf->scale = sc;
	kf->design = d_sc;
	kf->fn_len = len = strlen(j->fn);

	kf->fontname = malloc((unsigned)(len+10)); /* leave space for mag */
	strcpy(kf->fontname,j->fn);

	make_font_name(kf->scale, kf->design, &kf->fontname[len], FT_NWF);

#ifdef NWF_605
	calc_nwf_data(kf);
#endif
#ifdef NWF_604 /*  Use font_db */
	/* NWF format does not contain font information.
	 * We refer internal database and set
	 * h, w, and bytes_wide.
	 */
	set_nwf_data(kf);
#endif /* NWF_605 or NWF_604 */
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

calc_nwf_data(kf)
  register struct k_font *kf;
{
    register short bit_width;

/**/ bit_width = (int) (((fraction*pixels_per_inch/100000/2.54) *
			 ((float) kf->scale * kf->j->param[J_NP_X_HEIGHT] / 
			  (1<<20) )) + 0.5);
/**//*
  printf("%s: %d %d %d\n",kf->fontname,bit_width,kf->j->param[J_NP_X_HEIGHT],
         kf->design);
*/
    
    kf->file_h = kf->h = bit_width;
    kf->file_w = kf->w = bit_width;
    kf->file_bytes_wide = kf->bytes_wide = 
      (bit_width + (BITS_PER_BYTE - 1)) / BITS_PER_BYTE;
}

#ifdef NWF_604
set_nwf_data(kf)
  struct k_font *kf;
{
    int	i;
    
    for (i=0; ; i++) {
	if (kanji_font_db[i].fontname == NULL) {
	    /* Font not found in font_db. Calculate data from jfm file
	     * to make Box Char later
	     */
	    calc_nwf_data(kf);
	    break;
	} else
	if ( strcmp(kf->fontname,kanji_font_db[i].fontname) == 0 ) {
	    kf->file_h = kf->h = kanji_font_db[i].h;
	    kf->file_w = kf->w = kanji_font_db[i].w;
	    kf->file_bytes_wide = kf->bytes_wide = 
	      kanji_font_db[i].bytes_wide;
	    break;
	}
    }
}
#endif /* NWF_604 */

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
	  kf->glyph[i].x = 0; /****/
	  kf->glyph[i].y = pixel_round(
	       ( (double) kf->scale *
		(j->height[GET_HEIGHT_INDEX(j->info[i].height_depth_ix)]) /
			  (1<<20) ) );
		kf->glyph[i].pxl_adv = pixel_round(kf->glyph[i].dvi_adv);
	}
}

read_nwfadj(kf,j)
struct k_font	*kf;
struct jfm	*j;

{
	FILE	*f, *open_nwfadj();
	int	type, x, y;
	struct k_bitmap *kb;

/**/ /* printf("Opening %s%s\n", j->fn, NWFADJ_SUFFIX); */
#ifdef DNF
	if ( kf->fonttype == FT_DNF )
	  return;
#endif
	f = open_nwfadj(j);
	if ( f == NULL ) {
		fprintf(stderr,"Warning:%s%s [not found]\n", 
		       j->fn, NWFADJ_SUFFIX);
		return NULL;
	}

	while ( read_1_nwf_adj(f,&type,&x,&y) != NULL ) {
/**/ /* printf("type: %d  x: %d  y: %d\n",type,x,y); */
kf->glyph[type].x -= pixel_round( (double) kf->scale * x / (1<<20) ); /****/
kf->glyph[type].y -= pixel_round( (double) kf->scale * y / (1<<20) ); /****/
	}

	close_nwfadj(f);
}

read_1_nwf_adj(f,type,x,y)
FILE *f;
int	*type, *x, *y;
{
	char	*buf[256];

READ_NWF_AGAIN:
	if ( fgets(buf,256,f) == NULL ) {
		return NULL;
	}
	if ( sscanf(buf,"%d %d %d %*[^\n]",
			 type,x,y) != 3 ) {
		goto READ_NWF_AGAIN;
	}

	return 1;
}

FILE *
open_nwfadj(j)
struct jfm *j;
{
	char filename[300];
	FILE *file;

	sprintf(filename, "%s/%s%s",
		NWFADJ_DIR, j->fn, NWFADJ_SUFFIX);

	file = fopen(filename, "r");
	if (file == NULL) {
		return NULL;
	}
	return file;
}

close_nwfadj(f)
FILE *f;
{
	fclose(f);
}

/*
**      define_font reads in
**      the specified alphabet font file, adding it to the global 
**      linked-list holding all of the fonts used in the job.
**	define_k_font do same thing except that it reads in
**	kanji font file.
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
	fontp->fn_len = len = fn_len;

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
	make_font_name(fontp->scale, fontp->design, 
		       &fontp->fontname[len],FT_PXL);
	if ( open_pxl_file(fontp) ) {
		fontp->fonttype = FT_PXL;
		read_pxl_glyphs(fontp);
	} else {
	  fontp->fontname[len] = '\0';	/* e.g. cmr7.2000pxl -> cmr7 */
	  make_font_name(fontp->scale, fontp->design, 
			 &fontp->fontname[len],FT_PK);
	  if ( open_PK_file(fontp) ) {
	    fontp->fonttype = FT_PK;
	    read_PK_file(fontp);
	  } else {
	      font_message(F_NOT_FOUND,(Ptr)fontp);
	      font_not_found = 1;
	      return FALSE;
	  }
	}

    if (list_fonts) {
	printf("%s\n", fontp->fontname);
    }

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

define_k_font(cmnd,num,cksm,sc,d_sc,fn_len,fn)
  int cmnd;
  int num, cksm, sc, d_sc,fn_len;
  char *fn;
{
    struct jfm *j;
    struct k_font *kf, *add_k_font();
    struct k_font *search_k_font_by_name();
    
    j = (struct jfm *)malloc(sizeof(struct jfm));
    if (j == NULL)
      error("jxdvi: Can't alloc jfm structure");

    read_jfm(fn,j);
    
    kf = add_k_font(num,sc,d_sc,j);

#ifdef	DNF
    if (open_dnf_file(kf)) {
        kf->fonttype = FT_DNF;
	goto found;
      }
#endif	/* DNF */
    if (open_nwf_file(kf)) {
	kf->fonttype = FT_NWF;
	goto found;
    }
#ifdef V_NWF
    if (open_v_file(kf) >= 0) {
	kf->fonttype = FT_V_NWF;
	goto found;
    }
#endif /* V_NWF */
    {
	kf->fonttype = FT_JFM;
	goto found;
    }

  found:

    make_k_glyph(kf,j);
    read_nwfadj(kf,j);
#ifdef DNF
    if ( kf->fonttype == FT_DNF )
      init_dnf_index( kf );
#endif

    if (list_fonts) {
	printf("%s (%d)\n", kf->fontname, kf->w);
    }
    if (!font_not_found) {
	setup_special_k_bitmap(j,kf);
    }
    return TRUE;
}

make_font_name(scale,design,name,kind)
  int scale, design;
  char *name;
  int kind;
{
    double realsize;
    int unmodsize;
    int size;
    char *suffix;

/*
**	In the actual implementation, scaled-size/design-size hasn't been
**	stored with sufficient precision, hence the messing around to find
**	its actual value.
*/
    realsize = (magnification/1000.) * ((float) scale / design);
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
	switch (kind) {
	case FT_PXL:
	  size = (realsize * pixels_per_inch * 5) + 0.5;
	  suffix = PXL_FONT_SUFFIX;
	  break;
	case FT_PK:
	  size = (realsize * pixels_per_inch) + 0.5;
	  suffix = PK_FONT_SUFFIX;
	  break;
	case FT_NWF:
	  size = (realsize * pixels_per_inch * 5) + 0.5;
	  suffix = NWF_FONT_SUFFIX;
	  break;
	case FT_DNF:
	  size = (realsize * pixels_per_inch * 5 ) + 0.5;
	  suffix = DNF_FONT_SUFFIX;
	  break;
	case FT_NOT_FOUND:
	  size = (realsize * pixels_per_inch) + 0.5;
	  suffix = NOT_FOUND_SUFFIX;
	  break;
	}
	sprintf(name, suffix, size);
}

struct glyph *
set_glyph(cc)
unsigned int	cc;
{
    struct glyph *g;

	g = &current_font->glyph[cc];

#  ifdef JXDVI
	if (g->bitmap.bits == NULL) {
		if (current_font->fonttype == FT_PXL) {
			read_pxl_bitmap(cc, g);
		} else if (current_font->fonttype == FT_PK) {
			read_pk_bitmap(cc, g);
		}
#    ifndef X11 /* X10 */
		if (backwards) reverse_bytes(&g->bitmap);
#    endif
	}
#  endif JXDVI
#  ifdef DVI2NWP
	if (g->bitmap.bits == NULL) {
		if (current_font->fonttype == FT_PXL) {
			read_pxl_bitmap(cc, g);
		} else if (current_font->fonttype == FT_PK) {
			read_pk_bitmap(cc, g);
		}
	}
#  endif DVI2NWP

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
** make_k_bitmap allocates k-bitmap area and
** adds it to the global linked-list holding
** all of the kanji font bitmap used in the job.
*/

struct k_bitmap *
make_k_bitmap(kf,code,info)
	struct k_font *kf;
	unsigned int code, info;
{
	struct k_bitmap	*kb;
	short	row;

	kb = (struct k_bitmap *) malloc(sizeof(struct k_bitmap));
	kb->next = kb->prev = NULL;
	kb->uc = 0;
	/* kf->preFontNum and TeXnumber are always same when font is
	 * demand-loaded. If font is pre-loaded, preFontNum gives you
	 * real font number (in memory).
	 */
	kb->TeXnumber = kf->preFontNum;
	kb->info = info;
	kb->bits = NULL;

	add_k_bitmap(kb,code);

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
		kb = make_k_bitmap(k,cc,0);	/* demand-loaded fonts 
						   are always type 0 */
	if (kb->bits == NULL) {
	    switch (k->fonttype) {
#ifdef DNF
	      case FT_DNF:
	        read_dnf_bits(k,kb,cc);
		break;
#endif
	      case FT_NWF:
		read_nwf_bits(k,kb,cc);
		break;
#ifdef V_NWF
	      case FT_V_NWF:
		read_v_bits(k,kb,cc);
		break;
#endif /* V_NWF */
	      case FT_JFM:
		read_jfm_bits(k,kb,cc);
		break;
	    }
	}

	if ( k->fonttype != FT_DNF ) {
	  current_kanji_glyph.dvi_adv = k->glyph[kb->info].dvi_adv;
	  current_kanji_glyph.pxl_adv = k->glyph[kb->info].pxl_adv;
	  current_kanji_glyph.x = k->glyph[kb->info].x;
	  current_kanji_glyph.y = k->glyph[kb->info].y;
	  current_kanji_glyph.bitmap.h = k->h;
	  current_kanji_glyph.bitmap.w = k->w;
	  current_kanji_glyph.bitmap.bytes_wide = k->bytes_wide;
	  current_kanji_glyph.bitmap.bits = kb->bits;
	}
	else {
	  current_kanji_glyph.dvi_adv = k->glyph[kb->info].dvi_adv;
	  current_kanji_glyph.pxl_adv = k->glyph[kb->info].pxl_adv;
	  current_kanji_glyph.x = kb->x;
	  current_kanji_glyph.y = kb->y;

	  k->glyph[kb->info].x = kb->x;
	  k->glyph[kb->info].y = kb->y;

	  current_kanji_glyph.bitmap.h = k->h = kb->h;
	  current_kanji_glyph.bitmap.w = k->w = kb->w;
	  current_kanji_glyph.bitmap.bytes_wide = k->bytes_wide = 
						kb->bytes_width;
	  current_kanji_glyph.bitmap.bits = kb->bits;
	}

	return (&current_kanji_glyph);
}

open_PK_file(font)
	struct font *font;
{
  char filename[300];

  if (font->file == NULL) {
    sprintf(filename, "%s/%s",
	    PK_FONT_DIR, font->fontname);
    
    if (n_open_fonts == MAX_OPEN_FONTS)
      close_lru();
    font->file = fopen(filename, "r");
    if (font->file == NULL) {
/*
#ifdef JXDVI
      printf("%s [not found]\n", font->fontname);
#endif
#ifdef DVI2NWP
      fprintf(stderr,"%s [not found]\n", font->fontname);
#endif
*/
      return 0;
    }
    n_open_fonts += 1;
  }
/*
  if (list_fonts)
    printf("%s\n", font->fontname);
*/
  return 1;
}

read_PK_file(font)
     struct font *font;
{
  register struct glyph	*g;

  for (g = &font->glyph[0]; g < &font->glyph[MAXCHARS]; g++) {
    g->bitmap.bits = NULL;
  }

  read_pk_file(font,font->fontname);
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
#ifdef DEBUG
printf("Closing K_font:%d\n",f->TeXnumber);
#endif /* DEBUG */
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
	sprintf(tfmname,"%s/%s%s",JFM_DIR, fn, JFM_SUFFIX);
	tfm = fopen(tfmname,"r");
	if (tfm == NULL) {
		font_not_found = 1;
		printf("%s%s [not found]\n", fn, JFM_SUFFIX);
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

	sprintf(tfmname,"%s/%s%s",JFM_DIR, fn, JFM_SUFFIX);
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

	/* read jfm glue/kern */
	/* read through glue/kern table and throw away */
	for (i=0; i<j->table[J_NL]; i++) {
	  four(tfm);
	}

	/* read jfm kern */
	/* read through kern table and throw away */
	for (i=0; i<j->table[J_NK]; i++) {
	  four(tfm);
	}

	/* read jfm glue */
	/* read through kern table and throw away */
	for (i=0; i<j->table[J_NG]; i++) {
	  four(tfm);
	}

	/* read jfm param */
	j->param = (unsigned long *)
		malloc((sizeof(unsigned long)*(j->table[J_NP])));
	for (i=0; i<j->table[J_NP]; i++) {
	  j->param[i] = four(tfm);
/*	  printf("%s param i: %d, value: %d\n", fn, i, j->param[i]); */
	}

	fclose(tfm);

/* for debugging */
/* print_jfm(j); */
}

/*
 * We read some kanji fonts since they need special treatment.
 */
setup_special_k_bitmap(j,kf)
struct jfm *j;
struct k_font *kf;
{
	int	i;

	for (i=1; i < j->table[J_NT]; i++) {
		make_k_bitmap(kf,(unsigned int) j->type[i].code,
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
		} while( k != k_st );	/* Kanji Font List is ring list */
		printf("Total use:%d, load:%d, bytes:%d\n", use, load, bytes);
	}
}
#endif

#ifdef JXDVI
# ifndef X11 /* X10 */
sample(bitmap, x, y, w, h)
	register struct bitmap *bitmap;
	int x, y, w, h;
{
	register unsigned char *ptr, *endp;
	register int b, i, j, m, n;

	ptr = bitmap->bits
		+ (y * bitmap->bytes_wide)
		+ (x / BITS_PER_BYTE);
	endp = bitmap->bits + (bitmap->h * bitmap->bytes_wide);
	b = (1 << (x % BITS_PER_BYTE));
	n = 0;
	for (i = 0; i < h && ptr < endp; i += 1, ptr += bitmap->bytes_wide) {
		for (m = b, j = 0; (j<w && m<0x100); j += 1, m <<= 1) {
			if (*ptr & m)
				n += 1;
		}
		for (m = 0x01; j<w; j += 1, m <<= 1) {
		  if (*(ptr+1) & m)	/* assumeing w<=8 */
		    n += 1;
		}
	}
/*      return (n >= (i * w) / 3);*/
/* In some cases (for example, 400dpi font shrunk by 2), shrink_bitmap results
 * underlined font. This is corrected by Atsuo, K. by changing sample() so
 * that it returns (n>=(h*w)/3) instead (n>=(i*w)/3). 
 */
        return (n >= (h * w) / 3);
}
# else /* X11 */
sample(bitmap, x, y, w, h)
	register struct bitmap *bitmap;
	int x, y, w, h;
{
	register unsigned char *ptr, *endp;
	register int b, i, j, m, n;

	ptr = bitmap->bits
		+ (y * bitmap->bytes_wide)
		+ (x / BITS_PER_BYTE);
	endp = bitmap->bits + (bitmap->h * bitmap->bytes_wide);
	b = (1 << (BITS_PER_BYTE - (x % BITS_PER_BYTE)));
	n = 0;
	for (i = 0; i < h && ptr < endp; i += 1, ptr += bitmap->bytes_wide) {
		for (m = b, j = 0; (j<w && m>0); j += 1, m >>= 1) {
			if (*ptr & m)
				n += 1;
		}
		for (m = (1 << (BITS_PER_BYTE - 1)); j < w; j += 1, m >>= 1) {
		  if (*(ptr+1) & m)	/* assumeing w<=8 */
		    n += 1;
		}
	}
/*      return (n >= (i * w) / 3);*/
/* In some cases (for example, 400dpi font shrunk by 2), shrink_bitmap results
 * underlined font. This is corrected by Atsuo, K. by changing sample() so
 * that it returns (n>=(h*w)/3) instead (n>=(i*w)/3). 
 */
        return (n >= (h * w) / 3);
}
# endif /* X Version */

# ifndef X11 /* X10 */
shrink_bitmap(bitmap, x_factor, y_factor)
     register struct bitmap *bitmap;
     int x_factor, y_factor;
{
  unsigned char *shrunk_bits;
  int shrunk_height, shrunk_width, shrunk_bytes_wide;
  register unsigned char *ptr;
  unsigned char *cp;
  register int x, y, b, m;

  shrunk_height = ROUNDUP(bitmap->h, y_factor);
  shrunk_width = ROUNDUP(bitmap->w, x_factor);
  /* width must be multiple of 16 bits for raster_op */
  shrunk_bytes_wide = ROUNDUP(shrunk_width, BITS_PER_SHORT)*BYTES_PER_SHORT;

  ptr = shrunk_bits = (unsigned char *)
    calloc((unsigned) shrunk_height * shrunk_bytes_wide, 1);
  if (ptr == NULL)
    error("Can't allocate shrunken bitmap (%d by %d)",
	  shrunk_height, shrunk_width);
  for (y = 0; y < bitmap->h; y += y_factor) {
    b = 0;
    m = (1 << 0);
    cp = ptr;
    for (x = 0; x < bitmap->w; x += x_factor) {
      if (sample(bitmap, x, y, x_factor, y_factor))
	*ptr |= m;
      else
	*ptr &= ~m;
      b += 1;
      m <<= 1;
      if (b % BITS_PER_BYTE == 0) {
	b = 0;
	m = (1 << 0);
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

# else /* X11 */
shrink_bitmap(bitmap, x_factor, y_factor)
     register struct bitmap *bitmap;
     int x_factor, y_factor;
{
  unsigned char *shrunk_bits;
  int shrunk_height, shrunk_width, shrunk_bytes_wide;
  register unsigned char *ptr;
  unsigned char *cp;
  register int x, y, b, m;

  shrunk_height = ROUNDUP(bitmap->h, y_factor);
  shrunk_width = ROUNDUP(bitmap->w, x_factor);
  shrunk_bytes_wide = ROUNDUP(shrunk_width, BITS_PER_BYTE);

  ptr = shrunk_bits = (unsigned char *)
    calloc((unsigned) shrunk_height * shrunk_bytes_wide, 1);
  if (ptr == NULL) {
    error("Can't allocate shrunken bitmap (%d by %d)",
	  shrunk_height, shrunk_width);
  }

  for (y = 0; y < bitmap->h; y += y_factor) {
    b = 0;
    m = (1 << (BITS_PER_BYTE - 1));
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
	m = (1 << (BITS_PER_BYTE - 1));
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
# endif /* X Version */
#endif JXDVI

font_message(kind,font)
  int kind;
  Ptr font;		/* Pointer to k_font or font */
{
    struct k_font *kf;
    struct font *f;
    char buff[256];

    switch (kind) {
      case KF_NOT_FOUND:
	    kf = (struct k_font *) font;
#ifdef JXDVI
	    printf("%s (%d) [not found]\n", kf->fontname, kf->w);
#endif /* JXDVI */
#ifdef DVI2NWP
	    fprintf(stderr,"%s (%d) [not found]\n", kf->fontname, kf->w);
#endif /* DVI2NWP */
	    break;
      case F_NOT_FOUND:
	    f = (struct font *) font;

	    strncpy(buff,f->fontname,f->fn_len);
	    make_font_name(f->scale, f->design, 
			   (buff+f->fn_len), FT_NOT_FOUND);
#ifdef JXDVI
	    printf("%s\n", buff);
#endif /* JXDVI */
#ifdef DVI2NWP
	    fprintf(stderr,"%s\n", buff);
#endif /* DVI2NWP */
	    break;
    }
}

read_jfm_bits(kf,kb,code)
	struct k_font *kf;
	struct k_bitmap	*kb;
	unsigned int code;
{
	int i, j;
/*	int byte_paddings; */
	struct bitmap b;
	unsigned char	*ptr;

	/* Calculate & Set shrunken bitmap data */
	b.h = kf->h = ROUNDUP(kf->file_h, shrink_factor);
	b.w = kf->w = ROUNDUP(kf->file_w, shrink_factor);
	b.bytes_wide = kf->bytes_wide =
#ifdef JXDVI
#  ifndef X11 /* X10 */
	  ROUNDUP(b.w, BITS_PER_SHORT)*BYTES_PER_SHORT;
#  else /* X11 */
	  ROUNDUP(b.w, BITS_PER_BYTE);
#  endif /* X Version */
#endif /* JXDVI */
#ifdef DVI2NWP
	  ROUNDUP(b.w, BITS_PER_BYTE);
#endif /* DVI2NWP */

	ptr = b.bits = kb->bits = (unsigned char *)
		calloc( ( sizeof(char) * b.h * b.bytes_wide ),
		       sizeof(char) );
	if (ptr == NULL)
	  error("jxdvi: Can't allocate bitmap for char %d, font %s (%d by %d)",
		code, kf->fontname, b.h, b.w);

	/* Draw box */
	draw_box_to_bitmap(&b);

#ifdef JXDVI
/*
	if (shrink_factor != 1) {
	  shrink_bitmap(&b, shrink_factor, shrink_factor);
	}

	kf->h = b.h;
	kf->w = b.w;
	kf->bytes_wide = b.bytes_wide;

	kb->bits = b.bits;
*/
#  ifndef X11 /* X10 */
	if (backwards) reverse_bytes(&b);
#  endif
#endif JXDVI
/*
#ifdef STATISTICS
	kf->load += 1;
	kf->load_bytes += (kf->file_h * kf->file_bytes_wide);
#endif
*/
}

draw_box_to_bitmap(b)
  register struct bitmap *b;
{
    register unsigned char *ptr;
    register unsigned char pattern;
    register int j;

    ptr = b->bits;

    /* Upper bar */
    for (j = b->w; j > BITS_PER_BYTE; j -= BITS_PER_BYTE) {
	*ptr++ = 0xff;
    }
    if ( j > 0 ) {
#ifdef JXDVI
# ifndef X11 /* X10 */
	*ptr++ = reverse_byte[(0xff << (BITS_PER_BYTE - j))];
# else /* X11 */
	*ptr++ = 0xff << (BITS_PER_BYTE - j);;
# endif /* X Version */
#endif /* JXDVI */
# ifdef DVI2NWP
	*ptr++ = 0xff << (BITS_PER_BYTE - j);;
# endif DVI2NWP
    }

    /* Left bar */
    ptr = b->bits + b->bytes_wide;
    for (j = 1; j < b->h; j++) {
#ifdef JXDVI
#  ifndef X11 /* X10 */
	*ptr = reverse_byte[(0x80)];
#  else /* X11 */
	*ptr = 0x80;
#  endif /* X Version */
#endif /* JXDVI */
#ifdef DVI2NWP
	*ptr = 0x80;
#endif DVI2NWP
	ptr += b->bytes_wide;
    }

    /* Right bar */
    ptr = b->bits + b->bytes_wide + (b->w / BITS_PER_BYTE);
#ifdef JXDVI
#  ifndef X11 /* X10 */
    pattern = reverse_byte[(0x01 << (BITS_PER_BYTE - (b->w % BITS_PER_BYTE)))];
#  else /* X11 */
    pattern = 0x01 << (BITS_PER_BYTE - (b->w % BITS_PER_BYTE));
#  endif /* X Version */
#endif /* JXDVI */
#ifdef DVI2NWP
    pattern = 0x01 << (BITS_PER_BYTE - (b->w % BITS_PER_BYTE));
#endif DVI2NWP
    for (j = 1; j < b->h; j++) {
	*ptr = pattern;
	ptr += b->bytes_wide;
    }

    /* Lower bar */
    ptr = b->bits + (b->h - 1) * b->bytes_wide;
    for (j = b->w; j > BITS_PER_BYTE; j -= BITS_PER_BYTE) {
	*ptr++ = 0xff;
    }
    if ( j > 0 ) {
#ifdef JXDVI
# ifndef X11 /* X10 */
	*ptr++ = reverse_byte[(0xff << (BITS_PER_BYTE - j))];
# else /* X11 */
	*ptr++ = 0xff << (BITS_PER_BYTE - j);;
# endif /* X Version */
#endif /* JXDVI */
#ifdef DVI2NWP
	*ptr++ = 0xff << (BITS_PER_BYTE - j);;
#endif DVI2NWP
    }
}


