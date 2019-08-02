/*
 * pstex:
 *
 *	This is a BiBTeX-like program which
 *	performs figure inclusions in conjunction
 *	with a suitable set of macros.
 *
 * Neil Hunt (Neil%Teleos.com@ai.sri.com).
 *
 * Copyright (c) 1989 Teleos Research, Inc 1989.
 *
 * Anyone can use this software in any manner they choose,
 * including modification and redistribution, provided they make
 * no charge for it, and these conditions remain unchanged.
 *
 * This program is distributed as is, with all faults (if any), and
 * without any wrranty.  No author or distributor accepts responsibility
 * to anyone for the consequences of using it, or for whether it serves any
 * particular purpose at all, or any other reason.
 *
 * $Log:	pstex.c,v $
 * Revision 1.8  89/07/28  10:25:53  neil
 * Added copyright and conditions notice.
 * 
 * Revision 1.7  89/07/28  09:51:03  neil
 * Cleaned up, and removed some references to spar in pathnames.
 * 
 * Revision 1.6  89/02/10  18:40:48  neil
 * Removed dependencies on newlib.
 * Now uses ./std.h for standard definitions.
 * 
 * Revision 1.5  89/01/09  13:05:05  hunt
 * Updated to use a single .psz file per job rather than a .tps file
 * for each included figure.
 * 
 * Revision 1.4  88/12/20  11:52:58  hunt
 * General cleanup.
 *
 * Revision 1.3  88/09/27  07:04:33  hunt
 * Added a `%' comment char to end of postscriptbox output lines, to avoid
 * additional space which is otherwise left within the psbox area.
 * 
 * Revision 1.2  88/09/20  13:11:26  hunt
 * Writes `file' instead of `file.ps' into .psz file.
 * 
 * Revision 1.1  88/08/04  12:34:17  hunt
 * Initial revision
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <varargs.h>
#include "std.h"
#include "args.h"

extern FILE *		fopenp();
extern char *		getenv();
extern double		atof();

#define DEBUG
#define STRSIZE	256
#define DEFPSPATH	".:/usr/local/lib/tex/font/ps"
#define DEFTEXINPUTS	".:/usr/local/lib/tex/inputs"

char			line[1000];
char *			ps_path;
char *			texinputs;
bool			framemode = FALSE;
bool			verbose = FALSE;
FILE *			pszfile;
FILE *			logfile;

forward bool		parse();
forward bool		process();
forward char *		getarg();
forward bool		makepszentry();
forward bool		readpsfile();
forward void		printerr();
forward void		printlog();

int
main(argc, argv)
int argc;
char *argv[];
{
	char opt;
	int f;
	char auxname[STRSIZE];
	char pszname[STRSIZE];
	char logname[STRSIZE];

	/*
	 * Read paths from environment.
	 */
	if((ps_path = getenv("DVIPSPATH")) == NULL)
		ps_path = DEFPSPATH;
	if((texinputs = getenv("TEXINPUTS")) == NULL)
		texinputs = DEFTEXINPUTS;
	if(getenv("FMHOME"))
		framemode = TRUE;

	/*
	 * Parse args.
	 */
	for(f = 0; (opt = a_next(argc, argv)) != A_END; )
	{
		switch(opt)
		{
		default:
			fprintf(stderr, "%s: unrecognised option -%c\n",
			  a_prog_name, opt);
			/* FALLTHROUGH */

		case 'H':
usage:;
			fprintf(stderr, "usage: %s [-z DVIPSPATH] texfile\n",
			  a_prog_name);
			exit(1);

		case 'z':
			ps_path = a_arg(argc, argv);
			break;

		case 'f':
		case 'F':
			framemode = TRUE;
			break;

		case 'v':
			verbose = TRUE;
			break;

		case A_ARG:
			switch(f++)
			{
			case 0:
				strcpy(auxname, a_arg(argc, argv));
				strcpy(pszname, auxname);
				strcpy(logname, auxname);
				strcat(auxname, ".aux");
				strcat(pszname, ".psz");
				strcat(logname, ".plg");
				break;

			default:
				fprintf(stderr, "%s: too many filenames: %s\n",
				  a_prog_name, a_arg(argc, argv));
				goto usage;
			}
			break;
		}
	}
	if(f != 1)
		goto usage;

	if((logfile = fopen(logname, "w")) == NULL)
	{
		fprintf(stderr, "Cant open log file %s\n", logname);
		exit(-1);
	}

	if((pszfile = fopen(pszname, "w")) == NULL)
	{
		fprintf(stderr, "Cant open psz file %s\n", pszname);
		exit(-1);
	}

	if(! parse(auxname))
		exit(1);

	fclose(logfile);

	exit(0);
}

