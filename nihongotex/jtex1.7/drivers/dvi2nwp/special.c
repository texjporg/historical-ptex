/*
 *  Graphic Primitives for TPIC
 *  Sun Sep 10 15:06:44 JST 1989
 * Copyright (C) Shinji Kono, 1989
 * 連絡先：
 *  〒141東京都品川区東五反田3-14-13高輪ミューズビル3F
 *  ソニーコンピュータサイエンス研究所
 *  河野真治
 *  kono@csl.sony.co.jp
 *
 * このプログラムのいかなる複写、移植、改変、修正も許諾します。
 * Everyone is permitted to do anything on this program
 * including copying, porting, debugging, and modifying.
 */

#include "config.h"

#ifdef DVI2NWP

#include "add.h"

#ifndef lint
static char rcsid[] = "$Header";
#endif

#include <stdio.h>
#include <ctype.h>

struct frame {
	long pxl_h, dvi_h, pxl_v, dvi_v, w, x, y, z;
};

extern struct frame *stack;
extern int stackp;
extern int landscape;
#define PXL_H   stack[stackp].pxl_h
#define PXL_V   stack[stackp].pxl_v

#define RESOLUTION DEFAULT_DPI

float specialConv = (float) 	RESOLUTION / 1000.0;
typedef struct XPoint {
  int x,y;
  struct XPoint *next;
} XPoint;

static XPoint *pointListTop = NULL;
static XPoint **pointList = &pointListTop;
static XPoint *freePointList = NULL;
static penSize = 1;

extern double atan(),cos(),sin(),sqrt(),fabs();
extern char *malloc();
static int width;
static int dashed;
static float interval;

#define MAXWIDTH 300
#define M_PI    3.14159265358979323846
#define M_PI_2  1.57079632679489661923

static XPoint *
consXPoint()
{
  register XPoint *p;
  if (freePointList) {
    p = freePointList;
    freePointList = p->next;
    return p;
  } else {
    return (XPoint *)malloc(sizeof(XPoint));
  }
}

static void
freeXPoint(p)
XPoint *p;
{
  *pointList = freePointList;
  freePointList = pointListTop;
  pointListTop = NULL;
  pointList = &pointListTop;
}

static char *
skipWhite(c)
char *c;
{
    while (c != 0 && *c != 0 && isspace(*c)) c++;
    return(c);
}

static char *
skipNonWhite(c)
char *c;
{
    while (c != 0 && *c != 0 && ! isspace(*c)) c++;
    return(c);
}


line_width(w)
int w;
{
  /* penSize unit: 1/80inch * 0.7 */
  w *= ((1.0/80.0) * RESOLUTION / 2.0 ) * 0.1;
  /* factor 0.1 is good for me */
  if (w<0) w=1;
  if (w>MAXWIDTH) w=MAXWIDTH; /* max 1 inch width */
  width = w;
}

