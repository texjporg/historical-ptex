/*
 *	read_pk.c for jxdvi/dvi2nwp
 *	$Header: read_pk.c,v 3.2 89/03/14 13:30:09 atsuo Exp $
 */

#include "config.h"

#ifndef lint
static char *rcsid = "$Header: read_pk.c,v 3.2 89/03/14 13:30:09 atsuo Exp $";
		/* from readpk.c for DVI driver for LBP8AJ2 */
static char *rcsid_ = "$-Header: readpk.c,v 1.2 87/05/15 16:15:13 tex Exp $";
#endif lint

/* Codes are derived from: */
/*
 * read PK font file
 *	the module is derived from PKtoPX processor
 *
 *		Author:	Yukio Ikadai
 *			ikadai@sra.junet
 *			Software Research Associates, Inc.
 *			1-1-1 Hirakawa-cho, chiyoda-ku,
 *			Tokyo, 102 Japan
 *				&
 *			ikadai@hst.junet
 *			Hirata Software Technology Co., Ltd.
 *			6-1-45 Toroku, Kumamoto-shi,
 *			Kumamoto, 862 Japan
 *
 *
 * $-Log:	readpk.c,v $
 * Revision 1.2  87/05/15  16:15:13  tex
*/

#include <stdio.h>
#include "pk.h"
#include "jxdvi.h"
#include "font.h"

#define fpremark remark

#define ROUND(x)	((long)((x)+0.5))


/*
 * define SHORT_ALIGN to make the unit of rows of bitmatp to SHORT
 * define REV_BYTE, if the device word is not BigEndian order
 * define REV_BIT, if the LSB displays at left
 */

#ifdef TEST_PK
#define SHORT_ALIGN
#endif /* TEST_PK */

#ifdef JXDVI
# ifndef X11 /* X10 */
#define SHORT_ALIGN
#define REV_BIT
# endif /* X Version */
#endif /* JXDVI */

#ifdef SHORT_ALIGN
typedef unsigned short	WORD;
#define BITS_PER_WORD	16
#define BYTES_PER_WORD	2

#else
typedef unsigned char	WORD;
#define BITS_PER_WORD	8
#define BYTES_PER_WORD	1
#endif

#ifdef REV_BYTE
#  ifdef REV_BIT
extern unsigned char reverse_byte[];
#    define _BMWORD(v)	\
		(((WORD)reverse_byte[(v)&0xff]<<8)|((WORD)reverse_byte[(v)>>8]&0xff))
#  else
#    define _BMWORD(v)	\
		((((v)&0xff)<<8)|(((v)>>8)&0xff))
#  endif
#else
#  ifdef REV_BIT
extern unsigned char reverse_byte[];
#    define _BMWORD(v)	\
		(((WORD)reverse_byte[(v)&0xff]&0xff)|((WORD)reverse_byte[(v)>>8]<<8))
#  else
#    define _BMWORD(v)	(v)
#  endif
#endif

#define BMWORD(word)	_BMWORD(word)

#ifndef BMWORD
BMWORD(word)
WORD	word;
{
	return _BMWORD(word);
}
#endif


#define MAX_WORD_WEIGHT	(BITS_PER_WORD-1)


#define MAX_BITS_PER_ROW	3200
#define MAX_WORDS_PER_ROW	(MAX_BITS_PER_ROW/BITS_PER_WORD)
			/* maximum char width divided by bits-per-word */

/*
 * global varibles
 */
static FILE	*pk_file;
static long	pk_loc;	/* how many bytes have we seen ? */
static int	flag_byte;	/* command or character flag byte */
static int	dyn_f;	/* dynamic packing variable */
static int	car;	/* the character we are reading */
static WORD	power[BITS_PER_WORD];
static WORD	gpower[BITS_PER_WORD+1];
static int	repeat_count;	/* how many times to repeat the next row ? */


static pk_initialize()
{
	register int	i;
#define ONE_FOURTH	(1<<(BITS_PER_WORD-2))

	/* set initial values */
	if(power[0] == 0) {
		power[0] = 1;
		for(i = 1; i <= BITS_PER_WORD-2; i++)
			power[i] = power[i-1]*2;
		power[BITS_PER_WORD-1] = -ONE_FOURTH - ONE_FOURTH;
		gpower[0] = 0;
		for(i = 1; i <= BITS_PER_WORD; i++)
			gpower[i] = gpower[i-1] + power[i-1];
	}
#undef ONE_FOURTH
}

