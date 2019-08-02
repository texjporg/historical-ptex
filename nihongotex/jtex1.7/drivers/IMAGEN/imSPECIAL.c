/*
 * Copyright 1988 dit Co., Ltd.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of dit Co., Ltd. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * Dit Co., Ltd. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * DIT CO., LTD. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL DIT CO., LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/*
 * imSPECIAL.c - SPECIAL impress code
 */

/*
 * $Id: imSPECIAL.c,v 1.3 90/03/27 11:02:33 void Exp $
 */

#include <stdio.h>
#include <math.h>
#include <imagen/imPcodes.h>
#include "config.h"
#include "fio.h"
#include "dvicom.h"
#include "special.h"

extern FILE	*out;
extern FILE	*dvi;

extern short	family;
extern		UserMag;
extern int	use_resolution;

#define CONVERT(x,y)	((int)((((double)(x)/1000.0) * ((double)(y)) * \
			((double)1000.000/1000.000)) + 0.5))
			/* ((double)UserMag/1000.000)) + 0.5)) */

#define	OR	7
#define WHITE	0
#define SHADE	3
#define BLACK	15
#define UNKNOWN	WHITE

#define DEFAULT_PEN_SIZE    2
#define MAX_PEN_SIZE	    20
#define	MAX_POINTS	    300
#define	SPLINE_POINTS	    900
#define	RADTOPXL	    2607.435436
#define	TWOPI		    (3.14157926536*2.0)

#define FNUM	family
#define	THEIGHT	32		/* bits high */
#define	TWIDTH	4		/* bytes wide */

hidden int  xx[MAX_POINTS], yy[MAX_POINTS];
hidden int  pen_size  = DEFAULT_PEN_SIZE;
hidden int  path_tone = UNKNOWN;
hidden int  path_len;
static int  family_defined = FALSE;
static int  texture_defined = FALSE;

/*
 * \specail{pn <size>}:  Set the pen size.
 *
 * The size is the number of milli-inches for the diameter of the pen.
 * This routine converts that value to device-dependent pixels,
 * and makes sure that the resulting value is with in legal bounds.
 */

static char stexture[THEIGHT][TWIDTH]={
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}
};

/*
 * Copy a default texture into the stexture array
 */
static void
init_Glyph()
{
    static char btexture[THEIGHT][TWIDTH] = {
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}
    };

    register int i;

    for (i = 0; i < THEIGHT; i++)
	bcopy(btexture[i], stexture[i], TWIDTH);
}

hidden
im_PEN(s)
char *s;
{
    int size;

    if (sscanf(s, "%d", &size) != 1) {
	(void) fprintf(stderr, "! Special error: `pn': unknown pen_size.\n");
	return;
    }

    pen_size = CONVERT(size, use_resolution);

    if (pen_size < 1)
	pen_size = 1;
    else if (pen_size > MAX_PEN_SIZE)
	pen_size = MAX_PEN_SIZE;

#if MESSAGE
    (void) fprintf(stderr, "im_PEN: pen_size = %d\n", pen_size);
#endif
}

hidden
im_PATH(s)
char *s;
{
    int x, y;

    if (sscanf(s, "%d %d", &x, &y) != 2) {
	(void) fprintf(stderr,"! Special error: `pa'.\n");
	return;
    }

    if (++path_len >= MAX_POINTS) {
	(void) fprintf(stderr,"! Special error: `pa': too many points.\n");
	return;
    }

    xx[path_len] = CONVERT(x, use_resolution);
    yy[path_len] = CONVERT(y, use_resolution);

#if MESSAGE
    (void) fprintf(stderr, "im_PATH: xx[%d] = %d, yy[%d] = %d\n",
			path_len, xx[path_len], path_len, yy[path_len]);
#endif
}

hidden
im_DRAW_DOT(s)
char *s;
{
#if MESSAGE
    (void) fprintf(stderr, "im_DRAW_DOT:\n");
#endif

    Draw_Dash_or_Dot(s, 1);
}

hidden
im_DRAW_DASH(s)
char *s;
{
#if MESSAGE
    (void) fprintf(stderr, "im_DRAW_DASH:\n");
#endif

    Draw_Dash_or_Dot(s, 0);
}

