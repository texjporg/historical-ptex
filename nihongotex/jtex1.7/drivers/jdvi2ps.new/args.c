/*
 * Argument parsing utilies.
 *
 * Neil Hunt (Neil%Teleos.com@ai.sri.com).
 *
 * Copyright (c) 1989 Teleos Research, Inc 1989.
 * Copyright (c) 1989 Schlumberger Technologies, Inc 1989.
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
 * $Log:	args.c,v $
 * Revision 1.2  89/02/20  11:21:31  neil
 * Removed sunview.h include.
 * 
 * Revision 1.1  89/02/10  18:40:00  neil
 * Initial revision
 * 
 * Taken from newlib library:
 * Revision 1.6  88/08/08  17:54:07  hunt
 * Removed prexit from a_ungetc in case where a_arg_ptr is NULL,
 * as this case is indestinguishable from the case where a_arg_ptr
 * pointed to the end of a word (to the application).
 * 
 * Revision 1.5  88/08/08  16:54:27  hunt
 * Made a_arg_ptr etc non static, for special purpose applications such
 * as parsing window arguments from the array.
 * 
 * Revision 1.4  88/05/11  16:44:11  hunt
 * Added a_getc and a_ungetc functions.
 * 
 * Revision 1.3  88/03/17  18:29:54  hunt
 * Completely rewritten; cleaner implementation.
 * Added a_scaled function and some scale tables.
 * 
 * Revision 1.2  88/01/12  18:33:54  hunt
 * Added a_arg(argc, argv) function for completeness and to hide a_arg_ptr.
 * Currently a_arg_ptr is not made static, so as not to break old code.
 * 
 * Revision 1.1  87/11/24  11:20:01  hunt
 * Initial revision
 */

#include <stdio.h>
#include <math.h>
#include <sys/param.h>		/* For MAXPATHLEN */
#include "std.h"
#include "args.h"

static char *	a_arg_ptr = NULL;
static int	a_arg_index = 0;
static bool	a_escape_seen;
char *		a_prog_name = "Anonymous";

/*
 * a_next:
 *	Returns the next flag in the command line,
 *	or A_ARG if it is not a flag,
 *	or A_END if there are no more args.
 */

char
a_next(argc, argv)
int argc;
char **argv;
{
	char opt;

	/*
	 * Checks.
	 */
	if(argv == NULL || argc < 1)
	{
		fprintf(stderr, "a_arg: bad arguments\n");
		exit(-1);
	}

	/*
	 * Get program name on first call.
	 */
	if(a_arg_index == 0)
	{
		a_prog_name = argv[0];
		a_arg_index = 1;
	}

	/*
	 * If there is part of the previous word left, then return it.
	 */
	if(a_arg_ptr && *a_arg_ptr)
		return *a_arg_ptr++;

	/*
	 * Return A_END after the end of the list.
	 */
	if(a_arg_index >= argc)
		return A_END;

	/*
	 * Look at the next word.
	 */
	a_arg_ptr = argv[a_arg_index++];

	/*
	 * If we have seen the escape "--",
	 * or if the first char of the word * is not a '-',
	 * or if this is an isolated "-",
	 * then return ARG.
	 */
	if(a_escape_seen || a_arg_ptr[0] != '-' || a_arg_ptr[1] == '\0')
		return A_ARG;

	/*
	 * Look at the next char.
	 */
	a_arg_ptr++;
	opt = *a_arg_ptr++;

	/*
	 * If the next char is '-', then this is the escape.
	 * start over...
	 */
	if(opt == '-')
	{
		a_escape_seen = TRUE;
		return a_next(argc, argv);
	}

	/*
	 * Otherwise, return this option.
	 */
	return opt;
}

/*
 * a_arg:
 *	Returns the next argument in the command line,
 *	or NULL if there are no more args.
 */

char *
a_arg(argc, argv)
int argc;
char **argv;
{
	char *arg;

	/*
	 * Checks.
	 */
	if(argv == NULL || argc < 1)
	{
		fprintf(stderr, "a_arg: bad arguments\n");
		exit(-1);
	}

	/*
	 * Get program name on first call.
	 */
	if(a_arg_index == 0)
	{
		a_prog_name = argv[0];
		a_arg_index = 1;
	}

	/*
	 * If there is part of the previous word left, then return it.
	 */
	if(a_arg_ptr && *a_arg_ptr)
	{
		arg = a_arg_ptr;
		a_arg_ptr = NULL;
		return arg;
	}

	/*
	 * Return NULL after the end of the list.
	 */
	if(a_arg_index >= argc)
		return NULL;

	/*
	 * Return the next word.
	 */
	return argv[a_arg_index++];
}

