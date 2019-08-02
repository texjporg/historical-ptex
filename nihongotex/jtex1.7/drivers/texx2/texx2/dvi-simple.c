/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 */

#include <stdio.h>
#include "dvi-simple.h"
#include "conv.h"
#include <assert.h>
#include <sys/file.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#ifdef BUGGY /* kagotani */
#define FALSE 1
#else /* !BUGGY */
#define FALSE 0
#endif /* !BUGGY */
#endif

#ifdef __STDC__
  extern void bzero(char *, int);
  extern char* malloc(int);
  extern void free(char *);
  extern char *getenv(char *);
  extern strcpy(char *, char *);
  extern strncpy(char *, char *,int);
  extern strcat(char *, char *);
  extern FILE *fopen(char *, char *);
#else
  extern void bzero();
  extern char* malloc();
  extern void free();
  extern char *getenv();
  extern strcpy();
  extern strcat();
  extern FILE *fopen();
#endif

struct PostAmbleInfo ThePostAmbleInfo;

int RegisteredFonts = 0;

#define MAX_GLYPHS 256
DviFontInfo TheFontInfo[ MAX_POSTAMBLE_FONTS ];

long dviPageOffsets;

static long Magnification;
static long	Numerator;		/* numerator from DVI file */
static long	Denominator;		/* denominator from DVI file */

long	*DviCount[DVI_COUNT];
long	*DviPageOffsets = 0;

int	DviHHMargin = -1;	/* horizontal margin (in DEVs) */
int	DviVVMargin = -1;	/* vertical margin (in DEVs) */
int	DviTallestPage = -1;
int	DviWidestPage = -1 ;
int	DviCurrentPage = -1;
int	DviTotalPages = -1;
int	DviDpi = -1;
int	DviMaxDrift = -1;
int	DviUserMag = -1;	/* user-specified magnification */
int	DviBlackness = -1;
FILE	*DviFile;
long	DviFileSize;
char	*DviFileName;

static Conv Conversion;

extern int errno;

static int anError;

static void savePostAmblePointer(p)
struct PostAmbleInfo *p;
{
  int i;
  int page;
  int pageSize;
  int stackSize;
  long prevPagePointer;
  long wuzAt;
  
  ThePostAmbleInfo = *p;

  Numerator = p->pai_Numerator;
  Denominator = p->pai_Denominator;
  Magnification = p->pai_DVIMag;
  
  /*
   *	Set the conversion factor.  This must be done before using
   *	any fonts or the fromSP routine.
   */
  
  CSetConversion(&Conversion,
		 DviDpi, DviUserMag, Numerator, Denominator, Magnification);
  
  DviTotalPages = p -> pai_NumberOfPages;
  DviWidestPage = cfromSP(&Conversion, p -> pai_WidestPageWidth);
  DviTallestPage = cfromSP(&Conversion, p -> pai_TallestPageHeight);

  /*
   *	Set of the table of pointers to pages and room for the
   *	count variables.
   */
  
  pageSize = DviTotalPages * sizeof(long);
  if (DviPageOffsets != 0) {
    free( (char *) DviPageOffsets);
    DviPageOffsets = 0;
  }
  DviPageOffsets = (long *) malloc(pageSize);
  
  if (DviPageOffsets == NULL) {
    error(1,0,"Can not allocate page directory (%d pages)", DviTotalPages);
    exit(1);
  }
  
  for (i = 0; i < DVI_COUNT; i++) {
    if (DviCount[i] != 0) {
      free( (char *) DviCount[i]);
      DviCount[i] = 0;
    }
    DviCount[i] = (long *) malloc(pageSize);
    if (DviCount[i] == NULL) {
      error(1,0,"Can not allocate count directory (%d pages)", DviTotalPages);
      exit(1);
    }
  }
  
  /*
   * Follow back pointers through pages in the DVI file,
   * storing the offsets in the DviPageOffsets table.
   */
  
  prevPagePointer = p->pai_PrevPagePointer;
  wuzAt = (long) ftell(DviFile);

  fseek(DviFile, 0L, 2);
  DviFileSize = ftell(DviFile);
  
