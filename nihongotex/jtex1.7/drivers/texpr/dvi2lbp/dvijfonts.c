#include	"dvi2.h"
#include	"dvifonts.h"
#include	<malloc.h>
#include	<fcntl.h>
#include	<sys\stat.h>
#include	<io.h>

extern	char	pxlpath[];	/* font file path */
extern	float   globalmag, mag;
extern	double	iconv,conv,xconv,wconv;
extern	FILE	*in;
extern	long	magfactor;
extern	char	verbose;

char	farname[128];
char	pxlname[128];
char	jfmname[128];
double	pxlwconv[NO_FONTS];	/* design size for font */
float   dvifontrelmag[NO_FONTS];/* to store the relative magnification of */
long	pxlfontmag,		/* magnification of font */
	dvifontmag,		/* fonts magnification factor */
	dvifontdsz,		/* fonts designsize info */
	fontcount[NO_FONTS];	/* no. of use font */

int	fontno;			/* current font no. */
int	intmagfactor;
ftype	fontstat[NO_FONTS];
fartype	farstat[NO_FARS+1];
int	farnum;
char	*pathptr;
fontdescr fontmap[NO_FONTS + 1];/* the mapping table for font indices */
int	cmpname();
char	far *mallocx();
int	adjust_yoffset();


GetFontDef()
{
    char    str[128], *calloc ();
    unsigned char   byte;
    int     i, fnamelen, flag;
    int	    fontnx;

    flag = 0;
    fontno = fontmap[NO_FONTS].index;
    while (((byte = GetByte (in)) >= FNTDEF1) && (byte <= FNTDEF4)) {
#ifdef Debug
	fprintf(stderr, "!!%o\r\n", byte);
#endif
	switch(byte) {
	    case FNTDEF1: 
		fontnx = GetByte(in);
		break;
	    case FNTDEF2: 
		fontnx = Get2Byte(in);
		break;
	    case FNTDEF3: 
		fontnx = Get3Byte(in);
		break;
	    case FNTDEF4: 
		fontnx = Get4Byte(in);
		break;
	    default: 
		Error("Bad byte value in font defs");
		break;
	}
	InsertFontDescr(fontno, fontnx);
	Get4Byte(in);			/* skip check sum */
	dvifontmag = Get4Byte(in);
	dvifontdsz = Get4Byte(in);
	/* for fonts used 'at' different magnifications we need that in order
	   to adjust the intercharacter width correctly */
	dvifontrelmag[fontno] = (float)dvifontmag / (float)dvifontdsz;
	fnamelen = GetByte(in) + GetByte(in);
	for(i = 0; i < fnamelen; i++) {
	    str[i] = GetByte(in);
	}
	str[i] = '\0';
	flag |= CheckPXLFile(str, fontno);
	fontno++;		/* this is the index */
    }
    if(flag) exitx(0);
    freefardir();
    fontmap[NO_FONTS].index = fontno;
    /* the number of valid elements in fontmap */
    if(byte != POSTPOST)
	Error ("POSTPOST missing after fontdefs");
}

InsertFontDescr (fontindex, fontnr)
int     fontindex,fontnr;
/* to insert the fontnr into the mapping table and update fontindex */
{
    register int     upper, lower, mid, i;

    if (fontindex < NO_FONTS) {
	lower = 0;
	upper = fontindex - 1;
	while (lower <= upper) {
	    mid = (lower + upper) / 2;
	    if (fontnr < fontmap[mid].key)
		upper = mid - 1;
	    else
		lower = mid + 1;
	}
	/* fontnr to be inserted at 'lower' */
	for (i = fontindex - 1; i >= lower; i--)
	    fontmap[i + 1] = fontmap[i];
	fontmap[lower].key = fontnr;
	fontmap[lower].index = fontindex;
    }
    else
	Error ("too many different fonts used");
}