/*
 * a_getc:
 *	Returns the next char in the current word,
 *	or NULL if no more chars in word.
 */

char
a_getc(argc, argv)
int argc;
char **argv;
{
	return a_arg_ptr ? *a_arg_ptr++ : '\0';
}

/*
 * a_ungetc:
 *	Pushes char back onto current word, if still available.
 */

void
a_ungetc(argc, argv ,c)
int argc;
char **argv;
char c;
{
	if(a_arg_ptr)
		*--a_arg_ptr = c;
}

/*
 * a_number:
 *	Interpret the next word or part word as a number.
 */

double
a_number(argc, argv)
int argc;
char **argv;
{
	char *arg;

	if((arg = a_arg(argc, argv)) == NULL)
		return 0.0;
	else
		return atof(arg);
}

/*
 * a_integer:
 *	Interpret the next word or part word as an integer.
 */

int
a_integer(argc, argv)
int argc;
char **argv;
{
	char *arg;

	if((arg = a_arg(argc, argv)) == NULL)
		return 0;
	else
		return atoi(arg);
}

/*
 * a_scale:
 *	Interpret the next word of part word as a number with scale factor.
 */

double
a_scaled(argc, argv, scale_table)
int argc;
char **argv;
struct scale_table *scale_table;
{
	char *arg;
	double val;
	char scale[100];
	int scale_len;

	val = 0.0;

	/*
	 * Get the word.
	 */
	if((arg = a_arg(argc, argv)) == NULL)
		return 0.0;

	/*
	 * Read a double value and a units specifier.
	 * If no units, just return the value.
	 */
	if(sscanf(arg, " %lf %s", &val, &scale[0]) < 2 || scale_table == NULL)
		return val;

	/*
	 * Get the length of the scale string.
	 */
	if((scale_len = strlen(scale)) <= 0)
		return val;

	/*
	 * Try to match the units specifier.
	 */
	for( ; scale_table->scale_name; scale_table++)
		if(strncmp(scale_table->scale_name, scale, scale_len) == 0)
			return val * scale_table->scale_factor;

	return val;
}

/*
 * Prepared scale tables.
 */

struct scale_table scale_units[] =
{
	{ "pico",	1e-12, },
	{ "p",		1e-12, },
	{ "nano",	1e-9, },
	{ "n",		1e-9, },
	{ "micro",	1e-6, },
	{ "u",		1e-6, },
	{ "milli",	1e-3, },
	{ "m",		1e-3, },

	{ "kilo",	1e3, },
	{ "k",		1e3, },
	{ "Mega",	1e6, },
	{ "mega",	1e6, },
	{ "M",		1e6, },
	{ "Giga",	1e9, },
	{ "giga",	1e9, },
	{ "G",		1e9, },
	{ "Tera",	1e12, },
	{ "tera",	1e12, },
	{ "T",		1e12, },

	{ (char *)NULL,	0.0, },
};

#define	IN_PER_M	39.37007874
#define M_PER_IN	0.0254

struct scale_table scale_inches[] =
{
	{ "meters",	1.0 * IN_PER_M, },	/* Must come first */
	{ "metres",	1.0 * IN_PER_M, },
	{ "angstroms",	1e-10 * IN_PER_M, },
	{ "nm",		1e-9 * IN_PER_M, },
	{ "microns",	1e-6 * IN_PER_M, },
	{ "um",		1e-6 * IN_PER_M, },
	{ "millimeters",1e-3 * IN_PER_M, },
	{ "millimetres",1e-3 * IN_PER_M, },
	{ "mm",		1e-3 * IN_PER_M, },
	{ "centimeters",1e-2 * IN_PER_M, },
	{ "centimetres",1e-2 * IN_PER_M, },
	{ "cms",	1e-2 * IN_PER_M, },
	{ "kilometers",	1e3 * IN_PER_M, },
	{ "kilometres",	1e3 * IN_PER_M, },
	{ "km",		1e3 * IN_PER_M, },
	{ "nmile",	1852 * IN_PER_M, },

	{ "mils",	0.001, },
	{ "inches", 	1.0, },
	{ "\"",		1.0, },
	{ "feet",	12.0, },
	{ "foot",	12.0, },
	{ "ft",		12.0, },
	{ "\'",		12.0, },
	{ "yards",	36.0, },
	{ "yd",		36.0, },
	{ "rod",	198, },
	{ "rd",		198, },
	{ "miles",	63360, },

