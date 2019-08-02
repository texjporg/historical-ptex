/* 
 *	F2p : Fig-to-tpic translator
 *
 *	Copyright (c) 1985, 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : October 1985.
 *	2nd revision : March 1988 - read fig 1.4
 *      For tpic
 *		by S. Kono
 *			Fri Feb 24 00:03:56 JST 1989
 *
 *	%W%	%G%
 
 Compile it in xfig 1.4 directry.
 Add following line to Makefile, and do "make f2tpic" 
 
 F2TPICOBJ =     arrow.o f2tpic.o free.o read.o read1_3.o
 f2tpic: $(F2TPICOBJ)
 $(RM) $ @
 $(CC) -o $ @ $(F2TPICOBJ)  $(LDFLAGS)
 
 */
#include "fig.h"
#include "object.h"

double		atan(),sqrt();
char		Usage[] = "Usage: %s [ -s scale ] [-d] [ input_file [ output_file ] ]\n";
char		*from = NULL, *to = NULL;
FILE		*ffp = NULL, *tfp = NULL;
double 		scale = 1000.0;
char		Err_incomp[] = "Incomplete %s object at line %d.";
char		Err_mem[] = "Running out of memory.";
int	        develop_open_spline = 0;
int             xmin,xmax,ymin,ymax;

