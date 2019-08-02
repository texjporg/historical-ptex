/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: dvi_private.c,v 1.1 90/04/17 13:05:41 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 90/04/17 13:05:41 $
 */

/*
 * Private DVI file functions.
 */

#include "../dvi/dvi_public.h"
#include "../fonts/public.h"
#include "jdvi2kps.h"

/*
 * Private functions
 */
void set_fnt_num(), set_char(), set_rule(), do_special();
bool InitDVI(), CheckSheet(), BOP(), EOP(), ShowPage();

/*
 * External functions and variables.
 */

/*
 * Global variables
 */
FILE	*outfp		= NULL;	/* output file */
AppInfo app_info = {
  Fatal, InitDVI, CheckSheet, BOP, EOP,
  set_fnt_num, set_char, set_rule, do_special,
};

/*
 * Private variables
 */
#define STRINGBUFSIZE 256

typedef struct font_table {
	long	n_dload;
	long	n_tpxl;
	long	n_cset;
} FontTable;

typedef struct {
        char *tex_name;		/* name in TeX */
        char *ps_name;		/* real font name */
        int len;		/* length of the name in TeX */
} JFontName;

static int Snbpxl	= 0;	/* # of bytes of pixel data */
static int Sonbpx	= 0;	/* "optimal" number of bytes of pixel data */
static int Sndc		= 0;	/* count set bitmap character */
static int Stnc		= 0;	/* total # of chars typeset */
static int Snbpx0, Sndc0, Stnc0;/* used for printing incremental changes per */
				/*  dvi page */
static FontTable	*psfe;		/* PostScript Font Entry Pointer */
static FontEntry	*currentfont;	/* font_entry pointer, current font */
static int	ndone		= 0;	/* number of pages converted */
static int StringPtr;
static char StringBuf[STRINGBUFSIZE];

/*-->PutInt*/
/**********************************************************************/
/*****************************  PutInt  *******************************/
/**********************************************************************/

static void
PutInt(n)               /* output an integer followed by a space */
register int n;
{
  char buf[10];
  register char *b;

  if( n == 0 ) {
    EMITC('0'); 
  } else {
    if( n < 0 ) {
      EMITC('-');
      n = -n;
    }

    for(b=buf;  n>0;  ) {
      *b++ = digit[n%10];
      n /= 10;
    }
    
    for( ; b>buf; ) {
      EMITC(*--b);
    }
  }

  EMITC(' ');
}

/*-->PutOct*/
/**********************************************************************/
/*****************************  PutOct  *******************************/
/**********************************************************************/

static void
PutOct(n)               /* output an 3 digit octal number preceded by a "\" */
register int n;
{                  
  EMITC( '\\' ); 
  EMITC( digit[(n&0300)>>6] );
  EMITC( digit[(n&0070)>>3] ); 
  EMITC( digit[n&0007] );
}

/*-->SetPosn*/
/**********************************************************************/
/*****************************  SetPosn  ******************************/
/**********************************************************************/

static void
SetPosn(x, y)           /* output a positioning command */
register int x, y;
{
  EMITN(x);
  EMITN(y);
  EMITS("p ");
}

JFontName JFontNameTable[] = {
  { "min", "min" },
  { "goth", "goth" },
  { NULL, NULL },
};

/*-->JFontInit		(improvement proc) oka 27-Aug-1989 */
/**********************************************************************/
/****************************  JFontInit  *****************************/
/**********************************************************************/

static void
JFontInit()
{
  register JFontName *fn = JFontNameTable;

  while (fn->tex_name) {
    fn->len = strlen(fn->tex_name);
    ++fn;
  }
}

/*-->JapaneseFont	(improvement proc) oka 27-Aug-1989 */
/**********************************************************************/
/***************************  JapaneseFont  ***************************/
/**********************************************************************/

static JFontName *
JapaneseFont(fe)
register FontEntry *fe;
{
  register JFontName *fn = JFontNameTable;

  while (fn->tex_name) {
    if (!strncmp(fe->common.n, fn->tex_name, fn->len)) {
      return (fn);
    } else {
      ++fn;
    }
  }
  return (NULL);
}

