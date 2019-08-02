#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/dir.h>
#include	<sys/file.h>
#include	"font.h"

#define	roundup(x)	(int)((x) + 0.5)
#define	TRUE		1
#define	FALSE		0

char	bestname[MAXNAMLEN];
char	bestfamily[128];
int	bestpoint;
int	bestmag;
int	min_df;
int	min_dpm;

int
findfile(dptr, area, fptr)
	char		*dptr[];
	char		*area;
	struct fontinfo	*fptr;
{
	char		*name = fptr->n;
	int		mag = fptr->fontmag;
	char		*s = fptr->name;
	register char	**ptr;
	int		point;

	/*
	 * First check dir area given in DVI file
	 */
	fptr->bitmag = 1;
	if (area != NULL && *area) {
		if (findalias(area, name, mag, s, &fptr->bitmag))
			return TRUE;
		if (findindir(area, name, roundup(mag / 5.0), "pk", s))
			return TRUE;
		if (findindir(area, name, mag, "pxl", s))
			return TRUE;
		return FALSE;
	}

	/*
	 * Then check along dptr
	 */
	for (ptr = dptr; *ptr != NULL; ptr++) {
		if (findalias(*ptr, name, mag, s, &fptr->bitmag))
			return TRUE;
		if (findindir(*ptr, name, roundup(mag / 5.0), "pk", s))
			return TRUE;
		if (findindir(*ptr, name, mag, "pxl", s))
			return TRUE;
	}

	/*
	 * next check for closest magnification along dptr
	 */
	return findany(dptr, name, roundup(mag / 5.0), "pk", s);
}

int
findalias(area, name, mag, s, bitmag)
	char	*area;
	char	*name;
	int	mag;
	char	*s;
	int	*bitmag;
{
	FILE	*afp;
	char	pxlname[MAXFILENAME];
	char	pkname[MAXFILENAME];
	char	original[MAXFILENAME];
	char	alias[MAXFILENAME];
	int	amag;

	sprintf(s, "%s/ALIAS", area);
	if ((afp = fopen(s, "r")) == NULL)
		return FALSE;
	sprintf(pxlname, "%s.%dpxl", name, mag);
	sprintf(pkname, "%s.%dpk", name, roundup(mag / 5.0));
	while (!feof(afp)) {
		fscanf(afp, "%s%s%d", original, alias, &amag);
		if (amag != 1 && amag != 2)
			continue;
		if (strcmp(original, pxlname) == 0 ||
		    strcmp(original, pkname) == 0) {
			sprintf(s, "%s/%s", area, alias);
			if (!access(s, R_OK)) {
				/*
				 * found the specified file
				 */
				*bitmag = amag;
				return TRUE;
			}
		}
	}
	return FALSE;
}

int
findindir(area, name, mag, sufix, s)
	char	*area;
	char	*name;
	int	mag;
	char	*sufix;
	char	*s;
{
	FILE	*fp;
	char	buf[BUFSIZ];

	sprintf(s, "%s/SUBDIR", area);
	if (!access(s, F_OK))
		sprintf(s, "%s/%s/%s.%d%s", area, name, name, mag, sufix);
	else
		sprintf(s, "%s/%s.%d%s", area, name, mag, sufix);
	if (!access(s, R_OK))
		/*
		 * found the specified file
		 */
		return TRUE;
	/*
	 * not found
	 */
	sprintf(buf, "%s/NEEDED", area);
	if (access(buf, W_OK))
		return FALSE;
	sprintf(s, "%s.%d%s\n", name, mag, sufix);
	fp = fopen(buf, "r+");
	while (fgets(buf, sizeof(buf), fp) != NULL)
		if (strcmp(buf, s) == 0)
			goto errfound;
	fputs(s, fp);
errfound:
	fclose(fp);
	return FALSE;
}

int
strdiff(s1, s2)
	register char	*s1;
	register char	*s2;
{
	register int	diff = 0;

	while (*s1 && *s2)
		diff += abs(*s1++ - *s2++);
	while (*s1)
		diff += *s1++;
	while (*s2)
		diff += *s2++;
	return diff;
}