Draw_Dash_or_Dot(s, dotted)
char *s;
int dotted;
{
    int    x0, y0, x1, y1, i, numdots;
    double cx0, cy0, cx1, cy1;
    double d, a, b, dx, dy, pix_per_dash, space_size;
    float  inches_per_dash;

    if (sscanf(s, "%f", &inches_per_dash) != 1) {
    	(void) fprintf(stderr, "! Special error: 'da'.\n");
	return;
    }

    if (path_len <= 1) {
    	(void) fprintf(stderr, "! Special error:da:path_len is too short.\n");
	return;
    }

    pix_per_dash = inches_per_dash * ((float) use_resolution);

    x0 = xx[1];
    x1 = xx[2];
    y0 = yy[1];
    y1 = yy[2];

    dx = x1 - x0;
    dy = y1 - y0;
    Push_Location();

    if (dotted) {
	numdots = sqrt(dx * dx + dy * dy) / pix_per_dash + 0.5;
	if (numdots > 0) {
	    for (i = 0; i <= numdots; ++i) {
		a = (float) i / (float) numdots;
		cx0 = ((float) x0) + (a * dx) + 0.5;
		cy0 = ((float) y0) + (a * dy) + 0.5;
		dot_at((int) cx0, (int)cy0);
	    }
	}
    }
    else {
	d = sqrt(dx * dx + dy * dy);
	if (d <= 2 * pix_per_dash) {
	    cat(x0, y0, x1, y1);
	    path_len = 0;
	    Pop_Location();
	    return;
	}
	numdots = d / (2 * pix_per_dash) + 1;
	space_size = (d - numdots * pix_per_dash) / (numdots - 1);

	for (i = 0; i < numdots - 1; ++i) {
	    a = i * (pix_per_dash + space_size) / d;
	    b = a + pix_per_dash / d;
	    cx0 = ((float) x0) + (a * dx) + 0.5;
	    cy0 = ((float) y0) + (a * dy) + 0.5;
	    cx1 = ((float) x0) + (a * dx) + 0.5;
	    cy1 = ((float) y0) + (a * dy) + 0.5;
	    cat((int)cx0, (int)cy0, (int)cx1, (int)cy1);
	    b += space_size / d;
	}
	cx0 = ((float) x0) + (b * dx) + 0.5;
	cy0 = ((float) y0) + (b * dy) + 0.5;
	cat((int)cx0, (int)cy0, x1, y1);
    }
    path_len = 0;
    Pop_Location();
}

/*
 * \special{ar <x-center> <y-center> <x-radius> <y-radius>
 *             <start-angle> <end-angle>}
 *
 * (1) [xy]-center, [xy]-radius are in 1/1000's of an inch.
 * (2) [start, end]-angle are in radians.
 */

hidden
im_ARC(s)
char *s;
{
    int    xc, yc, xr, yr;
    float  s_angle, e_angle;
    double x_center, y_center, x_radius, y_radius;
    double angle, theta, r;
    int    n;

    if (sscanf(s, "%d %d %d %d %f %f",
	&xc, &yc, &xr, &yr, &s_angle, &e_angle) != 6) {
	(void) fprintf(stderr, "! Special error: 'ar'.\n");
	return;
    }

#if MESSAGE
    (void) fprintf(stderr, "im_ARC:\n");
    (void) fprintf(stderr, "       x_center = %d, y_center = %d\n", xc, yc);
    (void) fprintf(stderr, "       x_radius = %d, y_radius = %d\n", xr, yr);
    (void) fprintf(stderr, "       start_angle = %f, end_angle = %f\n",
	    s_angle, e_angle);
#endif

    x_center = xc;
    y_center = yc;
    x_radius = xr;
    y_radius = yr;

    r = (x_radius + y_radius) / 2.0;
    theta = sqrt(1.0 / r);
    n = TWOPI / theta + 0.5;

    if (n < 6)
	n = 6;
    if (n > 80)
	n = 80;

    theta = TWOPI / n;

    Fill_Draw(x_center + x_radius * cos(s_angle),
		y_center + y_radius * sin(s_angle));

    angle = s_angle + theta;

    while (angle < e_angle) {
	Fill_Draw(x_center + x_radius * cos(angle),
		    y_center + y_radius * sin(angle));
	angle += theta;
    }

    Fill_Draw(x_center + x_radius * cos(e_angle),
		y_center + y_radius * sin(e_angle));

    im_FLUSH("dummy");
}

hidden
Fill_Draw(x, y)
float x, y;
{
    int	im_x, im_y;
    char buf[BUFSIZ];

    im_x = (int) x + 0.5;
    im_y = (int) y + 0.5;
    (void) sprintf(buf, "%d %d", im_x, im_y);
    im_PATH(buf);
}