put_msg(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
     char   *format, *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8;
{
  fprintf(stderr, format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

get_args(argc, argv)
     int	 argc;
     char	*argv[];
{
  char	*a,**n;  double s;
  int	first = 1;
  
  n = argv;
  while (--argc) {
    a = *++argv;
    if (*a == '-') {
      if (*++a == 's') {	/* Font size followed */
	if (--argc) {
	  sscanf(*++argv," %lf ",&s);
	  if (s <= 0.0) {
	    fprintf(stderr, Usage,n[0]);
	    exit(0);
	  }
	  scale *= s;
	}
	else {
	  fprintf(stderr, Usage,n[0]);
	  exit(0);
	}
      }
      if (*a == 'd') {	/* spline development */
	develop_open_spline = 1;
      }
      else {
	fprintf(stderr, Usage,n[0]);
	exit(0);
      }
    }
    else if (first) {
      from = a;	/*  from file  */
      first = 0;
    }
    else if (first == 0) {
      to = a;		/*  to file  */
      first = -1;
    }
    else
      fprintf(stderr, Usage,n[0]);
  }
}

main(argc, argv)
     int	 argc;
     char	*argv[];
{
  F_compound	objects;
  int		status;
  char		c;
  
  get_args(argc, argv);
  if (to == NULL)
    tfp = stdout;
  else if ((tfp = fopen(to, "w")) == NULL) {
    fprintf(stderr, "Couldn't open %s\n", to);
    fprintf(stderr, Usage);
    exit(0);
  }
  
  if (from)
    status = read_fig(from, &objects);
  else 	/* read from stdin */
    status = readfp_fig(stdin, &objects);
  
  if (status != 0) {
    if (from) read_fail_message(from, status);
    exit(0);
  }
  genpic_objects(&objects);
  if (tfp != stdout) fclose(tfp);
}

#define			TOP	(int)(10.5*1000) /* top of page is 10.5 inch */
static double		ppi;
static int		CONV = 0;

double
  convy(a)
double	a;
{
  return((double)(CONV ? a :TOP-a ));
}

genpic_objects(objects)
     F_compound	*objects;
{
  int		coord_system;
  F_arc		*a;
  F_compound	*c;
  F_ellipse	*e;
  F_line		*l;
  F_spline	*s;
  F_text		*t;
  
  if (0 == (ppi = (double)objects->nwcorner.x/scale)) {
    fprintf(stderr, "Resolution is zero!! default to 80 ppi\n");
    ppi = 80.0;
  }
  coord_system = objects->nwcorner.y;
  if (coord_system != 1 && coord_system != 2) {
    fprintf(stderr, "Wrong coordinate system; cannot continue\n");
    return;
  }
  if (coord_system == 2) CONV = 1;
  
  fprintf(tfp,"\\catcode`@=11\n");
  fprintf(tfp, "\\expandafter\\ifx\\csname graph\\endcsname\\relax");
  fprintf(tfp, " \\alloc@4\\box\\chardef\\insc@unt\\graph\\fi\n");
  fprintf(tfp, "\\catcode`@=12\n");
  fprintf(tfp, "\\setbox\\graph=\\vtop{%%\n");
  fprintf(tfp, "  \\baselineskip=0pt \\lineskip=0pt ");
  fprintf(tfp, "\\lineskiplimit=0pt\n");
  fprintf(tfp, "  \\vbox to0pt{\\hbox{%%\n");
  xmin = 30000;	xmax = -30000;
  ymin = 30000;	ymax = -30000;
  for (a = objects->arcs; a != NULL; a = a->next) genpic_arc(a);
  for (c = objects->compounds; c != NULL; c = c->next) genpic_compound(c);
  for (e = objects->ellipses; e != NULL; e = e->next) genpic_ellipse(e);
  for (l = objects->lines; l != NULL; l = l->next) genpic_line(l);
  for (s = objects->splines; s != NULL; s = s->next) genpic_spline(s);
  for (t = objects->texts; t != NULL; t = t->next) genpic_text(t);
  fprintf(tfp, "    \\kern %6.3fin\n  }\\vss}%%\n", (xmax+1)/1000.0);
  fprintf(tfp, "  \\kern %6.3fin\n}\n", (ymax+1)/1000.0);
}

#define check(x, y) \
{ \
    register int xx,yy; \
    xx = x; yy = y;\
	if (xx < xmin) xmin = xx; \
	if (xx > xmax) xmax = xx; \
	if (yy < ymin) ymin = yy; \
	if (yy > ymax) ymax = yy; \
}

/*
  The line thickness is, unfortunately, multiple of pixel.
  One pixel thickness is a little too thick on the hard copy
  so I scale it with 0.7; i.e., it's a kludge.  The best way is
  to allow thickness in fraction of pixel.
  
  Note that the current version of psdit (a ditroff to postcript filter)
  won't take the legitimate line thickness command.
  */
set_linewidth(w)
     int	w;
{
  static int	cur_thickness = -1;
  
  if (w == 0) return;
  if (w != cur_thickness) {
    cur_thickness = w;
    fprintf(tfp,"\\special{pn %d}%%\n", /* cur_thickness); */
	    (int)(0.7 * cur_thickness / ppi));
  }
}

float	style_val = 0.02;
int     style = SOLID_LINE;

set_style(s, v)
     int	s;
     float	v;
{
  
  if (s == DASH_LINE || s == DOTTED_LINE) {
    if (s == style) return;
    if (v == 0.0) return;
    style = s;
    style_val = v;
  } else {
    style =  SOLID_LINE ;
  }
}

genpic_compound(com)
     F_compound	*com;
{
  F_arc		*a;
  F_compound	*c;
  F_ellipse	*e;
  F_line		*l;
  F_spline	*s;
  F_text		*t;
  
  for (a = com->arcs; a != NULL; a = a->next) genpic_arc(a);
  for (c = com->compounds; c != NULL; c = c->next) genpic_compound(c);
  for (e = com->ellipses; e != NULL; e = e->next) genpic_ellipse(e);
  for (l = com->lines; l != NULL; l = l->next) genpic_line(l);
  for (s = com->splines; s != NULL; s = s->next) genpic_spline(s);
  for (t = com->texts; t != NULL; t = t->next) genpic_text(t);
}

#define close_line	if (style == DASH_LINE) \
  fprintf(tfp, "\\special{da %f}%%\n",style_val); \
else if (style == DOTTED_LINE) \
       fprintf(tfp, "\\special{dt %f}%%\n",style_val); \
else \
  fprintf(tfp, "\\special{fp}%%\n"); 

genpic_line(l)
     F_line	*l;
{
  int i;F_point		*p, *q;
  
  set_linewidth(l->thickness);
  set_style(l->style, l->style_val);
  p = l->points;
  q = p->next;
  check((int)(p->x/ppi), (int)(convy(p->y/ppi)));
  if (q == NULL) { /* A single point line */
    fprintf(tfp,"\\special{pa %d %d}\\special{pa %d %d}\\special{fp}%%\n",
	    (int)(p->x/ppi), (int)(convy(p->y/ppi)), 
	    (int)(p->x/ppi), (int)(convy(p->y/ppi)));
    return;
  }
  if (l->back_arrow)
    draw_arrow_head(q->x/ppi, convy(q->y/ppi), p->x/ppi,
		    convy(p->y/ppi), l->back_arrow->ht/ppi, l->back_arrow->wid/ppi);
  fprintf(tfp, "\\special{pa %d %d}%%\n", (int)(p->x/ppi), (int)(convy(p->y/ppi)));
  i = 1;
  while (q->next != NULL) {
    p = q;
    q = q->next;
    check((int)(p->x/ppi), (int)(convy(p->y/ppi)));
    fprintf(tfp, "\\special{pa %d %d}%%\n", (int)(p->x/ppi), (int)(convy(p->y/ppi)));
    if (i++%100==0) { 
      close_line;
      fprintf(tfp, "\\special{pa %d %d}%%\n", 
	      (int)(p->x/ppi), (int)(convy(p->y/ppi)));
    }
  }
  fprintf(tfp, "\\special{pa %d %d}%%\n", (int)(q->x/ppi), (int)(convy(q->y/ppi)));
  check((int)(q->x/ppi), (int)(convy(q->y/ppi)));
  close_line;
  if (l->for_arrow)
    draw_arrow_head(p->x/ppi, convy(p->y/ppi), q->x/ppi,
		    convy(q->y/ppi), l->for_arrow->ht/ppi, l->for_arrow->wid/ppi);
}

genpic_spline(s)
     F_spline	*s;
{
  if (int_spline(s))
    genpic_itp_spline(s);
  else
    genpic_ctl_spline(s);
}

genpic_ctl_spline(s)
     F_spline	*s;
{
  if (closed_spline(s))
    genpic_closed_spline(s);
  else
    genpic_open_spline(s);
}

int spline_count;

genpic_open_spline(s)
     F_spline	*s;
{
  double	cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
  double		x1, y1, x2, y2;
  F_point		*p, *q;
  
  set_style(s->style, s->style_val);
  
  p = s->points;
  x1 = p->x/ppi; y1 = convy(p->y/ppi);
  p = p->next;
  x2 = p->x/ppi; y2 = convy(p->y/ppi);
  
  if (s->back_arrow)
    draw_arrow_head(x2, y2, x1, y1,
		    s->back_arrow->ht/ppi, s->back_arrow->wid/ppi);
  
  if (develop_open_spline) {
    p = s->points;
    x1 = p->x/ppi;  y1 = convy(p->y/ppi);
    fprintf(tfp, "\\special{pa %d %d}%%\n", (int)x1,(int)y1);
    check((int)x1,(int)y1);
    p = p->next;
    x2 = p->x/ppi;  y2 = convy(p->y/ppi);
    check((int)x2,(int)y2);
    cx1 = (x1 + x2) / 2;      cy1 = (y1 + y2) / 2;
    cx2 = (x1 + 3 * x2) / 4;  cy2 = (y1 + 3 * y2) / 4;
    
    spline_count = 1;
    for (p = p->next; p != NULL; p = p->next) {
      fprintf(tfp, "\\special{pa %d %d}%%\n", (int)cx1,(int)cy1);
      x1 = x2;  y1 = y2;
      x2 = p->x/ppi;  y2 = convy(p->y/ppi);
      check((int)x2,(int)y2);
      cx3 = (3 * x1 + x2) / 4;  cy3 = (3 * y1 + y2) / 4;
      cx4 = (x1 + x2) / 2;      cy4 = (y1 + y2) / 2;
      quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4);
      cx1 = cx4;  cy1 = cy4;
      cx2 = (x1 + 3 * x2) / 4;  cy2 = (y1 + 3 * y2) / 4;
    }
    fprintf(tfp, "\\special{pa %d %d}%%\n", (int)x2,(int)y2);
    close_line;
  } else {
    p = s->points;
    x2 = p->x/ppi;  y2 = convy(p->y/ppi);
    fprintf(tfp, "\\special{pa %d %d}%%\n", (int)x2,(int)y2);
    check((int)x2,(int)y2);
    do {
      x1 = x2; y1 = y2;
      x2 = p->x/ppi;  y2 = convy(p->y/ppi);
      fprintf(tfp, "\\special{pa %d %d}%%\n", (int)x2,(int)y2);
      check((int)x2,(int)y2);
      p = p->next;
    } while(p != NULL);
    fprintf(tfp, "\\special{sp}%%\n");
  }
  if (s->for_arrow)
    draw_arrow_head(x1,y1, x2, y2,
		    s->for_arrow->ht/ppi, s->for_arrow->wid/ppi);
}

genpic_ellipse(e)
     F_ellipse	*e;
{
  int x, y, rx, ry, start, end;
  set_linewidth(e->thickness);
  
  x = e->center.x/ppi; y = convy(e->center.y/ppi);
  rx = e->radiuses.x/ppi; ry = e->radiuses.y/ppi;
  start = 0; end = 0;
  check(x-rx,y-ry);
  check(x+rx,y-ry);
  check(x-rx,y+ry);
  check(x+rx,y+ry);
  
  fprintf(tfp, "\\special{ar %d %d %d %d %d %d}%%\n", x, y, rx, ry, start, end);
}

/*
  Text is display on the screen with the base line starting at
  (base_x, base_y); some characters extend below this line.
  Pic displays the center of the height of text at the given
  coordinate. HT_OFFSET is use to compensate all the above factors
  so text position in fig 1.4 should be at the same position on
  the screen as on the hard copy.
  */
#define			HT_OFFSET	(0.2 / 72.0)

genpic_text(t)
     F_text	*t;
{
  float	x,y;
  
  x = t->base_x/ppi;
  y = convy(t->base_y/ppi);
  fprintf(tfp,"\\rlap{\\kern %6.3fin\\lower%6.3fin",
	  x / scale, y / scale);
  fprintf(tfp,"\\hbox to0pt{%s",t->cstring);
  fprintf(tfp,"\\hss}}%%\n");
}

genpic_arc(a)
     F_arc	*a;
{
  double		x, y,ts,te,r;
  double		cx, cy, sx, sy, ex, ey;
  
  cx = a->center.x/ppi; cy = convy(a->center.y/ppi);
  sx = a->point[0].x/ppi; sy = convy(a->point[0].y/ppi);
  ex = a->point[2].x/ppi; ey = convy(a->point[2].y/ppi);
  /* This is far from approximation..... */
  check(sx,sy);
  check(ex,ey);
  check(cx,cy);
  
  if (sy-cy==0.0) ts = 0; 
  else 
    ts = atan((sy-cy)/(sx-cx));
  if (sx-cx<0) ts += M_PI;
  if (ey-cy==0.0) te = 0; 
  else 
    te = atan((ey-cy)/(ex-cx));
  if (ex-cx<0) te += M_PI;
  r = sqrt((sx-cx)*(sx-cx)+(sy-cy)*(sy-cy));
  
  set_linewidth(a->thickness);
  
  if (a->for_arrow) {
    arc_tangent(cx, cy, ex, ey, 1, &x, &y);
    draw_arrow_head(x, y, ex, ey,
		    a->for_arrow->ht/ppi, a->for_arrow->wid/ppi);
  }
  if (a->back_arrow) {
    arc_tangent(cx, cy, sx, sy, 0, &x, &y);
    draw_arrow_head(x, y, sx, sy,
		    a->back_arrow->ht/ppi, a->back_arrow->wid/ppi);
  }
  
  if (a->direction)
    fprintf(tfp, "\\special{ar %d %d %d %d %6.3lf %6.3lf}%%\n", 
	    (int)cx, (int)cy, (int)r, (int)r, te, ts);
  else
    fprintf(tfp, "\\special{ar %d %d %d %d %6.3lf %6.3lf}%%\n", 
	    (int)cx, (int)cy, (int)r, (int)r, ts, te);
  
}

arc_tangent(x1, y1, x2, y2, direction, x, y)
     double	x1, y1, x2, y2, *x, *y;
     int	direction;
{
  if (direction) { /* counter clockwise  */
    *x = x2 + (y2 - y1);
    *y = y2 - (x2 - x1);
  }
  else {
    *x = x2 - (y2 - y1);
    *y = y2 + (x2 - x1);
  }
}

/*	draw arrow heading from (x1, y1) to (x2, y2)	*/

draw_arrow_head(x1, y1, x2, y2, arrowht, arrowwid)
     double	x1, y1, x2, y2, arrowht, arrowwid;
{
  double	x, y, xb, yb, dx, dy, l, sina, cosa;
  double	xc, yc, xd, yd;
  
  dx = x2 - x1;  dy = y1 - y2;
  l = sqrt((dx*dx + dy*dy));
  sina = dy / l;  cosa = dx / l;
  xb = x2*cosa - y2*sina;
  yb = x2*sina + y2*cosa;
  x = xb - arrowht;
  y = yb - arrowwid / 2;
  xc = x*cosa + y*sina;
  yc = -x*sina + y*cosa;
  y = yb + arrowwid / 2;
  xd = x*cosa + y*sina;
  yd = -x*sina + y*cosa;
  fprintf(tfp,"\\special{pa %d %d}\\special{pa %d %d}\\special{pa %d %d}\\special{fp}%%\n",
	  (int)xc, (int)yc, (int)x2, (int)y2, (int)xd, (int)yd);
}

#define		THRESHOLD	40	/* 1/1000 inch */

quadratic_spline(a1, b1, a2, b2, a3, b3, a4, b4)
     double	a1, b1, a2, b2, a3, b3, a4, b4;
{
  double	x1, y1, x4, y4;
  double	xmid, ymid;
  
  x1 = a1; y1 = b1;
  x4 = a4; y4 = b4;
  
  xmid = (a2 + a3) / 2;
  ymid = (b2 + b3) / 2;
  if (fabs(x1 - xmid) < THRESHOLD && fabs(y1 - ymid) < THRESHOLD) {
    fprintf(tfp, "\\special{pa %d %d}%%\n", (int)xmid, (int)ymid);
    if (spline_count++%100==0) {
      close_line;
      fprintf(tfp, "\\special{pa %d %d}%%\n", (int)xmid, (int)ymid);
    }
  }
  else {
    quadratic_spline(x1, y1, ((x1+a2)/2), ((y1+b2)/2),
		     ((3*a2+a3)/4), ((3*b2+b3)/4), xmid, ymid);
  }
  
  if (fabs(xmid - x4) < THRESHOLD && fabs(ymid - y4) < THRESHOLD) {
    fprintf(tfp, "\\special{pa %d %d}%%\n", (int)x4, (int)y4);
    if (spline_count++%100==0) {
      close_line;
      fprintf(tfp, "\\special{pa %d %d}%%\n", (int)x4, (int)y4);
    }
  }
  else {
    quadratic_spline(xmid, ymid, ((a2+3*a3)/4), ((b2+3*b3)/4),
		     ((a3+x4)/2), ((b3+y4)/2), x4, y4);
  }
}

genpic_closed_spline(s)
     F_spline	*s;
{
  F_point	*p;
  double	cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
  double	x1, y1, x2, y2;
  
  p = s->points;
  x1 = p->x/ppi;  y1 = convy(p->y/ppi);
  check(x1,y1);
  p = p->next;
  x2 = p->x/ppi;  y2 = convy(p->y/ppi);
  check(x2,y2);
  cx1 = (x1 + x2) / 2;      cy1 = (y1 + y2) / 2;
  cx2 = (x1 + 3 * x2) / 4;  cy2 = (y1 + 3 * y2) / 4;
  
  spline_count = 1;
  for (p = p->next; p != NULL; p = p->next) {
    fprintf(tfp, "\\special{pa %d %d}%%\n", (int)cx1,(int)cy1);
    x1 = x2;  y1 = y2;
    x2 = p->x/ppi;  y2 = convy(p->y/ppi);
    check(x2,y2);
    cx3 = (3 * x1 + x2) / 4;  cy3 = (3 * y1 + y2) / 4;
    cx4 = (x1 + x2) / 2;      cy4 = (y1 + y2) / 2;
    quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4);
    cx1 = cx4;  cy1 = cy4;
    cx2 = (x1 + 3 * x2) / 4;  cy2 = (y1 + 3 * y2) / 4;
  }
  x1 = x2;  y1 = y2;
  p = s->points->next;
  x2 = p->x/ppi;  y2 = convy(p->y/ppi);
  cx3 = (3 * x1 + x2) / 4;  cy3 = (3 * y1 + y2) / 4;
  cx4 = (x1 + x2) / 2;      cy4 = (y1 + y2) / 2;
  fprintf(tfp, "\\special{pa %d %d}%%\n", (int)cx1,(int)cy1);
  quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4);
  close_line;
}