getfardir(p)
fartype	*p;
{
	long	dirpos;
	long	s;

	if(!p->farfp){
		if((p->farfp = fopen(p->fname, "rb")) == NULL){
			perror(p->fname);
			exitx(-1);
		}
	}
	fseek(p->farfp, -8L, 2);
	p->numfont = get2bytex(p->farfp);
	dirpos = get4bytex(p->farfp);
	if(FAR_ID != get2bytex(p->farfp))
		Error("Illegal FAR_ID!!");
	lseek(fileno(p->farfp), dirpos, 0);
	s = sizeof(f_stat) * p->numfont;
	if((p->fardir = mallocx(s)) == NULL)
		Error("Out of memory");
	readx(fileno(p->farfp), p->fardir, s);
}

freefardir()
{
	int	i;

	for(i = 0; i < NO_FARS; i++){
		if(farstat[i].fardir){
			freex(farstat[i].fardir);
			farstat[i].fardir = NULL;
		}
	}
}

f_stat far *
farsearch(p, fname)
fartype	*p;
char	*fname;
{
	f_stat	far	*l;
	f_stat	far	*h;
	f_stat	far	*m;
	unsigned	half;
	int		r, n;
	char		sname[10];

	strncpy(sname, fname, 8);
	sname[8] = '\0';
	n = p->numfont;
	l = (f_stat far *)p->fardir;
	h = l + (n - 1);
	while(l <= h){
		if(half = n >> 1){
			m = l + (n & 1 ? half : (half - 1));
			if(!(r = cmpnamex(sname, m->name))){
				return(m);
			}else{
				if(r < 0){
					h = m - 1;
					n = n & 1 ? half : half - 1;
				}else{
					l = m + 1;
					n = half;
				}
			}
		}else{
			if(n)
				if(!cmpnamex(sname, l->name)){
					return(l);
				}
			return(0);
		}
	}
	return(0);
}

CheckFARFile(farpath, fname, fontno)
char	*farpath;
char	*fname;
int	fontno;
{
	fartype	*p;
	ftype	*q;
	int	i;
	f_stat	far *r;

	if(access(farpath, 4)) return(-1);
	i = 0;
	p = farstat;
	q = &fontstat[fontno];
	while((i < farnum) && strcmpi(farpath, p->fname)){
		p++;
		i++;
	}
	if(i < farnum){
		if(!p->fardir)
			getfardir(p);
		if((r = farsearch(p, fname)) == NULL)
			return(-1);
		q->farptr = r->fptr;
		q->length = r->size;
		fseek(p->farfp, r->fptr, 0);
		q->type = get4bytex(p->farfp) - 1000;
		q->farno = i;
		return(0);
	}else{
		p = &farstat[farnum];
		if((p->fname = calloc (strlen(farpath) + 1, 1)) == 0){
			perror("Check font");
			exitx(-1);
		}
		strcpy(p->fname, farpath);
		getfardir(p);
		if(!(r = farsearch(p, fname))){
			freex(p->fardir);
			fclose(p->farfp);
			free(p->fname);
			p->fardir = NULL;
			p->farfp = NULL;
			p->fname = NULL;
			return(-1);
		}else{
			if(farnum >= NO_FARS)
				Error("too many different FAR files used");
			q->farptr = r->fptr;
			q->length = r->size;
			fseek(p->farfp, r->fptr, 0);
			q->type = get4bytex(p->farfp) - 1000;
			q->farno = farnum++;
			return(0);
		}
	}
}

CheckJFMFile(jfmpath, fname, fontno)
char	*jfmpath;
char	*fname;
int	fontno;
{
	FILE	*vnt;
	long	vnum;
	int	i;
	ftype	*p;
	char	*sp;

	p = &fontstat[fontno];
	if(access(jfmpath, 4)) return(-1);
	vnt = fopen(jfmpath, "rb");
	p->type = vnum = get2bytex(vnt);
	p->length = filelength(fileno(vnt));
	fclose(vnt);
	if(vnum != JFMID) return(-1);
	p->farno = -1;
	if((p->fname = calloc(strlen(jfmpath) + 1, 1)) == NULL){
		perror("Check font");
		exitx(-1);
	}
	strcpy(p->fname, jfmpath);
	return(0);
}

