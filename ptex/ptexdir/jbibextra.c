/*
 * Auxilliary routines for BibTeX in C.
 *
 * Tim Morgan 2/15/88
 * Eduardo Krell 4/21/88
 */

/*
 * JBibTeX Version by Shouichi Matsui
 *  matsui@denken.or.jp (matsui%denken.or.jp@uunet.uu.net)
 *  1989-05-02
 *  Last updated: 1990-12-29
 *  many codes are from ../cjtex/extra.c by T. Sakurai 
 */
/*
 * Modified for pTeX 2.1 by Kazunori Aasayama
 *  asayama@kuis.kyoto-u.ac.jp  Sep. 4, 1995
 * Modified for pTeX 2.1.5 by Kazunori Aasayama
 *  asayama@vsp.cpg.sony.co.jp March. 14, 1997
 */

/*
   Change DEFAULT_FILE_CODE, DEFAULT_TERM_CODE definition for your site code
   1	JIS; Kanji-in ESC$@, Kanji-out ESC(J
   2	JIS; Kanji-in ESC$@, Kanji-out ESC(H
   3	JIS; Kanji-in ESC$@, Kanji-out ESC(B
   4	JIS; Kanji-in ESC$B, Kanji-out ESC(J
   5	JIS; Kanji-in ESC$B, Kanji-out ESC(H
   6	JIS; Kanji-in ESC$B, Kanji-out ESC(B
   10	Shift JIS
   20	EUC
 */

/* #define KANJI */
#define KANJI_C
#include "kanji.h"
#include "jbibd.h"
#include <kpathsea/variable.h>

#ifdef HAVE_SYS_PARAM_H
#  include <sys/param.h>
#else
#  include <limits.h>
#endif
#ifndef NOFILE
#  define NOFILE OPEN_MAX
#endif

/*
   const_string based_prog_name="BibTeX";
   const_string based_prog_version="0.99c";
   */

#ifdef OUTJIS
#  define DEFAULT_FILE_CODE  6     /* JIS */
#  define DEFAULT_TERM_CODE  6     /* JIS */
#else
#  ifdef SJISPTEX
#    define DEFAULT_FILE_CODE  10     /* SJIS */
#    define DEFAULT_TERM_CODE  10     /* SJIS */
#  else /* EUC */
#    ifdef EUCPTEX
#      define DEFAULT_FILE_CODE  20     /* EUC */
#      define DEFAULT_TERM_CODE  20     /* EUC */
#    else
#      error /* unexpected. */
#    endif
#  endif
#endif

/* END OF SITE DEPENDENT DEFINITIONS */

typedef	ASCIIcode textchar;
typedef struct kaninfo_s kaninfo;
struct kaninfo_s{
  char ktype, kstat;
  textchar kpend;
  int (*in_ln)();
  int (*out_ch)();
  kaninfo *next;
};

static kaninfo kanjiinfo_table[NOFILE];

static int kanji_file_code, kanji_term_code; /* kanji code same as NTT-JTeX */
static get_kanji_code();
static set_kanji_type();

boolean jbibtex_open_input (f, path_index, fopen_mode)
     FILE **f;
     int path_index;
     const_string fopen_mode;
{
  boolean result;
  result  = open_input(f,path_index,fopen_mode);
  if (result == false) return false;
  set_kanji_type(*f, kanji_file_code);
  return result;
}

boolean jbibtex_open_output (f, fopen_mode)
     FILE **f;
     const_string fopen_mode;
{
  boolean result;
  result  = open_output(f,fopen_mode);
  if (result == false) return false;
  set_kanji_type(*f, kanji_file_code);
  return result;
}

boolean inputline(f)
     FILE *f;
{
  int in;
  register kaninfo *ki;

  last = 0;
  ki = &kanjiinfo_table[fileno(f)];
  in = (*(ki->in_ln))(f,ki);
  if (in == EOF && last == 0)
    return false;
  if (in != EOF && in != '\n')
    return false;

  /* Don't bother using xord if we don't need to.  */
#ifdef NONASCII
  /*
     for (in = 0; in <= last; in++)
     buffer[in] = xord[buffer[in]];
     */
#endif

  return true;
}

jbibtex_printstr(s, c)
     char *s, c;
{
  static unsigned char temp_buff[4096];	/* maybe too long */
  sprintf(temp_buff, "%s%c", s, c);
  jbibtex_Fputs(logfile, temp_buff);
  jbibtex_Fputs(stdout, temp_buff);
}

jbibtex_putc(c, f)
     textchar c;
     FILE *f;
{
  register kaninfo *ki;

  ki = &kanjiinfo_table[fileno(f)];
  (*(ki->out_ch))(c, f, ki);
}

