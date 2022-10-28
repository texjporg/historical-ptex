/*
  unicode-jp.c -- JIS X 0208 <=> UCS-2 converter
  written by N. Tsuchimura
*/

#include <ptexenc/c-auto.h>
#include <ptexenc/unicode-jp.h>
#include <ptexenc/kanjicnv.h>

#if defined(KANJI_ICONV) && defined(HAVE_ICONV_H)
#include <iconv.h>
#include <stdlib.h> /* for atexit() */

#define ICONV_INVALID ((iconv_t)(-1))
#define ICONV_JIS "ISO-2022-JP"
#define ICONV_UNI "UCS-2BE"

static int inited = FALSE;
static iconv_t i_jis2ucs2 = ICONV_INVALID;
static iconv_t i_ucs22jis = ICONV_INVALID;

#ifdef HAVE_ATEXIT
static void close_iconv(void)
#else /* HAVE_ATEXIT */
static void close_iconv(int dummy1, void *dummy2)
#endif /* HAVE_ATEXIT */
{
    if (i_jis2ucs2 != ICONV_INVALID) iconv_close(i_jis2ucs2);
    if (i_ucs22jis != ICONV_INVALID) iconv_close(i_ucs22jis);
}

static void open_iconv(void)
{
    inited = true;
    i_jis2ucs2 = iconv_open(ICONV_UNI, ICONV_JIS);
    i_ucs22jis = iconv_open(ICONV_JIS, ICONV_UNI);

#ifdef HAVE_ATEXIT
    atexit(close_iconv);
#endif /* HAVE_ATEXIT */
#ifdef HAVE_ON_EXIT
    on_exit(close_iconv, NULL);
#endif /* HAVE_ON_EXIT */
}

/* convert a JIS X 0208 char to UCS-2 */
static int JIStoUCS2native(int jis)
{
    unsigned char jis_seq[8] = {
	0x1b, 0x24, 0x42, /* JIS X 0208-1983 */
	(unsigned char)HI(jis), (unsigned char)LO(jis),
	0x1b, 0x28, 0x42, /* ASCII */
    };
    unsigned char unicode[2];
    size_t from = sizeof jis_seq;
    size_t to   = sizeof unicode;
    char *from_ptr = (char *)jis_seq;
    char *to_ptr   = (char *)unicode;

    if (!inited) open_iconv();
    if (i_jis2ucs2 == ICONV_INVALID) return 0;

    if (iconv(i_jis2ucs2, &from_ptr, &from, &to_ptr, &to) == (size_t)(-1)) {
	return 0;
    }
    return HILO(unicode[0], unicode[1]);
}

/* convert a UCS-2 char to JIS X 0208 */
static int UCS2toJISnative(int ucs2)
{
    unsigned char unicode[2] = {
        (unsigned char)HI(ucs2), (unsigned char)LO(ucs2),
    };
    unsigned char jis_seq[10] = { 0,0,0,0,0,0,0,0,0,0 };
    size_t from = sizeof unicode;
    size_t to   = sizeof jis_seq;
    char *from_ptr = (char *)unicode;
    char *to_ptr   = (char *)jis_seq;
    unsigned char *ret = jis_seq;

    if (!inited) open_iconv();
    if (i_ucs22jis == ICONV_INVALID) return 0;

    if (iconv(i_ucs22jis, &from_ptr, &from, &to_ptr, &to) == (size_t)(-1)) {
	return 0;
    }
    if (ret[0] == 0x1b) ret += 3;
    return HILO(ret[0], ret[1]);
}

#else /* KANJI_ICONV && HAVE_ICONV_H */

#include "jisx0208.h"
#define MAXJIS 84

/* convert a JIS X 0208 char to UCS-2 */
static int JIStoUCS2native(int jis)
{
    int hi, low;

    if (jis == 0x2121) return 0x3000; /* UnicodeTbl[0][0] bug fix */
    hi  = HI(jis) - 0x21;
    low = LO(jis) - 0x21;
    if (0 <= hi  && hi  < MAXJIS &&
        0 <= low && low < 94) return (int)UnicodeTbl[hi][low];
    return 0;
}

/* convert a UCS-2 char to JIS X 0208 */
static int UCS2toJISnative(int ucs2)
{
    int i, j;

    if (ucs2 == 0x3000) return 0x2121; /* UnicodeTbl[0][0] bug fix */
    for (i=0; i<MAXJIS; i++) {
        for (j=0; j<94; j++) {
            if (UnicodeTbl[i][j] == ucs2) {
                return HILO(i, j) + 0x2121;
            }
        }
    }
    return 0;
}
#endif /* KANJI_ICONV && HAVE_ICONV_H */


