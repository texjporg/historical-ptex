/*
 *
 * Copyright (C) 1987 	Pehong Chen	(phc@renoir.berkeley.edu)
 * Computer Science Division
 * University of California, Berkeley
 *
 */

#include	"mkind.h"

static	int	idx_gc;

#ifdef	KANJI
int	iskanji();
#endif


void
sort_idx()
{
	MESSAGE("Sorting entries...", "");
	idx_dc = idx_gc = 0;
	qsort((char*)idx_key, (int)idx_gt, (int)sizeof(FIELD_PTR), compare);
	MESSAGE("done (%d comparisons).\n", idx_gc);
}


static int
compare(a, b)
	FIELD_PTR	*a;
	FIELD_PTR	*b;
{
	int		i;
	int		dif;

	idx_gc++;
	IDX_DOT(CMP_MAX);

	for (i = 0; i < FIELD_MAX; i++) {
		/* compare the sort fields */
		if ((dif = compare_one((*a)->sf[i], (*b)->sf[i])) != 0)
			break;

		/* compare the actual fields */
		if ((dif = compare_one((*a)->af[i], (*b)->af[i])) != 0)
			break;
	}

	/* both key aggregates are identical, compare page numbers */
	if (i == FIELD_MAX)
		dif = compare_page(a, b);

	return(dif);
}

static int
compare_one(x, y)
	char		*x;
	char		*y;
{
	int		m;
	int		n;

	if ((x[0] == NULL) && (y[0] == NULL))
		return (0);

	if (x[0] == NULL)
		return (-1);

	if (y[0] == NULL)
		return (1);

	m = group_type(x);
	n = group_type(y);

	/* both pure digits */
	if ((m >= 0) && (n >= 0))
		return (m-n);

	/* x digit, y non-digit */
	if (m >= 0)
		return ((n == -1) ? 1 : -1);

	/* x non-digit, y digit */
	if (n >= 0)
		return ((m == -1) ? -1 : 1);

	/* strings started with a symbol (including digit) */
	if ((m == SYMBOL) && (n == SYMBOL))
		return (check_mixsym(x, y));

	/* x symbol, y non-symbol */
	if (m == SYMBOL)
		return (-1);

	/* x non-symbol, y symbol */
	if (n == SYMBOL)
		return (1);

	/* strings with a leading letter, the ALPHA type */
	return (compare_string(x, y));
}

static int
check_mixsym(x, y)
	char		*x;
	char		*y;
{
	int		m;
	int		n;

	m = ISDIGIT(x[0]);
	n = ISDIGIT(y[0]);

	if (m && !n)
		return(1);

	if (!m && n)
		return (-1);

#ifdef KANJI
	return (jstrcmp(x, y));
#else
	return (strcmp(x, y));
#endif
}


static int
compare_string(a, b)
	char		*a;
	char		*b;
{
	int		i = 0;
	int		j = 0;
#ifdef KANJI
	int		classa = 0;
	int		classb = 0;
	unsigned char		al;
	unsigned char		bl;
#else
	char		al;
	char		bl;
#endif

	while ((a[i] != NULL) || (b[j] != NULL)) {
		if (a[i] == NULL)
			return (-1);
		if (b[j] == NULL)
			return (1);
		if (letter_ordering) {
			if (a[i] == SPC)
				i++;
			if (b[j] == SPC)
				j++;
		}
#ifdef KANJI
		if (classa == 1)
			classa++;
		else
			classa = iskanji(a[i])? 1: 0;
		if (classb == 1)
			classb++;
		else
			classb = iskanji(b[i])? 1: 0;
		al = classa? a[i]&0377 : TOLOWER(a[i]);
		bl = classb? b[i]&0377 : TOLOWER(b[i]);
#else
		al = TOLOWER(a[i]);
		bl = TOLOWER(b[j]);
#endif

		if (al != bl)
			return (al - bl);
		i++;
		j++;
	}
#ifdef KANJI
	return (strcmp(a, b));
#else
	return (strcmp(a, b));
#endif
}

static int
compare_page(a, b)
	FIELD_PTR	*a;
	FIELD_PTR	*b;
{
	int		m;
	short		i = 0;

	while ((i < (*a)->count) && (i < (*b)->count) &&
	       ((m = (*a)->npg[i] - (*b)->npg[i]) == 0)) {
		i++;
	}
	if (m == 0) {
		if ((i == (*a)->count) && (i == (*b)->count)) {
			if (STREQ((*a)->encap, (*b)->encap))
				/* identical entries */
				(*b)->type = DUPLICATE;
			else if ((*(*a)->encap == idx_ropen) ||
				 (*(*b)->encap == idx_rclose))
				m = -1;
			else if ((*(*a)->encap == idx_rclose) ||
				 (*(*b)->encap == idx_ropen))
				m = 1;
		} else if ((i == (*a)->count) && (i < (*b)->count))
			m = -1;
		else if ((i < (*a)->count) && (i == (*b)->count))
			m = 1;
	}

	return (m);
}
