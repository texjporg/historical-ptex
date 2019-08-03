/*
**	Code conversion routines
*/

#define EXTERN extern
#define KANJI_C
#include "config.h"

boolean
iskanji(c)
{
	c &= 0377;
#	ifdef EUC
		return(c>=0xa1 && c<=0xfe);
#	else
		return ((c>=0x81 && c<=0x9f) || (c>=0xe0 && c<=0xfc));
#	endif
}

boolean
iskanji2(c)
{
	c &= 0377;
#	ifdef EUC
		return(c>=0xa1 && c<=0xfe);
#	else
		return (c>=0x40 && c<=0xfc && c!=0x7f);
#	endif
}

boolean
iskana(c)
{
	c &= 0377;
	return(c>=0xa1 && c<=0xdf);
}

boolean
checkkanji(c)
{
	if(c<0 || c>256)
		return(iskanji(c>>8) && iskanji2(c & 0xff));
	return(-1);
}

integer
calcpos(c)
{
	register int c1, c2;

	if(c<256) return(c<<1);
	c1=c>>8;
	c2=c & 0xff;
	if(c1)
#		ifdef EUC
			return((c2+(c2<<(c1-0xa1)) & 0xff)<<1);
#		else
			return((c2+(c2<<(c1-0x81)) & 0xff)<<1);
#		endif
	else
		return(((c2+c2+1) & 0xff)<<1);
}



/* #ifdef EUC */
/*
 *	EUC to JIS X0208 code conversion
 */
integer
EUCtoJIS(c)
{
	return(c & 0x7f7f);
}

/*
 *	JIS X0208 to EUC code conversion
 */
integer
JIStoEUC(c)
{
	return(c | 0x8080);
}

/*
 *	Shift JIS to EUC Kanji code conversion
 */
integer
SJIStoEUC(kcode)
{
	register k1;

	kcode -= (kcode<0xe000 ? 0x811f : 0xc11f);
	k1 = ((kcode<<1) & 0xff00) + 0xa100;
	kcode &= 0xff;
	kcode += (kcode<0x80 ? kcode<0x61 ? 0x80 : 0x7f : 0x21);
	return(k1 + kcode);
}

/*
 *	KUTEN to EUC Kanji code conversion
 */
integer
KUTENtoEUC(kcode)
{
	if(!((kcode>>8)>=1 && (kcode>>8)<=94 &&
		(kcode&255)>=1 && (kcode&255)<=94))
			return(-1);
	return(kcode + 0xa0a0);
}

/* #endif /* EUC */
/* #ifdef SJIS */
/*
 *	SJIStoJIS : Shift JIS to JIS Kanji code conversion
 */
integer
SJIStoJIS(kcode)
{
	register short	byte1, byte2;

	byte1 = kcode>>8;
	byte2 = kcode & 0xff;
	byte1 -= ( byte1>=0xa0 ) ? 0xc1 : 0x81;
	kcode = ((byte1<<1) + 0x21)<<8;
	if( byte2>=0x9f ) {
		kcode += 0x0100;
		kcode |= (byte2 - 0x7e) & 0xff;
	} else {
		kcode |= (byte2 - ( (byte2<=0x7e) ? 0x1f : 0x20 )) & 0xff;
	}
	return(kcode);
}

/*
**	JIS X0208 to Shift JIS code conversion
*/
integer
JIStoSJIS(c)
{
	register int high, low;
	register int	nh,nl;

	high = (c>>8) & 0xff;
	low = c & 0xff;
	nh = ((high-0x21)>>1) + 0x81;
	if (nh>0x9f)
		nh += 0x40;
	if (high & 1) {
		nl = low + 0x1f;
		if (low>0x5f)
			nl++;
	}
	else
		nl = low + 0x7e;
	if(iskanji(nh) && iskanji2(nl))
		return((nh<<8) | nl);
	else
		return(0x813f);
}

/*
 * EUC to SJIS Kanji code conversion
 */
integer
EUCtoSJIS(kcode)
{
	return(JIStoSJIS(kcode & 0x7f7f));
}

integer
kuten2jis(incode)
{
	register unsigned short	byte_1, byte_2;

	byte_1 = incode>>8;
	byte_2 = incode&0x00ff;

/* in case of undefined in kuten code table */
	if (byte_1 == 0 || byte_1>95 ||
	    byte_2 == 0 || byte_2>95)
		return(-1);

	byte_1 += 0x0020;
	byte_2 += 0x0020;

	return ((unsigned short)(byte_1<<8|byte_2));
}

/*
 *	KUTENtoSJIS Kanji code conversion
 */
integer
KUTENtoSJIS(kcode)
{
	return(JIStoSJIS(kuten2jis(kcode)));
}
/* #endif /* SJIS */

#include <sys/param.h>
void
putc2(c, fp)
unsigned char c;
FILE *fp;
{
      static int kanji[NOFILE];
      static unsigned char c1[NOFILE];
      integer jc;
      register fd;

      fd = fileno(fp);

      if (kanji[fd] == 1) {
              jc = (c1[fd] << 8) | c;
#ifdef EUC
              jc = EUCtoJIS(jc);
#else
              jc = SJIStoJIS(jc);
#endif
              (void) putc(jc >> 8, fp);
              (void) putc(jc & 0xff, fp);
              kanji[fd] = 2;
      } else if (iskanji(c)) {
              if (kanji[fd] == 0) {
                      (void) putc('\033', fp);
                      (void) putc('$', fp);
                      (void) putc('B', fp);
              }
              c1[fd] = c;
              kanji[fd] = 1;
      } else {
              if (kanji[fd] == 2) {
                      (void) putc('\033', fp);
                      (void) putc('(', fp);
                      (void) putc('B', fp);
                      kanji[fd] = 0;
              }
              (void) putc(c, fp);
      }
}
