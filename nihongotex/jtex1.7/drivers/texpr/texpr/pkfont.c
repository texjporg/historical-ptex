/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

#include <stdio.h>
#include "../common/pkcom.h"
#include "../common/def.h"


FONT	font[];
FILE	*pk;

load_pk(fn)
{
	register FONT *t;
	register cmd, i, c;

	t = &font[fn];
#ifdef DEBUG
	printf("pkfont:%s\n", t->a);
#endif
	if((pk = fopen(t->a, "r")) == NULL)
		return(-1);
/*
		error("can't open pk file %s\n", t->a);
*/
	i = 0;
	do {
		if(fseek(pk, --i, 2) == -1)
			error("can't seek to the end of pk file %s\n", t->a);
	} while((c=get_uint(pk, 1)) == PK_NO_OP);
	fseek(pk, 0, 0);
	if(c != PK_POST || get_uint(pk,1) != PK_PRE || get_uint(pk,1) != PK_ID)
		error("bad pk file %s\n", t->a);
	fseek(pk, get_uint(pk, 1), 1);		/* comment */
	get_int(pk, 4);				/* design size */
	if((c = get_int(pk, 4)) && t->c && c != t->c)
		error("pk file %s version error\n", t->a);
	fseek(pk, 8, 1);			/* hppp,vppp */
	if((t->ch = (FONT_CH *)malloc(sizeof(FONT_CH [NCHAR]))) == NULL)
		error("can't allocate memory for font table.\n");
	
	for( ; ; ) {
		switch(cmd = get_uint(pk, 1)) {

		case PK_XXX1: case PK_XXX2: case PK_XXX3: case PK_XXX4:
			fseek(pk, get_uint(pk, cmd-PK_XXX1+1), 1);
			continue;

		case PK_YYY:
			get_int(pk,4);
			continue;

		case PK_NO_OP:
			continue;

		case PK_POST:
			break;

		default:
			if(cmd <= PK_FLAG)
				def_char(cmd, t);
			else
				error("illegal pk command %d\n", cmd);
			continue;
		}
		break;
	}
	fclose(pk);
	t->id = PK_ID;
	return(0);
}

/*	character shape definition	*/

def_char(n, t)
register FONT *t;
{
	register FONT_CH *p;
	register pl;

	if((n &= 7) == 7) {			/* long form */
		pl = get_int(pk, 4) - 28;
		p = &t->ch[get_int(pk, 4)];
		get_int(pk, 4);		/* tfmw */
		get_int(pk, 4); 	/* dx */
		n = 4;
	} else if(n >= 4) {		/* extended short form */
		pl = (n-4)*65536 + get_uint(pk, 2) - 13;
		p = &t->ch[get_uint(pk, 1)];
		get_uint(pk, 3);	/* tfmw */
		n = 2;
	} else {			/* short form */
		pl = n*256 + get_uint(pk, 1) - 8;
		p = &t->ch[get_uint(pk, 1)];
		get_uint(pk, 3);	/* tfmw */
		n = 1;
	}
	get_uint(pk, n); 		/* dy/dm */
	p->width = (get_uint(pk, n)+7)/8;
	p->height = get_uint(pk, n);
	p->xoff = get_int(pk, n);
	p->yoff = get_int(pk, n);
	p->pxl = (char *)1;		/* defined mark */
	fseek(pk, pl, 1);		/* skip shape */
}
