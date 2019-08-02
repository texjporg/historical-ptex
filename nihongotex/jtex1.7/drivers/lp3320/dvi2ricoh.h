#define	FALSE		0
#define	TRUE		1
#define	OK		1
#define	ERR		-1

#define	MAXJOBNAME	256

#define	MAXPATH		45
#ifndef FONTAREA
#define	FONTAREA	"/usr/lib/tex/fonts.300C:/usr/lib/tex/fonts:"
#endif

#define	MAXOPENFONT	15

#define	DVIFORMAT	2

#define	UNDEFPOS	0x80000000

#define	BUFFERSIZE	(20 * 1024)	/* 20 Kbyte */

/*
 * stack entry
 */
struct status {
	int	h;
	int	v;
	int	w;
	int	x;
	int	y;
	int	z;
};

/*
 * device status
 */
struct devpos {
	int	hh;
	int	vv;
};

extern char		*progname;

extern struct status	*stack;
extern int		stacklimit;
extern struct status	curstat;
extern struct devpos	devstat;
extern int		count[10];
extern struct fontinfo	*curfont;
extern struct fontinfo	*devfont;

extern int		num;
extern int		den;
extern int		mag;
extern int		usermag;
extern int		factor;

extern int		totalpage;
extern char		jobname[MAXJOBNAME];
extern int		pageno;

extern int		firstpage;
extern int		lastpage;
extern int		ncopies;

extern int		fpageflag;
extern int		lpageflag;
extern int		verbose;
extern int		warnflag;
extern int		sflag;

extern int		curpage;
extern int		prevpage;

extern FILE		*outfp;

extern int		initflag;

extern int		xmargin;
extern int		ymargin;

extern char		*fontpath[MAXPATH + 1];
extern struct dlfont	dlfont;
