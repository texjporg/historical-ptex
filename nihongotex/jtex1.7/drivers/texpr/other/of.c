/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

#include <signal.h>
#include <stdio.h>

int	lineno ;
int	width = 118;
int 	maxline = 58;

main(argc,argv)
int argc ;
char *argv[] ;
{
	register int i ;

	init();
	lineno = 0;
	while((i = getchar()) != EOF) {
		if(i == '\031') {
			if((i = getchar()) == '\1') {
				fflush(stdout);
				kill(getpid(), SIGSTOP);
				init();
				continue;
			}
			ungetc(i, stdin);
			i = '\031';
		}
		putchar(i);
		if (i == '\n' && ++lineno >= maxline)
			putchar(i='\f');
		if(i == '\f')
			lineno = 0;
	}
	if (lineno)
		putchar('\f');
	exit(0) ;
}

init()	/* initialize LBP */
{
	register i;

	printf("\033<");		/* soft reset */
	printf("\033[1p");		/* landscape */
	printf("\033(J\033[72 C");	/* G0 = JIS roman LP15 */
	printf("\033[90 G");		/* 8 lpi, 15 cpi */
	printf("\033[4d\033[2t");	/* top margin = 4 */
	printf("\033[30`\033[t");	/* left margin = 30 */
	for(i=1; i<=width; i+=8)
	    printf("\033[8a\033H");	/* set horizontal tab */
	putchar('\r');			/* return to origin */
}