jbibtex_Fputs(f, s)
     register FILE *f;
     register textchar *s;
{
  register kaninfo *ki;

  ki = &kanjiinfo_table[fileno(f)];
  for (; *s; s++)
    (*(ki->out_ch))(*s, f, ki);
}

initkanji()
{
  get_kanji_code();
}

/*** internal routines ***/

/* kanji state */
#define	KAN_ASCII	0
#define	KAN_JIS_FIRST	1
#define	KAN_JIS_SECOND	2
#define	KAN_ASCII_KAN	3

#define	ESC		'\033'
#define	KANJIBIT	0x80
#define	isTeXkanji(c)	((c)&KANJIBIT)
#define	deTeXkanji(c)	((c)&~KANJIBIT)

static int in_ln_gen();
static int in_ln_ascii();
static int in_ln_jis();
static int in_ln_sjis();

static int out_ch_ascii();
static int out_ch_jis();
static int out_ch_sjis();
static int out_ch_euc();

static integer check_euc_kanji();
static integer check_sj_kanji();
static integer fix_kanji_range();

static int (*in_ln_tab[])() = {
  in_ln_ascii,
  in_ln_jis,
  in_ln_jis,
  in_ln_jis,
  in_ln_jis,
  in_ln_jis,
  in_ln_jis,
  0,
  0,
  0,
  in_ln_sjis,
  0, 0, 0, 0, 0, 0, 0, 0, 0,
  in_ln_jis
};
static int (*out_ch_tab[])() = {
  out_ch_ascii,
  out_ch_jis,
  out_ch_jis,
  out_ch_jis,
  out_ch_jis,
  out_ch_jis,
  out_ch_jis,
  0,
  0,
  0,
  out_ch_sjis,
  0, 0, 0, 0, 0, 0, 0, 0, 0,
  out_ch_euc
};
struct {
  char esc_in, esc_out;
} jis_esc_tab[] = {
  {'\0', '\0'},			/* dummy */
  {'@', 'J'},
  {'@', 'H'},
  {'@', 'B'},
  {'B', 'J'},
  {'B', 'H'},
  {'B', 'B'}
};

static
get_kanji_code()
{
  extern int atoi();
  register char *p;

  kanji_file_code = DEFAULT_FILE_CODE;
  kanji_term_code = DEFAULT_TERM_CODE;

  if( p=kpse_var_value("BIBFILECODE") )
    kanji_file_code = atoi(p);
  if( (kanji_file_code<1)||(kanji_file_code>20)||
     (in_ln_tab[kanji_file_code] == 0) ) {
    fprintf(stderr,"Kanji File Code:%d is illegal. I'll use system default\n",
	    kanji_file_code);
    kanji_file_code = DEFAULT_FILE_CODE;
  }
  if( p=kpse_var_value("BIBTERMCODE") )
    kanji_term_code = atoi(p);
  if( (kanji_term_code<1)||(kanji_term_code>20)||
     (in_ln_tab[kanji_term_code] == 0 ) ) {
    fprintf(stderr,"Kanji Term Code:%d is illegal. I'll use system default\n",
	    kanji_term_code);
    kanji_term_code = DEFAULT_TERM_CODE;
  }
  set_kanji_type(stdout,kanji_term_code);
  set_kanji_type(stderr,kanji_term_code);
}

static
set_kanji_type(f, ktype)
     FILE *f;
     int ktype;
{
  kaninfo *ki;
  ki = &kanjiinfo_table[fileno(f)];
  ki->ktype = ktype;
  ki->kstat = KAN_ASCII;
  ki->in_ln = in_ln_tab[ktype];
  ki->out_ch = out_ch_tab[ktype];
}

static
int in_ln_ascii(f, ki)
     register FILE *f;
     kaninfo *ki;
{
  register int i;

  while ( last < bufsize && ((i = getc(f)) != EOF) && i != '\n') {
#ifdef	NONASCII
    buffer[last++] = i;
#else
    buffer[last++] = (i > 127 || i < 0)?' ':i;
#endif
  }
  return (i);
}

static
int in_ln_jis(f, ki)
     FILE *f;
     kaninfo *ki;
{
  return (in_ln_gen(f, ki, check_euc_kanji, fix_kanji_range));
}

static
int in_ln_sjis(f, ki)
     FILE *f;
     kaninfo *ki;
{
  return (in_ln_gen(f, ki, check_sj_kanji, SJIStoEUC));
}

static integer
check_euc_kanji(c, ki)
     textchar c;
     kaninfo *ki;
{
  /* An illegal kanji should be detected, but ... */
  if (isTeXkanji(c)) {
    ki->kpend = c;
    ki->kstat = KAN_ASCII_KAN;
  } else
    buffer[last++] = c;
}