scanpdir(dir, name, family, point, mag, suffix)
	char	*dir;
	char	*name;
	char	*family;
	int	point;
	int	mag;
	char	*suffix;
{
	register DIR		*dfp;
	register struct direct	*drec;
	char			qfamily[128];
	int			qpoint, qmag, df;
	int			dpm, dstpm;
	char			qsuffix[128];

	if ((dfp = opendir(dir)) == NULL)
		return;

	if (point <= 0 && mag <= 0)
		dstpm = 1;
	else if (point <= 0)
		dstpm = mag;
	else if (mag <= 0)
		dstpm = point;
	else
		dstpm = point * mag;

	while (drec = readdir(dfp)) {
		qpoint = 0;
		qmag = 0;
		qsuffix[0] = '\0';
		sscanf(drec->d_name, "%[^0123456789.]%d.%d%s",
					qfamily, &qpoint, &qmag, qsuffix);
		if (strcmp(qsuffix, suffix))
			continue;
		df = strdiff(family, qfamily);
		if (df > min_df)
			continue;
		if (qpoint <= 0 && qmag <= 0)
			dpm = 1;
		else if (qpoint <= 0)
			dpm = qmag;
		else if (qmag <= 0)
			dpm = qpoint;
		else
			dpm = qpoint * qmag;

		if (dpm < dstpm)
			dpm = (dstpm * 1000) / dpm;
		else
			dpm = (dpm * 1000) / dstpm;
		if (dpm >= 1200)
			dpm *= 2;

		if (df < min_df || (df == min_df && dpm < min_dpm)) {
			sprintf(bestname, "%s/%s", dir, drec->d_name);
			strcpy(bestfamily, qfamily);
			bestpoint = qpoint;
			bestmag = qmag;
			min_df = df;
			min_dpm = dpm;
		}
	}
	closedir(dfp);
}

scandir(dir, name, family, point, mag, suffix)
	char	*dir;
	char	*name;
	char	*family;
	int	point;
	int	mag;
	char	*suffix;
{
	register DIR		*dfp;
	register struct direct	*drec;
	int			df;
	char			pdir[MAXNAMLEN];

	if (!(dfp = opendir(dir)))
		return;
	while (drec = readdir(dfp)) {
		if (drec->d_name[0] == '.')
			continue;
		sprintf(pdir, "%s/%s", dir, drec->d_name);
		scanpdir(pdir, name, family, point, mag, suffix);
	}
	closedir(dfp);
}

int
findany(dptr, name, mag, sufix, s)
	char	*dptr[];
	char	*name;
	int	mag;
	char	*sufix;
	char	*s;
{
	char	nname[MAXNAMLEN];
	char	family[128];
	int	point;
	int	i, ni;

	point = -1;
	family[0] = '\0';
	sscanf(name, "%[^0123456789.]%d", family, &point);

	bestname[0] = '\0';
	min_df = 9999999;
	min_dpm = 9999999;
	for (; *dptr != NULL; dptr++) {
		sprintf(s, "%s/SUBDIR", *dptr);
		if (!access(s, F_OK))
			scandir(*dptr, name, family, point, mag, sufix);
		else
			scanpdir(*dptr, name, family, point, mag, sufix);
	}
	if (bestname[0] == '\0')
		return FALSE;
	if (bestpoint > 0)
		sprintf(nname, "%s%d", bestfamily, bestpoint);
	else
		strcpy(nname, bestfamily);
	strcpy(s, bestname);
	if (strcmp(bestfamily, family) || bestpoint != point ||
						abs(bestmag - mag) > 2) {
		i = (mag * 4 + 3) / 6;
		ni = (bestmag * 4 + 3) / 6;
		warning("%s.%dpk(%dpxl) ==> %s.%dpk(%dpxl).",
			name, i * 3 / 2, i * 15 / 2,
			nname, ni * 3 / 2, ni * 15 / 2);
	}
	return TRUE;
}