hidden
im_SPLINE(s)
char *s;
{
    int xp, yp, n;
    int i, j, step;
    int x_spline[SPLINE_POINTS], y_spline[SPLINE_POINTS], spline_len;
    float t1, t2, t3, w;

    Push_Location();
    Set_Pen_Size();
    (void) Put1Byte(iC_PATH, out);

    spline_len = 0;
    n = path_len + 1;
    xx[0] = xx[1];
    yy[0] = yy[1];
    xx[n] = xx[n - 1];
    yy[n] = yy[n - 1];

#if MESSAGE
    (void) fprintf(stderr, "im_SPLINE\n");
#endif

    for (i = 0; i < n - 1; ++i) {
	step = (dist(xx[i], yy[i], xx[i + 1], yy[i + 1]) +
		 dist(xx[i + 1], yy[i + 1], xx[i + 2], yy[i + 2])) / 20;

	for (j = 0; j < step; ++j) {
	    w = ((float) j) / ((float) step);
	    t1 = 0.5 * w * w;
	    w -= 0.5;
	    t2 = 0.75 - w * w;
	    w -= 0.5;
	    t3 = 0.5 * w * w;
	    xp = t1 * xx[i + 2] + t2 * xx[i + 1] + t3 * xx[i] + 0.5;
	    yp = t1 * yy[i + 2] + t2 * yy[i + 1] + t3 * yy[i] + 0.5;

	    if (spline_len >= SPLINE_POINTS) {
		(void) fprintf(stderr, "! Special error: `spline'.\n");
		return;
	    }
	    x_spline[spline_len] = xp;
	    y_spline[spline_len++] = yp;
	}
    }

#if MESSAGE
    (void) fprintf(stderr, "spline_len = %d\n", spline_len);
    for (i = 0; i < spline_len; ++i) {
	(void) fprintf(stderr, "x_spline[%d] = %d\n", i, x_spline[i]);
	(void) fprintf(stderr, "y_spline[%d] = %d\n", i, y_spline[i]);
    }
#endif
    (void) Put2Byte(spline_len, out);

    for (i = 0; i < spline_len; ++i) {
	(void) Put2Byte(x_spline[i], out);
	(void) Put2Byte(y_spline[i], out);
    }

    path_len = 0;
    (void) Put1Byte(iDRAW_PATH, out);
    (void) Put1Byte(BLACK, out);
    Pop_Location();
}

hidden int
dist(x1, y1, x2, y2)
int  x1, y1, x2, y2;
{
    float dx, dy;

    dx = x2 - x1;
    dy = y2 - y1;
    return (sqrt(dx * dx + dy + dy) + 0.5);
}

hidden
im_SHADE(s)
char *s;
{
#if MESSAGE
    (void) fprintf(stderr, "im_SHADE:\n");
#endif
    path_tone = SHADE;
}

hidden
im_WHITEN(s)
char *s;
{
#if MESSAGE
    (void) fprintf(stderr, "im_WHITE:\n");
#endif
    path_tone = WHITE;
}

hidden
im_BLACKEN(s)
char *s;
{
#if MESSAGE
    (void) fprintf(stderr, "im_BLACK:\n");
#endif
    path_tone = BLACK;
}

hidden
im_TEXTURE(s)
char *s;
{
#if MESSAGE
    (void) fprintf(stderr, "im_TEXTURE:\n");
#endif
}

hidden
Set_Pen_Size()
{
    (void) Put1Byte(iSET_PEN, out);
    (void) Put1Byte(pen_size, out);
}

hidden
Pop_Location()
{
    (void) Put1Byte(iPOP, out);
}

hidden
Push_Location()
{
    (void) Put1Byte(iPUSH, out);
    (void) Put1Byte(iSET_HV_SYSTEM, out);
    (void) Put1Byte(0140, out);
}

/*
 * \special{fp}: Flush the path.
 * Flush the path that we've built up with im_DRAW_XXX.
 */
