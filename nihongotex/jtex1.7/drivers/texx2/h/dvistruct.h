/*
 * Copyright (c) 1987 University of Maryland Department of Computer Science.
 * All rights reserved.  Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/* DVI file info */

/*
 * Units of distance are stored in scaled points, but we can convert to
 * units of 10^-7 meters by multiplying by the numbers in the preamble.
 */

/* the structure of the stack used to hold the values (h,v,w,x,y,z) */
typedef struct dvi_stack {
	i32	h;		/* the saved h */
	i32	v;		/* the saved v */
	i32	w;		/* etc */
	i32	x;
	i32	y;
	i32	z;
} DviStack;
