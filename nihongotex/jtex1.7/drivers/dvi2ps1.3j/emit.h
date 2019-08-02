			/* output a formatted string */
#define EMIT      fprintf
			/* output a simple string */
#define EMITS(s)  fputs(s,outfp)
			/* output an escaped octal number */
#define EMITO(c)  putoct(c)
			/* output a decimal integer */
#define EMITN(n)  putint(n)
			/* output a byte value in Hex */
#define EMITH(h)  putc(*(digit+((h>>4)&0xF)),outfp),\
		  putc(*(digit+(h&0xF)),outfp)
			/* output a single character */
#define EMITC(c)  putc(c,outfp)

extern char *digit;
