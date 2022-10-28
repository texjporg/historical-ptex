/*
 *  KANJI Code conversion routines.
 */

#define KANJI_C
#include "kanji.h"

boolean iskanji1(c)
  unsigned char c;
{
    c &= 0xff;
	if (prockanjicode == SJIS)
      return((c>=0x81 && c<=0x9f) || (c>=0xe0 && c<=0xfc));
	else
      return(c>=0xa1 && c<=0xfe);
}

boolean iskanji2(c)
  unsigned char c;
{
    c &= 0xff;
	if (prockanjicode == SJIS)
      return(c>=0x40 && c<=0xfc && c!=0x7f);
	else
      return(c>=0xa1 && c<=0xfe);
}

boolean checkkanji(c)
  integer c;
{
    if(c<0 || c>256)
        return(iskanji1(c>>8) && iskanji2(c & 0xff));
    return(-1);
}

#ifdef OLDSTYLE
integer calcpos(c)
{
    register int c1, c2;

    if(c<256) return(c<<1);
    c1 = c>>8;
    c2 = c & 0xff;
    if(c1) {
		if (prockanjicode == SJIS)
			return((c2+(c2<<(c1-0x81)) & 0xff)<<1);
		else
			return((c2+(c2<<(c1-0xa1)) & 0xff)<<1);
    } else
        return(((c2+c2+1) & 0xff)<<1);
}
#else /* OLDSTYLE */
integer calcpos(c)
    integer c;
{
    register unsigned char c1, c2;
	integer ret;

    if(c>=0 && c<=255) return(c);
    c1 = (c >> 8) & 0xff;
    c2 = c & 0xff;
    if(iskanji1(c1)) {
		if (prockanjicode == SJIS) {
			c1 = ((c1 - 0x81) % 4) * 64;  /* c1 = 0, 64, 128, 192 */
			c2 = c2 % 64;                 /* c2 = 0..63 */
		} else {
			c1 = ((c1 - 0xa1) % 4) * 64;  /* c1 = 0, 64, 128, 192 */
			c2 = c2 % 64;                 /* c2 = 0..63 */
		}
		return(c1 + c2);              /* ret = 0..255 */
    } else
        return(c2);
}
#endif /* OLDSTYLE */

/*
 *  EUC to JIS X0208 code conversion
 */
integer EUCtoJIS(kcode)
  integer kcode;
{
    return(kcode & 0x7f7f);
}

/*
 *  JIS X0208 to EUC code conversion
 */
integer JIStoEUC(kcode)
  integer kcode;
{
    return(kcode | 0x8080);
}

/*
 *  SJIStoJIS : Shift JIS to JIS Kanji code conversion
 */
integer SJIStoJIS(kcode)
  integer kcode;
{
    register short  byte1, byte2;

    byte1 = (kcode>>8) & 0xff;
    byte2 = kcode & 0xff;
    byte1 -= ( byte1>=0xa0 ) ? 0xc1 : 0x81;
    kcode = ((byte1<<1) + 0x21)<<8;
    if( byte2>=0x9f ) {
        kcode += 0x0100;
        kcode |= (byte2 - 0x7e) & 0xff;
    } else {
        kcode |= (byte2 - ((byte2<=0x7e) ? 0x1f : 0x20 )) & 0xff;
    }
    return(kcode);
}

/*
 *  JIS X0208 to Shift JIS code conversion
 */
integer JIStoSJIS(kcode)
  integer kcode;
{
    register integer high, low;
    register integer   nh,  nl;

    high = (kcode>>8) & 0xff;
    low = kcode & 0xff;
    nh = ((high-0x21)>>1) + 0x81;
    if (nh>0x9f) nh += 0x40;
    if (high & 1) {
        nl = low + 0x1f;
        if (low>0x5f) nl++;
    } else
        nl = low + 0x7e;
    if(iskanji1(nh) && iskanji2(nl))
        return((nh<<8) | nl);
    else
        return(0x813f);
}

/*
 *  Shift JIS to EUC Kanji code conversion
 */
integer SJIStoEUC(kcode)
  integer kcode;
{
    return(SJIStoJIS(kcode) | 0x8080 );
}

/*
 * EUC to SJIS Kanji code conversion
 */
integer EUCtoSJIS(kcode)
  integer kcode;
{
    return(JIStoSJIS(kcode & 0x7f7f));
}

/*
 *  KUTEN to JIS kanji code conversion
 */
integer kuten2jis(kcode)
  integer kcode;
{
    register short  byte1, byte2;

    byte1 = (kcode>>8) & 0xff;
    byte2 = kcode & 0xff;

    /* in case of undefined in kuten code table */
    if (byte1 == 0 || byte1 > 95 || byte2 == 0 || byte2 > 95)
        return(-1);

    byte1 += 0x20;
    byte2 += 0x20;

    return (byte1<<8 | byte2);
}

/*
 *  KUTEN to EUC Kanji code conversion
 */
integer KUTENtoEUC(kcode)
  integer kcode;
{
    return(JIStoEUC(kuten2jis(kcode)));
}

/*
 *  KUTENtoSJIS Kanji code conversion
 */
integer KUTENtoSJIS(kcode)
  integer kcode;
{
    return(JIStoSJIS(kuten2jis(kcode)));
}

void putc2(c, fp)
  unsigned char c;
  FILE *fp;
{
	static integer kanji[NOFILE];
	static unsigned char c1[NOFILE];
	integer jc;
	register fd;

	fd = fileno(fp);
	if (kanji[fd] == 1) {
		jc = (c1[fd] << 8) | c;
		if (prockanjicode == JIS) jc = EUCtoJIS(jc);
		(void) putc(jc >> 8, fp);
		(void) putc(jc & 0xff, fp);
		kanji[fd] = 2;
	} else if (iskanji1(c)) {
		if (kanji[fd] == 0) {
			if (prockanjicode == JIS) {
				(void) putc('\033', fp);
				(void) putc('$', fp);
				(void) putc('B', fp);
			}
		}
		c1[fd] = c;
		kanji[fd] = 1;
	} else {
		if (kanji[fd] == 2) {
			if (prockanjicode == JIS) {
				(void) putc('\033', fp);
				(void) putc('(', fp);
				(void) putc('B', fp);
			}
			kanji[fd] = 0;
		}
		(void) putc(c, fp);
	}
}
