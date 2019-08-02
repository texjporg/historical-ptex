typedef struct conversion {
	double	c_fromsp;	/* multiplier to convert from scaled points */
	double	c_tosp;		/* multiplier to convert to scaled points:
				   could divide by c_fromsp, but this should
				   be faster and more accurate */
	double	c_mag;		/* the magnification this conversion
				   represents; mainly for GetFont() */
	double	c_dpi;		/* dpi (should be pixels per point?) */
} Conv;
