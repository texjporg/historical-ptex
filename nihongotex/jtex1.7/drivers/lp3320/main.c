#include	<stdio.h>
#include	<signal.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	"dvi2ricoh.h"
#include	"font.h"

extern char	*defaultpath;
extern int	resolution;

char		*fpath;

char		outbuffer[BUFFERSIZE];

struct status	*stack;
int		stacklimit;
struct status	curstat;
struct devpos	devstat;
int		count[10];
struct fontinfo	*curfont;
struct fontinfo	*devfont;

int		num;
int		den;
int		mag;
int		usermag = 1000;
int		factor;

int		totalpage;
char		jobname[MAXJOBNAME];
int		pageno;

int		firstpage;
int		lastpage;
int		ncopies = 1;

int		fpageflag;
int		lpageflag;
int		warnflag;
int		iflag;
int		rflag;
int		sflag;
char		*tempfile;

int		verbose;
int		curpage;
int		prevpage;

FILE		*outfp = stdout;
FILE		*dvifp = stdin;

int		initflag;

char		*fontpath[MAXPATH + 1];

char		*progname;
char		*loginname;
char		*hostname = "";

char		*infile;
char		*acctfile;

char *
getfilename(str)
	char	*str;
{
	char		*p;
	extern char	*rindex();

	p = rindex(str, '/');
	if (p == NULL)
		return str;
	return p + 1;
}

void
getcmdoption(str)
	char	*str;
{
	register char	*p;
	extern char	*rindex();

	if (strncmp(str, "dvi2", 4) == 0)
		iflag = FALSE;
	else
		iflag = TRUE;
	p = rindex(str, '-');
	if (p == NULL)
		return;
	do {
		switch (*p) {
		case 'r':
			rflag = TRUE;
			break;
		case 'i':
			iflag = TRUE;
			break;
		case 's':
			sflag = TRUE;
			break;
		}
	} while (*++p != '\0');
}

main(ac, av)
	int	ac;
	char	**av;
{
	register int	i;
	register char	*path, *ptr;
	extern int	optind;
	extern char	*optarg;
	extern char	*getenv();
	extern char	*rindex();
	extern double	actualfactor();
	extern void	onsignal();
	extern void	getoption();

	progname = getfilename(*av);

	if ((fpath = getenv("TEXFONTS")) == NULL)
		fpath = defaultpath;

	signal(SIGINT, onsignal);
	signal(SIGHUP, onsignal);
	signal(SIGTERM, onsignal);
	signal(SIGKILL, onsignal);
	signal(SIGQUIT, onsignal);
	getcmdoption(progname);
	if (!iflag)
		sflag = TRUE;
	getoption(ac, av);

	setbuffer(outfp, outbuffer, sizeof(outbuffer));

	path = fpath;
	i = 0;
	while (i < MAXPATH) {
		fontpath[i] = path;
		while (*path != ':')
			if (*path++ == '\0') {
				if (path - fontpath[i] >= 2)
					i++;
				goto breakfor;
			}
		*path++ = '\0';
		if (path - fontpath[i] >= 2)
			i++;
	}
breakfor:
	fontpath[i] = NULL;

	/*
	 * initialize font open cache
	 */
	closeallfont();

	/*
	 * read preamble
	 */
	readpre(dvifp);
	if (!rflag)
		curpage = ftell(dvifp);
	/*
	 * read postamble
	 */
	i = readpost(dvifp);
	if (rflag)
		curpage = i;

	mag = mag * actualfactor(usermag);
	factor = calcconvfact(num, den, mag, resolution);

	i = -1;
	fseek(dvifp, curpage, 0);
	for (;;) {
		switch (cmd(dvifp, 0)) {
		case 0:
			goto pass2;
		case 1:
			if (i < 0)
				i = curpage;
			break;
		case 2:
			break;
		case 3:
			goto pass2;
		}
		if (rflag) {
			if (prevpage == -1)
				goto pass2;
			fseek(dvifp, prevpage, 0);
		}
	}
pass2:
	if (i < 0) {
		/*
		 * nothing to be printed
		 */
		doexit(0);
	}
	fseek(dvifp, i, 0);
	download();
	for (;;) {
		switch (cmd(dvifp, 1)) {
		case 0:
		case 3:
			goto passend;
		}
		if (rflag) {
			if (prevpage == -1)
				goto passend;
			fseek(dvifp, prevpage, 0);
		}
	}
passend:
	delallfont();
	mesgend();
	if (!iflag)
		acct();
	doexit(0);
}