/*
 * Recurses over \jobname.aux files.
 */

bool
parse(filename)
char *filename;
{
	static int level = 0;
	FILE *fp;
	char fullname[STRSIZE];
	char args[STRSIZE];
	bool status = TRUE;

	if((fp = fopenp(texinputs, filename, fullname, "r")) == NULL)
	{
		printerr("WARNING: Cant open aux file %s\n", filename);
		return FALSE;
	}
	else
		printlog("Level %d aux file: %s\n", level++, filename);

	while(fgets(line, 1000, fp))
	{
		if(sscanf(line, " \\@input { %[^{}] } ", args) == 1)
		{
			if(! parse(args))
				status = FALSE;
			continue;
		}
		if(sscanf(line, " \\psboxaux %[^\n] ", args) == 1)
		{
			if(! process(args))
				status = FALSE;
			continue;
		}
	}

	fclose(fp);

	--level;

	return status;
}


/*
 * Processes lines in the .aux files of the form:
 *	\psboxaux{option-string}{filanem}
 * args is the rest of the line when psboxaux is recognised.
 */

bool
process(args)
char *args;
{
	char *p, *a;
	char width[STRSIZE];
	char height[STRSIZE];
	char aspect[STRSIZE];
	char scale[STRSIZE];
	char name[STRSIZE];

	width[0] = '\0';
	height[0] = '\0';
	aspect[0] = '\0';
	scale[0] = '\0';
	name[0] = '\0';

	a = args;

	if(*a++ != '{')
		goto abort;
	while(*a == ' ' || *a == '\t')
		a++;
	while(*a)
	{
		switch(*a)
		{
		case 'w':
			a = getarg(a, width);
			break;
		case 'h':
			a = getarg(a, height);
			break;
		case 'a':
			a = getarg(a, aspect);
			break;
		case 's':
			a = getarg(a, scale);
			break;
		case '}':
			break;
		default:
			goto abort;
		}
		if(a == NULL)
			goto abort;
		if(*a++ == '}')
			break;
	}
	while(*a == ' ' || *a == '\t')
		a++;
	if(*a++ != '{')
		goto abort;
	while(*a == ' ' || *a == '\t')
		a++;
	p = name;
	while(*a && *a != '}')
		*p++ = *a++;
	*p = '\0';

	return makepszentry(name, width, height, aspect, scale);

abort:
	printerr("WARNING: Bad arguments: \\psboxaux%s\n", args);
	return FALSE;
}

/*
 * Returns pointer to char after end of arg.
 * Arg ends with ',' ';' unmatched '}' or end of string.
 * Arg is terminated with '\0'.
 */

char *
getarg(p, arg)
char *p;
char *arg;
{
	int nesting;

	/*
	 * Skip rest of `xxx = '
	 */
	while((*p >= 'a' && *p <= 'z') ||
	  (*p >= 'A' && *p <= 'Z'))
		p++;
	while(*p == ' ' || *p == '\t')
		p++;
	if(*p++ != '=')
	{
		printerr("WARNING: Bad psbox format syntax (no '=' found).\n");
		return NULL;
	}
	while(*p == ' ' || *p == '\t')
		p++;

	/*
	 * Copy arg.
	 */
	for(nesting = 0;
	  *p && (nesting > 0 || (*p != ',' && *p != ';' &&*p != '}')); p++)
	{
		if(*p == '{')
			nesting++;
		else if(*p == '}')
			--nesting;
		*arg++ = *p;
	}
	*arg = '\0';

	/*
	 * Check.
	 */
	if(nesting != 0)
	{
		printerr("WARNING: Unmatched braces in \\psboxaux command\n");
		return NULL;
	}

	return p;
}

/*
 * Creates a psz file entry for a ps figure.
 * The option string has been parsed into width, height, aspect, and scale.
 */

