#ifndef lint
/*
"$Header: dvikanji.c,v 1.5 88/04/26 21:39:13 spock Locked $";
*/
#endif

#include <stdio.h>
#include "commands.h"
#define NEW(A) ((A *) malloc(sizeof(A)))
#define STRSIZE         257
#define  NPXLCHARS      256
#define TRUE 1
#define FALSE 0

unsigned long num();
long snum();
#define get1()          num(1)
#define get2()          num(2)
#define get3()          num(3)
#define get4()          num(4)
#define sget1()         snum(1)
#define sget2()         snum(2)
#define sget3()         snum(3)
#define sget4()         snum(4)

extern FILE *DVIfile;

/**********************************************************************/
/*************************  Global Variables  *************************/
/**********************************************************************/


struct char_entry {		/* character entry */
short width, height;		/* width and height in pixels */
short xOffset, yOffset;      /* x offset and y offset in pixels */
struct {
   struct {
       int fileOffset;
       char *pixptr;
   } address;
} where;
int tfmw;			/* TFM width */
int nbpl;
short loaded;
};

struct font_entry {  /* font entry */
    int	k, c, s, d, a, l;
    char	n[STRSIZE];	/* FNT_DEF command parameters 		    */
    int	font_space;	/* computed from FNT_DEF s parameter        */
    int	font_mag;	/* computed from FNT_DEF s and d parameters */
    int	font_type;
    int	dic_pack;
    char	name[STRSIZE];	/* full name of PXL file                    */
    int	isDownLoad;	/* font is down loaded			    */
		    /*  0 non loaded, 1 loaded, 2 internal */
    int	gfxSetNumber;	/* LPB8 graphics set number		    */
    int	magnification;	/* magnification read from PXL file         */
    int	designsize;	/* design size read from PXL file           */
    FILE 	*font_file_id;  /* file identifier (NO_FILE if none)	*/
    struct	char_entry ch[NPXLCHARS];  /* character information         */
    struct	font_entry *next;
};

int	isDownLoaded;
struct	font_entry *fontptr;    /* font_entry pointer */
struct	font_entry *hfontptr =( (struct  font_entry *)NULL);
			    /* font_entry pointer                  */

/**********************************************************************/
/***********************  external definitions  ***********************/
/**********************************************************************/

char	*index();
char	*malloc();
char	*mktemp();
char	*rindex();
char	*sprintf();
char	*strcpy();

#define NAMSIZ 76
#define SYNC 4

void
Fatal(s)
char *s;
{
fprintf(stderr,s);
exit(1);
}

void
GetFontDef(DVIfile)
FILE *DVIfile;

/***********************************************************************
Read the font  definitions as they  are in the  postamble of the  DVI
file.  Note that the font directory  is not yet loaded.  In order  to
adapt ourselves to the existing "verser" the following font paramters
are  copied	 onto  output	fontno  (4   bytes),  chksum,	fontmag,
fontnamelength (1 byte), fontname.  At the end, a -1 is put onto  the
file.
***********************************************************************/

{
char    *calloc ();
unsigned char   byte;

    while (((byte = get1()) >= FNT_DEF1) && (byte <= FNT_DEF4)) {
	switch (byte) {
	case FNT_DEF1:
	    ReadFontDef(get1());
	    break;
	case FNT_DEF2:
	    ReadFontDef(get2());
	    break;
	case FNT_DEF3:
	    ReadFontDef(get3());
	    break;
	case FNT_DEF4:
	    ReadFontDef(get4());
	    break;
	default:
	    Fatal("Bad byte value in font defs");
	    break;
	}
    }
    if (byte != POST_POST)
	Fatal("POST_POST missing after fontdefs");
}


/*-->ReadFontDef*/
/**********************************************************************/
/****************************  ReadFontDef  ***************************/
/**********************************************************************/

int
ReadFontDef(k)
int k;
{
	int	t, i,j, fdir, lastchar;
	register struct font_entry *tfontptr;	/* font_entry pointer  */
	register struct char_entry *tcharptr;	/* char_entry pointer  */

	if (CheckFontNum(k)) return;

	if ((tfontptr = NEW(struct font_entry)) == NULL)
		Fatal("can't malloc space for font_entry");
	tfontptr->next = hfontptr;
	fontptr = hfontptr = tfontptr;

	tfontptr->gfxSetNumber = 0;
	tfontptr->k = k;
	tfontptr->c = get4(); /* checksum */
	tfontptr->s = get4(); /* space size */
	tfontptr->d = get4(); /* design size */
	tfontptr->a = get1(); /* area length for font name */
	tfontptr->l = get1(); /* device length */
	j=tfontptr->a+tfontptr->l;
	for(i=0;i<j;i++) 
		tfontptr->n[i] = getc(DVIfile); 
	tfontptr->n[tfontptr->a+tfontptr->l] = '\0';

	/* check font name is kanji or not */

	if ((!strncmp(tfontptr->n,"jisj",4) 
	     &&(i=subfont_num(tfontptr->n+4,'j'))>0) ||
	     (!strncmp(tfontptr->n,"dmj",3) 
	     &&(i=subfont_num(tfontptr->n+3,'d'))>0) ||
	     (!strncmp(tfontptr->n,"dgj",3) 
	     &&(i=subfont_num(tfontptr->n+3,'d'))>0)) { 
	  tfontptr->gfxSetNumber = i;
	  tfontptr->isDownLoad = 2;
	} else
	  tfontptr->isDownLoad = 0;
}

