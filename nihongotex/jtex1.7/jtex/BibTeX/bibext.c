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

#define DEFAULT_FILE_CODE  20     /* EUC; site dependent */
#define DEFAULT_TERM_CODE  20     /* EUC; site dependent */

#define EXTERN
#include <stdio.h>
#include "texd.h"
#ifndef	BSD
#include <string.h>
#else
#include <strings.h>
#endif


#define TRUE 1
#define FALSE 0
#define filenamesize 1024

extern char realnameoffile[];

#ifdef KANJI
#include "kanji.h"
extern char nameoffile[];
static int kanji_file_code, kanji_term_code; /* kanji code same as NTT-JTeX */
#define	MAXOPEN		64
static kaninfo kanjiinfo[MAXOPEN];
#else
extern char xord[], buffer[], nameoffile[];
#endif

char **gargv;
int gargc;

static char input_path[filenamesize];
static char bib_path[filenamesize];

FILE *openf(name, mode)
char *name, *mode;
{
    FILE *result;

    result = fopen(name, mode);
    if (!result) {
	perror(name);
	exit(1);
    }
#ifdef KANJI
    set_kanji_type(result, kanji_file_code);
#endif
    return(result);
}

int eoln(f)
FILE *f;
{
    register int c;

    if (feof(f)) return(1);
    c = getc(f);
    if (c != EOF)
	(void) ungetc(c, f);
    return (c == '\n' || c == EOF);
}

void lineread(f, size)
FILE *f;
int size;
{
    extern long last;
    int in;

#ifdef KANJI
    register kaninfo *ki;
#endif

    last = 0;
#ifdef KANJI
    ki = &kanjiinfo[fileno(f)];
    in = (*(ki->in_ln))(f,ki);
#else
    while (last < size && (in = getc(f)) != EOF && in != '\n') {
#ifdef	NONASCII
	buffer[last++] = xord[in];
#else
	buffer[last++] = in;
#endif
    }
#endif
    while (in != EOF && in != '\n')	/* Skip past eoln if buffer full */
	(void) getc(f);
}

void setpaths()
{
    extern char *getenv();
    char *p;

    if (p = getenv("TEXINPUTS"))
	(void) strcpy(input_path, p);
    else
	(void) strcpy(input_path, TEXINPUTS);

    if (p = getenv("BIBINPUTS"))
	(void) strcpy(bib_path, p);
    else
	(void) strcpy(bib_path, BIBINPUTS);
}


int testaccess(wam, filepath)
int wam, filepath;
{
    char *path = NULL;
    register int ok;
    int f;

    if (filepath == 1)
	path = input_path;
    if (filepath == 2)
        path = bib_path;
    if (nameoffile[0] == '/')
	path = NULL;
    do {
	packrealnameoffile(&path);
	if (wam == 4)
	    if (access(realnameoffile, 4) == 0) ok = TRUE;
	    else ok = FALSE;
        else {
	    f = creat(realnameoffile, 0666);
	    if (f >= 0) ok = TRUE;
	    else ok = FALSE;
	    if (ok)
		(void) close(f);
	}
    }
    while (!ok && path != NULL);
/*
    if (ok) {
	for (p=realnameoffile; *p; p++);
	while (p < &(realnameoffile[filenamesize]))
	    *p++ = ' ';
    }
*/
    return(ok);
}

static packrealnameoffile(cpp)
char **cpp;
{
    register char *p, *realname;

    realname = realnameoffile;
    if ((p = *cpp)) {
	while ((*p != ':') && *p) {
	    *realname++ = *p++;
	    if (realname == &(realnameoffile[filenamesize-1]))
		break;
	}
	if (*p == '\0') *cpp = NULL;
	else *cpp = p+1;
	*realname++ = '/';
    }
    p = nameoffile;
    while (*p != ' ') {
	if (realname >= &(realnameoffile[filenamesize-1])) {
	    fprintf(stderr, "! Full file name is too long\n");
	    break;
	}
	*realname++ = *p++;
    }
    *realname = '\0';
}

int
getcmdline(buf, n)
char *buf;
{
	strncpy(buf, gargv[1], n);
	return (strlen(buf));
}

main(argc, argv)
char *argv[];
{
    extern schar history;
    int status;

    lab31=0;
    gargc = argc;
    gargv = argv;
#ifdef KANJI
    (void) get_kanji_io_code();
#endif
    setpaths();
    main_body();
    status = (history != 0);
    exit(status);
}

#ifdef KANJI


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

#define TEMP_BUFF_SIZE 4096           /* maybe too long */
static unsigned char temp_buff[TEMP_BUFF_SIZE];

get_kanji_io_code()
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
#ifdef DEBUG
  fprintf(stderr,"kanji_file_code = %d\n",kanji_file_code);
  fprintf(stderr,"kanji_term_code = %d\n",kanji_term_code);
#endif
}

void PRINTSTR(f, s, c)
FILE *f;
char *s, c;
{
  (void) sprintf(temp_buff, "%s%c", s, c);
  (void) Fputs(temp_buff, f);
}

set_kanji_type(f, ktype)
FILE *f;
int ktype;
{
    kanjiinfo[fileno(f)].ktype = ktype;
    kanjiinfo[fileno(f)].kstat = KAN_ASCII;
    kanjiinfo[fileno(f)].in_ln = in_ln_tab[ktype];
    kanjiinfo[fileno(f)].out_ch = out_ch_tab[ktype];
}

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

int in_ln_jis(f, ki)
FILE *f;
kaninfo *ki;
{
    return (in_ln_gen(f, ki, check_euc_kanji, euc2TeXkanji));
}

int in_ln_sjis(f, ki)
FILE *f;
kaninfo *ki;
{
    return (in_ln_gen(f, ki, check_sj_kanji, sj2TeXkanji));
}

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

Fputc(c, f)
textchar c;
FILE *f;
{
    register kaninfo *ki;

    ki = &kanjiinfo[fileno(f)];
    (*(ki->out_ch))(c, f, ki);
}

Fputs(f, s)
register FILE *f;
register textchar *s;
{
    register kaninfo *ki;

    ki = &kanjiinfo[fileno(f)];
    for (; *s; s++)
	(*(ki->out_ch))(*s, f, ki);
}

out_ch_ascii(c, f, ki)
textchar c;
FILE *f;
kaninfo *ki;
{
    putc(deTeXkanji(c), f);
}

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
jis2TeXkanji(i1, i2, c1, c2)
textchar i1, i2, *c1, *c2;
{
    *c1 = makeTeXkanji(i1);
    *c2 = makeTeXkanji(i2);
    fix_kanji_range(c1, c2);
}

TeXkanji2jis(i1, i2, c1, c2)
textchar i1, i2, *c1, *c2;
{
    *c1 = deTeXkanji(i1);
    *c2 = deTeXkanji(i2);
}

euc2TeXkanji(i1, i2, c1, c2)
textchar i1, i2, *c1, *c2;
{
    *c1 = i1;
    *c2 = i2;
    fix_kanji_range(c1, c2);
}

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

fix_kanji_range(c1, c2)
textchar *c1, *c2;
{
    if (*c1 < 0xa1 || (*c1 >= 0xa9 && *c1 <= 0xaf) || *c1 >= 0xf5) {
	*c1 = 0xa2;
	*c2 = 0xa3;
    }
}

#endif

