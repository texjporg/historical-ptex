/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

/*
 *		texpr
 *	(front end of TeX print)
 */
#define	 VERSION  "-- This is texpr v1.2 --\n"
#include <stdio.h>
#include <sys/file.h>
#include <strings.h>
#include "../common/dvicom.h"
#include "../common/def.h"

#ifdef	DEBUG
int	vflag = {1};
#else
int	vflag = {0};
#endif

int	beg_page = {-9999}, end_page = {9999}, eo_flag = {0}, rflag = {0};
int	t_margin = {0}, l_margin = {0}, copy = {1}, dflag = {0}, hflag = {0};
float	mag = {0.0};
FONT	font[MAX_FONT];
FILE	*dvi;
char	dvi_file[80];
char	*myname;


main(argc, argv)
int	argc;
char	*argv[];
{
	register i;
	int	 a, b, c;
	char	*p, *q, *get_str();
	float	ratio();
	int	adr, bop_adr, post_adr, post_post_adr;

	options(argc, argv);
	if(vflag) fprintf(stderr, VERSION);

	/* dvi file format check */

	p = rindex(dvi_file, '.');
	q = rindex(dvi_file, '/');
	if(p == NULL || q && p < q)
		strcat(dvi_file, ".dvi");
	if((dvi = fopen(dvi_file, "r")) == NULL)
		error("can't open dvi file %s.\n", dvi_file);
	if(get_uint(dvi, 1) != PRE || get_uint(dvi, 1) != DVI_ID)
		error("bad dvi file.\n");
	i = 0;
	do	fseek(dvi, --i, 2);
	while ((c=get_uint(dvi, 1)) == DVI_PAD);
	fseek(dvi, i-5, 2);
	post_post_adr = ftell(dvi);
	if(get_uint(dvi, 1) != POST_POST || c != DVI_ID)
		error("bad dvi file.\n");

	/* postamble */

	post_adr = get_int(dvi, 4);
	fseek(dvi, post_adr+13, 0);
	i = get_int(dvi, 4);
	if(!mag)  mag = ratio(i, 1000);
	if(vflag) fprintf(stderr, "magnification=%d\n", fix(mag*1000.0));
	fseek(dvi, 8, 1);
	if(get_uint(dvi, 2) > MAX_STACK)
		error("stack overflow.\n");
	get_uint(dvi, 2);

	/* font definition */

	while((c=get_uint(dvi, 1)) >= FNT_DEF1 && c <= FNT_DEF4) {
		i = get_uint(dvi, c-FNT_DEF1+1);/* font # */
		font[i].c = get_int(dvi, 4);	/* check sum */
		a = get_int(dvi, 4);		/* at size */
		b = get_int(dvi, 4);		/* design size */
		font[i].d = b;
		font[i].fmag = ratio(a, b);	/* font mag */
		a = get_uint(dvi, 1);
		b = get_uint(dvi, 1);
		font[i].a = get_str(dvi, a);	/* area (directory) name */
		font[i].n = get_str(dvi, b);	/* font name */
		font[i].down = 0;
		real_font(i, 1);		/* make real font name */
		if(load_pk(i)){
			real_font(i, 0);	/* make real font name */
			load_pixel(i);
		}
	}

	/* make page chain */

	chain_page(post_adr);

	/* start copying */

#ifndef	DEBUG
	spool();
#endif
	fseek(dvi, 0, 0);
	fcopy(10);				/* pre */
	get_int(dvi, 4);
	put_int(fix(mag*1000.0), 4);
	fcopy(i=put_int(get_int(dvi, 1), 1));
	adr = 15 + i;

	bop_adr = copy_pages(&adr);

	put_int(POST, 1);			/* post */
	put_int(bop_adr, 4);
	fseek(dvi, post_adr+5, 0);
	fcopy(8);
	put_int(fix(mag*1000.0), 4);

	put_int(0x80+hflag, 1);			/* hacked mark */
	put_int(copy, 1);
	if((i=font_search()) < 0)
		dflag = -1;
	put_int(dflag, 1);
	put_int(i, 1);
	put_int(t_margin, 2);
	put_int(l_margin, 2);
	fseek(dvi, 12, 1);

	fcopy((post_post_adr+1)-(post_adr+25)); /* fnt_def, post_post */
	put_int(adr, 4);
	put_int(DVI_ID, 1);
	adr += post_post_adr - post_adr + 6;
	for(i=(-adr & 3)+4; --i >= 0; )
		putchar(DVI_PAD);
#ifndef	DEBUG
	fclose(stdout);
	wait(0);
#endif
}


/*	extract options		*/


options(argc, argv)
register int	argc;
register char	*argv[];
{
	register tmp;
	char	*p;

	myname = *argv;
	while(--argc >= 2 && (*++argv)[0] == '-') {
		switch((*argv)[1]) {
		case 'p':
			p = *++argv;
			--argc;
			beg_page = atoi(&p);
			if(*p++ == ',' && *p) {
				if(*p != ',')
					end_page = atoi(&p);
				if(*p++ == ',' && *p) {
					if(*p == '1')	   eo_flag = 1;
					else if(*p == '2') eo_flag = 2;
				}
			}	/* do more strict syntax check, if you want. */
			if(beg_page > end_page) {
				tmp = beg_page;  /* for roman numeral page */
				beg_page = end_page;
				end_page = tmp;
			}
			continue;
		case 'r':
			rflag = 1;
			continue;
		case 't':
			t_margin = atoi(++argv);
			--argc;
			continue;
		case 'l':
			l_margin = atoi(++argv);
			--argc;
			continue;
		case 'c':
			copy = atoi(++argv);
			--argc;
			continue;
		case 'm':
			switch((*argv)[2]) {
			case '0': mag = 1.0;	   break;
			case 'h': mag = 1.0954451; break;
			case '1': mag = 1.2;	   break;
			case '2': mag = 1.44;	   break;
			case '3': mag = 1.728;	   break;
			case '4': mag = 2.0736;	   break;
			case '5': mag = 2.48832;   break;
			}
			continue;
		case 'd':
			dflag = 1;
			continue;
		case 'v':
			vflag = 1;
			continue;
		case 'h':
#if DPI == 240			/* AJ2 model */
			hflag = 1;
#endif
			continue;
		}
		break;
	}
	if(argc != 1) {
		fprintf(stderr, "usage: %s [-prtlcmdvh] <dvi-file>\n", myname);
		exit(1);
	}
	strcpy(dvi_file, *++argv);
}


error(s, a1, a2, a3, a4)
char	*s;
{
	fprintf(stderr, "%s: ", myname);
	fprintf(stderr, s, a1, a2, a3, a4);
	exit(1);
}