CheckPXLFile(fname, fontno)
char	*fname;
int	fontno;
{
	int	endpath;
	FILE	*vnt;
	long	vnum;
	int	i;
	ftype	*p;
	char	*sp;

	p = &fontstat[fontno];
	pathptr = 0;
	do{
	    endpath = GetPXLFileName(fname);
	    if(access(pxlname, 4)){
		if(CheckFARFile(farname, fname, fontno)){
			if(!CheckJFMFile(jfmname, fname, fontno))
				return(0);
		}else{
			if((p->fname = calloc(20, 1)) == NULL){
				perror("Check font");
				exitx(-1);
			}
			sp = pxlname + strlen(pxlname);
			while(*sp-- != '\\');
			while(*sp != '\\') sp--;
			strcpy(p->fname, sp);
			return(0);
		}
	    }else{
		vnt = fopen(pxlname, "rb");
		vnum = get4bytex(vnt);
		p->type = vnum - 1000;
		p->length = filelength(fileno(vnt));
		fclose(vnt);
		p->farno = -1;
		if((p->fname = calloc(strlen(pxlname) + 1, 1)) == NULL){
			perror("Check font");
			exitx(-1);
		}
		strcpy(p->fname, pxlname);
		return(0);
	    }
	}while(!endpath);
	fprintf(stderr, "\\%d\\%s --- Font Missing\r\n",
						intmagfactor, fname);
	return(-1);
}

FontNo (key)
register int     key;
/* to retrieve the index into the fonttables by searching
   through fontmap for the given key */
{

    register int     upper, lower, mid;

    upper = fontmap[NO_FONTS].index;
    lower = 0;
    while (upper >= lower) {
	mid = (upper + lower) / 2;
	if (key < fontmap[mid].key)
	    upper = mid - 1;
	else
	    if (key > fontmap[mid].key)
		lower = mid + 1;
	    else
		return (fontmap[mid].index);
    };
}

/* this routine takes a integer representation of a mag factor (value of the
   magnification times 1000) and returns the float representation (no 1000
   factor).  The routine does a certain amount of faking to make sure that
   the magnification returned is correct.  This routine is also defined and
   used in verser2.c */
float   buildmagfactor (intmag)
float     intmag;
{

    switch((int)(intmag+0.5)){
	case 1094:
	case 1095:
	case 1096:
			return(1.095445);	/* stephalf */
	case 1314:
	case 1315:
	case 1316:
			return(1.314534);	/* stepihalf */
	case 2073:
	case 2074:
	case 2075:
			return(2.0736);		/* stepiv */
	case 2487:
	case 2488:
	case 2489:
			return(2.48832);	/* stepv */
	case 2985:
	case 2986:
	case 2987:
			return(2.985984);	/* stepiv */
	default:	return((float) intmag / 1000);
    }
    /* remaining mags have been ok */
}

GetPXLFileName(fname)
char	*fname;
/* to get the file name of the used fonts */
{
    char    name[10];
    char    *strptr;

    strncpy(name, fname, 8);
    name[8] = '\0';
    intmagfactor = (int)(buildmagfactor(dvifontrelmag[fontno]
		 * magfactor) * mag * 1000.0 + 0.5);
    if(!pathptr)
	pathptr = pxlpath;
    strptr = farname;
    while((*pathptr != ';') && (*pathptr != '\0'))
	*(strptr++) = *(pathptr++);
    *(strptr++) = '\\';
    if (intmagfactor > 9999)
	intmagfactor = 9999;	/* pretty arbitrary bounds */
    if (intmagfactor < 0)
	intmagfactor = 0;
    *(strptr) = '\0';
    sprintf(jfmname, "%s%s.tfm", farname, name);
    sprintf(strptr, "%d", intmagfactor);
    sprintf(pxlname, "%s\\%s.pxl", farname, name);
    strcat(farname, ".far");
    /* compute extension for PXL file name */
    if(*pathptr != '\0'){
	pathptr++;
    	return(0);
    }else
	return(-1);
}