/*-->ChkOpt*/   /* check a user supplied option for validity */
/*********************************************************************/
/****************************** ChkOpt *******************************/
/*********************************************************************/

#define ISOPT(s) if( EQ(str,s) ) return( True )

static int
ChkOpt( str )
register char *str;
{
/*        lcase(str);  doesn't work */

  ISOPT("a4");
  ISOPT("b4");
  ISOPT("a4landscape");
  ISOPT("b4landscape");
  ISOPT("note");          /* its a shame to build this into the program */
  ISOPT("letter");
  ISOPT("legal");
  ISOPT("landscape");
  ISOPT("manualfeed");
  return( False );
}

/*-->CopyFile*/   /* copy a file straight through to output */
/*********************************************************************/
/***************************** CopyFile ******************************/
/*********************************************************************/

void copy_file(fp, filename)
FILE *fp;
char *filename;
{
  register char t;

  EMIT(outfp, "%%%%BeginDocument: %s\n", filename);
  while( (t = (char) getc(fp)) != EOF ) {
    EMITC(t);
  }              
  EMIT(outfp, "\n%%%%EndDocument\n");
}

static void
CopyFile( str )
register char *str;
{
  register FILE *spfp;
  static char fullname[STRSIZE];

  if( (spfp = fopenp(Option.ps_path, str, fullname, "r")) == NULL) {
    prerror("Unable to open file %s\n", str );
    return;
  }
  if( !Option.G_quiet ) fprintf(stderr," [%s", str);
  copy_file(spfp, fullname);
  fclose(spfp);
  if( !Option.G_quiet ) fprintf(stderr,"]");
}

/*-->OutHeader		(improvement proc) oka 27-Feb-1989 */
/**********************************************************************/
/****************************  OutHeader  *****************************/
/**********************************************************************/

static void
OutHeader()
{
  EMIT(outfp, "%%!PS-Adobe-2.0\n");
  EMIT(outfp, "%%%%Title: %s\n", Option.dvi_fname);
  EMIT(outfp, "%%%%Creator: %s\n", Option.G_progname);
  /* EMIT(outfp, "%%%%CreationDate:\n"); */
  /* EMIT(outfp, "%%%%For:\n"); */
  /* EMIT(outfp, "%%%%Routing:\n"); */
  /* EMIT(outfp, "%%%%BoundingBox: \n"); */
  EMIT(outfp, "%%%%Pages: (atend)\n");
  /* EMIT(outfp, "%%%%DocumentFonts: (atend)\n"); */
  /* EMIT(outfp, "%%%%DocumentNeededFonts: \n"); */
  /* EMIT(outfp, "%%%%DocumentSuppliedFonts: \n"); */
  /* EMIT(outfp, "%%%%DocumentPaperSizes: \n"); */
  EMIT(outfp, "%%%%EndComments\n");

  if (Option.G_header) {
    char *getenv(), *filename = getenv("KPSHEADER");

    CopyFile( filename ? filename : HDRFILE );	/* output header file */
  }
}

static bool
InitDVI(fullname, name)
register char *fullname, *name;
{
  register int i;

  JFontInit();		/* Japanese font name Initialize */

  outfp = stdout;		/* output file set */

  OutHeader();		/* output header file preamble */

  for( i=0; i<Option.n_Ifile; i++ ) {
    CopyFile( Option.Ifile[i] );/* output all included files */
  }

  EMITS("%%EndProlog\n");
  EMITS("%%BeginSetup\n");
  EMITS("TeXDict begin\n");
  EMIT(outfp, "/Resolution %d def\n",Option.FontDensity);
  /* EMITS("%%IncludeFont: Ryumin-Light-Ext-H\n"); */
  /* EMITS("%%IncludeFont: GothicBBB-Medium-Ext-H\n"); */
  EMITS("JFontSetup @start\n");
  
  /* prefix valid PostScript options with a "@" */
  for( i=0; i<Option.n_PS; i++ ) {
    if( ChkOpt(Option.PScmd[i]) ) {
      EMIT(outfp, "@%s\n", Option.PScmd[i]);
    } else {
      Fatal( "%s is an invalid option\n", Option.PScmd[i] );
    }
  }

  EMIT(outfp, "%d @copies\n", Option.n_copies);
  EMITS("%%EndSetup\n");

  return(True);
}