genpic_itp_spline(s)
     F_spline	*s;
{
  F_point		*p1, *p2;
  F_control	*cp1, *cp2;
  double		x1, x2, y1, y2;
  
  p1 = s->points;
  cp1 = s->controls;
  cp2 = cp1->next;
  x2 = p1->x/ppi; y2 = convy(p1->y/ppi);
  
  if (s->back_arrow)
    draw_arrow_head(cp2->lx/ppi, convy(cp2->ly/ppi), x2, y2,
		    s->back_arrow->ht/ppi, s->back_arrow->wid/ppi);
  
  spline_count = 1;
  for (p2 = p1->next, cp2 = cp1->next; p2 != NULL;
       p1 = p2, cp1 = cp2, p2 = p2->next, cp2 = cp2->next) {
    check(x2,y2);
    fprintf(tfp, "\\special{pa %d %d}%%\n", (int)x2,(int)y2);
    x1 = x2; y1 = y2;
    x2 = p2->x/ppi; y2 = convy(p2->y/ppi);
    bezier_spline(x1, y1, (double)cp1->rx/ppi, convy(cp1->ry/ppi),
		  (double)cp2->lx/ppi, convy(cp2->ly/ppi), x2, y2);
    close_line;
  }
  
  if (s->for_arrow)
    draw_arrow_head((double)cp1->lx/ppi, convy(cp1->ly/ppi), x2, y2,
		    (double)s->for_arrow->ht/ppi, (double)s->for_arrow->wid/ppi);
}

