/*
 *	add.h	include file for dvi2nwp.
 *	$Header: add.h,v 1.1 89/07/31 12:31:40 kono Locked $
 */
#include "atsuo-copyright.h"

#define NWP_DEV			"/dev/lbp"
#define MAX_LBP_CLOSE_COUNT 	1
#define	CELL_FILL		0xff
#define	BITS_PAR_CELL		8
typedef unsigned char		CELL;

#define MAXBUF		128	/* Size of Line buffer, */
				/* it may be enough for 200 pt char. */
				/* Landscape buffer allocates */
				/* MAXBUF * MAXBUF area.        */
				/* If the line and Landscape */
				/* buffers are insuffcient to work, */
				/* they will be malloc()'ed dynamically. */

extern unsigned int FRAME_BIT_WIDTH;
extern unsigned int FRAME_BIT_HEIGHT;
extern unsigned int FRAME_SIZE;
/* = (FRAME_BIT_WIDTH*FRAME_BIT_HEIGHT/BITS_PAR_CELL) */
extern unsigned int FRAME_WIDTH;
/* = (FRAME_BIT_WIDTH/BITS_PAR_CELL) */
extern unsigned int FRAME_HEIGHT;
/* = FRAME_BIT_HEIGHT */

#define BIT_LEFT_OFFSET	0		/* About 1.6cm (400dpi) */

/* extern unsigned char frame[];*/
extern unsigned char *frame;

#define TMPFILE "/tmp/dvi.XXXXXX"

extern int	begin_page_num, end_page_num;
extern int	x_offset, y_offset;
extern int	odd_pages, even_pages;
extern int	direct, double_side, verbose, noprint;
extern int	page_step;
extern int	landscape;

#ifdef ACCT
extern int	npages ;
#endif

/* Return values of print_frame() */
#define PRINT_SUCCEEDED	1
#define PRINT_PREV	2
#define PRINT_THIS	3

/* Return values of check_lbp_stop_status() and check_lbp_start_status() */
#define READY		1	/* lbp is ready */
#define RESEND_PREV	2	/* Reprinting the previous. (stop only) */
#define RESTART_THIS	3	/* Restart printing this page (start only) */
#define RESEND_THIS	4	/* Re-send image and restart printing. (start only) */

/* Exit status of dvi2nwp */
#define NO_ERROR_EXIT	0	/* Printing is succeded. */
#define REPRINT_EXIT	1	/* The job should be reprinted. */
#define THROW_AWAY_EXIT	2	/* The job should be thrown away. */

#define SLEEP60()			\
    sleep(10); fprintf(stderr,".");	\
    sleep(10); fprintf(stderr,".");	\
    sleep(10); fprintf(stderr,".");	\
    sleep(10); fprintf(stderr,".");	\
    sleep(10); fprintf(stderr,".");	\
    sleep(10); fprintf(stderr,".\n")

