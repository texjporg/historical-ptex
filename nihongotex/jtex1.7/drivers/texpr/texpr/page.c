/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

#include <stdio.h>
#include "../common/dvicom.h"
#include "../common/def.h"

int	beg_page, end_page, eo_flag;
int	vflag, rflag;
FONT	font[];
FILE	*dvi;

struct	chain	{
	int	sadr;
	int	eadr;
	int	page;
	struct	chain	*next;
}	*chain	= { NULL };

/*	make page chain		*/

chain_page(adr)
register adr;
{
	register struct chain *p, *q, *r;
	register i, n;

	fseek(dvi,adr+1,0);
	for( ; (i=get_int(dvi,4)) != -1; adr=i) {
		fseek(dvi, i+1, 0);
		n = get_int(dvi,4);		/* get \count0 */
		if(n >= beg_page && n <= end_page &&
		    (eo_flag == 0 || (n & 1) == (eo_flag & 1)) ) {
	        	r = (struct chain *)malloc(sizeof(struct chain));
			if(r == NULL)
				error("can't allocate memory for chain.\n");
			r->sadr = i;
			r->eadr = adr;
			r->page = n;
			r->next = chain;
			chain = r;
		}
		fseek(dvi, i+41, 0);
	}
	if(rflag) {	/*  reverse  */
		for(p=chain,r=NULL; p; ) {
			q = p->next;
			p->next = r;
			r = p;
			p = q;
		}
		chain = r;
	}
}

/*	copy pages	*/

copy_pages(adr)
register int	*adr;
{
	register struct chain *r;
	int	bop_adr, col;
	char	buf[16];

	bop_adr = -1; col = 0;
	for(r=chain; r; r=r->next) {
		if(vflag) {
			sprintf(buf, "[%d] ", r->page);
			fputs(buf, stderr);
			if((col += strlen(buf)) >= 72) {
				putc('\n', stderr);
				col = 0;
			}
		}
		fseek(dvi, r->sadr, 0);
		copy_a_page(bop_adr);		/* each page */
		while(ftell(dvi) < r->eadr)
			putchar(getc(dvi));	/* nop,xxx,fnt_def can appear */
		bop_adr = *adr;
		*adr += r->eadr - r->sadr;
	}
	if(vflag && col)
		putc('\n', stderr);
	return	bop_adr;
}

/*	copy one page	*/

copy_a_page(bop_adr)
{
	register cmd, f;

	for( ; ; ) {
		switch(cmd=put_int(get_uint(dvi,1),1)) {

		case BOP:
			fcopy(40);
			put_int(bop_adr, 4);
			fseek(dvi, 4, 1);
			continue;

		case EOP:
			break;

		case SET1: case SET2: case SET3: case SET4:
			fcopy(cmd-SET1+1);
			font[f].down++;
			continue;

		case PUT1: case PUT2: case PUT3: case PUT4:
			fcopy(cmd-PUT1+1);
			font[f].down++;
			continue;

		case SET_RULE: case PUT_RULE:
			fcopy(8);
			continue;

		case RIGHT1: case RIGHT2: case RIGHT3: case RIGHT4:
			fcopy(cmd-RIGHT1+1);
			continue;

		case DOWN1: case DOWN2: case DOWN3: case DOWN4:
			fcopy(cmd-DOWN1+1);
			continue;

		case W0: case W1: case W2: case W3: case W4:
			fcopy(cmd-W0);
			continue;

		case X0: case X1: case X2: case X3: case X4:
			fcopy(cmd-X0);
			continue;

		case Y0: case Y1: case Y2: case Y3: case Y4:
			fcopy(cmd-Y0);
			continue;

		case Z0: case Z1: case Z2: case Z3: case Z4:
			fcopy(cmd-Z0);
			continue;

		case FNT1: case FNT2: case FNT3: case FNT4:
			f = put_int(get_uint(dvi, cmd-FNT1+1), cmd-FNT1+1);
			continue;

		case FNT_DEF1: case FNT_DEF2: case FNT_DEF3: case FNT_DEF4:
			fcopy(cmd-FNT_DEF1+13);
			fcopy(put_int(get_uint(dvi,1),1)+
			      put_int(get_uint(dvi,1),1));
			continue;

		case XXX1: case XXX2: case XXX3: case XXX4:
			fcopy(put_int(get_uint(dvi,cmd-XXX1+1),cmd-XXX1+1));
			continue;

		case PUSH: case POP: case NOP:
			continue;

		default:	
			if(cmd <= SET_CHAR_127) {
				font[f].down++;
			} else if(cmd >= FNT_NUM_0 && cmd <= FNT_NUM_63)
				f = cmd-FNT_NUM_0;
			else
				error("illegal dvi command %d.\n", cmd);
			continue;
		}
		break;
	}
}