  for (page = DviTotalPages - 1; page >= 0 ; page--) {
    
    /*
     *	Skip to previous page, but skip over BOP marker.
     */
    DviPageOffsets[page] = prevPagePointer;

    if( fseek(DviFile, prevPagePointer+1, 0) < 0) {
      error(1, errno, "Improper seek looking up pages\n");
      anError |= 1;
      return;
    }
    
    /*
     *	Read the other counters at the beginning of the page
     */
    
    for (i = 0; i < DVI_COUNT; i++) {
      long value;
      fGetLong(DviFile, value);
      DviCount[i][page] = value;
    }
    fGetLong(DviFile, prevPagePointer);
  }
  fseek(DviFile, wuzAt, 0);
}

#ifndef DEFAULT_TFM_PATH
#define DEFAULT_TFM_PATH "/usr/local/lib/tex82/fonts"
#endif

static char *EnvFontPath;

static FILE *openTfmFile(name)
char *name;
{
  char *usePath;
  char path[1024];

  if ( EnvFontPath == 0 ) {
    extern char *getenv();
    EnvFontPath = getenv("TEXFONTS");
  }

  usePath = (EnvFontPath == 0) ? DEFAULT_TFM_PATH : EnvFontPath;

  for (;;) {
    char *p = path;

    if ( usePath == 0 || *usePath == 0 ) {
      return( 0 );
    }

    *p = 0;

    while ( *usePath && *usePath != ':' ) {
      *(p++) = *(usePath++);
    }

    /* append name */

    *(p++) = '/';
    strcpy(p, name);
    strcat(p, ".tfm");
    if ( access( path, R_OK ) == 0 ) {
      return( fopen(path, "r") );
    }

    if ( *usePath == ':' ) {
      usePath++;
    }
  }
}

static void
registerFont(f)
struct PostAmbleFont *f;
{
  int thisFont;
  FILE *tfmFile;
  char *fontName[1024];

  assert( RegisteredFonts < MAX_POSTAMBLE_FONTS );

  thisFont = RegisteredFonts++;
/* Attempt to locate the TFM files */
 
