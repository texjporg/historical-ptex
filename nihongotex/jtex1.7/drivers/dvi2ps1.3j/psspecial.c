#include	<stdio.h>
#include	<ctype.h>
#include	<math.h>
#include	"file.h"
#include	"defs.h"
#include	"emit.h"
#include	"global.h"

char   *GetKeyStr();
int     GetKeyVal();
int     IsSame();
extern char *malloc();
extern	double	cos(), sin(), sqrt();
static void ps_pensize();
static void ps_flushpath();
static void dashed_line();
static void ps_drawto();
static void ps_arc();
static void flush_spline();
static void ps_whiten();
static void ps_blacken();
static void ps_shade();
static void push_location(), pop_location();

#define	TWOPI		(3.14157926536*2.0)
#define	MAXPOINTS	600	/* Most number of points in a path */
#define	SPLINEPOINTS	5000	/* Most points in a spline */
#define	RADTODEG	(360/(TWOPI))

extern int resolution;
extern int hconvresolution, vconvresolution;

#define	xconv(x)	((int) ((x) * ( ((float) resolution) / 1000.0) + 0.5))
#define	yconv(y)	((int) ((y) * ( ((float) resolution) / 1000.0) + 0.5))

extern	int NextFamilyNumber;	/* Number of next ImPress family to use */
#define	fnum	NextFamilyNumber

static	int xx[MAXPOINTS], yy[MAXPOINTS], pathlen,
	pensize = 2;	/* Size we want Imagen to draw at, default 2 pixels */

static int
	family_defined = FALSE,	/* Have we chosen family yet? */
	texture_defined = FALSE,/* Have we done a set_texture yet? */
	whiten_next = FALSE,	/* Should next object be whitened? */
	blacken_next = FALSE,	/* Should next object be blackened? */
	shade_next = FALSE;	/* Should next object be shaded? */

/*-->DoSpecial*/
/*********************************************************************/
/*****************************  DoSpecial  ***************************/
/*********************************************************************/

typedef enum {None, String, Integer, Number, Dimension, EEPIC} ValTyp;

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
#define EPSFILE 1

KeyDesc KeyTab[] = {{"psfile", String},
		    {"epsfile", String},
		    {"hsize", Dimension},
		    {"vsize", Dimension},
		    {"hoffset", Dimension},
		    {"voffset", Dimension},
		    {"hscale", Number},
		    {"vscale", Number},
		    {"rotation", Dimension}};

#define NKEYS (sizeof(KeyTab)/sizeof(KeyTab[0]))