void
applicationDoSpecial(command)
char *command;
{
    char com[20];
    int x,y,x1,y1,i,style,hh,vv;
    char *c, *oc;
    XPoint *p,*q;

    c = skipWhite(command);
    if (*c == 0) return;

    hh = PXL_H;
    vv = PXL_V;

    oc = c;
    c = skipNonWhite(c);
    x = *c;
    *c = '\000';
    strncpy(com, oc, 20);
    *c = x;
    c++;

    if (strcmp(com, "pa") == 0) {
	sscanf(c, " %d %d ", &x, &y);
	*pointList = p = consXPoint();
	if (p==NULL) {
	  perror("Too many TPIC points");
	  freeXPoint();
	  *pointList = p = consXPoint();
	}
	p -> next = NULL;
	pointList = &p->next;
	if (landscape) {
	  p->y =
	    specialConv * x + hh - x_offset;
	  p->x =
	    FRAME_BIT_WIDTH - (specialConv * y + vv - y_offset);
	} else {
	  p->x =
	    specialConv * x + hh - x_offset;
	  p->y =
	    specialConv * y + vv - y_offset;
	}
      } else if (   ((style=1)&&strcmp(com,"fp") == 0)
		 || ((style=2)&&strcmp(com,"da") == 0)
		 || ((style=3)&&strcmp(com,"dt") == 0)
		 || ((style=4)&&strcmp(com,"sp") == 0)) {
	p = pointListTop; if (p==NULL) {
	  /*****************************************************/
	  /* A little extension of TPIC protocols              */
	  /* if fp,da,dt,sp are seen on empty point situation, */
	  /* it sets default line pattern                      */
	  /*****************************************************/
	  dashed = style;
	  if (style==2 || style==3)
	    if(sscanf(com+2,"%f",&interval)!=1) interval = 0.05 /* inch */ ;
	  return;
	}
	q = p->next;
	switch(style) {
	case  1: /* plain */
	  while(q) {
	    line_gen(p->x,p->y,q->x,q->y);
	    p = q; q = q->next;
	  }
	  break;
	case  2: /* dashed */
	case  3: /* doted */
	  if(sscanf(com+2,"%f",&interval)!=1) interval = 0.05 /* inch */ ;
	  if (interval<=0) interval = 0.05;
	  { int n; double x,y,x1,y1,dx,dy,ddx,ddy,dl;
	    while(q) {
	      x1 = q->x; x = p->x; y1 = q->y; y = p->y;
	      if (x==x1&&y==y1) {
		disk((int)x,(int)y,(int)width);
		continue;
	      }
	      n = sqrt((double)((x1-x)*(x1-x)+(y1-y)*(y1-y)))
		/(interval*RESOLUTION)+0.5;
	      if(style==2) n /= 2;
	      if (n<=2) n=2;
	      dy = (y1-y)/(double)n; dx = (x1-x)/(double)n;
	      if(style==2) {
		dl = sqrt(dx*dx+dy*dy); /* dl != 0.0 */
		if ((double)width < 0.5*dl ) {
		  ddx = dx * (0.5 -(double)(width)/dl);
		  ddy = dy * (0.5 -(double)(width)/dl);
		} else {
		  ddx = ddy = 0.0;
		}
		while(n-->0) {
		  line_gen((int)x,(int)y,
			   (int)(x+ddx),(int)(y+ddy));
		  x += dx; y += dy;
		}
	      } else if (style==3) {
		while(n-->0) {
		  disk((int)x,(int)y,(int)width);
		  x += dx; y += dy;
		}
	      }
	      p = q; q = p->next;
	    }
	  }
	  break;
	case  4: /* spline */
	  spline(pointListTop);
	  break;
        }
	freeXPoint();
    } else if (strcmp(com,"ar") == 0) {
	int rx,ry,s,e,xx; double start,end;
	sscanf(c, " %d %d %d %d %lf %lf", &x, &y, &rx, &ry, &start, &end);
	if (landscape) {
	  xx = x;
	  x = FRAME_BIT_WIDTH - (specialConv * y + vv - y_offset);
	  y = specialConv * xx + hh - x_offset;
	  xx = rx;
	  rx = specialConv * ry;
	  ry = specialConv * xx;
	  start += M_PI_2;
	  end += M_PI_2;
	} else {
	  x = specialConv * x + hh - x_offset;
	  y = specialConv * y + vv - y_offset;
	  rx = specialConv * rx;
	  ry = specialConv * ry;
	}
	arc(x,y,rx,ry,start,end);
    } else if (strcmp(com, "pn") == 0) {
	sscanf(c," %d ", &penSize);
	/* penSize *= specialConv/3; /* I don't know why... */
	line_width(penSize);
    } else {
	fprintf(stderr, "[%s] special \"%s\" not implemented\n",
		"dvinews", command);
    }
}


#define H_Tread 10
#define LC_Const 30

#define abs(a) ((a)>0?(a):-(a))