void
getoption(ac, av)
	int	ac;
	char	**av;
{
	float		x;
	register int	c;
	struct stat	buf;
	extern FILE	*makeseekable();

	while ((c = getopt(ac, av, "F:X:Y:a:c:f:h:m:n:rt:v:wx:y:")) != EOF) {
		switch (c) {
		case 'F':
			if (!iflag)
				goto unknown;
			infile = optarg;
			break;
		case 'X':
			/*
			 * x offset
			 */
			if (!iflag)
				goto unknown;
			if (sscanf(optarg, "%g", &x) != 1)
				fatal("invalid x offset(%s)", optarg);
			xmargin += x / 2.54 * resolution;
			break;
		case 'Y':
			/*
			 * x offset
			 */
			if (!iflag)
				goto unknown;
			if (sscanf(optarg, "%g", &x) != 1)
				fatal("invalid y offset(%s)", optarg);
			ymargin += x / 2.54 * resolution;
			break;
		case 'a':
			/*
			 * font area paths
			 */
			if (!iflag)
				goto unknown;
			fpath = optarg;
			break;
		case 'c':
			/*
			 * next arg is number of copies to print
			 */
			if (!iflag)
				goto unknown;
			if (sscanf(optarg, "%d", &ncopies) != 1)
				fatal("invalid copy number(%s)", optarg);
			if (ncopies <= 0)
				ncopies = 1;
			break;
		case 'f':
			/*
			 * next arg is starting pagenumber
			 */
			if (!iflag)
				goto unknown;
			if (sscanf(optarg, "%d", &firstpage) != 1)
				fatal("invalid begin page number(%s)", optarg);
			fpageflag = TRUE;
			break;
		case 'h':
			if (iflag)
				goto unknown;
			hostname = optarg;
			break;
		case 'm':
			/*
			 * specify magnification to use
			 */
			if (!iflag)
				goto unknown;
			switch (*optarg) {
			default:
				if(sscanf(optarg, "%d", &usermag) != 1)
					fatal("invalid mag(%s)", optarg);
				if (usermag < 10)
					fatal("invalid mag(%d)", usermag);
				break;
			case '0': 
				usermag = 1000; 
				break;
			case 'h': 
				usermag = 1095; 
				break;
			case '1': 
				usermag = 1200; 
				break;
			case '2': 
				usermag = 1440; 
				break;
			case '3': 
				usermag = 1728; 
				break;
			case '4': 
				usermag = 2074; 
				break;
			case '5': 
				usermag = 2488; 
				break;
			}
			break;
		case 'n':
			if (iflag)
				goto unknown;
			loginname = optarg;
			break;
		case 't':
			/*
			 * next arg is ending pagenumber
			 */
			if (!iflag)
				goto unknown;
			if(sscanf(optarg, "%d", &lastpage) != 1)
				fatal("invalid end of page(%s)", optarg);
			lpageflag = TRUE;
			break;
		case 'v':
			/*
			 * verbose operation
			 */
			if (!iflag)
				goto unknown;
			verbose = atoi(optarg) + 1;
			break;
		case 'w':
			/*
			 * don't print warnings
			 */
			if (!iflag)
				goto unknown;
			warnflag = TRUE;
			break;
		case 'x':
			/*
			 * # of x pixels
			 */
			break;
		case 'y':
			/*
			 * # of y pixels
			 */
			break;
		case 'r':
			/*
			 * reverse order
			 */
			rflag = !rflag;
			break;
		default:
unknown:
			fatal("unknown option '%c'", c);
			/* never return */
		}
	}
	if (optind < ac) {
		if (iflag)
			infile = av[optind];
		else
			acctfile = av[optind];
	}

	if (infile != NULL) {
		if ((dvifp = fopen(infile, "r")) == NULL)
			fatal("can't open DVI file '%s'", infile);
	}

	if (fstat(fileno(dvifp), &buf) < 0)
		fatal("can not get status of the file");
	
	if ((buf.st_mode & S_IFMT) != S_IFREG) {
		if (!iflag)
			fatal("invalid input file");
		/*
		 * make the file seekable
		 */
		dvifp = makeseekable(dvifp);
	}
}

FILE *
makeseekable(fp)
	register FILE	*fp;
{
	register FILE	*newfp;
	register int	c;
	extern char	*mktemp();

	tempfile = mktemp("/tmp/DviXXXXXX");
	if (tempfile == NULL)
		fatal("can not allocate temporary file");
	newfp = fopen(tempfile, "w");
	if (newfp == NULL)
		fatal("can not make temporary file");
	while ((c = getc(fp)) != EOF)
		putc(c, newfp);
	fclose(newfp);
	newfp = fopen(tempfile, "r");
	if (newfp == NULL)
		fatal("can not open temporary file");
	return newfp;
}

void
onsignal()
{
	feedpage();
	if (!iflag)
		acct();
	doexit(0);
}

doexit(code)
	int	code;
{
	if (tempfile != NULL)
		unlink(tempfile);
	exit(code);
}

acct()
{
	FILE	*fp;

	if (loginname != NULL && acctfile != NULL &&
	    access(acctfile, 2) >= 0 &&
	    (fp = fopen(acctfile, "a")) != NULL) {
		fprintf(fp, "%7.2f\t%s:%s\n",
			(float)(pageno * ncopies), hostname, loginname);
		fclose(fp);
	}
}