freefont()
{
	register int	i, j;
	char	far *fdir;
	char	far *fpos;
	long	k;
	long	far *dp;
	int	type;
	char	far * far *tp;
	ftype	*p;

	k = 0x7ffffff0;
	i = 0;
	j = -1;
	while(i < NO_FONTS){
		if(fontcount[i] && (i != fontno) && (fontcount[i] < k) 
				&& (fontstat[i].type != JFMID)){
			k = fontcount[i];
			j = i;
		}
		i++;
	}
	if(j == -1){
		if(!fontcount[fontno])
			return(-1);
		j = fontno;
	}
	fontcount[j] = 0;
	fdir = fontstat[j].fontpos;
	type = fontstat[j].type;
	if(verbose){
		p = &fontstat[j];
		printf("freefont:%s[%d]\n", p->fname, type);
	}
	if(type == 2){
		(char far *)tp = fdir + DIRLEN + 512;
		freex(*tp);
		*(long far *)tp = 0;
		dp = fdir;
		for(i = 0; i < 128; i++){
			*(dp + 2) = 0;
			dp += 4;
		}
	}else{
		for(i = 0; i < 128; i++){
			(char far *)tp = fdir + (i << 4) + 8;
			fpos = *tp;
			if(fpos && ((long)fpos != -1L)){
				*(long far *)tp = 0;
				if(type == 3){
					if(freex(fpos)){
						fprintf(stderr, "Illegal free font!!\n");
					}
				}
			}
		}
	}
	return(0);
}

TouchFont(fontno)
int     fontno;
{
	fontcount[fontno] += 100;
	wconv = pxlwconv[fontno];
#ifdef	LBP8
	selectjfont(fontno);
#endif
}

ReadDir(fnum)
{
    int	    j;
    long    i;
    long    pxldir;
    char    *tfntname;
    FILE    *vnt;
    long    vnum;
    long    lseek();
    char    far *fontptr;
    long    far *tp;
    long    base;
    ftype   *p;

	p = &fontstat[fnum];
	if(p->type == JFMID){
		read_jfm(fnum);
		return;
	}
	if(p->farno != -1){
		vnt = farstat[p->farno].farfp;
	}else{
    		if((vnt = fopen(p->fname, "rb")) == NULL)
	    		Error(" -- Font Missing");
	}
    	if((p->fontpos = mallocx((long)DIRLEN + 516)) == 0){
    		printf("Out of memory at %s\n", p->fname);
    		exitx(0);
    	}
    /* and now, must collect needed information from PXL */
    /* verify that version number is as was expected */
    	fseek(vnt, 0L, 0);
    	fseek(vnt, (p->farptr + p->length - 16L), 0);
    	pxlfontmag = get4bytex(vnt);
   	pxlwconv[fnum] = UNFIX( xconv * (UNFIX (get4bytex(vnt)))
    					* (double) dvifontrelmag[fnum]);
    	pxldir = get4bytex(vnt) + p->farptr;
    	vnum = get4bytex(vnt);
    	if ((vnum != 1002) && (vnum != 1003)){
            printf ("Illgal font Id : %s is %ld\n", p->fname, vnum);
	    exitx(0);
	}
    	fontptr = p->fontpos;
    	lseek(fileno(vnt), pxldir, 0);
	readx(fileno(vnt), fontptr, (long)DIRLEN);
	dirconv(fontptr);
	tp = (long far *)(fontptr + DIRLEN);
	for(i = 0; i < 128; i++){
		*(long far *)tp++ = *(long far *)(fontptr + 8);
		*(long far *)(fontptr + 8) = 0;
		fontptr += 16;
	}
	*(long far *)tp = 0;
	if(p->farno == -1)
    		fclose(vnt);
}