hidden
im_FLUSH(s)
char *s;
{
    register int i;

#if MESSAGE
    (void) fprintf(stderr, "im_FLUSH\n");
#endif

    Push_Location();

    if (path_len <= 0)
	return;

    Set_Pen_Size();
    (void) Put1Byte(iC_PATH, out);
    (void) Put2Byte(path_len, out);

    for (i = 1; i <= path_len; ++i) {
	(void) Put2Byte(xx[i], out);
	(void) Put2Byte(yy[i], out);
    }

    path_len = 0;
    (void) Put1Byte(iDRAW_PATH, out);
    (void) Put1Byte(BLACK, out);
	do_attributes();
    Pop_Location();
}
hidden
im_FILES(s)
char *s;
{
    register int count, fd;
    char buf[BUFSIZ];

    if ((fd = open(s, READMODE)) == -1) {
	(void) fprintf(stderr,
	    "! Can't open the (%s) file for \\special...ignore\n", s);
	return;
    }

    while ((count = read(fd, buf, BUFSIZ)) > 0)
	(void) fwrite(buf, sizeof(char), count, out);

    (void) close(fd);
}

hidden
cat(x0, y0, x1, y1)
int x0, y0, x1, y1;
{
    Set_Pen_Size();
    (void) Put1Byte(iC_PATH, out);
    (void) Put2Byte(2, out);
    (void) Put2Byte(x0, out);
    (void) Put2Byte(y0, out);
    (void) Put2Byte(x1, out);
    (void) Put2Byte(y1, out);
    (void) Put1Byte(iDRAW_PATH, out);
    (void) Put1Byte(BLACK, out);
}

hidden
dot_at(x, y)
int x, y;
{
    Set_Pen_Size();
    (void) Put1Byte(iC_PATH, out);
    (void) Put2Byte(1, out);
    (void) Put2Byte(x, out);
    (void) Put2Byte(y, out);
    (void) Put1Byte(iDRAW_PATH, out);
    (void) Put1Byte(BLACK, out);
}

hidden
do_attributes()
{
    static int Family;
    static int Member;
    int i, j;

#if MESSAGE
    (void) fprintf(stderr, "in do_attribues:");
#endif

    switch (path_tone) {
    case SHADE:
#if MESSAGE
    (void) fprintf(stderr, " SHADE\n");
#endif
	path_tone = UNKNOWN;
	if (!family_defined) {
	    family_defined = TRUE;
	    Family = FNUM++;
	    Member = -1;
	}
	if (!texture_defined) {
	    texture_defined = TRUE;
	    Member++;
	    (void) Put1Byte(iBGLY, out);
	    (void) Put1Byte((Family & 0x7e) >> 1, out);
	    (void) Put1Byte((Family & 0x01) << 7 | (Member & 0x7f), out);
	    (void) Put2Byte(32, out);
	    (void) Put2Byte(TWIDTH * 8, out);
	    (void) Put2Byte(32, out);
	    (void) Put2Byte(THEIGHT, out);
	    (void) Put2Byte(32, out);

	    for (i = 0; i < THEIGHT; ++i)
		for (j = 0; j < TWIDTH; ++j)
		    (void) Put1Byte(stexture[i][j], out);
	}

	(void) Put1Byte(iSET_TEXTURE, out);
	(void) Put1Byte((Family & 0x7e) >> 1, out);
	(void) Put1Byte((Family & 0x01) << 7 | (Member & 0x7f), out);
	(void) Put1Byte(iFILL_PATH, out);
	(void) Put1Byte(SHADE, out);
	init_Glyph();
	break;
    case WHITE:
#if MESSAGE
    (void) fprintf(stderr, " WHITE\n");
#endif
	path_tone = UNKNOWN;
	(void) Put1Byte(iFILL_PATH, out);
	(void) Put1Byte(WHITE, out);
	break;
    case BLACK:
#if MESSAGE
    (void) fprintf(stderr, " BLACK\n");
#endif
	path_tone = UNKNOWN;
	(void) Put1Byte(iFILL_PATH, out);
	(void) Put1Byte(BLACK, out);
	break;
    default:
	(void) fprintf(stderr, "! do_attributes error (undef path_tone).\n");
    break;
    }
}

visible
Make_imPRESS_SPECIAL(imPRESS)
NEW_SPECIAL *imPRESS;
{
    imPRESS->pn_com = im_PEN;
    imPRESS->fp_com = im_FLUSH;
    imPRESS->da_com = im_DRAW_DASH;
    imPRESS->dt_com = im_DRAW_DOT;
    imPRESS->pa_com = im_PATH;
    imPRESS->ar_com = im_ARC;
    imPRESS->sp_com = im_SPLINE;
    imPRESS->wh_com = im_WHITEN;
    imPRESS->bk_com = im_BLACKEN;
    imPRESS->sh_com = im_SHADE;
    imPRESS->tx_com = im_TEXTURE;
    imPRESS->fl_com = im_FILES;
}
