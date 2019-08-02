#include "texx2.h"
#include "dvi-simple.h"
#include "texx2-font.h"
#include "conv.h"
#include <assert.h>
#include <ctype.h>

extern Widget TopLevelWidget;

/* font handlers */

int MagAtShrink[] =
{-1,
   2489, 2074, 1728, 1440, 1200, 1098, 1000, 500,
   333, 250, 200, 166, 142, 125, 111, -1};

XFontStruct **FontsAtMag[ MAX_SHRINK + 2 ];

int FontRefCnt[ MAX_SHRINK + 2 ];

XFontStruct **
TeXFontRef(usermag)
int usermag;
{
  int magSlot;

  for ( magSlot = MIN_SHRINK; magSlot <= MAX_SHRINK; magSlot++ ) {
    if ( usermag >= MagAtShrink[ magSlot ] )
      break;
  }

  assert( magSlot >= MIN_SHRINK && magSlot <= (MAX_SHRINK+1) );

  if ( FontsAtMag[ magSlot ] == 0 ) {

    int nameSize;
    char *name;
    char *args[128];
    int argCnt;
    char tmpStr[128];
    int lth;
    int missing;

    int font;

    XFontStruct **thisMag = (XFontStruct **)
      XtMalloc( sizeof( XFontStruct *) * (RegisteredFonts + 1) );

    for ( font = 0; font <= RegisteredFonts; font++ ) {
      thisMag[ font ] = 0;
    }

    /* try to load the fonts */

    missing = 0;

    for (font = 0; font < RegisteredFonts; font++ ) {

      /* get the canonical font name */

      char tmpStr[128];
      double dvimag = TheFontInfo[font].paf.paf_DVIMag;
      double dvidsz = TheFontInfo[font].paf.paf_DVIDesignSize;

      int mag = usermag;

      double dscaled
	= (dvimag * (double) ThePostAmbleInfo.pai_DVIMag) / dvidsz;
      int scaled = ROUND(dscaled);

      double dsuff
	= ((double) DviDpi * scaled * mag) / ((double) 1000.0 * 1000.0) ;
      int suff = ROUND(dsuff);

      int slop;
      int slopCount;

      for (slopCount = 0; slopCount < 3; slopCount++ ) {
	int len;
	char *p;

	slop = (slopCount > 1) ? -1 : slopCount;

	sprintf(tmpStr,"%s.%d",
		TheFontInfo[font].paf.paf_name, (int) suff + slop);

	/* convert to lower-case for X */

	len = strlen(tmpStr);
	for (p = tmpStr; len > 0; len--, p++) {
	  if ( isupper(*p) ) {
	    *p = tolower(*p);
	  }
	}
	
	thisMag[ font ]
	  = XLoadQueryFont( XtDisplay ( TopLevelWidget ), tmpStr );
	if (thisMag[font]) break;
      }

      if ( thisMag[ font ]  == 0 ) {
	missing++;

	/* correct the name of the missing font so it's not off by one */

	sprintf(tmpStr,"%s.%d",
		TheFontInfo[font].paf.paf_name, (int) suff);

	error(0,0,"Missing %s ( -mag %d -scaled %d %s )",
	      tmpStr, mag, scaled, TheFontInfo[font].paf.paf_name);
#ifdef ASCIIJTEX /* kagotani */
	if (!strncmp(tmpStr, "min", 3) || !strncmp(tmpStr, "goth", 4))
	  thisMag[ font ] = XLoadQueryFont(XtDisplay(TopLevelWidget), "k14");
#endif /* ASCIIJTEX */
      }
    }

    FontsAtMag[ magSlot ] = thisMag;
  }

  FontRefCnt[ magSlot ] ++;

  return( FontsAtMag[ magSlot ] );
}

XFontStruct **
  TeXFontUnref(usermag)
int usermag;
{
  int magSlot;
  
  for ( magSlot = MIN_SHRINK; magSlot <= MAX_SHRINK; magSlot++ ) {
    if ( usermag >= MagAtShrink[ magSlot ] )
      break;
  }
  
  assert( magSlot >= MIN_SHRINK && magSlot <= MAX_SHRINK );
  
  if ( FontsAtMag [ magSlot]  ) {
    
    FontRefCnt[ magSlot ] --;
    
    if (FontRefCnt[ magSlot ] <= 0 ) {
      int font;
      XFontStruct **fonts = FontsAtMag[magSlot];
      
      /* fonts list is null terminated */
      
      for (font = 0; fonts[ font ] ; font++ ) {
	XFreeFont(XtDisplay( TopLevelWidget ), fonts[font] );
	fonts[ font ] = 0;
      }
      XtFree( FontsAtMag[ magSlot ] );
      FontsAtMag[ magSlot ] = 0;
      FontRefCnt[ magSlot ] = 0;
    }
  }
}

void
TeXFontNewFile()
{
  int mag;

  for ( mag = 1; mag <= MAX_SHRINK ; mag++ ) {
    TeXFontUnref( MagAtShrink[ mag ] );
  }
}
