#include	<stdio.h>
#include	"dvi2ricoh.h"
#include	"commands.h"

void
readpre(dvifp)
	FILE	*dvifp;
{
	int	k;

	if (getuint(dvifp, 1) != PRE)
		fatal("The first byte is not PRE");
	if (getuint(dvifp, 1) != DVIFORMAT)
		fatal("Bat format");
	getuint(dvifp, 4);
	getuint(dvifp, 4);
	getuint(dvifp, 4);
	k = getuint(dvifp, 1);
	fread(jobname, 1, k, dvifp);
	jobname[k] = '\0';
	if (verbose >= 2)
		fprintf(stderr, "Header = `%s`\n", jobname);
}

int
findpost(dvifp)
	register FILE	*dvifp;
{
	register int	i;

	/*
	 * goto (eof position - 4)
	 */
	fseek(dvifp, -5, 2);
	for (;;) {
		i = getuint(dvifp, 1);
		fseek(dvifp, -2, 1);
		if (i == DVIFORMAT)
			/*
			 * found
			 */
			break;
		else if (i != 223)
			fatal("Bad end of DVI file");
	}
	fseek(dvifp, -3, 1);
	i = getsint(dvifp, 4);
	fseek(dvifp, i, 0);
	if (getuint(dvifp, 1) != POST)
		fatal("POST missing at head of postamble");
	return i;
}

int
readpost(dvifp)
	FILE	*dvifp;
{
	int	i;

	(void)findpost(dvifp);
	i = getuint(dvifp, 4);	/* last page offset */
	num = getuint(dvifp, 4);
	den = getuint(dvifp, 4);
	mag = getuint(dvifp, 4);
	getuint(dvifp, 4);	/* maximum width */
	getuint(dvifp, 4);	/* maximum height */
	stacklimit = getuint(dvifp, 2);
	stack = (struct status *)malloc(sizeof(*stack) * stacklimit);
	if (stack == NULL)
		fatal("Can not allocate stack area");
	totalpage = getuint(dvifp, 2);
	getfontdefs(dvifp);

	return i;
}