static bool
CheckSheet(new_sheet)
register int new_sheet;
{
  return(True);
}

/*-->PSFontInit		(improvement proc) oka 27-Aug-1989 */
/**********************************************************************/
/***************************  PSFontInit  *****************************/
/**********************************************************************/

static FontTable *
PSFontInit(fe)
register FontEntry *fe;
{
  register FontTable *ft = (FontTable *) GetFontAttribute(fe);

  if (ft) return(ft);

  ft = (FontTable *) AllocMemory(sizeof(FontTable));
  ft->n_dload = 0;
  ft->n_tpxl = 0;
  ft->n_cset = 0;
  PutFontAttribute(fe, (long) ft);

  return(ft);
}

/*-->PSFontClearAll		(improvement proc) oka 27-Aug-1989 */
/**********************************************************************/
/**************************  PSFontClearAll  **************************/
/**********************************************************************/

static void
PSFontClearAll(fe)
register FontEntry *fe;
{
  register FontTable *ft = (FontTable *) GetFontAttribute(fe);

  if (ft) {
    ft->n_dload = 0;
    ft->n_tpxl = 0;
    ft->n_cset = 0;
  }
}

static bool
BOP()
{
  StringPtr = 0;
  currentfont = NULL;

  /* once pass begin of page */
  EMIT(outfp, "%%%%Page: %d %d\n", CurrentPage->sheet_page,
       Option.ordinal_page++);
  /* EMIT(outfp, "%%%%PageFonts: (atend)\n"); */
  /* EMIT(outfp, "%%%%PageBoundingBox:\n"); */
  EMIT(outfp, "%%%%BeginPageSetup\n");
  EMIT(outfp, "%%%%EndPageSetup\n");
  EMIT(outfp,"@bop0\n", CurrentPage->sheet_page);
#ifndef GLOBAL_FONT
  FontsForAll(PSFontClearAll);
  Stnc = 0;
  Sndc = 0;
  Snbpxl = 0;
#else
  if (Option.Stats) {
    Stnc0 = Stnc;
    Sndc0 = Sndc;
    Snbpx0 = Snbpxl;
  }
#endif
  if( !Option.G_quiet ) 
    fprintf(stderr, "[%d", CurrentPage->sheet_page);
  return(True);
}

/*-->show_status*/
/**********************************************************************/
/***************************  show_status  ****************************/
/**********************************************************************/

static void
show_status(fe)
register FontEntry *fe;
{
  register FontTable *ft = (FontTable *) GetFontAttribute(fe);

  if (ft && (ft->n_cset > 0 || ft->n_tpxl > 0)) {
    fprintf(stderr, "  %10d%4d", ft->n_cset, (100*ft->n_cset + Stnc/2)/Stnc);
    fprintf(stderr, "  %10d%4d", ft->n_tpxl, (100*ft->n_tpxl + Snbpxl/2)/Snbpxl);
    fprintf(stderr, "  %10s[%4d]\n", fe->common.n, (int) (fe->common.font_mag + 0.5));
  }
}

/*-->font_status*/
/**********************************************************************/
/***************************  font_status  ****************************/
/**********************************************************************/

static void
font_status()
{
  fprintf( stderr, "     Total chars       pxl bytes\n" );
  fprintf( stderr, "           #   %%           #   %%\n" );
  fprintf( stderr, "     ------- ---     ------- ---\n" );
  FontsForAll(show_status);
  fprintf(stderr, "     ------- ---       ----- ---    -----------------\n" );
  fprintf(stderr, "     %7d%4d[%%]   %6d%4d[%%]\n\n" ,Stnc, 100, Snbpxl, 100);
  fprintf(stderr,
	  "Total number of bitmap characters registration:    %8d\n", Sndc);
  fprintf(stderr,
	  "Total number of characters typeset:                %8d\n", Stnc);
  fprintf(stderr,
	  "Number of bytes of pxl data downloaded:            %8d\n", Snbpxl);
  fprintf(stderr,
	  "Optimal # of bytes of pxl data:                    %8d\n", Sonbpx);
}

