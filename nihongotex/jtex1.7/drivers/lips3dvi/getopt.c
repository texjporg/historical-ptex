/*
 *	getopt -- take option characters from argument's vector
 *
 * This is a extension of getopt(3c) routine. It allows the empty option string.
 * It is useful when one want to allow the option abbreviations.
 *
 * Synopsis: same as getopt(3c)
 *
 * Functions:
 *	Different from getopt(3c) on optional string required parameters
 *	when the global variable 'optextend' is not 0.. It *NOT* return
 *	and display error when the empty string is geven for optional string
 *	required parameter and return NULL in optarg external variable. 
 *	When external variable 'optextend' is 0, its work is same as getopt(3c)
 *
 * Example:
 *	On getopt(3c), when optstring is "s:" and argv is only "-s", it returns
 *	error condition. But this routine returns normal condition 's' and
 *	optarg external variable show NULL when optextend is NOT 0.
 *
 *		Written by H.Nagahara on Feb. 17 1988.
 *		This routine is in the public domain.
 *
 *	Modified on Aug. 21 1990 to get negative value as an argument.
 */
#ifndef	EOF
# define		EOF	(-1)
#endif
#ifndef	NULL
# define		NULL	(char *)0
#endif
#define		ERRFD	2

#if	MSDOS
# include	<string.h>
# include	<io.h>
#else
extern char	*strchr();
extern int	strlen();
#endif

#ifdef	MSDOS
void		errorprint(char*, char*, int);
#else
void		errorprint();
#endif

char		*optarg;	/* indicate optional string */
int		optind = 1,	/* indicate argument vector element number */
		opterr = 0;	/* error status display control flag */
static int	optindex = 0;	/* index in an argument */
static int	eofseen = 0;	/* once retuned EOF flag */
int		optextend = 0;	/* compatible for getopt(3c) falg */

int	getopt( argc, argv, optstring )
int	argc;		/* argument number */
char	*argv[];	/* argument vector */
char	*optstring;	/* option chars indicator */
{
	register char	*cc;	/* currentlly indicated char pointer */
	char		*optcc;	/* currentlly indicated optstring char pointer */

	if( eofseen || (optind >= argc) )
		return( EOF );

	/* set pointer to current char */
	cc = argv[optind] + optindex;
	if( optindex == 0 ) {
		if( *cc == '-' ) {
			if( (*(cc + 1) == '-') || (*(cc + 1) == '\0') ) {
				/* expresslly end of option */
				optind++;
				eofseen = 1;
				return( EOF );
			}
			else {	/* option start */
				optindex++;
				cc++;
			}
		}
		else {
			eofseen = 1;
			return( EOF );
		}
	}

	/* check option character */
	optcc = strchr( optstring, *cc );
	if( optcc == NULL ) {
		/* NOT a option */
		if( !opterr )
			errorprint( argv[0], ": illegal option -- ", *cc );
		optindex++;
		if( *(argv[optind] + optindex) == '\0' ) {
			optind++;
			optindex = 0;
		}
		return( '?' );
	}

	if( *(optcc + 1) == ':' ) {
		/* with optional string */
		if( *(cc + 1) != '\0' ) {
			/* in same argv */
			optarg = cc + 1;
			optind++;
			optindex = 0;
			return( *cc );
		}
		else {	/* in next argv or empty */
			optind++;
			optindex = 0;
			if( optextend ) {	/* extended feature */
				if( (optind >= argc) || (*(argv[optind]) == '-') ) {
					/* empty -> return normally */
					optarg = NULL;
					return( *cc );
				}
				else {
					optarg = argv[optind++];
					return( *cc );
				}
			}
			else {	/* normal feature */
				if( optind >= argc ) {
					/* empty -> cause error */
					if( !opterr )
						errorprint( argv[0], ": option requires an argument -- ", *cc );
					return( '?' );
				}
				else {
					optarg = argv[optind++];
					return( *cc );
				}
			}
		}
	}
	else {	/* no optional string option */
		optindex++;
		if( *(argv[optind] + optindex) == '\0' ) {
			optind++;
			optindex = 0;
		}
		return( *cc );
	}
}

void	errorprint( st1, st2, c )
char	*st1, *st2;
int	c;
{
	char	ln[2];

	ln[0] = (char)c;
	ln[1] = '\n';
	(void)write( ERRFD, st1, strlen(st1) );
	(void)write( ERRFD, st2, strlen(st2) );
	(void)write( ERRFD, ln,  sizeof(ln)  );
}
