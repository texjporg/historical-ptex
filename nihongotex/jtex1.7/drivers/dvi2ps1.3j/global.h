#ifdef SYSV
#define	index	strchr
#define	rindex	strrchr
#endif
char *malloc();
int  free();
char *index();
char *rindex();
char *sprintf();
char *strcpy();
char *getenv();

extern int FirstPage, LastPage;

extern int h, v;
extern int mag, usermag;
extern struct font_entry *fontptr;
extern struct font_entry *hfontptr;
extern char *fontdescfile;

extern char filename[];
extern FILE *dvifp;
extern char rootname[];
#ifdef CREOPT
extern char  outfname[];
#endif CREOPT
extern FILE *outfp;

extern char *Ifile[];
extern int ncopies;
extern int ndone;
extern int nif;

#ifdef DEBUG
extern int Debug;
#endif
extern int PreLoad;
extern int Reverse;
extern int G_create;
extern int G_errenc;
extern int G_header;
extern char G_Logname[];
extern int G_interactive;
extern int G_logging;
extern int G_logfile;
extern FILE *G_logfp;
extern char *G_progname;
extern int G_quiet;
extern int G_nowarn;
#ifdef STATS
extern int Stats;
extern int Snbpxl;
extern int Sonbpx;
extern int Sndc;
extern int Stnc;
extern int Snbpx0, Sndc0, Stnc0;
#endif

void AbortRun();
void AllDone();
void Fatal();
void Warning();

void openfontfile();

void copyfile();
#ifdef CREOPT
FILE *openoutput();
#endif CREOPT
void getbytes();
int getuint();	/* see cautionary note in code, re arithmetic vs logical shifts */
int getint();	/* see cautionary note in code, re arithmetic vs logical shifts */
void putint();
void putoct();

char *alloc_space();
int makeuint();
char *strsave();

float actfact();
float apprfact();
extern float mag_table[];
extern int mag_index;
extern int magtabsize;
float dev_fontmag();

void skipline();