static void
EmitString()
{
  if (StringPtr == 0) return;
  else {
    register int i, c;

    EMITC('(');
    for (i = 0; i < StringPtr; ++i) {
      c = StringBuf[i];
      if (c < ' ' || c >= 0177) EMITO(c);
      else if (c == '(' || c == ')' || c == '\\') {
	EMITC('\\'); 
	EMITC(c); 
      } else EMITC(c);
    }
    EMITS(") s\n");
  }
  StringPtr = 0;
}

static void
HoldChar(c, hh, vv)
register int c, hh, vv;
{
  if (StringPtr == 0) SetPosn(hh, vv);
  if (StringPtr < STRINGBUFSIZE) StringBuf[StringPtr++] = c;
  else {
    EmitString();
    HoldChar(c, hh, vv);
  }
}

static void
show_fontname(fe)
register FontEntry *fe;
{
  EMIT(outfp, "%%%%+ %s.%d\n", fe->common.n, (int) (fe->common.font_mag));
}

static bool
EOP()
{
  EmitString();
  /* end of second pass, and of page processing */
  if (Option.Tombo) EMITS("@tombo\n");
  EMITS("@eop\n");
  EMIT(outfp, "%%%%PageTrailer\n");
  /* EMITS("%%PageFonts: \n");
  FontsForAll(show_fontname); */
  if (Option.Stats) {
#ifndef GLOBAL_FONT
    font_status();
#else
    fprintf(stderr, " - %d total character on this page\n", Stnc-Stnc0);
#endif
  } else {
    if( !Option.G_quiet ) {
      fprintf(stderr,"] ");
      if( (++ndone % 10) == 0 ) fprintf(stderr,"\n");
    }
  }
  return(True);
}

/*-->SetFntNum		(improvement proc) oka 27-Feb-1989 */
/**********************************************************************/
/****************************  SetFntNum  *****************************/
/**********************************************************************/

static void
set_fnt_num(fe)
register FontEntry *fe;
/*  this routine is used to specify the font to be used in printing future
 *  characters
 */
{
  char		family[STRSIZE];
  int		jf_size;
  JFontName	*fn;

  EmitString();
  psfe = PSFontInit(fe);
  if ((fn = JapaneseFont(fe)) != NULL) {
    sscanf(fe->common.n,"%[^0123456789.]%d", family,&jf_size);
    EMIT(outfp,"%s %d @jsf\n", fn->ps_name,
	 (int) (jf_size *
		actual_factor((int)(((float)fe->common.s/
				     (float)fe->common.d)*1000.0 + 0.5))
		* actual_factor(dvi_info.mag)));
  } else {
    if (psfe->n_dload == 0) {
      EMIT(outfp, "/%s.%d @newfont\n",
	   fe->common.n, (int) fe->common.font_mag);
    }
    EMIT(outfp,"%s.%d @sf\n", fe->common.n, (int)fe->common.font_mag);
  }
  currentfont = fe;
}

static void
EmitChar(c, ce)              /* output a character bitmap */
register int c;
register CharEntry *ce;
{
  register FontEntry	*fe = currentfont;
  register int i, j;
  register unsigned char *sl;
  register int cc;
  register int nbpl, nspl;

  /* Output in PostScript coord system (y +ive up, x +ive right)
     (0,0) of char bitmap at lower left.  Output scan lines
     from bottom to top */

  EMITS("[<");
  cc = 2;
  nbpl = (ce->width + 7) >> 3;

  nspl = UWIDTH(ce->width);
  for(i = ce->height-1;  i >= 0;  i--) {
    sl = (unsigned char *)(ce->where.address.bitmap + i*nspl);
    for(j = 0;  j < nbpl;  j++, sl++) {
      if( cc > 100 ) {
	EMITS("\n  ");
	cc = 2;
      }
      EMITH(*sl);
      cc += 2;
    }
  }

#if 0
  /* print font bitmap */
  {
    int	i,j,k;
    fprintf(stderr, "<%c>[%d,%d]offset[%d,%d]nb[%d]nw[%d]\n"
	    ,c , ce->width,ce->height,
	    ce->xOffset,ce->yOffset,nbpl,nspl);
    for (i = 0 ; i < ce->height ; i++) {
      for (j = 0 ; j < nspl ; j++) {
	for (k = 0 ; k < 8 ; k++) {
	  fprintf(stderr,
		  "%c",
		  (*(((char *)(ce->where.address.bitmap) + i*nspl+j)))
		  & (0x80 >> k) ? '0' : '_');
	}
      }
      fprintf(stderr, "\n");
    }
  }
#endif

  EMIT(outfp,"> %d %d %d %d %d] %d @dc\n", 
       nbpl<<3, ce->height, ce->xOffset,
       (((int)ce->height)-ce->yOffset)-1, HConv(ce->tfmw), c);
  PutGlyphAttribute(fe, c, (long) CurrentPage);

  Snbpxl += nbpl*ce->height;
  Sonbpx += (ce->width*ce->height + 7) >> 3;
  Sndc++;
  psfe->n_tpxl += nbpl*ce->height;
  psfe->n_dload++;
}