/*
 * open PK file. returns true if suceeded, false otherwise
 */
int open_pk_file(fontp, fontname)
register struct font	*fontp;
char	*fontname;
{
        char    filename[1025];
	int	unmodsize;
	float	realsize;
	int     size;

	if (debug)				      /*changed: see 4.2notes*/
	  remark("scale = %d, design = %d", fontp->scale, fontp->design); /*"*/
#ifdef OLD					      /*changed: see 4.2notes*/
	realsize = (float) fontp->scale / fontp->design);	      /*ditto*/
#else						      /*changed: see 4.2notes*/
	realsize = (magnification/1000.)*((float) fontp->scale / fontp->design);
#endif						      /*changed: see 4.2notes*/
	unmodsize = (realsize * 1000) + 0.5;
	if (debug)				      /*changed: see 4.2notes*/
	  remark("real = %g, unmod = %d", realsize, unmodsize);	      /*ditto*/
	/* a real hack to correct for rounding in some cases -- rkf */
	if(unmodsize==1095) realsize = 1.095445;	/* stephalf */
	else if(unmodsize==1315) realsize=1.314534;	/* stepihalf */
	else if(unmodsize==2074) realsize=2.0736;	/* stepiv */
	else if(unmodsize==2488) realsize=2.48832;	/* stepv */
	else if(unmodsize==2986) realsize=2.985984;	/* stepiv */
	/* the remaining magnification steps are represented with sufficient
		accuracy already */
	size = (realsize * PIXELS_PER_INCH) + 0.5;
	
	sprintf(filename, "%s.%dpk", fontname, size);
	
	if ((pk_file = fopen(filename, "r")) == NULL)
	{
	    sprintf(filename, "%s/%s.%dpk", PK_FONT_DIR, fontname, size);
	    
	    if ((pk_file = fopen(filename, "r")) == NULL)
            {
		if(debug) {
			remark("scale = %d, design = %d",fontp->scale, fontp->design);
			fpremark("Can't find font: \"%s\"", filename);
		}
		return 0;
            }
	}

	fontp->file = pk_file;
	if (debug)				      /*changed: see 4.2notes*/
		remark("read font %s", filename);     /*changed: see 4.2notes*/

	return 1;
}

static int pk_byte()
{
	pk_loc++;
	return one(pk_file);
}

static int get_16()
{
	pk_loc += 2;
	return two(pk_file);
}

static int get_32()	
{
	pk_loc += 4;
	return sfour(pk_file);
}

static int
pk_seek(stream, offset, ptrname)
     FILE *stream;
     long offset;
     int ptrname;
{
  switch (ptrname) {
  case 0:		/* from the beggining */
    pk_loc = offset;
    break;
  case 1:		/*      the current position */
    pk_loc += offset;
    break;
  case 2:		/*      the end of file (this is a special case) */
    {
      int i;

      i = fseek(stream,offset,ptrname);
      pk_loc = ftell(stream);
      return i;
    }
  }
  return fseek(stream,offset,ptrname);
}

static int	input_byte;	/* the byte we are currently decimating */
static int	bit_weight;	/* weight of the current bit */
static WORD	row[MAX_WORDS_PER_ROW+1];

static int get_nyb()
{
	register int	temp;

	if(bit_weight == 0) {
		input_byte = pk_byte();
		bit_weight = 16;
	}
	temp = input_byte / bit_weight;
	input_byte -= temp * bit_weight;
	bit_weight /= 16;
	return temp;
}

static int get_bit()
{
	register int	temp;

	bit_weight /= 2;
	if(bit_weight == 0) {
		input_byte = pk_byte();
		bit_weight = 128;
	}
	temp = input_byte >= bit_weight;
	if(temp)
		input_byte -= bit_weight;
	return temp;
}

/*
 * packed number procedure 19, used in 44
 */