int
KanjiFont()
{
  return (fontptr->isDownLoad == 2);
}

/*-->SetChar*/
/**********************************************************************/
/*****************************  SetChar  ******************************/
/**********************************************************************/

void
Kanji(k,t)
int *k, *t;
{
	  ku_ten_compute(fontptr->gfxSetNumber,*k,k,t);
	  *k += 0x80;
}


int
CheckFontNum(k)
int	k;
{
	register struct	font_entry	*tfontptr;

	tfontptr = hfontptr;
	while(tfontptr != NULL) {
		if (tfontptr->k == k)
			return(TRUE);
		tfontptr = tfontptr->next;
	}
	return(FALSE);
}


/*-->SetFntNum*/
/**********************************************************************/
/****************************  SetFntNum  *****************************/
/**********************************************************************/

/*
 *  this routine is used to specify the font to be used in printing future
 *  characters
 */

void
SetFntNum(k)
int k;
{
	fontptr = hfontptr;
	while ((fontptr!=NULL) && (fontptr->k!=k))
		fontptr = fontptr->next;
	if (fontptr == NULL)
		Fatal("font %d undefined", k);
}

/* ku-ten code to code and font number ( form NttJTeX) */

compute_f_c(ku,ten)
int ku,ten;
{
  int f,c,n;
  
  if ((ku<=0)||((ku>=9)&&(ku<=15))||(ku>84))
    /* @<Report invalid ku in JIS code and |goto exit|@>; */
    return 256+1;
  if ((ten<1)||(ten>94))
    /* then @<Report invalid ten in JIS code and |goto exit|@>; */
    return 256+1;
  if (ku<=8) {
    switch(ku) {
    case 1: f=1; c=ten; break;
    case 2: f=1; c=ten+100; break;
    case 3: f=2; c=ten+32; break;
    default:f=ku-1; c=ten;
    }
  } else if (ku<=47) { /* then {Daiichi Suijun} */
    n=(ku-16)*94+ten-1; f=(n / 256)+8; c=n-(f-8)*256;
  } else { /* {Daini Suijun} */
    n=(ku-48)*94+ten-1; f=(n / 256)+20; c=n-(f-20)*256;
  }
  return 256*f+c;
}

/* @<Utility functions and procedures for Japanese@>= */
ku_ten_compute(f,c,k,t)
int f,c;
int *k,*t;
{
  int ku, ten, n;
  if (f<=7) 
    switch(f) {
    case 1: 
      if (c>=100) {
	ku=2; ten=c-100;
      }else {
	ku=1; ten=c;
      }
      break;
    case 2: 
      ku=3; ten=c-32;
      break;
    default: 
      ku=f+1; ten=c;
    }
  else if (f<=19) { /* then {Daiichi Suijun} */
    n=(f-8)*256+c; ku=(n / 94)+16; ten=(n % 94)+1;
  }else { /* {Daini Suijun} */
    n=(f-20)*256+c; ku=(n / 94)+48; ten=(n % 94)+1;
  }
  *k = ku+32;
  *t = ten+32;
}

struct sub_name {
  char *name;
  int  no;
} kanji_font_name[] = {
      {"sy",1},
      {"roma",2},
      {"hira",3},
      {"kata",4},
      {"greek",5},
      {"russian",6},
      {"keisen",7},
      {"ka",8},
      {"kb",9},
      {"kc",10},
      {"kd",11},
      {"ke",12},
      {"kf",13},
      {"kg",14},
      {"kh",15},
      {"ki",16},
      {"kj",17},
      {"kk",18},
      {"kl",19},
      {"km",20},
      {"kn",21},
      {"ko",22},
      {"kp",23},
      {"kq",24},
      {"kr",25},
      {"ks",26},
      {"kt",27},
      {"ku",28},
      {"kv",29},
      {"kw",30},
      {"kx",31},
      {"ky",32},
      {"kz",33}
};

int
subfont_num(name,c)
char *name;
int  c;
{
  int i;
  char *n,*m;
  struct sub_name *font;

  font = kanji_font_name;
  for(i=0;i<(sizeof(kanji_font_name)/sizeof(kanji_font_name[0]));i++) {
    n = font-> name;
    m = name;
    while(*n++ == *m++)
      if(*n=='\0') 
        return font->no;
    font++;
  }
  return -1;
}