char far *
ReadFont(fnum, c)
int	fnum;
unsigned char	c;
{
	char far *fontdir;
	char far *fptr;
	int	size, w, h, i;
	int	vnt;
	ftype	*p;
	long	fpos,
		lsize;
	long far *tp;
	long far *dp;

	p = &fontstat[fnum];
	if(c > 127)
		Error("Illegal char code");
	if(p->type == 2){
		fontdir = p->fontpos;
		if(*(long far *)(fontdir + DIRLEN + 512))
			return(0);
		lsize = p->length - (long)DIRLEN - 24;
		if(p->farno != -1)
			vnt = fileno(farstat[p->farno].farfp);
		else
    			if((vnt = open(p->fname, O_RDONLY | O_BINARY)) == NULL)
				return(0);
    		while((fptr = mallocx(lsize)) == 0){
    	    		if(freefont()){
				Error("Out of memory");
			}
		}
		lseek(vnt, p->farptr + 4, 0);
		readx(vnt, fptr, lsize);
		dp = (long far *)(fontdir);
		tp = (long far *)(fontdir + DIRLEN);
		for(i = 0; i < 128; i++){
			ltow(dp, &h, &w);
			if(h && w){
				*(dp + 2) = (long)addfptrx(fptr, *tp - 4);
			}
			dp += 4;
			tp++;
		}
		*tp = (long)fptr;
		if(p->farno == -1) close(vnt);
		return(*(char far * far *)(fontdir + (c << 4) + 8));
	}else{
		fontdir = p->fontpos + (c << 4);
		fpos = *(long far *)(p->fontpos + DIRLEN + (c << 2));
		if(p->farno != -1)
			vnt = fileno(farstat[p->farno].farfp);
		else
    			if((vnt = open(p->fname, O_RDONLY | O_BINARY)) == NULL)
				return(0);
		ltow(fontdir, &h, &w);
		size = h * ((w + 7) >> 3);
    		while((fptr = mallocx ((long)size)) == 0){
       			if(freefont()){
				Error("Out of memory");
			}
		}
		lseek(vnt, fpos + p->farptr, 0);
		Expand(vnt, fptr, (w + 7) >> 3, w, h);
		*(long far *)(fontdir + 8) = (long)fptr;
		if(p->farno == -1)
			close(vnt);
		return(fptr);
	}
}

#define	ID	jfm.id
#define	NT	jfm.nt
#define	LF	jfm.lf
#define	LH	jfm.lh
#define	BC	jfm.bc
#define	EC	jfm.ec
#define	NW	jfm.nw
#define	NH	jfm.nh
#define	ND	jfm.nd
#define	NI	jfm.ni
#define	NL	jfm.nl
#define	NK	jfm.nk
#define	NG	jfm.ng
#define	NP	jfm.np