static int pk_packed_num()
{
	register int	i, j;

	i = get_nyb();

	if(i == 0) {
		do {
			j = get_nyb(); i++;
		} while(j == 0);
		while(i > 0) {
			j = j*16 + get_nyb(); i--;
		}
		return j - 15 + (13 - dyn_f)*16 + dyn_f;
	}

	if(i <= dyn_f)
		return i;
	if(i < 14)
		return (i - dyn_f - 1)*16 + get_nyb() + dyn_f + 1;

	if(i == 14)
		repeat_count = pk_packed_num();
	else
		repeat_count = 1;
	return pk_packed_num();
}

static skip_specials()
{
	register int	i, j;

	do {
		flag_byte = pk_byte();
		if(flag_byte >= 240) {
			switch(flag_byte) {
			case 240: case 241: case 242: case 243:
				i = 0;
				for(j = 240; j <= flag_byte; j++)
					i = 256*i + pk_byte();
				for(j = 1; j <= i; j++)
					(void) pk_byte();
				break;
			case 244:
				(void) get_32();
				break;
			case 245:
				break;
			case 246:
				break;
			default:
				remark("Unexpected %d flag byte!", flag_byte);
				break;
			}
		}
	} while(!(flag_byte < 240 || flag_byte == PK_POST));
	return flag_byte;
}





#define Send_row()	\
	for(i = 0; i <= repeat_count; i++) \
		for(j = 1; j <= word_width; j++) \
			*ptr++ = row[j];

#define Create_normally_packed_raster()	\
    {	\
	rows_left = c_height;	\
	h_bit = c_width;	\
	repeat_count = 0;	\
	word_weight = BITS_PER_WORD;	\
	word = 0;	\
	r_p = 1;	\
	while(rows_left > 0) {	\
	    count = pk_packed_num();	\
	    while(count > 0) {	\
		if(count < word_weight && count < h_bit) {	\
			if(turn_on)	\
				word += gpower[word_weight] - gpower[word_weight - count];	\
			h_bit -= count;	\
			word_weight -= count;	\
			count = 0;	\
		} else if(count >= h_bit && h_bit <= word_weight) {	\
			if(turn_on)	\
				word += gpower[word_weight] - gpower[word_weight - h_bit];	\
			row[r_p] = BMWORD(word); \
			Send_row();	\
			rows_left -= repeat_count +1;	\
			repeat_count = 0;	\
			r_p = 1;	\
			word = 0;	\
			word_weight = BITS_PER_WORD;	\
			count -= h_bit;	\
			h_bit = c_width;	\
		} else {	\
			if(turn_on)	\
				word += gpower[word_weight];	\
			row[r_p] = BMWORD(word); \
			r_p++;	\
			word = 0;	\
			count -= word_weight;	\
			h_bit -= word_weight;	\
			word_weight = BITS_PER_WORD;	\
		}	\
	    }	\
	    turn_on = !turn_on;	\
	}	\
	if(rows_left != 0 || h_bit != c_width) {	\
		remark("Bad pk file---more bits than required!");	\
		(void) fclose(pk_file);	\
		return 0;	\
	}	\
    }

/*
 * read PK font file and set font paramters to struct pointed by fontp
 *	it returns true when it sucessfully load the font
 *	otherwise false.
 */