void
dev_dospecial(str, n)	/* interpret a \special command, made up of keyword=value pairs */
char    *str;
int n;
{ 
	char spbuf[STRSIZE]; 
	char *sf = NULL;
	KeyWord k;
	int i, count = 0, epsflag = FALSE;
	float	llx, lly, urx, ury;

	end_string();

	str[n] = '\0';
	spbuf[0] = '\0';

	dev_setposn(h, v);

	while( (str=GetKeyStr(str,&k)) != NULL ) {      /* get all keyword-value pairs */
			      /* for compatibility, single words are taken as file names */
		if( k.vt == EEPIC)
		    return;

		if (count++ == 0)
		    EMITS("@beginspecial\n");
		if( k.vt == None && access(k.Key,F_OK) == 0) {
			if( sf ) Warning("  More than one \\special file name given. %s ignored", sf );
			strcpy(spbuf, k.Key);
			sf = spbuf;
			}
		else if( GetKeyVal( &k, KeyTab, NKEYS, &i ) && i != -1 ) {
			if( i == PSFILE || i == EPSFILE ) {
				if( sf ) Warning("  More than one \\special file name given. %s ignored", sf );
				strcpy(spbuf, k.Val);
				sf = spbuf;
				if ( i == EPSFILE )
				    epsflag = TRUE;
				    EMITS("@epsf\n");
				}
			else            /* the keywords are simply output as PS procedure calls */
				EMIT(outfp, "%f @%s\n", k.v.n, KeyTab[i].Entry);
			}
		else
		    Warning("  Invalid keyword or value in \\special - \"%s\" ignored", k.Key );
		}
	if (epsflag == TRUE && scanfile(sf, &llx, &lly, &urx, &ury) == TRUE)
	    EMIT(outfp, "%f %f %f %f @bbox\n", llx, lly, urx, ury);
	    
	EMITS("@setspecial\n");

	if( sf )
		ps_copyfile( sf );
	else
		Warning("  No special file name provided.");
	EMITS("@endspecial\n");
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

char *GetKeyStr( str, kw )
char    *str;
KeyWord *kw;
{
	char *s, *k, *v, t;

	if( !str ) return( NULL );
	for(s = str ; *s == ' '; s++ ) ;
	if( *s == '\0' ) return( NULL );

	for( k=KeyStr;                          /* extract keyword portion */
	     *s != ' ' && *s != '\0' && *s != '='; 
	     *k++ = *s++ ) ;
	*k = '\0';

	kw->Key = KeyStr;
	kw->Val = v = NULL;
	kw->vt = None;
	if (strcmp(kw->Key, "pn") == 0) {
	    ps_pensize(s);
	    kw->vt = EEPIC;
	    s += strlen(s);
	    return s;
	} else if (strcmp(kw->Key, "fp") == 0) {
	    ps_flushpath();
	    kw->vt = EEPIC;
	    return s;
	} else if (strcmp(kw->Key, "da") == 0) {
	    dashed_line(s, 0);
	    kw->vt = EEPIC;
	    s += strlen(s);
	    return s;
	} else if (strcmp(kw->Key, "dt") == 0) {
	    dashed_line(s, 1);
	    kw->vt = EEPIC;
	    s += strlen(s);
	    return s;
	} else if (strcmp(kw->Key, "pa") == 0) {
	    ps_drawto(s);
	    kw->vt = EEPIC;
	    s += strlen(s);
	    return s;
	} else if (strcmp(kw->Key, "ar") == 0) {
 	    ps_arc(s);
	    kw->vt = EEPIC;
	    s += strlen(s);
	    return s;
	} else if (strcmp(kw->Key, "sp") == 0) {
	    flush_spline();
	    kw->vt = EEPIC;
	    return s;
	} else if (strcmp(kw->Key, "sh") == 0) {
	    ps_shade();
	    kw->vt = EEPIC;
	    return s;
	} else if (strcmp(kw->Key, "wh") == 0) {
	    ps_whiten();
	    kw->vt = EEPIC;
	    return s;
	} else if (strcmp(kw->Key, "bk") == 0) {
	    ps_blacken();
	    kw->vt = EEPIC;
	    return s;
/* not yet!
	} else if (strcmp(kw->Key, "tx") == 0) {
	    do_texture(s);
	    kw->vt = EEPIC;
	    s += strlen(s);
	    return s;*/
	}

	for( ; *s == ' '; s++ ) ;                       /* skip over blanks */
	if( *s != '=' )                         /* look for "=" */
		return( s );

	for( s++ ; *s == ' '; s++ ) ;                   /* skip over blanks */
	if( *s == '\'' || *s == '\"' )          /* get string delimiter */
		t = *s++;
	else
		t = ' ';
	for( v=ValStr;                          /* copy value portion up to delim */
	     *s != t && *s != '\0';
	     *v++ = *s++ ) ;
	if( t != ' ' && *s == t ) s++;
	*v = '\0';
	kw->Val = ValStr;
	kw->vt = String;

	return( s );
}


/*-->GetKeyVal*/
/**********************************************************************/
/*****************************  GetKeyVal  ****************************/
/**********************************************************************/

	/* get next keyword-value pair
	 * decode value according to table entry
	 */

int GetKeyVal( kw, tab, nt, tno)
KeyWord *kw; 
KeyDesc tab[];
int     nt;
int     *tno;
{
	int i;
	char c = '\0';

	*tno = -1;

	for(i=0; i<nt; i++)
		if( IsSame(kw->Key, tab[i].Entry) ) {
			*tno = i;
			switch( tab[i].Type ) {
			        case None: 
					if( kw->vt != None ) return( FALSE );
					break;
				case String:
					if( kw->vt != String ) return( FALSE );
					break;
				case Integer:
					if( kw->vt != String ) return( FALSE );
					if( sscanf(kw->Val,"%d%c", &(kw->v.i), &c) != 1
					    || c != '\0' ) return( FALSE );
					break;
				case Number:
				case Dimension:
					if( kw->vt != String ) return( FALSE );
					if( sscanf(kw->Val,"%f%c", &(kw->v.n), &c) != 1
					    || c != '\0' ) return( FALSE );
					break;
				}
			kw->vt = tab[i].Type;
			return( TRUE );
			}

	return( TRUE );
}

char ToLower(c)
char c;
{
       return( isupper(c)?tolower(c):c );
}

int IsSame(a, b)        /* compare strings, ignore case */
char *a, *b;
{
	for( ; *a != '\0'; )
		if( ToLower(*a++) != ToLower(*b++) ) return( FALSE );
	return( *a == *b ? TRUE : FALSE );
}

/*-->scanfile*/   /* scan a file */
/*********************************************************************/
/***************************** scanfile ******************************/
/*********************************************************************/
BOOLEAN
scanfile( str, llx, lly, urx, ury )
char    *str;
float	*llx, *lly, *urx, *ury;
{
	FILE    *spfp;
	char	buffer[BUFSIZ];
	int	i;

	if( (spfp=fopen(str,"r")) == NULL ) {
		fprintf(stderr,"Unable to open file %s\n", str );
		return;
		}
	while(fgets(buffer, BUFSIZ, spfp) != NULL) {
	    i = 0;
	    if (buffer[i++] == '%' && buffer[i++] == '%') {
/*		if (!strncmp(&buffer[i] ,"BoundingBox:", 12) || !strncmp(&buffer[i], "PageBoundingBox:", 16)) {*//* not use */
		if (!strncmp(&buffer[i] ,"BoundingBox:", 12)) {
		    for (; buffer[i++] != ':';);
		    for (; buffer[i] == ' '; i++); /* skip space */
		    if (buffer[i] != '?' && strncmp(&buffer[i], "(atend)", 7)) {
			sscanf(&buffer[i], "%f %f %f %f", llx, lly, urx, ury);
			fclose(spfp);
			return TRUE;
		    }
		}
	    }
	}
	fclose(spfp);
	return FALSE;
}

/*
 * Support drawing routines for Chris Torek's DVI->ImPress program.
 *
 * Requires v1.7 or later ImPress to handle paths.
 * Better if v1.9 or later for arc, circle, and ellipse primitives
 * (define USEGRAPHICS).
 *
 *	Tim Morgan, UC Irvine, 11/17/85
 *
 * PostScript version
 *	Kazuhiro Kazama, NTT Software Laboratory.
 */


/*
 * Save the graphic state and set up a new coordinate system
 */
static void push_location()
{
    EMIT(outfp, "@push\n");
}


/*
 * Restore the graphic state
 */
static void pop_location()
{
    EMIT(outfp, "@pop\n");
}


/*
 * Set the pen size
 * Called as \special{pn size}
 *	 eg: \special{pn 8}
 * The size is the number of milli-inches for the diameter of the pen.
 * This routine converts that value to device-dependent pixels, and makes
 * sure that the resulting value is within legal bounds.
 */
static void	ps_pensize(cp)
char *cp;
{
    int size;

    if (sscanf(cp, " %d ", &size) != 1) return;
    pensize = yconv(size);
    if (pensize < 0) pensize = 0;
}


/*
 * Make sure the pen size is set.  Since we push and pop the state,
 * this has to be sent for each different object (I think).
 */
static void set_pen_size()
{
    EMIT(outfp, "%d @pn\n", pensize);
}


/*
 * Actually apply the attributes (shade, whiten, or blacken) to the currently
 * defined path/figure.
 */
static void do_attributes()
{
    if (shade_next) {
	shade_next = FALSE;
	EMIT(outfp, "@sh ");
    }
    else if (whiten_next) {
	whiten_next = FALSE;
	EMIT(outfp, "@wh ");
    }
    else if (blacken_next) {
	blacken_next = FALSE;
	EMIT(outfp, "@bk ");
    }
}


/*
 * Flush the path that we've built up with ps_drawto()
 * Called as \special{fp}
 */
static void ps_flushpath()
{
    register int i;

    push_location();
    if (pathlen <= 0) return;
    set_pen_size();
    EMIT(outfp, "%d %d p\n", xx[1], yy[1]);
    for (i=2; i<=pathlen; i++) {
	EMIT(outfp, "%d %d l\n", xx[i], yy[i]);
    }
    pathlen = 0;
    if (blacken_next == TRUE || shade_next == TRUE) {
	do_attributes();
	EMIT(outfp, "fill\n");
    } else {
	do_attributes();
	EMIT(outfp, "stroke\n");
    }
    pop_location();
}


/* Helper routine for dashed_line() */
static void connect(x0, y0, x1, y1)
int x0, y0, x1, y1;
{
    EMIT(outfp, "%d %d %d %d @ln\n", x0, y0, x1, y1);
}


/* Another helper.  Draw a dot at the indicated point */
static void dot_at(x, y)
int x,y;
{
    EMIT(outfp, "%d %d @pt\n", x, y);
}


/*
 * Draw a dashed or dotted line between the first pair of points in the array
 * Called as \special{da <inchesperdash>}	(dashed line)
 *	  or \special{dt <inchesperdot>}	(dotted line)
 *	eg:  \special{da 0.05}
 */
static void	dashed_line(cp, dotted)
char *cp;
int dotted;			/* boolean */
{
    int i, numdots, x0, y0, x1, y1;
    double cx0, cy0, cx1, cy1;
    double d, spacesize, a, b, dx, dy, pixperdash;
    float inchesperdash;

    if (sscanf(cp, " %f ", &inchesperdash) != 1) return;
    if (pathlen <= 1) return;
    pixperdash = inchesperdash * ((double) resolution);
    x0 = xx[1]; x1 = xx[2];
    y0 = yy[1]; y1 = yy[2];
    dx = x1 - x0;
    dy = y1 - y0;
    push_location();
    set_pen_size();
    if (dotted) {
	numdots = sqrt(dx*dx + dy*dy) / pixperdash + 0.5;
	if (numdots > 0)
	    for (i = 0; i <= numdots; i++) {
		a = (double) i / (double) numdots;
		cx0 = ((double) x0) + (a*dx) + 0.5;
		cy0 = ((double) y0) + (a*dy) + 0.5;
		dot_at((int) cx0, (int) cy0);
	    }
    } else {
	d = sqrt(dx*dx + dy*dy);
	if (d <= 2 * pixperdash) {
	    connect(x0, y0, x1, y1);
	} else {
	    numdots = d / (2 * pixperdash) + 1;
	    spacesize = (d - numdots * pixperdash) / (numdots - 1);
	    for (i=0; i<numdots-1; i++) {
		a = i * (pixperdash + spacesize) / d;
		b = a + pixperdash / d;
		cx0 = ((double) x0) + (a*dx) + 0.5;
		cy0 = ((double) y0) + (a*dy) + 0.5;
		cx1 = ((double) x0) + (b*dx) + 0.5;
		cy1 = ((double) y0) + (b*dy) + 0.5;
		connect((int) cx0, (int) cy0, (int) cx1, (int) cy1);
		a = b;
		b = a + spacesize / d;
	    }
	    cx0 = ((double) x0) + (b*dx) + 0.5;
	    cy0 = ((double) y0) + (b*dy) + 0.5;
	    connect((int) cx0, (int) cy0, x1, y1);
	}
	EMIT(outfp, "stroke\n");
    }
    pathlen = 0;
    pop_location();
}


/*
 * Virtually draw to a given x,y position on the virtual page.
 * X and Y are expressed in thousandths of an inch, and this
 * routine converts them to pixels.
 *
 * Called as \special{pa <x> <y>}
 *	 eg:  \special{pa 0 1200}
 */
static void	ps_drawto(cp)
char *cp;
{
    int x,y;

    if (sscanf(cp, " %d %d ", &x, &y) != 2) return;
    if (++pathlen >= MAXPOINTS)
	Fatal("Too many points specified\n", 0);
    xx[pathlen] = xconv(x);
    yy[pathlen] = yconv(y);
}

static void	ps_arc(cp)
char *cp;
{
    int xc, yc, xrad, yrad;
    float start_angle, end_angle;
    short alpha0, alpha1;

    if (sscanf(cp, " %d %d %d %d %f %f ", &xc, &yc, &xrad, &yrad, &start_angle, &end_angle) != 6) return;
    push_location();
    set_pen_size();

    alpha0 = start_angle * RADTODEG + 0.5;
    alpha1 = end_angle * RADTODEG + 0.5;
    EMIT(outfp, "%d %d %d %d %d %d @ar\n", xconv(xc), yconv(yc), xconv((double) xrad), yconv((double) yrad), alpha0, alpha1);

    if (blacken_next == TRUE || shade_next == TRUE) {
	do_attributes();
	EMIT(outfp, "fill\n");
    } else {
	do_attributes();
	EMIT(outfp, "stroke\n");
    }
    pop_location();
}


/*
 * Create a spline through the points in the array.
 * Called like flush path (fp) command, after points
 * have been defined via pa command(s).
 *
 * eg:	\special{sp}
 */
int splinex[SPLINEPOINTS], spliney[SPLINEPOINTS], splinelen;
static void flush_spline()
{
    int xp, yp, N;
    double t1, t2, t3, w;
    int i, j, steps;

    push_location();
    set_pen_size();
    splinelen = 0;
    N = pathlen + 1;
    xx[0] = xx[1];
    yy[0] = yy[1];
    xx[N] = xx[N-1];
    yy[N] = yy[N-1];
    for (i = 0; i < N-1; i++) {	/* interval */
	steps = (dist(xx[i],yy[i], xx[i+1],yy[i+1]) +
		dist(xx[i+1],yy[i+1], xx[i+2],yy[i+2])) / 20;
	for (j = 0; j < steps; j++) {	/* points within */
		w = ((double) j) / ((double) steps);
		t1 = 0.5 * w * w;
		w -= 0.5;
		t2 = 0.75 - w * w;
		w -= 0.5;
		t3 = 0.5 * w * w;
		xp = t1 * xx[i+2] + t2 * xx[i+1] + t3 * xx[i] + 0.5;
		yp = t1 * yy[i+2] + t2 * yy[i+1] + t3 * yy[i] + 0.5;
		if (splinelen >= SPLINEPOINTS)
		    Fatal("Too many points in spline\n", 0);
		splinex[splinelen] = xp;
		spliney[splinelen++] = yp;
	}
    }
    EMIT(outfp, "%d %d p\n", splinex[0], spliney[0]);
    for (i=1; i<splinelen; i++) {
	EMIT(outfp, "%d %d l\n", splinex[i], spliney[i]);
    }

    pathlen = 0;
    EMIT(outfp, "@bk stroke\n");
    pop_location();
}


static int dist(x1, y1, x2, y2)	/* integer distance from x1,y1 to x2,y2 */
{
	double dx, dy;

	dx = x2 - x1;
	dy = y2 - y1;
	return sqrt(dx*dx + dy*dy) + 0.5;
}


/*
 * Whiten the interior of the next figure (path).  Command is:
 *	\special{wh}
 */
static void ps_whiten()
{
    whiten_next = TRUE;
}


/*
 * Blacken the interior of the next figure (path).  Command is:
 *	\special{bk}
 */
static void ps_blacken()
{
    blacken_next = TRUE;
}


/*
 * Shade the interior of the next figure (path) with the predefined
 * texture.  Command is:
 *	\special{sh}
 */
static void ps_shade()
{
    shade_next = TRUE;
}