rectangle(x, x1, y, y1)
     int x, x1, y, y1;
{
  register unsigned int	i, j, xl, xr, l, r; 
  int h, w;
  CELL	lc, rc, *p, *q;
  
  if ((w = x1-x)<0) {
    w = x1; x1 = x; x = w; w = x1-x;
  }
  if ((h = y1-y)<0) {
    h = y1; y1 = y; y = h; h = y1-y;
  }
  /* Easy Clipping */
  if ( x>FRAME_BIT_WIDTH || x1<0 ) return;
  if ( x<0 ) x = 0;
  if ( x1>FRAME_BIT_WIDTH ) x1 = FRAME_BIT_WIDTH-1;
  if ( y>FRAME_BIT_HEIGHT || y1<0 ) return;
  if ( y<0 ) y = 0;
  if ( y1>FRAME_BIT_HEIGHT ) y1 = FRAME_BIT_HEIGHT-1;
  
  xl = (int) (x / BITS_PAR_CELL);
  l = (int) (x % BITS_PAR_CELL);
  xr = (int) ((x+w) / BITS_PAR_CELL);
  r = (int) ((x+w) % BITS_PAR_CELL);
  
  if ( w >= BITS_PAR_CELL) {
    lc = ((unsigned char)0xff) >> l;
    rc = ((unsigned char)0xff) << (BITS_PAR_CELL - r);
    p = frame + (xl+y*FRAME_WIDTH);
    while(y++ <= y1) {
      if (l != 0) {  /* Left to Right */
	*p++ |= lc;
      } else {
	*p++ |= (unsigned char) 0xff;
      }
      q = p + ( xr - xl - 1);
      while (p<q) {
	*p++ |= (unsigned char) 0xff;
      }
      if (r != 0) {
	*p |= rc;
      }
      p = q + ( xl - xr ) + FRAME_WIDTH;
    }
  } else {
    lc = ((unsigned)((0xff << (BITS_PAR_CELL - w)) & 0xff)) >> l;
    rc = ((unsigned)((0xff >> (BITS_PAR_CELL - w)) & 0xff)) <<
      (BITS_PAR_CELL - r);
    p = frame + (xl+y*FRAME_WIDTH);
    while(y++ <= y1) {
      *p++ |= lc;
      if (xl != xr) {
	*p |= rc;
      }
      p += FRAME_WIDTH - 1;
    }
  }
}


horizon(y,x,ww)
   int y,x,ww;
{
register CELL *p;
register unsigned int lc, rc;
unsigned int yy, xx, w,  xl, l, xr, r; 

  w = ww; 
  yy = y; 
  xx = x; 
  if ( w < BITS_PAR_CELL ) {
    xx = (xx <= FRAME_BIT_WIDTH) ? xx : FRAME_BIT_WIDTH; 
    w = ((w+xx) <= FRAME_BIT_WIDTH) ? w : FRAME_BIT_WIDTH-xx; 
    if( yy <= FRAME_BIT_HEIGHT ) { 
      xl = (int) ((xx) / BITS_PAR_CELL); 
      l = (int) ((xx) % BITS_PAR_CELL); 
      xr = (int) ((xx+w) / BITS_PAR_CELL); 
      r = (int) ((xx+w) % BITS_PAR_CELL); 
      lc = ((unsigned)((0xff << (BITS_PAR_CELL - (w))) & 0xff)) >> l; 
      rc = ((unsigned)((0xff >> (BITS_PAR_CELL - (w))) & 0xff)) << 
        (BITS_PAR_CELL - r); 
      p = frame + (xl+yy*FRAME_WIDTH);
      *p++ |= lc; 
      if (xl != xr) { 
	*p |= rc; 
      }
    }
  } else rectangle(x,x+w,y,y); 
}


/* 
  Basic Line Routine 

  Line Generation
  from  S. Harrington : Computer Graphics, A Programming Approach,
  McGraw-Hill Book Company
  
  Using Bresenham algorithm
  
  Line Style	Chain, Long Chain
  >0	chain
  <0	long chain with constant  
  LC_Const
  =0	solid line
  Line Width	Constant width, Rectangle
  using point shift ( it costs, but easy )
*/


/*  One dot line case is should be rare */

