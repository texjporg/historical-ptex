/*
 *	add.h	include file for dvi2nwp.
 *	$Header: add.h,v 1.1 88/02/06 11:28:30 atsuo Exp $
 */

#include "atsuo-copyright.h"

#define NWP_DEV			"/dev/lbp"
#define	CELL_FILL		0xff
#define	BITS_PAR_CELL	8
typedef unsigned char	CELL;

#define FRAME_BIT_WIDTH		3136
#define FRAME_BIT_HEIGHT	4516
#define FRAME_SIZE	(FRAME_BIT_WIDTH*FRAME_BIT_HEIGHT/BITS_PAR_CELL)
#define FRAME_WIDTH	(FRAME_BIT_WIDTH/BITS_PAR_CELL)
#define	FRAME_HEIGHT	FRAME_BIT_HEIGHT

#define BIT_LEFT_OFFSET	250		/* About 1.6cm (400dpi) */

extern unsigned char frame[];

#define TMPFILE "/tmp/dvi.XXXXXX"

extern int	begin_page_num, end_page_num;
extern int	x_offset, y_offset;
extern int	odd_pages, even_pages;
extern int	direct, double_side, verbose, noprint;

#ifdef ACCT
extern int	npages ;
#endif

#define RETRY 1		/* Retry print_frame()	*/
#define ABORT 2		/* Abort dvi2nwp	*/
#define RESTART 3	/* Restart lbp		*/ 