	{ (char *)NULL,	0.0, },
};

struct scale_table scale_meters[] =
{
	{ "meters",	1.0, },
	{ "metres",	1.0, },
	{ "angstroms",	1e-10, },
	{ "nm",		1e-9, },
	{ "microns",	1e-6, },
	{ "um",		1e-6, },
	{ "millimeters",1e-3, },
	{ "millimetres",1e-3, },
	{ "mm",		1e-3, },
	{ "centimeters",1e-2, },
	{ "centimetres",1e-2, },
	{ "cms",	1e-2, },
	{ "kilometers",	1e3, },
	{ "kilometres",	1e3, },
	{ "km",		1e3, },
	{ "nmile",	1852, },

	{ "mils",	0.001 * M_PER_IN, },
	{ "inches", 	1.0 * M_PER_IN, },
	{ "\"",		1.0 * M_PER_IN, },
	{ "feet",	12.0 * M_PER_IN, },
	{ "foot",	12.0 * M_PER_IN, },
	{ "ft",		12.0 * M_PER_IN, },
	{ "\'",		12.0 * M_PER_IN, },
	{ "yards",	36.0 * M_PER_IN, },
	{ "yd",		36.0 * M_PER_IN, },
	{ "rod",	198 * M_PER_IN, },
	{ "rd",		198 * M_PER_IN, },
	{ "miles",	63360 * M_PER_IN, },

	{ (char *)NULL,	0.0, },
};

struct scale_table scale_seconds[] =
{
	{ "us",			1e-6, },
	{ "microseconds",	1e-6, },
	{ "ms",			1e-3, },
	{ "milliseconds",	1e-3, },
	{ "seconds",		1.0, },
	{ "secs",		1.0, },
	{ "minutes",		60.0, },
	{ "mins",		60.0, },
	{ "hours",		3600.0, },
	{ "hrs",		3600.0, },
	{ "days",		86400.0, },
	{ "weeks",		604800.0, },
	{ "wks",		604800.0, },
	{ "years",		31556925.97, },
	{ "yrs",		31556925.97, },

	{ (char *)NULL,	0.0, },
};

#define KG_PER_LB	0.45359237
#define LB_PER_KG	2.204622622

struct scale_table scale_kilograms[] =
{
	{ "ounces",		0.0625 * KG_PER_LB, },
	{ "ozs",		0.0625 * KG_PER_LB, },
	{ "pounds",		1.0 * KG_PER_LB, },
	{ "lbs",		1.0 * KG_PER_LB, },
	{ "stones",		14.0 * KG_PER_LB, },
	{ "quarters",		28.0 * KG_PER_LB, },
	{ "hundredweights",	112.0 * KG_PER_LB, },
	{ "longtons",		2240.0 * KG_PER_LB, },
	{ "shorttons",		2000 * KG_PER_LB, },
	{ "tons",		2000 * KG_PER_LB, },

	{ "milligrams",		1e-6, },
	{ "mgs",		1e-6, },
	{ "grams",		1e-3, },
	{ "gms",		1e-3, },
	{ "kilograms",		1.0, },
	{ "kgs",		1.0, },
	{ "tonnes",		1e3, },
	{ "metrictons",		1e3, },

	{ (char *)NULL,	0.0, },
};

struct scale_table scale_pounds[] =
{
	{ "ounces",		0.0625, },
	{ "ozs",		0.0625, },
	{ "pounds",		1.0, },
	{ "lbs",		1.0, },
	{ "stones",		14.0, },
	{ "quarters",		28.0, },
	{ "hundredweights",	112.0, },
	{ "longtons",		2240.0, },
	{ "shorttons",		2000, },
	{ "tons",		2000, },

	{ "milligrams",		1e-6 * LB_PER_KG, },
	{ "mgs",		1e-6 * LB_PER_KG, },
	{ "grams",		1e-3 * LB_PER_KG, },
	{ "gms",		1e-3 * LB_PER_KG, },
	{ "kilograms",		1.0 * LB_PER_KG, },
	{ "kgs",		1.0 * LB_PER_KG, },
	{ "tonnes",		1e3 * LB_PER_KG, },
	{ "metrictons",		1e3 * LB_PER_KG, },

	{ (char *)NULL,	0.0, },
};
