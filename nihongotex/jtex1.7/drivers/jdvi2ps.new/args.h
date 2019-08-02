/*
 * Argument parsing utilies.
 *
 * Neil Hunt (Neil%Teleos.com@ai.sri.com).
 *
 * Copyright (c) 1989 Teleos Research, Inc 1989.
 * Copyright (c) 1988 Schlumberger Technologies, Inc 1988.
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
 * $Log:	args.h,v $
 * Revision 1.1  89/02/10  18:41:34  neil
 * Initial revision
 * 
 * Taken from newlib library:
 * Revision 1.4  88/05/11  16:46:49  hunt
 * Added a_getc and a_ungetc.
 * 
 * Revision 1.3  88/03/17  18:31:19  hunt
 * Added a_scaled function and some scale tables.
 * 
 * Revision 1.2  88/01/12  18:42:09  hunt
 * Added a_arg(argc, argv) function for completeness and to hide a_arg_ptr.
 * Currently a_arg_ptr is not made static, so as not to break old code.
 * 
 * Revision 1.1  87/11/24  11:27:40  hunt
 * Initial revision
 */

#define A_ARG	(-1)
#define A_END	0

/*
 * a_prog_name holds the program name (argv[0]) after starting. 
 */

extern char *	a_prog_name;

/*
 * a_next:
 *	Called with argc, argv, returns one of the following:
 *		The next flag to be processed.
 *		A_ARG indicating a filename etc.
 *		A_END indicating the end of the args.
 */

extern char	a_next();

/*
 * a_getc:
 *	Returns the next char in the current word,
 *	or '\0' if no more chars in word.
 */

extern char	a_getc();

/*
 * a_ungetc:
 *	Pushes char back onto current word, if still available.
 */

extern void	a_ungetc();

/*
 * a_arg:
 *	Called with argc, argv; returns the next word or part word.
 */

extern char	*a_arg();

/*
 * a_number:
 *      Called with argc, argv, interprets the next word or part word
 *	as a number (double precision).
 *	In case there is a missing parameter, 0.0 is returned.
 */

double		a_number();

/*
 * a_integer:
 *      Called with argc, argv, interprets the next word or part word
 *	as an integer.
 *	In case there is a missing parameter, 0 is returned.
 */

int		a_integer();

/*
 * scale_table:
 *	List of scale names and factors.
 */

struct scale_table
{
	char	*scale_name;
	double	scale_factor;
};

extern struct scale_table scale_units[];

extern struct scale_table scale_inches[];
extern struct scale_table scale_meters[];

extern struct scale_table scale_seconds[];

extern struct scale_table scale_kilograms[];
extern struct scale_table scale_pounds[];

/*
 * a_scaled:
 *      Called with argc, argv, interprets the next word or part word
 *	as an integer with a units specifier.
 *	In case there is a missing parameter, 0 is returned.
 */

double		a_scaled();
