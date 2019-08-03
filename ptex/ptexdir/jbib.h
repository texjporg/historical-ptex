#include "cpascal.h"

/* re-define aopenin, aopenout */
#undef aopenin
#undef aopenout
#define aopenin(f, p)	jbibtex_open_input (&(f), p, FOPEN_R_MODE)
#define aopenout(f)	jbibtex_open_output (&(f), FOPEN_W_MODE)

/* override open_input,open_output */
#define open_input(f,p,m) jbibtex_open_input(f,p,m)
#define open_output(f,p,m) jbibtex_open_output(f,p,m)

#undef Fputs
#define Fputs(f,s) jbibtex_Fputs(f,s)
#undef putc
#define putc(c,f) jbibtex_putc(c,f)

#define printstr(s,c)	jbibtex_printstr(s,c)

extern boolean jbibtex_open_input();
extern boolean jbibtex_open_output();
extern jbibtex_printstr();
extern jbibtex_Fputs();
extern jbibtex_putc();
extern boolean inputline();
extern initkanji();