static integer
check_sj_kanji(c, ki)
     textchar c;
     kaninfo *ki;
{
  if (c >= 0xe0 || c >= 0x80 && c < 0xa0) {
    ki->kpend = c;
    ki->kstat = KAN_ASCII_KAN;
  } else if (c < 0x80)
    buffer[last++] = c;
  else {
    /* Web procedure should be called for a better error handling. */
    fprintf(stderr, "\n1-byte kana (shift-jis) ignored.\n");
    buffer[last++] = '?';
  }
}

static
int in_ln_gen(f, ki, checkkanji, makekanji)
     register FILE *f;
     register kaninfo *ki;
     int (*checkkanji)(), (*makekanji)();
{
  register int i;

  while ( last < bufsize && ((i = getc(f)) != EOF) && i != '\n') {
    if (ki->kstat == KAN_ASCII) {
      if (i == ESC) {
	if ((i = getc(f)) == '$') {
	  getc(f);
	  ki->kstat = KAN_JIS_SECOND;
	} else if (i == '(') {
	  getc(f);
	  /* ki->kstat = KAN_ASCII; */
	} else {
	  /* Web procedure should be called for a better error handling. */
	  fprintf(stderr,"\nText line contains an invalid escape sequence\n");
	  exit(1);
	}
      } else
	(*checkkanji)(i, ki);
    } else if (ki->kstat == KAN_JIS_FIRST) {
      integer c;
      if (last >= bufsize-1)
	return(i);
      c = fix_kanji_range(JIStoEUC(((integer)ki->kpend << 8) | i));
      buffer[last++] = c >> 8;
      buffer[last++] = c & 0xff;
      ki->kstat = KAN_JIS_SECOND;
    } else if (ki->kstat == KAN_JIS_SECOND) {
      if (i == ESC) {
	if ((i = getc(f)) == '$') {
	  getc(f);
	  /* ki->kstat = KAN_JIS_SECOND; */
	} else if (i == '(') {
	  getc(f);
	  ki->kstat = KAN_ASCII;
	} else {
	  /* Web procedure should be called for a better error handling. */
	  fprintf(stderr,"\nText line contains an invalid escape sequence\n");
	  exit(1);
	}
      } else if (i < ' ') {
	buffer[last++] = i;
      } else {
	ki->kpend = i;
	ki->kstat = KAN_JIS_FIRST;
      }
    } else			/*if (ki->kstat == KAN_ASCII_KAN)*/ {
      integer c;
      if (last >= bufsize-1)
	return(i);
      c = fix_kanji_range((*makekanji)(((integer)ki->kpend << 8) | i));
      buffer[last++] = c >> 8;
      buffer[last++] = c & 0xff;
      ki->kstat = KAN_ASCII;
    }
  }
  return (i);
}

static
out_ch_ascii(c, f, ki)
     textchar c;
     FILE *f;
     kaninfo *ki;
{
  putc(deTeXkanji(c), f);
}

static
out_ch_jis(c, f, ki)
     textchar c;
     FILE *f;
     register kaninfo *ki;
{
  int cj;

  if (ki->kstat == KAN_JIS_FIRST) {
    cj = EUCtoJIS(((integer)ki->kpend << 8) | c);
    putc(cj >> 8, f);
    putc(cj & 0xff, f);
    ki->kstat = KAN_JIS_SECOND;
  } else if (isTeXkanji(c)) {
    if (ki->kstat == KAN_ASCII) {
      putc(ESC, f);
      putc('$', f);
      putc(jis_esc_tab[ki->ktype].esc_in, f);
    }
    ki->kpend = c;
    ki->kstat = KAN_JIS_FIRST;
  } else {
    if (ki->kstat == KAN_JIS_SECOND) {
      putc(ESC, f);
      putc('(', f);
      putc(jis_esc_tab[ki->ktype].esc_out, f);
    }
    putc(c, f);
    ki->kstat = KAN_ASCII;
  }
}

static
out_ch_sjis(c, f, ki)
     textchar c;
     FILE *f;
     register kaninfo *ki;
{
  textchar c1, c2;
  
  if (ki->kstat == KAN_ASCII_KAN) {
    integer csj;
    csj = EUCtoSJIS(((integer)ki->kpend << 8) | c);
    putc(csj >> 8, f);
    putc(csj & 0xff, f);
    ki->kstat = KAN_ASCII;
  } else if (isTeXkanji(c)) {
    ki->kpend = c;
    ki->kstat = KAN_ASCII_KAN;
  } else
    putc(c, f);
}

static
out_ch_euc(c, f, ki)
     textchar c;
     FILE *f;
     kaninfo *ki;
{
  putc(c, f);
}

/*
 * Kanji conversion functions
 */

static integer
fix_kanji_range(c)
     integer c;
{
  return (c < 0xa100 || (c >= 0xa900 && c <= 0xafff) || c >= 0xf500) ?
    0xa2a3 : c;
}
