/*
 * Hand-coded routines for Japanes C TeX.
 *
 */

#define	EXTERN	extern
#include "texd.h"

void
settok()
{
	curtok = ((short)curcmd << 8) | (curchr & 0xff);
}


halfword
mktok(cmd, c)
short	cmd;
char	c;
{
	return(((short)cmd << 8) | (c & 0xff));
}

#ifdef BUGCOMPATIBLE

quarterword
getjfmpos(kcode, f)
KANJIcode	kcode;
short		f;
{
	register int	n, r;
	register unsigned h;
	register unsigned short jc;
	register memoryword	*dp, *sp, *ep, *mp;

#ifdef EUC
	jc = EUCtoJIS(kcode);
#else
	jc = SJIStoJIS(kcode);
#endif
	n = fontnumext[f];
	dp = &fontinfo[ctypebase[f]];
	sp = dp + 1;
	ep = dp + --n;
	n--;
	if(ep->hh.v.RH < jc){
		return(dp->hh.v.LH);
	}
	while(sp <= ep){
		if(h = n >> 1){
			mp = sp + (n & 1 ? h : (h - 1));
			if(!(r = jc - mp->hh.v.RH)){
				return(mp->hh.v.LH);
			}else{
				if(r < 0){
					ep = mp - 1;
					n = n & 1 ? h : h - 1;
				}else{
					sp = mp + 1;
					n = h;
				}
			}
		}else{
			if(jc == sp->hh.v.RH){
				return(sp->hh.v.LH);
			}
			return(dp->hh.v.LH);
		}
	}
	return(dp->hh.v.LH);
}

#else

/* getjfmpos simpler version */

quarterword getjfmpos(kcode, f)
KANJIcode kcode;
short f;
{
	register unsigned short jc;
	register memoryword *bp, *sp, *mp, *ep;
	register int r;

#ifdef EUC
	jc = EUCtoJIS(kcode);
#else
	jc = SJIStoJIS(kcode);
#endif
	bp = &fontinfo[ctypebase[f]];
	sp = bp + 1; ep = bp + fontnumext[f] - 1;
	if (jc < sp->hh.v.RH || ep->hh.v.RH < jc)
		return (bp->hh.v.LH);
	while (sp <= ep) {
		mp = sp + (ep - sp) / 2;
		if ((r = jc - mp->hh.v.RH) < 0)
			ep = mp - 1;
		else if (r > 0)
			sp = mp + 1;
		else
			return (mp->hh.v.LH);
	}
	return (bp->hh.v.LH);
}

#endif /* BUGCOMPATIBLE */
