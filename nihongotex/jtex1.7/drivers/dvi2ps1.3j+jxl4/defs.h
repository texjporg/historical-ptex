/**********************************************************************/
/************************  Global Definitions  ************************/
/**********************************************************************/

/* This version purports to drive a PostScript device (slowly) */


#define DEBUG   1			/* for massive printing of input */
					/* trace information; select by -d */
					/* option after filename: */
					/* dviview filename -d */

			/* to enable statistics reporting via -s option */
#define STATS

#define BINARYOPEN fopen		/* byte-oriented host version */

#define ARITHRSHIFT 1                   /* define if ">>" operator is a */
					/* sign-propagating arithmetic  */
					/*   right shift */
#define USEGLOBALMAG 1			/* when defined, the dvi global */
					/*   magnification is applied   */
      
/* We can leave USEGLOBALMAG undefined when we have a limited
   number of font magnifications (at 300dpi) available.  Otherwise, we
   will simply complain about missing PXL files
 */
/* #undef USEGLOBALMAG */

/* define for enabling of -c option (create output file) */
/* #define CREOPT */

#define	FONTDESCENV	"TEXFONTDESC"
#ifndef FONTDESC
#define	FONTDESC	"/usr/local/lib/dvi2ps/fontdesc"
#endif

#ifndef HDRFILE
#define	HDRFILE		"/usr/local/lib/dvi2ps/tex.ps"
#endif

#define	MAXOPEN		12	/* limit on number of open PXL files */

#define	DVIFORMAT	2

#define	STACKSIZE	100
#define	STRSIZE		257
#define	PATHLEN		1024
#define	NO_FILE		(FILE *)-1
#define	NONFONT		-1
#define	NONCHAR		-1
#define	DUMMYCHAR	0

#define	NEW(A,e)	((A *)alloc_check(malloc(sizeof(A)), e))
#define	NEWTAB(A,n,e)	((A *)alloc_check(malloc(sizeof(A)*n), e))
#define	MAGSIZE(f)	((unsigned int)(1000.0*(f) + 0.5))
#define	ROUND(f)	((int)((f) + 0.5))
#define	STREQ(s,t)	(strcmp(s,t) == 0)

typedef int BOOLEAN;
#define	TRUE		1
#define	FALSE		0


/**********************************************************************/
/***********************  Font Data Structures  ***********************/
/**********************************************************************/

struct font_entry {  /* font entry */
    int k, c, s, d, a, l;
    char n[STRSIZE];		/* FNT_DEF command parameters		*/
    int (*dev_fontdict)();	/* returns the font number in device	*/
    int (*dev_setchar)();	/* device dependent part of setchar	*/
    int (*dev_setstring)();	/* device dependent part of setstring	*/
    struct finfo *finfo;	/* font/device dependent information	*/
    struct font_entry *next;
    char name[PATHLEN];		/* full name of font file		*/
    FILE *openfile;		/* file stream (NO_FILE if none)	*/
    struct openfont_list *openlist;
    int ncdl;			/* # of different chars actually downloaded */
#ifdef STATS
    int nbpxl;			/* # of bytes of PXL data downloaded	*/
    int ncts;			/* total # of characters typeset	*/
#endif
};

struct fontop {
    char *fo_type;
    int (*fo_access)();
    void (*fo_readfontinfo)();
};

struct confop {
    char *co_name;
    void (*co_get)();
};

#define	ACC_EXACT	0
#define	ACC_SUBST	1
#define	ACC_MAGSTEP	2
struct accarg {
    int acc_mode;
    float rawmagfact;
    float actmagfact;
    int submag, reqmag;
    float stepmagfact;
    char *pv_name;	/* font name */
    int pv_mag;		/* magnification */
    char *pv_fam;	/* family */
    int pv_ds;		/* design size */
    char *pv_jsub;	/* jsubfont name */
};