line_gen(xa,ya,xb,yb)
     int xa,ya,xb,yb;
{
  int dx,dy;
  int r,c, c1;
  int f,g,inc1,inc2,slope;
  int h,w;
  
  dx = xb-xa;
  dy = yb-ya;
  
  if (dx==0 && dy==0) {
    disk(xa,ya,width);
    return; 
  }
  slope = (dx > 0);
  if (dy<0) 
    slope = ! slope;
  if(abs(dx)>abs(dy)) {
    if (dy==0) {
      disk(xa,ya,width);
      disk(xb,yb,width);
      if (xa>xb) {w = xb; xb = xa; xa = w; dx = -dx; }
      rectangle(xa+1,xb-1,ya-width,ya+width);
      return;
    } 
    if (dx>0) {
      c = xa; r = ya; f = xb;
    } else {
      c = xb; r = yb; f = xa;
    }
    c1 = c;
    inc1 = 2 * abs(dy);
    g = 2 * abs(dy) - abs(dx);
    inc2 = 2 * ( abs(dy) - abs(dx) );
    disk(c,r,width);
    if (slope) {
      while (c++ <= f ) {
	if (g >= 0 ){
	  rectangle(c-1,c1+1,r-width,r+width);
	  disk(c,r,width);
	  r++; c1 = c;
	  g += inc2;
	} else
	  g += inc1;
      }
    } else {
      while ( c++ <= f ) {
	if (g>0) {
	  rectangle(c-1,c1+1,r-width,r+width);
          disk(c,r,width);
	  r--; c1 = c;
	  g += inc2;
	} else 
	  g += inc1;
      }
    }
    if (c1 < c - 1) {
      rectangle(c-1,c1+1,r-width,r+width);
      disk(c,r,width);
    }
  } else {
    if (dx==0) {
      disk(xa,ya,width);
      disk(xb,yb,width);
      if (ya>yb) {w = yb; yb = ya; ya = w; }
      rectangle(xa-width,xa+width,ya-1,yb+1);
      return;
    }
    if (dy>0) {
      c = ya;
      r = xa;
      f = yb;
    } else {
      c = yb;
      r = xb;
      f = ya;
    }
    c1 = c;
    inc1 = 2 * abs(dx);
    g = 2 * abs(dx) - abs(dy);
    inc2 = 2 * ( abs(dx) - abs(dy) );
    disk(r,c,width);
    if (slope) {
      while (c++ <= f ) {
	if (g >= 0 ){
	  rectangle(r-width,r+width,c-1,c1+1);
	  disk(r,c,width);
	  r++; c1 = c;
	  g += inc2;
	} else
	  g += inc1;
      }
    } else {
      while ( c++ <= f ) {
	if (g>0) {
	  rectangle(r-width,r+width,c-1,c1+1);
	  disk(r,c,width);
	  r--; c1 = c;
	  g += inc2;
	} else 
	  g += inc1;
      }
    }
    if (c1 < c - 1) {
      rectangle(r-width,r+width,c-1,c1+1);
      disk(r,c,width);
    }
  }
}

spline(p)
XPoint *p;
{
  double x1,x2,y1,y2,cx1,cy1,cx2,cy2,cx3,cy3,cx4,cy4;


  if (p==NULL) return;
  x1 = p->x; y1 = p->y;
  p = p->next;
  if (p==NULL) return;
  x2 = p->x; y2 = p->y;
  cx1 = (x1 + x2) / 2;      cy1 = (y1 + y2) / 2;
  cx2 = (x1 + 3 * x2) / 4;  cy2 = (y1 + 3 * y2) / 4;
  line_gen((int)x1,(int)y1,(int)cx1,(int)cy1);

  for (p = p->next; p != NULL; p = p->next) {
    x1 = x2;  y1 = y2;
    x2 = p->x;  y2 = p->y;
    cx3 = (3 * x1 + x2) / 4;  cy3 = (3 * y1 + y2) / 4;
    cx4 = (x1 + x2) / 2;      cy4 = (y1 + y2) / 2;
    quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4);
    cx1 = cx4;  cy1 = cy4;
    cx2 = (x1 + 3 * x2) / 4;  cy2 = (y1 + 3 * y2) / 4;
  }
  line_gen((int)cx4,(int)cy4,(int)x2,(int)y2);
}

/* s e : radian */
arc(x,y,rx,ry,s,e)
     unsigned int x,y,rx,ry; double s,e;
{
  int rxs,rys,rxe,rye;

  if (s==e) {
    curve(rx, 0, rx, 0, 1, (ry*ry), (rx*rx),x,y,0);
  } else {
    rxs = (double) rx * cos(e);
    rys = (double) ry * - sin(e);
    rxe = (double) rx * cos(s);
    rye = (double) ry * - sin(s);
    curve(rxs, rys, rxe, rye, 1, (ry*ry), (rx*rx),x,y,0);
  }
}