read_pk_file(fontp, fontname)
register struct font	*fontp;
char	*fontname;
{
	register int	i, j;
	long	end_of_packet;
	long	packet_length;
	long	maginification;
	long	design_size;	/* design size in FIXes */
	long	checksum;	/* check sum */
	long	hppp, vppp;	/* horizontal and vertical points per inch */
	long	tfm_wid;
	long	hor_esc;
	int	c_width;
	int	c_height;
	int	x_off;
	int	y_off;
#ifdef notdef
	int	byte_width;
	int	byte_weight;
	unsigned char byte;
#endif notdef
	int	word_width;
	int	word_weight;

	WORD	word;

	int	rows_left;
	int	turn_on;
	int	h_bit;
	int	count;
	int	r_p;
	register WORD	*ptr;
	register struct glyph	*g;

	pk_initialize();
#ifdef notdef
	for(i = 0; i < MAXCHARS; i++)
		fontp->state[i] = NotThere;
#endif notdef

	/* open file */
#ifndef TEST_PK
	if (!open_PK_file(fontp))
	  error("jxdvi: Can't find font file %s", fontp->fontname);
#endif /* TEST_PK */

	pk_file = fontp->file;
	pk_seek(pk_file,0,0);	/* goto the begging of the file */

	/* read preamble */
	if(pk_byte() != PK_PRE) {
		remark("Bad pk file!  pre command missing");
		(void) fclose(pk_file);
		return 0;
	}
	if(pk_byte() != PK_ID) {
		remark("Wrong version of packed file");
		(void) fclose(pk_file);
		return 0;
	}
	j = pk_byte();			/* k[1] */
	for(i = 0; i < j; i++)
		(void) pk_byte();	/* throw away x[k] */

	design_size = get_32();		/* ds[4] */
	checksum = get_32();		/* cs[4] */
	hppp = get_32();		/* hppp[4] */
	vppp = get_32();		/* vppp[4] */
	if(hppp != vppp)
		remark("Warning:  aspect ration not 1:1");
	maginification = ROUND(hppp*72.27*5/65536);

#ifdef TEST_PK
	{ /* for debugging */
	    long position_save;
	    position_save = ftell(fontp->file);

	    read_pk_glyph(fontp);

	    fseek(fontp->file,position_save,0);
	}
#else /* NORMAL */
	read_pk_glyph(fontp);
	return 1;
#endif /* TEST_PK */

#ifdef TEST_PK
        skip_specials();

	/* read characters */
	while(flag_byte != PK_POST) {
		/* Unpack character */
		dyn_f = flag_byte / 16;
		flag_byte %= 16;
		turn_on = flag_byte >= 8;
		if(turn_on)
			flag_byte -= 8;
		if(flag_byte == 7) {
				/* Read long character preamble */
			packet_length = get_32();
			car = get_32();
			end_of_packet = packet_length + pk_loc;
			if(car => MAXCHARS || car < 0)
				goto bad_char;
			tfm_wid = get_32();
			hor_esc = get_32();
			(void) get_32(); /* ignore vertical esc */
			c_width = get_32();
			c_height = get_32();
			if(c_width < 0 || c_height < 0 ||
					 c_width > 65535 || c_height > 65535)
				goto bad_char;
			x_off = get_32();
			y_off = get_32();
		} else if(flag_byte > 3) {
				/* Read extended short character preamble */
			packet_length = (flag_byte - 4)*65536 + get_16();
			car = pk_byte();
			end_of_packet = packet_length + pk_loc;
			if(car => MAXCHARS)
				goto bad_char;
			i = pk_byte();
			tfm_wid = i*65536 + get_16();
			hor_esc = get_16();
			c_width = get_16();
			c_height = get_16();
			x_off = get_16();
			y_off = get_16();
			if(x_off > 32767)
				x_off -= 65536;
			if(y_off > 32767)
				y_off -= 65536;
		} else {
				/* Read short character preamble */
			packet_length = flag_byte*256 + pk_byte();
			car = pk_byte();
			end_of_packet = packet_length + pk_loc;
			if(car => MAXCHARS)
				goto bad_char;
			i = pk_byte();
			tfm_wid = i*65536 + get_16();
			hor_esc = pk_byte();
			c_width = pk_byte();
			c_height = pk_byte();
			x_off = pk_byte();
			y_off = pk_byte();
			if(x_off > 127)
				x_off -= 256;
			if(y_off > 127)
				y_off -= 256;
		}
#ifdef notdef
		fontp->state[car] = Ready;
		fontp->dvi_adv[car] = ((double)fontp->scale*tfm_wid)/(1<<20);
		fontp->pxl_adv[car] = pixel_round(fontp->dvi_adv[car]);
		fontp->W[car] = c_width;
		fontp->H[car] = c_height;
		fontp->X[car] = x_off;
		fontp->Y[car] = y_off;
#endif notdef
		g = &fontp->glyph[car];
		g->dvi_adv = ((double)fontp->scale*tfm_wid)/(1<<20);
		g->pxl_adv = pixel_round(g->dvi_adv);
		g->x = x_off;
		g->y = y_off;
		g->bitmap.h = c_height;
		g->bitmap.w = c_width;

		word_width = (c_width + BITS_PER_WORD-1) / BITS_PER_WORD;
		g->bitmap.bytes_wide = word_width * BYTES_PER_WORD;
		g->bitmap.short_wide = word_width;

		/* Read and translate raster description */
		ptr = (WORD*)malloc((unsigned)(c_height*word_width*BYTES_PER_WORD));
		if(ptr == NULL)
			error("Out fo memory while reading character %d of font %s", car, fontname);
#ifdef notdef
		fontp->Glyph[car] = (char*)ptr;
#endif notdef
/* 		g->bitmap.bits = (short*)ptr; */
#ifdef TEST_PK
		g->bitmap.bits = (unsigned char *)ptr;
#endif
#ifdef JXDVI
		g->bitmap.bits = (unsigned char *)ptr;
#endif
#ifdef DVI2NWP
		g->bitmap.bits = (unsigned char *)ptr;
#endif
		bit_weight = 0;
		if(dyn_f == 14) {
			/* Get raster by bits */
			bit_weight = 0;
			for(i = 1; i <= c_height; i++) {
				word = 0;
				word_weight = MAX_WORD_WEIGHT; 
				for(j = 1; j <= c_width; j++) {
					if(get_bit())
						word += power[word_weight];
					word_weight--;
					if(word_weight == -1) {
						*ptr++ = BMWORD(word);
						word = 0;
						word_weight = MAX_WORD_WEIGHT;
					}
				}
				if(word_weight < MAX_WORD_WEIGHT)
					*ptr++ = BMWORD(word);
			}
		} else {
			/* Create normally packed raster */
/* 			Create_normally_packed_raster(); */
    {	
	rows_left = c_height;	
	h_bit = c_width;	
	repeat_count = 0;	
	word_weight = BITS_PER_WORD;	
	word = 0;	
	r_p = 1;	
	while(rows_left > 0) {	
	    count = pk_packed_num();	
	    while(count > 0) {	
		if(count < word_weight && count < h_bit) {	
			if(turn_on)	
				word += gpower[word_weight] - gpower[word_weight - count];	
			h_bit -= count;	
			word_weight -= count;	
			count = 0;	
		} else if(count >= h_bit && h_bit <= word_weight) {	
			if(turn_on)	
				word += gpower[word_weight] - gpower[word_weight - h_bit];	
			row[r_p] = BMWORD(word); 
			Send_row();	
			rows_left -= repeat_count +1;	
			repeat_count = 0;	
			r_p = 1;	
			word = 0;	
			word_weight = BITS_PER_WORD;	
			count -= h_bit;	
			h_bit = c_width;	
		} else {	
			if(turn_on)	
				word += gpower[word_weight];	
			row[r_p] = BMWORD(word); 
			r_p++;	
			word = 0;	
			count -= word_weight;	
			h_bit -= word_weight;	
			word_weight = BITS_PER_WORD;	
		}	
	    }	
	    turn_on = !turn_on;	
	}	
	if(rows_left != 0 || h_bit != c_width) {	
		remark("Bad pk file---more bits than required!");	
		(void) fclose(pk_file);	
		return 0;	
	}	
    }

		}

		if(end_of_packet != pk_loc) {
			remark("Bad PK file!  Bad packet length");
			(void) fclose(pk_file);
			return 0;
		}
		goto good_char;
	  bad_char:
		while(pk_loc != end_of_packet)
			(void) pk_byte();	/* throw away */
		remark("Character %d out of range\n", car);
	  good_char:
		skip_specials();
	}

	if(debug) {
		while(!feof(pk_file))
			(void) pk_byte();
		remark("%d bytes read from paked file.",  pk_loc);
	}

#ifdef notdef
	(void) fclose(pk_file);
#endif

	return 1;
#endif /* TEST_PK */
}