/*
 * set_char:
 */

static void
set_char(ptr, c, h, v, hh, vv)
register CharEntry *ptr;
register int c, h, v, hh, vv;
{
  register FontEntry *fe = currentfont;

  if (c > 127 && JapaneseFont(fe) != NULL) {
    EmitString();
    SetPosn(hh, vv);
    /* EMITN(c & 0xff); EMITN((unsigned int)c >> 8); EMITS("jc\n"); */
    EMIT(outfp, "<%02x%02x> jc\n", (unsigned int)c >> 8, c & 0xff);
  } else {
    static int hold_h, hold_v;

    if (GetGlyphAttribute(fe, c) != (long) CurrentPage)
      EmitChar(c, ptr);
    if (h != hold_h || v != hold_v)
      EmitString();
    /* emit the instructions */
    HoldChar(c, hh, vv);
    hold_h = h + ptr->tfmw;
    hold_v = v;
  }
  /*- status -*/
  ++Stnc;
  ++psfe->n_cset;
  /*- status -*/
}

/*
 * set_rule:
 *	This routine will draw a rule on the screen
 */

static void
set_rule(a, b, hh, vv, ehh, evv)
register int a, b, hh, vv, ehh, evv;
{
  EmitString();
  SetPosn(hh,evv);	/* lower left corner */
  EMITN(ehh - hh);	/* width */
  EMITN(evv - vv);	/* height */
  EMITS("ru\n");
}

/*-->DoSpecial*/
/*********************************************************************/
/*****************************  DoSpecial  ***************************/
/*********************************************************************/

typedef enum {None, String, Integer, Number, Dimension} ValTyp;

typedef struct {
  char    *Key;           /* the keyword string */
  char    *Val;           /* the value string */
  ValTyp  vt;             /* the value type */
  union {                 /* the decoded value */
    int  i;
    float n;
  } v;
} KeyWord;

typedef struct {
  char    *Entry;
  ValTyp  Type;
} KeyDesc;

#define PSFILE 0
KeyDesc KeyTab[] = {{"psfile", String},
                    {"hsize", Dimension},
                    {"vsize", Dimension},
                    {"hoffset", Dimension},
                    {"voffset", Dimension},
                    {"hscale", Number},
                    {"vscale", Number}};

/*-->IsSame*/
/**********************************************************************/
/*******************************  IsSame  *****************************/
/**********************************************************************/

static int
IsSame(a, b)        /* compare strings, ignore case */
char *a, *b;
{
  for( ; *a != '\0'; ) {
    if( tolower(*a++) != tolower(*b++) ) {
      return( False );
    }
  }
  return( *a == *b ? True : False );
}

/*-->GetKeyVal*/
/**********************************************************************/
/*****************************  GetKeyVal  ****************************/
/**********************************************************************/

/* get next keyword-value pair
 * decode value according to table entry
 */