  tfmFile = openTfmFile( f -> paf_name );
  if ( tfmFile == 0 ) {
    error(0,0,"Can't find TFM file for %s", f -> paf_name);
    anError = 1;
  }
  else {
    struct tfmdata t;
    i32 pspace;
    int len;

    TheFontInfo[ thisFont ].paf = *f;

    /* make a copy of the name */

    len = strlen( f -> paf_name );
    TheFontInfo[ thisFont ].paf.paf_name
      = malloc( sizeof(char) * (len + 1 ));
    strncpy( TheFontInfo[ thisFont ].paf.paf_name, f -> paf_name, len+1);

    pspace = f -> paf_DVIMag / 6;	/* a three-unit "thin space" */

    TheFontInfo[ thisFont ].pspace = pspace;
    TheFontInfo[ thisFont ].nspace = -4 * pspace;
    TheFontInfo[ thisFont ].vspace = 5 * pspace;

    bzero((char *) &t, sizeof(t));

#if defined(ASCIIJTEX) && defined(FAKEKANJI) /* kagotani */
    if ( readtfmfile( tfmFile, &t, FALSE ) != 0 ) {
      error(0,0,"Bogus TFM file: %s", f -> paf_name);
    }
#else /* !ASCIIJTEX || !FAKEKANJI */
    if ( readtfmfile( tfmFile, &t, TRUE ) != 0 ) {
      error(0,0,"Bogus TFM file: %s", f -> paf_name);
    }
#endif /* !ASCIIJTEX || !FAKEKANJI */
    else {
      int i;
      int nc = t.t_hdr.th_ec - t.t_hdr.th_bc + 1;
      int nw;
#if defined(ASCIIJTEX) && defined(FAKEKANJI) /* kagotani */
      int ne;
#endif /* ASCIIJTEX && FAKEKANJI */
      int *p;
      int s;	/* see dvitype */
      int d;

#ifdef ASCIIJTEX /* kagotani */
      struct char_type_word *tp;

      TheFontInfo[ thisFont ].nt = t.t_hdr.th_nt;
      if ( t.t_hdr.th_nt > 0 ) {
	      if ( TheFontInfo[ thisFont ].ct == 0 ) {
		 TheFontInfo[ thisFont ].ct = (struct char_type_word *)
		   malloc( sizeof(struct char_type_word) *  t.t_hdr.th_nt );
	      }

	      tp = TheFontInfo[ thisFont ].ct;
	      for (i = 0; i < t.t_hdr.th_nt; i++) {
		*(tp++) = t.t_ct[i];
	      }
      } else {
	      if ( TheFontInfo[ thisFont ].ct != 0 ) {
		      free( TheFontInfo[ thisFont ].ct );
		      TheFontInfo[ thisFont ].ct = 0;
	      }
      }

#endif /* ASCIIJTEX */

      if ( TheFontInfo[ thisFont ].widths == 0 ) {
	 TheFontInfo[ thisFont ].widths =
	   (int *) malloc( sizeof(int) *  MAX_GLYPHS );
      }

      /* convert widths */
      assert( t.t_width[0] == 0 );
      nw = t.t_hdr.th_nw;
      s = TheFontInfo[ thisFont ].paf.paf_DVIMag;
      d = TheFontInfo[ thisFont ].paf.paf_DVIDesignSize;

      for (i = 0; i < nw; i++) {

	int width = t.t_width[i];

	if (width != 0) {
	  width = ScaleOneWidth(width, s);
	}
	t.t_width[i] = width;
	
      }

      p = TheFontInfo[ thisFont ].widths;
      bzero( (char *) p , sizeof(int) * MAX_GLYPHS );

      p += t.t_hdr.th_bc;

      for (i = 0; i < nc; i++) {
	int wi = t.t_ci[i].ci_width;
	*(p++) = t.t_width[wi];
      }

#ifdef ASCIIJTEX /* kagotani */
#ifdef FAKEKANJI /* kagotani */
      if ( t.t_hdr.th_nt > 0 ) {
	if ( TheFontInfo[ thisFont ].glues == 0 ) {
	   TheFontInfo[ thisFont ].glues =
	     (int *) malloc( sizeof(int) *  MAX_GLYPHS );
	}

	/* convert glues */
	ne = t.t_hdr.th_ne / 3;

	for (i = 0; i <= ne; i++) {
	  int glue = t.t_glue[i].g_width;
	  if (glue != 0) {
	    glue = ScaleOneWidth(glue, s);
	  }
	  t.t_glue[i].g_width = glue;
	}

	p = TheFontInfo[ thisFont ].glues;
	bzero( (char *) p , sizeof(int) * MAX_GLYPHS );

	p += t.t_hdr.th_bc;

	for (i = 0; i < nc; i++, p ++) {
	  struct char_info_word ci = t.t_ci[0];
	  if (T_CI_T(&ci) == 1) {
	    struct glue_kern_word *gkp = &t.t_gk[ci.ci_remainder];
	    int c = i + t.t_hdr.th_bc;
	    do {
	      if (gkp->gk_type == c && gkp->gk_gk < 128) {
		int gi = gkp->gk_remainder;
		*p = t.t_glue[gi].g_width;
		break;
	      }
	    } while ((gkp++)->gk_seqend < 128);
	  }
	}
      }
#endif /* FAKEKANJI */
#ifndef BUGGY /* kagotani */
      if (t.t_ci != NULL) free((char*)t.t_ci);
      if (t.t_width != NULL) free((char*)t.t_width);
      if (t.t_height != NULL) free((char*)t.t_height);
      if (t.t_depth != NULL) free((char*)t.t_depth);
      if (t.t_ct != NULL) free((char*)t.t_ct);
#ifdef FAKEKANJI /* kagotani */
      if (t.t_gk != NULL) free((char*)t.t_gk);
      if (t.t_glue != NULL) free((char*)t.t_glue);
#endif /* FAKEKANJI */
#endif /* BUGGY */
#endif /* ASCIIJTEX */
    }

    fclose(tfmFile);
  }
}

/*
 *	Returns TRUE if everything is fine
 */

