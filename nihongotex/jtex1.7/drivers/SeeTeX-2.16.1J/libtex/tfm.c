/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/tfm.c,v 3.1 89/08/22 22:00:12 chris Exp $";
#endif

/*
 * TFM file reading routines.
 *
 * TODO:
 *	finish
 */

#include <stdio.h>
#include "types.h"
#include "fio.h"
#include "tfm.h"

static int trd_header(), trd_ci(), trd_fix();
#ifdef ASCIIJTEX
static int trd_ct();
#endif /* ASCIIJTEX */

char	*malloc();

#define	ALLOC(n, type)	((type *)malloc((unsigned)((n) * sizeof(type))))

int
readtfmfile(f, t, stopafterwidth)
	register FILE *f;
	register struct tfmdata *t;
	int stopafterwidth;	/* ??? */
{
#ifdef ASCIIJTEX
	i32 nt;
#endif /* ASCIIJTEX */
	i32 nc;

	if (trd_header(f, &t->t_hdr))
		return (-1);
	nc = t->t_hdr.th_ec - t->t_hdr.th_bc + 1;

#ifdef ASCIIJTEX
	t->t_ct = NULL;
#endif /* ASCIIJTEX */
	t->t_ci = NULL;
	t->t_width = NULL;
	t->t_height = NULL;
	t->t_depth = NULL;
#if defined(ASCIIJTEX) && defined(FAKEKANJI)
	t->t_italic = NULL;
	t->t_gk = NULL;
	t->t_kern = NULL;
	t->t_glue = NULL;
#endif /* ASCIIJTEX && FAKEKANJI*/

	(void) fseek(f, t->t_hdr.th_lh * 4L, 1);	/* XXX */

#ifdef ASCIIJTEX
	if ((t->t_ct = ALLOC(t->t_hdr.th_nt, struct char_type_word)) == NULL)
		goto bad;
	if (trd_ct(f, t->t_hdr.th_nt, t->t_ct))
		goto bad;
/*
 *	if ((t->t_ct = ALLOC(t->t_hdr.th_nt, struct char_type_word)) == NULL ||
 *	    trd_ct(f, nt, t->t_ct))
 *		goto bad;
 */
#endif /* ASCIIJTEX */
	if ((t->t_ci = ALLOC(nc, struct char_info_word)) == NULL ||
	    trd_ci(f, nc, t->t_ci) ||
	    (t->t_width = ALLOC(t->t_hdr.th_nw, i32)) == NULL ||
	    trd_fix(f, t->t_hdr.th_nw, t->t_width))
		goto bad;
	if (stopafterwidth)
		return (0);
	if ((t->t_height = ALLOC(t->t_hdr.th_nh, i32)) == NULL ||
	    trd_fix(f, t->t_hdr.th_nh, t->t_height) ||
	    (t->t_depth = ALLOC(t->t_hdr.th_nd, i32)) == NULL ||
	    trd_fix(f, t->t_hdr.th_nd, t->t_depth))
		goto bad;
#if defined(ASCIIJTEX) && defined(FAKEKANJI)
	if ((t->t_italic = ALLOC(t->t_hdr.th_ni, i32)) == NULL ||
	    trd_fix(f, t->t_hdr.th_ni, t->t_italic))
		goto bad;
	free((char *) t->t_italic); t->t_italic = 0; /* t_italic was dummy */
	if ((t->t_gk = ALLOC(t->t_hdr.th_nl, struct glue_kern_word)) == NULL ||
	    trd_fix(f, t->t_hdr.th_nl, t->t_gk))
		goto bad;
	if ((t->t_kern = ALLOC(t->t_hdr.th_nk, i32)) == NULL ||
	    trd_fix(f, t->t_hdr.th_nk, t->t_kern))
		goto bad;
	free((char *) t->t_kern); t->t_kern = 0; /* t_kern was dummy */
	if ((t->t_glue = (struct glue_3words *)ALLOC(t->t_hdr.th_ne, i32)) == NULL ||
	    trd_fix(f, t->t_hdr.th_ne, t->t_glue))
		goto bad;
#endif /* ASCIIJTEX && FAKEKANJI*/
	return (0);

bad:
#ifdef ASCIIJTEX
	if (t->t_ct != NULL)
		free((char *) t->t_ct);
#endif /* ASCIIJTEX */
	if (t->t_ci != NULL)
		free((char *) t->t_ci);
	if (t->t_width != NULL)
		free((char *) t->t_width);
	if (t->t_height != NULL)
		free((char *) t->t_height);
	if (t->t_depth != NULL)
		free((char *) t->t_depth);
#if defined(ASCIIJTEX) && defined(FAKEKANJI)
	if (t->t_italic != NULL)
		free((char *) t->t_italic);
	if (t->t_gk != NULL)
		free((char *) t->t_gk);
	if (t->t_kern != NULL)
		free((char *) t->t_kern);
	if (t->t_glue != NULL)
		free((char *) t->t_glue);
#endif /* ASCIIJTEX && FAKEKANJI*/
	return (-1);
}

static int
trd_header(f, th)
	register FILE *f;
	register struct tfmheader *th;
{
	register i32 *p;

#ifdef ASCIIJTEX
	fGetWord(f, th->th_id);
	if (th->th_id == 11) {
		fGetWord(f, th->th_nt);
		fGetWord(f, th->th_lf);
	} else {
		th->th_lf = th->th_id;
		th->th_id = 0;
		th->th_nt = 0;
	}
	for (p = &th->th_lf+1; p <= &th->th_np; p++)
#else /* !ASCIIJTEX */
	for (p = &th->th_lf; p <= &th->th_np; p++)
#endif /* !ASCIIJTEX */
		fGetWord(f, *p);
	if (feof(f))
		return (-1);
	return (0);
}

#ifdef ASCIIJTEX
static int
trd_ct(f, nt, ct)
/*
	register FILE *f;
	register int nt;
	register struct char_type_word *ct;
 */
	FILE *f;
	int nt;
	struct char_type_word *ct;
{
	while (--nt >= 0) {
		fGetWord(f, ct->ct_code);
		fGetWord(f, ct->ct_info);
		ct++;
	}
	if (feof(f))
		return (-1);
	return (0);
}
#endif /* ASCIIJTEX */

static int
trd_ci(f, nc, ci)
	register FILE *f;
	register int nc;
	register struct char_info_word *ci;
{

	while (--nc >= 0) {
		ci->ci_width = fgetbyte(f);
		ci->ci_h_d = fgetbyte(f);
		ci->ci_i_t = fgetbyte(f);
		ci->ci_remainder = fgetbyte(f);
		ci++;
	}
	if (feof(f))
		return (-1);
	return (0);
}

static int
trd_fix(f, nf, p)
	register FILE *f;
	register int nf;
	register i32 *p;
{

	while (--nf >= 0) {
		fGetLong(f, *p);
		p++;
	}
	if (feof(f))
		return (-1);
	return (0);
}

#ifdef ASCIIJTEX
i16
codetoinfo(nt, ct, code)
	int nt;
	register struct char_type_word *ct;
	register i16 code;
{
	register i16 l, m, h;

	if (nt == 0)
		return (code);

	l = 0;
	h = nt;
	m = (l + h) / 2;
	while (m > l) {
		if (ct[m].ct_code > code)
			h = m;
		else
			l = m;
		m = (l + h) / 2;
	}
	if (ct[m].ct_code == code)
		return (ct[m].ct_info);
	else
		return (0);
}
#endif /* ASCIIJTEX */
