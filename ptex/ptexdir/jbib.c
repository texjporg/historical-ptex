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

#define KANJI
#define KANJI_C
#include "cpascal.h"
#include "jbibd.h"

#ifdef JIS
#  define DEFAULT_FILE_CODE  6     /* JIS */
#  define DEFAULT_TERM_CODE  6     /* JIS */
#else
#  ifdef SJIS
#    define DEFAULT_FILE_CODE  10     /* SJIS */
#    define DEFAULT_TERM_CODE  10     /* SJIS */
#  else /* EUC */
#    ifdef EUC
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

#include <sys/param.h>
static kaninfo kanjiinfo_table[NOFILE];

static int kanji_file_code, kanji_term_code; /* kanji code same as NTT-JTeX */
static get_kanji_code();
static set_kanji_type();

boolean jbibtex_open_input (f, path_index, fopen_mode)
    FILE **f;
    path_constant_type path_index;
    char *fopen_mode;
{
	boolean result;
	result  = open_input(f,path_index,fopen_mode);
	if (result == false) return false;
    set_kanji_type(*f, kanji_file_code);
    return result;
}

boolean jbibtex_open_output (f, fopen_mode)
    FILE **f;
    char *fopen_mode;
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
#define	makeTeXkanji(c)	((c)|KANJIBIT)
#define	deTeXkanji(c)	((c)&~KANJIBIT)

static int in_ln_gen();
static int in_ln_ascii();
static int in_ln_jis();
static int in_ln_sjis();

static int out_ch_ascii();
static int out_ch_jis();
static int out_ch_sjis();
static int out_ch_euc();

static int check_euc_kanji();
static int check_sj_kanji();
static int euc2TeXkanji();
static int sj2TeXkanji();
static jis2TeXkanji();
static TeXkanji2jis();
static TeXkanji2sj();
static fix_kanji_range();

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
    {'\0', '\0'}, /* dummy */
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
  extern char *getenv();
  extern int atoi();
  register char *p;

  kanji_file_code = DEFAULT_FILE_CODE;
  kanji_term_code = DEFAULT_TERM_CODE;

  if( p=getenv("BIBFILECODE") )
    kanji_file_code = atoi(p);
  if( (kanji_file_code<1)||(kanji_file_code>20)||
      (in_ln_tab[kanji_file_code] == 0) ) {
    fprintf(stderr,"Kanji File Code:%d is illegal. I'll use system default\n",
	    kanji_file_code);
    kanji_file_code = DEFAULT_FILE_CODE;
  }
  if( p=getenv("BIBTERMCODE") )
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
    return (in_ln_gen(f, ki, check_euc_kanji, euc2TeXkanji));
}

static
int in_ln_sjis(f, ki)
FILE *f;
kaninfo *ki;
{
    return (in_ln_gen(f, ki, check_sj_kanji, sj2TeXkanji));
}

static
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

static
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
    textchar c1, c2;

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
	    if (last >= bufsize-1)
		return(i);
	    jis2TeXkanji(ki->kpend, i, &c1, &c2);
	    buffer[last++] = c1;
	    buffer[last++] = c2;
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
	} else /*if (ki->kstat == KAN_ASCII_KAN)*/ {
	    if (last >= bufsize-1)
		return(i);
	    (*makekanji)(ki->kpend, i, &c1, &c2);
	    buffer[last++] = c1;
	    buffer[last++] = c2;
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
    textchar c1, c2;

    if (ki->kstat == KAN_JIS_FIRST) {
	TeXkanji2jis(ki->kpend, c, &c1, &c2);
	putc(c1, f); putc(c2, f);
	ki->kstat = KAN_JIS_SECOND;
    } else if (isTeXkanji(c)) {
	if (ki->kstat == KAN_ASCII) {
	    putc(ESC, f); putc('$', f);
	    putc(jis_esc_tab[ki->ktype].esc_in, f);
	}
	ki->kpend = c;
	ki->kstat = KAN_JIS_FIRST;
    } else {
	if (ki->kstat == KAN_JIS_SECOND) {
	    putc(ESC, f); putc('(', f);
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
	TeXkanji2sj(ki->kpend, c, &c1, &c2);
	putc(c1, f); putc(c2, f);
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
static
jis2TeXkanji(i1, i2, c1, c2)
textchar i1, i2, *c1, *c2;
{
    *c1 = makeTeXkanji(i1);
    *c2 = makeTeXkanji(i2);
    fix_kanji_range(c1, c2);
}

static
TeXkanji2jis(i1, i2, c1, c2)
textchar i1, i2, *c1, *c2;
{
    *c1 = deTeXkanji(i1);
    *c2 = deTeXkanji(i2);
}

static
euc2TeXkanji(i1, i2, c1, c2)
textchar i1, i2, *c1, *c2;
{
    *c1 = i1;
    *c2 = i2;
    fix_kanji_range(c1, c2);
}

static
sj2TeXkanji(i1, i2, c1, c2)
textchar i1, i2, *c1, *c2;
{
    if (i2 >= 0x9f) {
	if (i1 >= 0xe0) *c1 = i1*2 - 0xe0;
	else *c1 = i1*2 - 0x60;
	*c2 = i2 + 2;
    } else {
	if (i1 >= 0xe0) *c1 = i1*2 - 0xe1;
	else *c1 = i1*2 - 0x61;
	if (i2 >= 0x7f) *c2 = i2 + 0x60;
	else *c2 = i2 +  0x61;
    }
    fix_kanji_range(c1, c2);
}

static
TeXkanji2sj(i1, i2, c1, c2)
textchar i1, i2, *c1, *c2;
{
    if (i1 & 1) {
	if (i1 < 0xdf) *c1 = i1/2 + 0x31;
	else *c1 = i1/2 + 0x71;
	if (i2 >= 0xe0) *c2 = i2 - 0x60;
	else *c2 = i2 - 0x61;
    } else {	
	if (i1 < 0xdf) *c1 = i1/2 + 0x30;
	else  *c1 = i1/2 + 0x70;
	*c2 = i2 - 2;
    }
}

static
fix_kanji_range(c1, c2)
textchar *c1, *c2;
{
    if (*c1 < 0xa1 || (*c1 >= 0xa9 && *c1 <= 0xaf) || *c1 >= 0xf5) {
	*c1 = 0xa2;
	*c2 = 0xa3;
    }
}
