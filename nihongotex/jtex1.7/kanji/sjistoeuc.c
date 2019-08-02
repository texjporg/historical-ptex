#include	<stdio.h>

main()
{
	int	c;

	while((c = getchar()) != EOF){
		if(!iskanji(c)) putchar(c);
		else{
			c = toeuc((c << 8) | (getchar() & 0xff));
			putchar(c >> 8);
			putchar(c);
		}
	}
}

iskanji(c)
register int	c;
{
	c &= 0377;
	return ((c > 0x80 && c < 0xa0)||(c > 0xdf && c < 0xfd));
}

toeuc(c)
short	c;
{
	register short	byte1, byte2;

	byte1 = c >> 8;
	byte2 = c & 0xff;
	byte1 -= ( byte1>=0xa0 ) ? 0xc1 : 0x81;
	c = ((byte1 << 1) + 0x21) << 8;
	if( byte2 >= 0x9f ) {
		c += 0x0100;
		c |= (byte2 - 0x7e) & 0xff;
	} else {
		c |= (byte2 - ( (byte2<=0x7e) ? 0x1f : 0x20 )) & 0xff;
	}
	return(c | 0x8080);
}