/* VARARGS1 */
remark(message, a, b, c, d, e, f)
	char *message;
{
	fprintf(stderr, message, a, b, c, d, e, f);
	putc('\n', stderr);
}

read_pk_glyph(fontp)
register struct font *fontp;
{
  register struct glyph *g;
  int	turn_on;
  long	end_of_packet;
  long	packet_length;
  long	tfm_wid;
  long	hor_esc;
  int	c_width;
  int	c_height;
  int	word_width;
  int	x_off;
  int	y_off;
  int	i, j;

  while(skip_specials() != PK_POST) {	/* skip specails and read flag */
    dyn_f = flag_byte / 16;
    flag_byte %= 16;
    turn_on = flag_byte >= 8;
    if(turn_on)
      flag_byte -= 8;
    /* now flag_byte contains 3 LSB's */
    if(flag_byte == 7) {      /* Read long character preamble */
      packet_length = get_32();			/* pl[4] */
      car = get_32();				/* cc[4] */
      end_of_packet = packet_length + pk_loc;
      if(car > 127 || car < 0)
/**/	remark("Character %d out of range\n", car);

      tfm_wid = get_32();			/* tfm[4] */
      hor_esc = get_32();			/* dx[4] */
      (void) get_32(); /* ignore vertical esc *//* dy[4] */
      c_width = get_32();			/* w[4] */
      c_height = get_32();			/* h[4] */
      if(c_width < 0 || c_height < 0 || c_width > 65535 || c_height > 65535)
/**/	remark("Character %d out of range\n", car);

      x_off = get_32();				/* hoff[4] */
      y_off = get_32();				/* voff[4] */
    } else if(flag_byte > 3) {	/* Read extended short character preamble */
      packet_length = (flag_byte - 4)*65536 + get_16(); /* pl[2] */
      car = pk_byte();				/* cc[1] */
      end_of_packet = packet_length + pk_loc;
      if(car > 127)
/**/	remark("Character %d out of range\n", car);

      i = pk_byte();
      tfm_wid = i*65536 + get_16();		/* tfm[3] */
      hor_esc = get_16();			/* dm[2]  */
      c_width = get_16();			/* w[2]   */
      c_height = get_16();			/* h[2]   */
      x_off = get_16();				/* hoff[2] */
      y_off = get_16();				/* voff[2] */
      if(x_off > 32767)
	x_off -= 65536;
      if(y_off > 32767)
	y_off -= 65536;
    } else {	      /* Read short character preamble */
      packet_length = flag_byte*256 + pk_byte();/* pl[1] */
      car = pk_byte();				/* cc[1] */
      end_of_packet = packet_length + pk_loc;
      if(car > 127)
/**/	remark("Character %d out of range\n", car);

      i = pk_byte();
      tfm_wid = i*65536 + get_16();		/* tfm[3] */
      hor_esc = pk_byte();			/* dm[1]  */
      c_width = pk_byte();			/* w[1]   */
      c_height = pk_byte();			/* h[1]   */
      x_off = pk_byte();			/* hoff[1] */
      y_off = pk_byte();			/* voff[1] */
      if(x_off > 127)
	x_off -= 256;
      if(y_off > 127)
	y_off -= 256;
    }

    g = &(fontp->glyph[car]);
#ifdef JXDVI
    g->bitmap.bits = NULL;
#endif JXDVI
#ifdef DVI2NWP
    g->bitmap.bits = NULL;
#endif DVI2NWP
    g->dvi_adv = ((double)fontp->scale*tfm_wid)/(1<<20);
    g->pxl_adv = pixel_round(g->dvi_adv);
    g->x = x_off / shrink_factor;
    g->y = y_off / shrink_factor;
    g->bitmap.h = c_height;
    g->bitmap.w = c_width;
    word_width = (c_width + BITS_PER_WORD-1) / BITS_PER_WORD;
    g->bitmap.bytes_wide = word_width * BYTES_PER_WORD;
    g->bitmap.short_wide = word_width;

    g->turn_on = turn_on;
    g->dyn_f = dyn_f;
    g->addr = ftell(fontp->file);	/* save address of raster data */

    pk_seek(fontp->file, end_of_packet, 0); /* goto the next packet */
  }
}

