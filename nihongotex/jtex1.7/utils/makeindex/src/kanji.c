/*	Copyright (C) ASCII Corporation, 1985	*/
#ifdef KANJI

iskanji(c)
register short	c;
{
	c &= 0377;
#ifdef SJIS
	return ((c > 0x80 && c < 0xa0)||(c > 0xdf && c < 0xfd));
#endif

#ifdef EUC
	return(c >= 0xa1 && c <= 0xfe);
#endif
}

#endif

#define MASK	0xff

int jstrcmp(s1, s2)
char *s1, *s2;
{
	register unsigned short c1, c2;

	for (;;) {
		c1 = *s1++&MASK;
		if (iskanji(c1))
			c1 = (*s1 == '\0')? 0: (c1<<8)|(*s1++&MASK);
		c2 = *s2++&MASK;
		if (iskanji(c2))
			c2 = (*s2 == '\0')? 0: (c2<<8)|(*s2++&MASK);
		if (c1 != c2)
			break;
		if (c1 == 0)
			return(0);
	}
	return((c1 < c2)? -1: 1);
}