static	long	info[256], wid[256], glue[256], ht[16], dp[16];
static	unsigned long gluekern[256];
read_jfm(f)
{
	struct JFM{
		short	id, nt, lf, lh, bc, ec, nw,
			nh, nd, ni, nl, nk, ng, np;
	}jfm;
	short	*ss;
	ftype	*p;
	register int	i;
	int	xsize, ysize, rsize, offset, j, k;
	long	dsize;
	FILE	*vnt;
	unsigned long	*u;
	CHAR_TYPE	far *s;
	FONT_CH		far *t;
	float	vad;

	p = &fontstat[f];
    	if((vnt = fopen(p->fname, "rb")) == NULL)
    		Error(" -- Font Missing");
	ss = &ID;
	for(i = 0; i < 14; i++){
		*ss++ = get2bytex(vnt);
	}
	if(ID != 11){
		printf("bad jfm file %s.\n", p->fname);
		exitx(0);
	}
	get4bytex(vnt);
   	dsize = get4bytex(vnt);
   	wconv = pxlwconv[f] = UNFIX( xconv * (UNFIX (dsize))
    					* (double) dvifontrelmag[f]);
/*
printf("wconv=%f; dsize=%lx; %f\n", wconv, dsize, dvifontrelmag[f]);
*/
	fseek(vnt, (long)(40+20), 1);
	p->face = get4bytex(vnt) & 0xff;
	fseek(vnt, (long)((LH-2)*4 - (40 + 20 + 4)), 1);
	if((s = p->ctype = (CHAR_TYPE far *)mallocx((long)((NT + 1) * 4)))
									==NULL){
    		printf("Out of memory at %s\n", p->fname);
    		exitx(0);
	}
	(s++)->type = NT;
	for(i = 0 ; i < NT; i++, s++){
		s->code = get2bytex(vnt);
		s->type = get2bytex(vnt);
	}
	for(i = BC; i <= EC; i++) 
		info[i] = get4bytex(vnt);
	if((t = (FONT_CH far *)p->fontpos =
		(FONT_CH far *)mallocx((long)(sizeof(FONT_CH)*(EC-BC+1))))
									==NULL){
			printf("can't allocate memory for font table.\n");
			exitx(0);
	}
	for(i = 0; i < NW; i++){
		wid[i] = (long)(get4bytex(vnt) * wconv);
	}
	for(i = 0; i < NH; i++){
		ht[i] = (long)(get4bytex(vnt) * wconv);
	}
	for(i = 0; i < ND; i++){
		dp[i] = (long)(get4bytex(vnt) * wconv);
	}
	fseek(vnt, (long)((NI)*4), 1);
	for(i = 0; i < NL; i++)
		gluekern[i] = get4bytex(vnt);
	fseek(vnt, (long)(NK*4), 1);
	for(i = 0; i < NG; i++){
		glue[i] = get4bytex(vnt);
		get4bytex(vnt);
		get4bytex(vnt);
	}
	j = ((unsigned long)info[0] >> 16) & 0xf;
	k = ((unsigned long)info[0] >> 20) & 0xf;
	ysize = Pixel(dp[j] + ht[k]);
	vad = (float)(dp[j])/(dp[j] + ht[k]);
	xsize = Pixel(wid[(unsigned long)info[0] >> 24]);
	p->size = rsize = calc_pxl_size(ysize, &p->face);
	offset = (xsize - rsize)/2;
	for(i = BC; i <= EC; i++, t++){
		t->tfmw = wid[(unsigned long)info[i] >> 24];
		t->yoff = adjust_yoffset(ysize) - ((int)(rsize * vad));
/*
		t->xoff = -offset + rsize/40 + 1;
*/
		t->xoff = -offset + adjust_xoffset(ysize);
		if(((info[0] >> 8) & 3) == 1){
			u = &gluekern[info[0] & 0xff];
			do{
				if((((*u >> 16) & 0xff) == i) &&
						(((*u >> 8) & 0xff) < 128)){
					t->xoff +=
						Pixel((long)(glue[(*u & 0xff)]
								* wconv));
					break;
				}
			}while((*u++ >> 24) < 128);
		}
/*
printf("%d--tfmw:%d, xoff:%d, yoff:%d, size=%d\n",
		i, Pixel(t->tfmw), t->xoff, t->yoff,rsize);
*/
	}
	fclose(vnt);
	return(1);
}

char
get_char_type(f, jc)
register unsigned short jc;
{
	register unsigned half;
	int	n, r;
	CHAR_TYPE	far *dp, far *sp, far *ep, far *mp;

	dp = fontstat[f].ctype;
	n = dp->type;
	dp++;
	sp = dp + 1;
	ep = dp + --n;
	n--;
	if(ep->code < jc)
		return((char)dp->type);
	while(sp <= ep){
		if(half = n >> 1){
			mp = sp + (n & 1 ? half : (half - 1));
			if(!(r = jc - mp->code)){
				return((char)mp->type);
			}else{
				if(r < 0){
					ep = mp - 1;
					n = n & 1 ? half : half - 1;
				}else{
					sp = mp + 1;
					n = half;
				}
			}
		}else{
			if(jc == sp->code)
				return((char)sp->type);
			return((char)dp->type);
		}
	}
	return((char)dp->type);
}