read_pk_bitmap(ch, g)
     ubyte ch;
     register struct glyph *g;
{
  register struct bitmap *bitmap;
  register int file_bytes_wide;
  register int i, j;
  int	word_weight;
  int	word_width;
  WORD	word;
#ifdef TEST_PK
  register WORD *ptr;
#endif /* TEST_PK */
#ifdef JXDVI
  register WORD *ptr;
#endif /* JXDVI */
#ifdef DVI2NWP
  register WORD *ptr;
#endif /* DVI2NWP */

  bitmap = &(g->bitmap);

  word_width = (bitmap->w + BITS_PER_WORD-1) / BITS_PER_WORD;
  ptr = (WORD *)malloc((unsigned)(bitmap->h * word_width * BYTES_PER_WORD));
#ifdef TEST_PK
  if(ptr == NULL)
    error("Out fo memory while reading character %d", ch);
#endif /* TEST_PK */
#ifdef JXDVI
  if (ptr == NULL)
    error("jxdvi: Can't allocate bitmap for char. %d, font %s (%d by %d)",
	  ch, current_font->fontname, bitmap->h, bitmap->w);
#endif JXDVI
#ifdef DVI2NWP
  if (ptr == NULL)
    error("jxdvi: Can't allocate n_bitmap for char. %d, font %s",
	  ch, current_font->fontname);
#endif DVI2NWP

#ifdef TEST_PK
  g->bitmap.bits = (unsigned char *)ptr;
#endif
#ifdef JXDVI
  g->bitmap.bits = (unsigned char *)ptr;
#endif
#ifdef DVI2NWP
  g->bitmap.bits = (unsigned char *)ptr;
#endif

  /* goto the current character packet */
#ifndef TEST_PK
  if (!open_PK_file(current_font))
    error("jxdvi: Can't find font file %s", current_font->fontname);
#endif /* TEST_PK */

  pk_file = current_font->file;
  pk_seek(current_font->file, g->addr, 0);

  /* Read and translate raster description */
  bit_weight = 0;
  if(g->dyn_f == 14) {
    /* Get raster by bits */
    bit_weight = 0;
    for(i = 1; i <= bitmap->h; i++) {
      word = 0;
      word_weight = MAX_WORD_WEIGHT; 
      for(j = 1; j <= bitmap->w; j++) {
	if(get_bit())
	  word += power[word_weight];
	word_weight--;
	if(word_weight == -1) {
	  *ptr++ = BMWORD(word);
	  word = 0;
	  word_weight = MAX_WORD_WEIGHT;
	}
      }
      if(word_weight < MAX_WORD_WEIGHT)
	*ptr++ = BMWORD(word);
    }
  } else {
    /* Create normally packed raster */
    int	rows_left;		/* initialization not needed */
    int	h_bit;			/* initialization not needed */
    int	r_p;			/* initialization not needed */
    int	count;			/* initialization not needed */

    int	c_width;
    int	c_height;
    int	turn_on;

    turn_on = g->turn_on;
    c_width = bitmap->w;
    c_height = bitmap->h;
    dyn_f = g->dyn_f;

/*     Create_normally_packed_raster();*/
    {	
	rows_left = c_height;	
	h_bit = c_width;	
	repeat_count = 0;	
	word_weight = BITS_PER_WORD;	
	word = 0;	
	r_p = 1;	
	while(rows_left > 0) {	
	    count = pk_packed_num();	
	    while(count > 0) {	
		if(count < word_weight && count < h_bit) {	
			if(turn_on)	
				word += gpower[word_weight] - gpower[word_weight - count];	
			h_bit -= count;	
			word_weight -= count;	
			count = 0;	
		} else if(count >= h_bit && h_bit <= word_weight) {	
			if(turn_on)	
				word += gpower[word_weight] - gpower[word_weight - h_bit];	
			row[r_p] = BMWORD(word); 
			Send_row();	
			rows_left -= repeat_count +1;	
			repeat_count = 0;	
			r_p = 1;	
			word = 0;	
			word_weight = BITS_PER_WORD;	
			count -= h_bit;	
			h_bit = c_width;	
		} else {	
			if(turn_on)	
				word += gpower[word_weight];	
			row[r_p] = BMWORD(word); 
			r_p++;	
			word = 0;	
			count -= word_weight;	
			h_bit -= word_weight;	
			word_weight = BITS_PER_WORD;	
		}	
	    }	
	    turn_on = !turn_on;	
	}	
	if(rows_left != 0 || h_bit != c_width) {	
		remark("Bad pk file---more bits than required!");	
		(void) fclose(pk_file);	
		return 0;	
	}	
    }

  }

/*
  if(end_of_packet != pk_loc) {
    remark("Bad PK file!  Bad packet length");
    (void) fclose(pk_file);
    return 0;
  }
*/
  goto good_char;
 
 bad_char:
/*
  while(pk_loc != end_of_packet)
    (void) pk_byte();	/ throw away /
*/
  remark("Character %d out of range\n", car);
  return NULL;

 good_char:

#ifdef JXDVI
  if (shrink_factor != 1)
		shrink_bitmap(bitmap, shrink_factor, shrink_factor);
	if (debug & DBG_BITMAP)
		print_char(ch, g);
#endif JXDVI
  return 1;
}

