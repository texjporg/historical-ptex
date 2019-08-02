/*
 *	Font Loader for jxdvi/dvi2nwp
 *	$Header: font_load.c,v 1.1 88/02/06 11:29:11 atsuo Exp $
 *		written by Atsuo, K.
 *		on 87/01/12
 */

#include "atsuo-copyright.h"  

#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include "font.h"
#include "font_load.h"

#define	LOAD_DEF	"fldtab"	/* font load table */

extern char *calloc();

int preFontNum = (-1);

extern long numerator, denominator, magnification;

#define SkipComment(f,sz,b) {	\
	do {						\
		if (feof((f))) {		\
			eof = 1;			\
			break;				\
		}						\
		fgets((b),(sz),(f));	\
	} while ( (b)[0] == '#' );	\
}

FILE *
setfldent(file,num,deno,mag)
char	*file;
long	*num, *deno, *mag;	/* numerator, denominator, magnification */
{
	FILE	*f;
	int		eof=0;
	char	buff[128], buff2[16];

	f = fopen(file,"r");
	if ( f==NULL ) return f;

	SkipComment(f,127,buff);
	if ( eof || sscanf(buff,"%s%*[^\n]",buff2) != 1) {
		fprintf(stderr,"Font Load Table: Format is wrong\n");
		return NULL;
	}
	*num = atoi(buff2);
	SkipComment(f,127,buff);
	if ( eof || sscanf(buff,"%s%*[^\n]",buff2) != 1) {
		fprintf(stderr,"Font Load Table: Format is wrong\n");
		return NULL;
	}
	*deno = atoi(buff2);
	SkipComment(f,127,buff);
	if ( eof || sscanf(buff,"%s%*[^\n]",buff2) != 1) {
		fprintf(stderr,"Font Load Table: Format is wrong\n");
		return NULL;
	}
	*mag = atoi(buff2);
	return f;
}

struct fldent *
getfldent(fld)
FILE *fld;
{
	struct fldent *ent;
	int		eof=0;
	char	buff[128], buff2[16];
	int		s, e;
	int		c;
	int		i;
	short	start, end;
	int		count;

	while ( (c=fgetc(fld)) == ' ' || c == '\t' || c == '\n')
		;
	if (feof(fld)) return NULL;
	ungetc(c,fld);

	SkipComment(fld,127,buff);
	if (eof) return NULL;

	ent = (struct fldent *) calloc(1,sizeof(struct fldent));
	if (ent == NULL) return NULL;

	if (sscanf(buff,"%s%*[^\n]",ent->fontname) != 1) {
		fprintf(stderr,"Font Load Table: Format is wrong\n");
		cfree(ent);
		return NULL;
	}
	SkipComment(fld,127,buff);
	if ( eof || sscanf(buff,"%s%*[^\n]",buff2) != 1) {
		fprintf(stderr,"Font Load Table: Format is wrong\n");
		cfree(ent);
		return NULL;
	}
	ent->sc = atoi(buff2);
	SkipComment(fld,127,buff);
	if ( eof || sscanf(buff,"%s%*[^\n]",buff2) != 1) {
		fprintf(stderr,"Font Load Table: Format is wrong\n");
		cfree(ent);
		return NULL;
	}
	ent->d_sc = atoi(buff2);
	SkipComment(fld,127,buff);
	if ( eof || sscanf(buff,"%s%*[^\n]",buff2) != 1) {
		fprintf(stderr,"Font Load Table: Format is wrong\n");
		cfree(ent);
		return NULL;
	}
	ent->count = atoi(buff2);

	ent->ranges = (struct code_range *) calloc(ent->count,
											sizeof(struct code_range));

	for (i=0; i < ent->count; i++) {
		SkipComment(fld,127,buff);
		if ( !eof && (sscanf(buff,"%x%[^\\;-]16",&s)==1) ) {
			(ent->ranges)[i].start = s;
		} else {
			fprintf(stderr,"Font Load Table: Format is wrong\n");
			cfree(ent->ranges);
			cfree(ent);
			return NULL;
		}
		if (sscanf(buff,"%*x%*[-]%x",&e) == 1) {
			(ent->ranges)[i].end = e;
		} else {
			(ent->ranges)[i].start = s;
		}
	}
	return ent;
}

endfldent(fld)
FILE *fld;
{
	fclose(fld);
}

#ifdef DEBUG
main()
{
	FILE *fld;
	struct fldent *ent;
	int	i;

	fld = setfldent(LOAD_DEF,&magnification);
	printf("magnification:%d\n",magnification);
	while ( (ent=getfldent(fld)) != NULL ) {
		printf("fontname: %-48s\n",ent->fontname);
		printf("scaled size: %d, design size: %d\n",ent->sc, ent->d_sc);
		printf("count: %d\n",ent->count);
		for (i=0; i < ent->count; i++) {
			printf("code %#4x --> %#4x\n", (ent->ranges)[i].start,
										   (ent->ranges)[i].end);
		}
	}
	endfldent(fld);
}
#endif DEBUG

pre_load_font()
{
	FILE	*def;
 	struct fldent *ent;
 	int	i;

	def = setfldent(LOAD_DEF,&numerator,&denominator,&magnification);
	if (def == NULL) {
		fprintf(stderr,"Can't open %s.",LOAD_DEF);
		exit(1);
	}
	define_fraction();
	define_conv();

	printf("magnification:%d\n",magnification);
 	while ( (ent=getfldent(def)) != NULL ) {
 		printf("fontname: %-48s\n",ent->fontname);
		printf("scaled size: %d, design size: %d\n",ent->sc, ent->d_sc);
		printf("count: %d\n",ent->count);
 		for (i=0; i < ent->count; i++) {
 			printf("code %#4x --> %#4x\n", (ent->ranges)[i].start,
 										   (ent->ranges)[i].end);
 		}

		font_manager(PRE_DEFINE_FONT, preFontNum,
				ent->sc, ent->d_sc, ent->fontname, NULL,NULL,NULL);
		for (i=0; i < ent->count; i++) {
			int	cc, s, e, ix;
			struct glyph *g;

			s = (ent->ranges)[i].start;
			e = (ent->ranges)[i].end;
			printf("Reading bitmap, code %#4x --> %#4x\n", s,e);
			for (cc=s; cc <= e; cc++) {
				if ( jis_to_index(cc) != -1) {
					printf("%#4x ",cc);
					font_manager(GET_GLYPH,cc,&g,NULL,NULL,NULL,NULL,NULL);
				}
			}
			printf("\n");
 		}
		preFontNum--;	/* preFontNum is always less than 0. */
 	}
	font_manager(CLOSE_FONT_FILE, NULL, NULL, NULL, NULL,NULL,NULL,NULL);

 	endfldent(def);
}