bool
makepszentry(name, width, height, aspect, scale)
char name[];
char width[];
char height[];
char aspect[];
char scale[];
{
	FILE *psfp;
	char psname[STRSIZE];
	char fullname[STRSIZE];
	char wu[STRSIZE], hu[STRSIZE];
	double w, h, a, s;
	double bbw, bbh;

	strcpy(psname, name);
	strcat(psname, ".ps");
	if((psfp = fopenp(ps_path, psname, fullname, "r")) == NULL)
	{
		printerr("WARNING: No file %s\n", psname);
		return FALSE;
	}

	if(*width)
	{
		if(sscanf(width, "%lg%s", &w, wu) != 2)
		{
			w = 1.0;
			strcpy(wu, width);
		}
	}
	if(*height)
	{
		if(sscanf(height, "%lg%s", &h, hu) != 2)
		{
			h = 1.0;
			strcpy(hu, height);
		}
	}

	if(*width && *height)
	{
		if(*scale)
			s = atof(scale);
		else
			s = 1.0;
		fprintf(pszfile, "\\pssize{%g%s}{%g%s}{%s}%%\n",
		  s*w, wu, s*h, hu, name);
		printlog("  \\pssize{%.4g%s}{%.4g%s}{%s}\n",
		  s*w, wu, s*h, hu, name);
		if(*aspect)
			printlog("  (aspect ignored)\n");
	}
	else
	{
		if(! readpsfile(psfp, &bbw, &bbh))
		{
			printerr(
  "WARNING: Postscript file %s contains no %%%%BoundingBox\n",
			  psname);
			goto abort;
		}

		if(*width)
		{
			if(*aspect)
				a = atof(aspect);
			else
				a = bbw / bbh;
			if(*scale)
				s = atof(scale);
			else
				s = 1.0;
			fprintf(pszfile, "\\pssize{%g%s}{%g%s}{%s}%%\n",
			  s*w, wu, s*w/a, wu, name);
			printlog("  \\pssize{%.4g%s}{%.4g%s}{%s}\n",
			  s*w, wu, s*w/a, wu, name);
		}
		else if(*height)
		{
			if(*aspect)
				a = atof(aspect);
			else
				a = bbw / bbh;
			if(*scale)
				s = atof(scale);
			else
				s = 1.0;
			fprintf(pszfile, "\\pssize{%g%s}{%g%s}{%s}%%\n",
			  s*h*a, hu, s*h, hu, name);
			printlog("  \\pssize{%.4g%s}{%.4g%s}{%s}\n",
			  s*h*a, hu, s*h, hu, name);
		}
		else
		{
			if(*scale)
				s = atof(scale);
			else
				s = 1.0;
			fprintf(pszfile, "\\pssize{%gin}{%gin}{%s}%%\n",
			   s * bbw/72.0, s * bbh/72.0, name);
			printlog("  \\pssize{%.4gin}{%.4gin}{%s}\n",
			  s*bbw/72.0, s*bbh/72.0, name);
			if(*aspect)
				printlog("  (aspect ignored)\n");
		}
	}

	fclose(psfp);

	return TRUE;

abort:
	fclose(psfp);

	return FALSE;
}

/*
 * Reads the .ps file
 * to obtain the bounding box data.
 */

bool
readpsfile(psfp, wp, hp)
FILE *psfp;
double *wp;
double *hp;
{
	char line[1000];
	double x1, y1, x2, y2;
	char dummy[100];

	while(fgets(line, 1000, psfp))
	{
		if(sscanf(line, " %%%% BoundingBox : %lf %lf %lf %lf",
		  &x1, &y1, &x2, &y2) == 4)
		{
			*wp = Abs(x2 - x1);
			*hp = Abs(y2 - y1);
			return TRUE;
		}
		if(framemode && sscanf(line, " %lf %lf %lf %lf %[C] ",
		  &x1, &y1, &x2, &y2, &dummy[0]) == 5)
		{
			*wp = Abs(x2 - x1);
			*hp = Abs(y2 - y1);
			return TRUE;
		}
	}

	return FALSE;
}

void
printerr(fmt, va_alist)
char *fmt;
va_dcl
{
	va_list ap;

	va_start(ap);
	{
		vfprintf(logfile, fmt, ap);
		vfprintf(stderr, fmt, ap);
	}
	va_end(ap);
}

void
printlog(fmt, va_alist)
char *fmt;
va_dcl
{
	va_list ap;

	va_start(ap);
	{
		vfprintf(logfile, fmt, ap);
		if(verbose)
			vfprintf(stdout, fmt, ap);
	}
	va_end(ap);
}
