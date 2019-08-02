
#define tonum(X) X
#define tokanji(X) X
#define	Hi(X) ((X >> 8) & 0xff)
#define	Lo(X) (X & 0xff)
#define	PutHi(X,Y) X &= 0xff; (X |= (Y << 8))
#define	PutLo(X,Y) X &= 0xff00; (X |= (Y & 0xff))
#define getchr(X) (X & 0xff);
#define xord(X) X
#define xchr(X) X
#define getcmd(X) curcmd = ((X >> 8) & 0xff); curchr = (X & 0xff)
#define getcmdx(T, M, C) M = (T) >> 8; C = ((T) & 0xff)

extern quarterword getjfmpos();
#ifndef SJIS
extern int KUTENtoSJIS();
extern int JIStoSJIS();
extern short SJIStoJIS();
#endif
extern short calcpos();
extern boolean iskanji();
extern boolean iskanji2();
extern boolean checkkanji();
extern void settok();
extern halfword mktok();