/*
  http://euc.jp/i18n/ucsnote.ja.html
  http://hp.vector.co.jp/authors/VA010341/unicode/
  http://www.jca.apc.org/~earthian/aozora/0213/jisx0213code.zip
*/
static unsigned short int variation[] = {
    /* JIS X 0208, UCS-2(1), UCS-2(2), ..., 0(sentinel) */
    0x2126 /* 1-6 */,  0x2022, 0x2219, 0x22C5, 0x30FB, 0,
    0x2131 /* 1-17 */, 0x203E, 0xFFE3, 0,
    0x213D /* 1-29 */, 0x2012, 0x2013, 0x2014, 0x2015, 0,
    0x2141 /* 1-33 */, 0x223C, 0x223E, 0x301C, 0xFF5E, 0,
    0x2142 /* 1-34 */, 0x2016, 0x2225, 0,
    0x2143 /* 1-35 */, 0x2223, 0xFF5C, 0,
    0x2144 /* 1-36 */, 0x2026, 0x22EF, 0,
    0x215D /* 1-61 */, 0x2212, 0xFF0D, 0,
    0x216f /* 1-79 */, 0x00A5, 0xFFE5, 0,
    0x2171 /* 1-81 */, 0x00A2, 0xFFE0, 0,
    0x2172 /* 1-82 */, 0x00A3, 0xFFE1, 0,
    0x224C /* 2-44 */, 0x00AC, 0xFFE2, 0,
    0x227E /* 2-94 */, 0x20DD, 0x25EF, 0,
    0 /* end */
};

/*
  voiced sound of kana
  http://developer.apple.com/technotes/tn/tn1150table.html
*/
static unsigned short int voiced_sounds[] = {
    /* from,  to */
    0x3046, 0x3094,
    0x304B, 0x304C,
    0x304D, 0x304E,
    0x304F, 0x3050,
    0x3051, 0x3052,
    0x3053, 0x3054,
    0x3055, 0x3056,
    0x3057, 0x3058,
    0x3059, 0x305A,
    0x305B, 0x305C,
    0x305D, 0x305E,
    0x305F, 0x3060,
    0x3061, 0x3062,
    0x3064, 0x3065,
    0x3066, 0x3067,
    0x3068, 0x3069,
    0x306F, 0x3070,
    0x3072, 0x3073,
    0x3075, 0x3076,
    0x3078, 0x3079,
    0x307B, 0x307C,
    0x309D, 0x309E,
    0x30A6, 0x30F4,
    0x30AB, 0x30AC,
    0x30AD, 0x30AE,
    0x30AF, 0x30B0,
    0x30B1, 0x30B2,
    0x30B3, 0x30B4,
    0x30B5, 0x30B6,
    0x30B7, 0x30B8,
    0x30B9, 0x30BA,
    0x30BB, 0x30BC,
    0x30BD, 0x30BE,
    0x30BF, 0x30C0,
    0x30C1, 0x30C2,
    0x30C4, 0x30C5,
    0x30C6, 0x30C7,
    0x30C8, 0x30C9,
    0x30CF, 0x30D0,
    0x30D2, 0x30D3,
    0x30D5, 0x30D6,
    0x30D8, 0x30D9,
    0x30DB, 0x30DC,
    0x30EF, 0x30F7,
    0x30F0, 0x30F8,
    0x30F1, 0x30F9,
    0x30F2, 0x30FA,
    0x30FD, 0x30FE,
    0 /* end mark */
};

/* semi voiced sound of kana */
static unsigned short int semi_voiced_sounds[] = {
    /* from,  to */
    0x306F, 0x3071,
    0x3072, 0x3074,
    0x3075, 0x3077,
    0x3078, 0x307A,
    0x307B, 0x307D,
    0x30CF, 0x30D1,
    0x30D2, 0x30D4,
    0x30D5, 0x30D7,
    0x30D8, 0x30DA,
    0x30DB, 0x30DD,
    0 /* end mark */
};

/* convert a JIS X 0208 char to UCS-2 */
int JIStoUCS2(int jis)
{
    return JIStoUCS2native(jis);
}

/* convert a UCS-2 char to JIS X 0208 */
int UCS2toJIS(int ucs2)
{
    int i, j;

    /* first: variation table */
    for (i=0; variation[i]!=0; i=j+1) {
        for (j=i+1; variation[j]!=0; j++) {
            if (variation[j] == ucs2) return variation[i];
        }
    }

    /* second: UnicodeTbl[][] */
    return UCS2toJISnative(ucs2);
}


/* for U+3099 or U+309A */
int get_voiced_sound(int ucs2, boolean semi)
{
    int i;
    unsigned short int *table;

    if (semi) table = semi_voiced_sounds;
    else      table = voiced_sounds;
    for (i=0; table[i]!=0; i+=2) {
        if (ucs2 == table[i]) return table[i+1];
    }
    return 0;
}

#ifdef TEST
#include <stdio.h>
int main(int argc, char **argv) {
    int hi, low;
    for (hi=0; hi<MAXJIS; hi++) {
        for (low=0; low<94; low++) {
	    int jis = HILO(hi, low) + 0x2121;
            int uni = JIStoUCS2(jis);
            int jis2 = UCS2toJIS(uni);
            if (uni != 0 && jis != jis2) {
                printf("0x%x(%c%c) uni=0x%04x, jis2=0x%04x\n",
                       jis, HI(jis)|0x80, LO(jis)|0x80, uni, jis2);
            }
        }
    }
    return 0;
}
#endif /* TEST */