static int
GetKeyVal( kw, tab, nt, tno)
KeyWord *kw; 
KeyDesc tab[];
int     nt;
int     *tno;
{
  int i;
  char c = '\0';

  *tno = -1;

  for(i=0; i<nt; i++) {
    if( IsSame(kw->Key, tab[i].Entry) ) {
      *tno = i;
      switch( tab[i].Type ) {
      case None: 
	if( kw->vt != None ) return( False );
	break;
      case String:
	if( kw->vt != String ) return( False );
	break;
      case Integer:
	if( kw->vt != String ) return( False );
	if( sscanf(kw->Val,"%d%c", &(kw->v.i), &c) != 1
	   || c != '\0' ) return( False );
	break;
      case Number:
      case Dimension:
	if( kw->vt != String ) return( False );
	if( sscanf(kw->Val,"%f%c", &(kw->v.n), &c) != 1
	   || c != '\0' ) return( False );
	break;
      }
      kw->vt = tab[i].Type;
      return( True );
    }
  }
  return( True );
}

/*-->GetKeyStr*/
/**********************************************************************/
/*****************************  GetKeyStr  ****************************/
/**********************************************************************/

/* extract first keyword-value pair from string (value part may be null)
 * return pointer to remainder of string
 * return NULL if none found
 */

char    KeyStr[STRSIZE];
char    ValStr[STRSIZE];

static char
*GetKeyStr( str, kw )
char    *str;
KeyWord *kw;
{
  char *s, *k, *v, t;

  if( !str ) return( NULL );

  for( s=str; *s == ' '; s++ ) ;	/* skip over blanks */
  if( *s == '\0' ) return( NULL );

  for( k=KeyStr;			/* extract keyword portion */
      *s != ' ' && *s != '\0' && *s != '='; 
      *k++ = *s++ ) ;
  *k = '\0';
  kw->Key = KeyStr;
  kw->Val = v = NULL;
  kw->vt = None;

  for( ; *s == ' '; s++ ) ;	/* skip over blanks */
  if( *s != '=' )			/* look for "=" */
    return( s );

  for( s++ ; *s == ' '; s++ ) ;	/* skip over blanks */
  if( *s == '\'' || *s == '\"' )	/* get string delimiter */
    t = *s++;
  else
    t = ' ';
  for( v=ValStr;			/* copy value portion up to delim */
      *s != t && *s != '\0';
      *v++ = *s++ ) ;
  if( t != ' ' && *s == t ) s++;
  *v = '\0';
  kw->Val = ValStr;
  kw->vt = String;

  return( s );
}

#define NKEYS (sizeof(KeyTab)/sizeof(KeyTab[0]))

static void
DoSpecial( str, n, hh, vv )          /* interpret a \special command, made up of keyword=value pairs */
char    *str;
int n, hh, vv;
{ 
  int access();
  char spbuf[STRSIZE]; 
  char *sf = NULL;
  KeyWord k;
  int i;

  str[n] = '\0';
  spbuf[0] = '\0';

  SetPosn(hh, vv);
  EMITS("@beginspecial\n");

  if (strncmp(str,"postscriptbox",13) == 0) {
    DoPostScriptBox(str);
  } else {
    /* get all keyword-value pairs */
    while( (str=GetKeyStr(str,&k)) != NULL ) {
      /* for compatibility, single words are taken as file names */
      if( k.vt == None && access(k.Key,0) == 0) {
	if( sf ) {
	  Warning("More than one \\special file name given. %s ignored", sf );
	}
	strcpy(spbuf, k.Key);
	sf = spbuf;
      } else if( GetKeyVal( &k, KeyTab, NKEYS, &i ) && i != -1 ) {
	if( i == PSFILE ) {
	  if( sf ) {
	    Warning("More than one \\special file name given. %s ignored", sf );
	  }
	  strcpy(spbuf, k.Val);
	  sf = spbuf;
	} else {
	  /* the keywords are simply output as PS procedure calls */
	  EMIT(outfp, "%f @%s\n", k.v.n, KeyTab[i].Entry);
	}
      } else {
	Warning("  Invalid keyword or value in \\special - \"%s\" ignored", k.Key );
      }
    }

    EMITS("@setspecial\n");
    if( sf ) {
      CopyFile( sf );
    } else {
      Warning("  No special file name provided.");
    }
  }
  EMITS("@endspecial\n");
}

static void
do_special(fp, l, hh, vv)
FILE *fp;
int l, hh, vv;
{
  static char SpecialStr[STRSIZE];/* "\special" strings */

  EmitString();
  fread(SpecialStr, sizeof(char), l, fp);
  DoSpecial(SpecialStr, l, hh, vv);
}

