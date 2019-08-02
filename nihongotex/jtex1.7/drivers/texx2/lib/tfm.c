/*
 * Copyright (c) 1987 University of Maryland Department of Computer Science.
 * All rights reserved.  Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /home/reed/grunwald/Projects/Iptex/lib/RCS/tfm.c,v 1.4 89/02/13 14:31:26 grunwald Exp Locker: grunwald $";
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

char	*malloc();

#define	ALLOC(n, type)	((type *) malloc((unsigned) ((n) * sizeof (type))))

static trd_header();
#ifdef ASCIIJTEX /* kagotani */
static trd_ct();
#endif /* ASCIIJTEX */
static trd_ci();
static trd_fix();

int
readtfmfile(f, t, stopafterwidth)
	register FILE *f;
	register struct tfmdata *t;
	int stopafterwidth;	/* ??? */
{
#ifdef ASCIIJTEX /* kagotani */
	i32 nt;
#ifdef FAKEKANJI
	i32 *t_italic, *t_kern;
#endif /* FAKEKANJI */
#endif /* ASCIIJTEX */
	i32 nc;

	if (trd_header(f, &t->t_hdr))
		return (-1);
#ifdef ASCIIJTEX /* kagotani */
	nt = t->t_hdr.th_nt;
#endif /* ASCIIJTEX */
	nc = t->t_hdr.th_ec - t->t_hdr.th_bc + 1;

#ifdef ASCIIJTEX /* kagotani */
	t->t_ct = NULL;
#endif /* ASCIIJTEX */
	t->t_ci = NULL;
	t->t_width = NULL;
	t->t_height = NULL;
	t->t_depth = NULL;
#if defined(ASCIIJTEX) && defined(FAKEKANJI) /* kagotani */
	t_italic = NULL;
	t->t_gk = NULL;
	t_kern = NULL;
	t->t_glue = NULL;
#endif /* ASCIIJTEX && FAKEKANJI*/

	(void) fseek(f, t->t_hdr.th_lh * 4L, 1);	/* XXX */

#ifdef ASCIIJTEX /* kagotani */
	if ((t->t_ct = ALLOC(nt, struct char_type_word)) == NULL ||
	    trd_ct(f, nt, t->t_ct))
		goto bad;
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
#if defined(ASCIIJTEX) && defined(FAKEKANJI) /* kagotani */
	if ((t_italic = ALLOC(t->t_hdr.th_ni, i32)) == NULL ||
	    trd_fix(f, t->t_hdr.th_ni, t_italic) ||		/* dummy */
	    (t->t_gk = ALLOC(t->t_hdr.th_nl, struct glue_kern_word)) == NULL ||
	    trd_fix(f, t->t_hdr.th_nl, t->t_gk) ||
	    (t_kern = ALLOC(t->t_hdr.th_nk, i32)) == NULL ||
	    trd_fix(f, t->t_hdr.th_nk, t_kern) ||		/* dummy */
	    (t->t_glue = (struct glue_3words *)ALLOC(t->t_hdr.th_ne, i32)) == NULL ||
	    trd_fix(f, t->t_hdr.th_ne, t->t_glue))
		goto bad;
	if (t_italic != NULL) {
	  free((char *) t_italic); (char *) t_italic = 0;
	}
	if (t_kern != NULL) {
	  free((char *) t_kern); (char *) t_kern = 0;
	}
#endif /* ASCIIJTEX && FAKEKANJI*/
	return (0);
	
      bad:
	if (t->t_ci != NULL) {
#ifndef gould
	  free((char *) t->t_ci); (char *) t->t_ci = 0;
	}
	if (t->t_width != NULL) {
	  free((char *) t->t_width); (char *) t->t_width = 0;
	}
	if (t->t_height != NULL) {
	  free((char *) t->t_height); (char *) t->t_height = 0;
	}
	if (t->t_depth != NULL) {
	  free((char *) t->t_depth); (char *) t->t_depth = 0;
	}
#ifdef ASCIIJTEX /* kagotani */
	if (t->t_ct != NULL) {
	  free((char *) t->t_ct); (char *) t->t_ct = 0;
	}
#ifdef FAKEKANJI
	if (t_italic != NULL) {
	  free((char *) t_italic); (char *) t_italic = 0;
	}
	if (t->t_gk != NULL) {
	  free((char *) t->t_gk); (char *) t->t_gk = 0;
	}
	if (t_kern != NULL) {
	  free((char *) t_kern); (char *) t_kern = 0;
	}
	if (t->t_glue != NULL) {
	  free((char *) t->t_glue); (char *) t->t_glue = 0;
	}
#endif /* FAKEKANJI */
#endif /* ASCIIJTEX */
#else
	  free((char *) t->t_ci);  t->t_ci = 0;
	}
	if (t->t_width != NULL) {
	  free((char *) t->t_width); t->t_width = 0;
	}
	if (t->t_height != NULL) {
	  free((char *) t->t_height); t->t_height = 0;
	}
	if (t->t_depth != NULL) {
	  free((char *) t->t_depth); t->t_depth = 0;
	}
#ifdef ASCIIJTEX /* kagotani */
	if (t->t_ct != NULL) {
	  free((char *) t->t_ct); t->t_ct = 0;
	}
#ifdef FAKEKANJI
	if (t_italic != NULL) {
	  free((char *) t_italic); t_italic = 0;
	}
	if (t->t_gk != NULL) {
	  free((char *) t->t_gk); t->t_gk = 0;
	}
	if (t_kern != NULL) {
	  free((char *) t_kern); t_kern = 0;
	}
	if (t->t_glue != NULL) {
	  free((char *) t->t_glue); t->t_glue = 0;
	}
#endif /* FAKEKANJI */
#endif /* ASCIIJTEX */
#endif
	return (-1);
      }

static int
trd_header(f, th)
	register FILE *f;
	register struct tfmheader *th;
{
	register i32 *p;

#ifdef ASCIIJTEX /* kagotani */
	fGetWord(f, th->th_id);
	if (th->th_id == 11) { /* this TFM is JFM for ASCII JTeX */
		fGetWord(f, th->th_nt);
		fGetWord(f, th->th_lf);
	} else {
		th->th_lf = th->th_id;
		th->th_id = 0;
		th->th_nt = 0;
	}
	for (p = &th->th_lf+1; p <= &th->th_np; p++)
		fGetWord(f, *p);
#else /* !ASCIIJTEX */
	for (p = &th->th_lf; p <= &th->th_np; p++)
		fGetWord(f, *p);
#endif /* !ASCIIJTEX */
	if (feof(f))
		return (-1);
	return (0);
}

#ifdef ASCIIJTEX /* kagotani */
static int
trd_ct(f, nt, ct)
	register FILE *f;
	register int nt;
	register struct char_type_word *ct;
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

#ifdef ASCIIJTEX /* kagotani */
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