bezier_spline(a0, b0, a1, b1, a2, b2, a3, b3)
     double	a0, b0, a1, b1, a2, b2, a3, b3;
{
  double	x0, y0, x3, y3;
  double	sx1, sy1, sx2, sy2, tx, ty, tx1, ty1, tx2, ty2, xmid, ymid;
  
  x0 = a0; y0 = b0;
  x3 = a3; y3 = b3;
  if (fabs(x0 - x3) < THRESHOLD && fabs(y0 - y3) < THRESHOLD) {
    fprintf(tfp, "\\special{pa %d %d}%%\n", (int)x3,(int)y3);
    if (spline_count++%100==0) {
      close_line;
      fprintf(tfp, "\\special{pa %d %d}%%\n", (int)x3,(int)y3);
    }
  }
  else {
    tx = (a1 + a2) / 2;		ty = (b1 + b2) / 2;
    sx1 = (x0 + a1) / 2;	sy1 = (y0 + b1) / 2;
    sx2 = (sx1 + tx) / 2;	sy2 = (sy1 + ty) / 2;
    tx2 = (a2 + x3) / 2;	ty2 = (b2 + y3) / 2;
    tx1 = (tx2 + tx) / 2;	ty1 = (ty2 + ty) / 2;
    xmid = (sx2 + tx1) / 2;	ymid = (sy2 + ty1) / 2;
    
    bezier_spline(x0, y0, sx1, sy1, sx2, sy2, xmid, ymid);
    bezier_spline(xmid, ymid, tx1, ty1, tx2, ty2, x3, y3);
  }
}