int
DviInit( dviFileName, copy )
char *dviFileName;
int copy;
{
    extern char *ProgName;
    FILE *dviFile;
    char *mktemp();

    anError = 0;

    DviFini();	/* clean up the old files */
    
    DviFile = 0;

    if (dviFileName == 0) {
	dviFile = stdin;
	dviFileName = "<stdin>";
	copy = 1;
    }
    else if ((dviFile = fopen(dviFileName, "r")) == NULL) {

	int n = strlen(dviFileName);
	char *dviName;
	
	if (strcmp(dviFileName + n - sizeof(".dvi") + 1, ".dvi") == 0) {
	  DviFini();
	  error(0, errno, "[fopen] can't open %s", dviFileName);
	  return(1);
	}

	dviName = (char *) malloc((unsigned) n + sizeof(".dvi") + 1);
	sprintf(dviName, "%s.dvi", dviFileName);

	if ((dviFile = fopen(dviName, "r")) == NULL) {
	  DviFini();
	  error(0, errno, "[fopen] can't open %s", dviName);
	  return(1);
	}
	dviFileName = dviName;
    }

    DviFileName = dviFileName;

    if ( copy ) {
      
      /*
       *	Copy the file to a temporary location if requested.
       *	This lets the person peruse the file while theyre re-texing it.
       */
      FILE *tmpFile;
      char dviTmpFileNameBuffer[256];
      char *dviTmpFileName;
      
      sprintf(dviTmpFileNameBuffer,"/tmp/Dvistuff.XXXXXX");
      
      dviTmpFileName = mktemp(dviTmpFileNameBuffer);
      
      if (!(tmpFile = fopen(dviTmpFileName,"w+"))) {
	error(0, errno, "[fopen] Unable to create temporary file");
	dviTmpFileName = 0;
      }
      else {
	char buffer[BUFSIZ];
	int b;
	
	rewind(dviFile);
	do {
	  b = fread(buffer, 1, BUFSIZ, dviFile);
	  fwrite(buffer, 1, b, tmpFile);
	} while (! feof(dviFile));
	
	fclose(dviFile);
	dviFile = tmpFile;
	rewind(dviFile);
	
	/*
	 *	Unlink the temporary file. This keeps tmp files from cluttering
	 *	up /tmp and it does it in a very application-independent way.
	 *	You can't reopen the tmp file, but we don't really allow that
	 *	anyway (the tmp file is hidden from the user).
	 */
	
	if (dviTmpFileName != 0 &&
	    strncmp(dviTmpFileName,"/tmp/",5) == 0) {
	  unlink(dviTmpFileName);
	}
      }
    }

    assert( dviFile != 0 );

    DviFile = dviFile;

    if (DviUserMag == -1) {
	DviUserMag = 1000;
    }

    if (DviMaxDrift == -1) {
	DviMaxDrift = DEFAULT_MAX_DRIFT;
    }

    if (DviDpi == -1) {
	DviDpi = DEFAULT_DPI;
    }

    if (DviBlackness == -1) {
	DviBlackness = DEFAULT_BLACKNESS;
    }

    /* Margins -- needs work! */

    if (DviHHMargin == -1) {
	DviHHMargin = DEFAULT_HHMARGIN;
    }

    if (DviVVMargin == -1) {
	DviVVMargin = DEFAULT_VVMARGIN;
    }

    anError |= ScanPostAmble( DviFile, savePostAmblePointer, registerFont);

    if (anError) {
      DviFini();
    }

    return( anError );
}

void
DviFini()
{
  int font;
  int c;

  if (DviFile != 0) {
    fclose(DviFile);
  }

  DviFile = 0;

  for (font = 0; font < RegisteredFonts; font++) {
    if ( TheFontInfo[ font ].paf.paf_name ) {
      free ( (char *) TheFontInfo[ font ].paf.paf_name );
      TheFontInfo[ font] .paf.paf_name = 0;
    }
    if ( TheFontInfo[ font ].widths ) {
      free ( (char *) TheFontInfo[ font ].widths );
      TheFontInfo[ font].widths = 0;
    }
  }
  RegisteredFonts = 0;

  for (c = 0; c < DVI_COUNT; c++) {
    if ( DviCount[ c ] ) {
      free( (char *) DviCount[ c ] );
      DviCount[ c ] = 0;
    }
  }
}
	

char *
DviFetchPage(pageNumber)
int pageNumber;
{
  if ( pageNumber < 0 || pageNumber > DviTotalPages ) {
    return(0);
  }
  else {
    long start = DviPageOffsets[ pageNumber ];
    long end = ( pageNumber == (DviTotalPages-1))
      ? DviFileSize : DviPageOffsets[ pageNumber + 1];
    long toRead = end - start;

    char *buffer = malloc( toRead );

    if (buffer == 0 ) {
      return( 0 );
    }
    else {

      long loc;
      long bytes;

      loc = fseek(DviFile, (long) start, 0);
      if (loc < 0) {
	error(0,0,"Improper seek: %d", errno);
      }

      bytes = fread( buffer, 1, toRead, DviFile );

      if (bytes != toRead) {
	error(0,0,"Tried to read %d, only got %d", toRead, bytes);
      }
      return(buffer);
    }
  }
}