disk(x,y,r)
     int x,y,r;
{
  int a,x0,y0,w,xl,bit;
  /* Draw filled Circle */
  if (r<=1) {
    if ( y<0 || y>FRAME_BIT_HEIGHT ) return;
    if ( x<0 || x>FRAME_BIT_WIDTH ) return;
    xl = (int) (x / BITS_PAR_CELL);
    bit = (int) (x % BITS_PAR_CELL);
    frame[xl+(y*FRAME_WIDTH)] |= (1 << (BITS_PAR_CELL - bit));
    return;
  }
  /*
    I use DDA here, wrote 4 parts circle using 1/8 circles.
    */
  a = r;
  x0 = r;
  y0 = 0;
  a = - (a + 1) /2;
  while( x0>y0 ) {
    /* draw x0,y0; */
    w = x0+x0;
    horizon( y0+y,-x0+x,w);     /*  /----\ */
    horizon( x0+y,-y0+x,y0+y0); /*  |    | */
    horizon(-x0+y,-y0+x,y0+y0); /*  |    | */
    horizon(-y0+y,-x0+x,w);     /*  \----/ */
    a += y0++;
    if (a>0) {
      x0--;
      a -= x0;
    }
  }
}

/* 
 *      Routines from
 *	FIG : Facility for Interactive Generation of figures
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 */

#define		round(a)	((int)((a) + .5))

/*	This routine plot two dimensional curve defined by a second degree
	polynomial of the form :
	            2    2
	f(x, y) = ax + by + g = 0
	
	(x0,y0) is the starting point as well as ending point of the curve.
	The curve will translate with the offset xoff and yoff.
	
	This algorithm is derived from the eight point algorithm in :
	"An Improved Algorithm for the generation of Nonparametric Curves"
	by Bernard W. Jordan, William J. Lennon and Barry D. Holm, IEEE
	Transaction on Computers Vol C-22, No. 12 December 1973.
	*/

curve(xstart, ystart, xend, yend, direction, a, b, xoff, yoff, val)
     int	xstart, ystart, xend, yend, a, b, xoff, yoff;
     int	direction, val;
{
  int	dfx, dfy, dfxx, dfyy;
  int	falpha, fx, fy, fxy, absfx, absfy, absfxy;
  int	margin, test_succeed, x, y, deltax, deltay, inc, dec;
  
  if (a == 0 || b == 0) return;
  x    = xstart;
  y    = ystart;
  dfx  = 2 * a * xstart;
  dfy  = 2 * b * ystart;
  dfxx = 2 * a;
  dfyy = 2 * b;
  
  falpha = 0;
  if (direction) {
    inc = 1;  dec = -1;
  }
  else {
    inc = -1;  dec = 1;
  }
  if (xstart == xend && ystart == yend) {
    test_succeed = margin = 1;
  }
  else {
    test_succeed = margin = 3; 
  }
  
  do {
    if (dfy < 0) deltax = inc; else deltax = dec;
    if (dfx < 0) deltay = dec; else deltay = inc;
    fx  = falpha + dfx * deltax + a;
    fy  = falpha + dfy * deltay + b;
    fxy = fx + fy - falpha;
    absfx  = abs(fx); absfy  = abs(fy); absfxy = abs(fxy);
    
    if ((absfxy <= absfx) && (absfxy <= absfy))
      falpha = fxy;
    else if (absfy <= absfx) {
      deltax = 0; falpha = fy;
    }
    else {
      deltay = 0; falpha = fx;
    }
    /* line_gen(xoff+x,xoff+x+deltax,yoff+y,yoff+y+deltay); */
    disk(xoff+x,yoff-y,width);
    x += deltax;  y += deltay;
    dfx += dfxx * deltax;
    dfy += dfyy * deltay;
    if (abs(x - xend) < margin && abs(y - yend) < margin) 
      test_succeed--;
  } while (test_succeed);
}

#define		THRESHOLD	5.0	/* 1/1000 inch unit */

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
    line_gen((int)x1,(int)y1,(int)xmid,(int)ymid);
  } else {
    quadratic_spline(x1, y1, ((x1+a2)/2), ((y1+b2)/2),
		     ((3*a2+a3)/4), ((3*b2+b3)/4), xmid, ymid);
  }
  
  if (fabs(xmid - x4) < THRESHOLD && fabs(ymid - y4) < THRESHOLD) {
    line_gen((int)xmid,(int)ymid,(int)x4,(int)y4);
  }
  else {
    quadratic_spline(xmid, ymid, ((a2+3*a3)/4), ((b2+3*b3)/4),
		     ((a3+x4)/2), ((b3+y4)/2), x4, y4);
  }
}
#endif
/* end */
