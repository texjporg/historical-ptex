/* kanji type */
#define	ascii_only	0
#define	jis_1		1	/* The third byte for enter kanji is at sign
				   and that for exit kanji is J */
#define	jis_2		2	/* at sign and H */
#define	jis_3		3	/* at sign and B */
#define	jis_4		4	/* B and J */
#define	jis_5		5	/* B and H */
#define	jis_6		6	/* B and B */
#define	shift_jis	10	/* shift jis code */
#define	EUC		20	/* Extended Unix Code,
				   eighth bit is on for both bytes */
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

typedef	unsigned char textchar;

typedef struct {
    char ktype, kstat;
    textchar kpend;
    int (*in_ln)();
    int (*out_ch)();
} kaninfo;

int in_ln_ascii();
int in_ln_jis();
int in_ln_sjis();

int out_ch_ascii();
int out_ch_jis();
int out_ch_sjis();
int out_ch_euc();

int check_euc_kanji();
int check_sj_kanji();
int euc2TeXkanji();
int sj2TeXkanji();
