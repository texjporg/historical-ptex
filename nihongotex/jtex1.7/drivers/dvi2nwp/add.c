 /*
 *   add.c
 *	$Header: add.c,v 1.2 89/10/28 13:46:44 kono Locked $
 *	written by Atsuo, K. (ISIR)
 */

#include "atsuo-copyright.h"

#include "config.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <newsiop/lbp.h>
#include "font.h"
#include "add.h"

char	tmp_file[64];
int	direct = 0;
int	verbose = 0;
int	double_side = 0;

#ifdef DEFAULT_X_OFFSET
int	x_offset = DEFAULT_X_OFFSET;
#else
int	x_offset = 0;
#endif /* X Offset */
#ifdef DEFAULT_Y_OFFSET
int	y_offset = DEFAULT_Y_OFFSET;
#else
int	y_offset = 0;
#endif /* Y Offset */

int	odd_pages = 0, even_pages = 0;
int	begin_page_num = 0, end_page_num = 9999;
int	noprint = 0;

extern	current_page;

unsigned int FRAME_BIT_WIDTH;
unsigned int FRAME_BIT_HEIGHT;
unsigned int FRAME_SIZE;
		/* = (FRAME_BIT_WIDTH*FRAME_BIT_HEIGHT/BITS_PAR_CELL) */
unsigned int FRAME_WIDTH;	/* = (FRAME_BIT_WIDTH/BITS_PAR_CELL) */
unsigned int FRAME_HEIGHT; 	/* = FRAME_BIT_HEIGHT */

/* unsigned char frame[(FRAME_SIZE+1)];*/	/* guard against overshoot */
unsigned char *frame = NULL;
int	frame_size_l = 0;
int	lbp = -1;

static	unsigned char *land_buf = NULL;
static	int	land_buf_size = 0;

static	unsigned char *line_buf = NULL;
static	int	line_buf_size = 0;

char *
stdinToTempFile()
{
	int	c;
	FILE	*dvi;

	strcpy(tmp_file,TMPFILE);
	mktemp(tmp_file);

	dvi = fopen(tmp_file,"w");
	if (dvi == NULL) {
		perror("fopen tmp_file");
#ifdef JXDVI
		exit(1);
#endif
#ifdef DVI2NWP
		exit(0);
#endif
	}
	while((c=getchar()) != EOF) {
		putc((unsigned char)c, dvi);
	}
	fclose(dvi);

	return(tmp_file);
}

rmTempFile()
{
	unlink(tmp_file);
}

clear_image_memory()
{
	bzero(frame,FRAME_SIZE);
/**/ /* *frame = 0xff; */
}

put_rectangle_to_image(x, y, w, h)
	long x, y, w, h;
{
	register unsigned int	i, j, xl, xr, l, r;
	CELL	lc, rc;

	x -= x_offset;
	y -= y_offset;
	/* current_page is odd when even pages are being printed.*/
	if (double_side == 1 && (current_page%2)==1) {
		x -= BIT_LEFT_OFFSET;
	}

	if ( landscape ) {
		long	x_save, h_save;
		x_save = x;
		h_save = h;

		x = FRAME_BIT_WIDTH - y - h;
		y = x_save;
		h = w;
		w = h_save;
	}

	if ( x<0 || y<0 ) return;
        if ( (x+w)>FRAME_BIT_WIDTH || (y+h)>FRAME_BIT_HEIGHT ) return;
/*printf("x:%d,y:%d ",x,y); fflush(stdout);*/

	xl = (int) (x / BITS_PAR_CELL);
	l = (int) (x % BITS_PAR_CELL);
	xr = (int) ((x+w) / BITS_PAR_CELL);
	r = (int) ((x+w) % BITS_PAR_CELL);
/*printf("x:%d y:%d xl:%d l:%d xr:%d r:%d\n",x,y,xl,l,xr,r); fflush(stdout);*/

	if ( w >= BITS_PAR_CELL) {
		lc = ((unsigned char)0xff) >> l;
		rc = ((unsigned char)0xff) << (BITS_PAR_CELL - r);
		for (j = y; j <= y+h; j++) {			/* Up to Down */
			if (l != 0) {						/* Left to Right */
/*				frame[xl+1+(j*FRAME_WIDTH)] |= lc;*/
				frame[xl+(j*FRAME_WIDTH)] |= lc;
			} else {
				frame[xl+(j*FRAME_WIDTH)] |= 0xff;
			}
			for ( i=xl+1; i<xr; i++) {
				frame[i+(j*FRAME_WIDTH)]  |= (unsigned char) 0xff;
			}
			if (r != 0) {
				frame[xr+(j*FRAME_WIDTH)] |= rc;
			}
		}
	} else {
		lc = ((unsigned)((0xff << (BITS_PAR_CELL - w)) & 0xff)) >> l;
		rc = ((unsigned)((0xff >> (BITS_PAR_CELL - w)) & 0xff)) <<
			(BITS_PAR_CELL - r);
/*printf(" | x:%d y:%d w:%d h:%d xl:%d xr:%d l:%d r:%d lc:%#2x rc:%#2x",
 *				x, y, w, h, xl, xr, l, r, lc, rc);
 */
		for (j = y; j <= y+h; j++) {			/* Up to Down */
			frame[xl+(j*FRAME_WIDTH)] |= lc;
			if (xl != xr) {
				frame[xr+(j*FRAME_WIDTH)] |= rc;
			}
		}
	}
}

/*
 * Put_bitmap_to_image() assumes that:
 *	1. bitmap->bits contains no garbage bits in padding bits.
 *	   That is, pad bits are all zeros.
 */
put_bitmap_to_image(bitmap, x, y)
	register struct bitmap *bitmap;
	long x;
	register long y;
{
  	if ( landscape )
	  land_put_bitmap_to_image(bitmap, x, y);
	else
	  port_put_bitmap_to_image(bitmap, x, y );
}

port_put_bitmap_to_image(bitmap, x, y)
	register struct bitmap *bitmap;
	long x;
	register long y;
{
	register int	i, j;
	register int	bw;
	int	rs;
	int	w, h, xx;
	register unsigned char	*b;
	register unsigned char	*f;
	register unsigned char	*nb;

	x -= x_offset;
	y -= y_offset;
	/* current_page is odd when even pages are being printed.*/
	if (double_side == 1 && (current_page%2)==1) {
		x -= BIT_LEFT_OFFSET;
	}
/*printf("x:%d,y:%d ",x,y); fflush(stdout);*/
	w = (int) bitmap->w;
	h = (int) bitmap->h;
	bw = (int) bitmap->bytes_wide;
	nb = (unsigned char *) bitmap->bits;

	if ( x<0 || y<0 ) return;
        if ( (x+w)>FRAME_BIT_WIDTH || (y+h)>FRAME_BIT_HEIGHT ) return;

	xx = (int) x/BITS_PAR_CELL;
	rs = (int) x%BITS_PAR_CELL;		/* 8 is bits width of char */

    	if ( bw > line_buf_size ) {
		if ( line_buf != NULL )
			free ( line_buf );
		if ((b = line_buf = (unsigned char *)malloc(bw +1)) == NULL)
	    		error("line buffer");	/* exit */
		line_buf_size = bw + 1;
    	}
    	else {
		b = line_buf;
	}

	for (j=0; j<h; j++) {
	    bcopy((nb+j*bw),b,bw);			/* Copy bits to buf */
	    if (rs != 0) {				/* Shift buf[] >> rs */
		*(b+bw) = (*(b+bw-1)) << (BITS_PAR_CELL-rs);
		for (i=bw-1; i>0; i--) {
		    *(b+i) = ( (*(b+i)) >> rs ) |
		      	( (*(b+i-1) << (BITS_PAR_CELL-rs)) );
		}
		*b = (*b) >> rs;
	    } else {
		*(b+bw) = 0x00;
	    }
	    f = &(frame[xx+(j+y)*FRAME_WIDTH]);
	    for (i=0; i<=bw; i++) {	/* Copy buf[] to frame[] */
					/* Note that it must be i<=bw */
		*(f+i) |= *(b+i);
/*		frame[i+xx+(j+y)*FRAME_WIDTH] |= *(b+i);*/
	}
    }
}

land_put_bitmap_to_image(bitmap, x, y)
	register struct bitmap *bitmap;
	long x;
	register long y;
{
	long	x_save;
	register int	i, j;
	register int	bw;
	int	rs;
	int	w, h, xx;

	register unsigned char	*b;
	register unsigned char	*f;
	register unsigned char	*nb;
	register unsigned char	*byte_ptr;
	register unsigned char	bit_mask, byte_image;

	int	k, l, bytes_v, counter, rest;

	x -= x_offset;
	y -= y_offset;
	/* current_page is odd when even pages are being printed.*/
	if (double_side == 1 && (current_page%2)==1) {
		x -= BIT_LEFT_OFFSET;
	}
/*printf("x:%d,y:%d ",x,y); fflush(stdout);*/

	w = (int) bitmap->w;
	h = (int) bitmap->h;

	x_save = x;
	x = FRAME_BIT_WIDTH - y - h;
	y = x_save;

	if ( x<0 || y<0 ) return;
        if ( (x+h)>FRAME_BIT_WIDTH || (y+w)>FRAME_BIT_HEIGHT ) return;

    	if ( (h + BITS_PAR_CELL - 1) > line_buf_size ) {
		if ( line_buf != NULL )
			free ( line_buf );
		if ((b = line_buf = (unsigned char *)
		     		malloc(h + BITS_PAR_CELL)) == NULL)
	    		error("line buffer");	/* exit */
		line_buf_size = h + BITS_PAR_CELL;
    	}
    	else {
		b = line_buf;
	}

	bw = (int) bitmap->bytes_wide;
	nb = (unsigned char *) bitmap->bits;

	xx = (int) x/BITS_PAR_CELL;
	rs = (int) x%BITS_PAR_CELL;		/* 8 is bits width of char */

	bytes_v = h / BITS_PAR_CELL;
	if ( ( rest = h % BITS_PAR_CELL ) != 0 )
		bytes_v++;

    	if ( (bw * (h + 7)) > land_buf_size ) {
		if ( land_buf != NULL )
			free ( land_buf );
		if (( land_buf = (unsigned char *)
		     		malloc(bw  * ( h + 8 )))
					 == NULL)
	    		error("line buffer");	/* exit */
		line_buf_size = bw  * ( h + 8 );
    	}
    	else {
		b = line_buf;
	}

	counter = 0;
	for ( i = 0; i < bw; i++ ) {
		for ( l = 0; l < h; l++ )
			b[ l ] = nb[ i + ( h - l - 1 ) * bw ];
		if ( rest != 0 )
			byte_ptr = &b[h];
			for ( l = h; l < BITS_PAR_CELL * bytes_v; l++)
				*byte_ptr++ = 0;
				
		bit_mask = 0x80;
		for ( j = 0; j < BITS_PAR_CELL; j++ ) {
			byte_ptr = b;
			for ( k = 0; k < bytes_v; k++ ) {
				byte_image = 0;
				for ( l = 0; l < BITS_PAR_CELL; l++ ) {
					byte_image <<= 1;
					if ( *(byte_ptr++) & bit_mask )
						byte_image |= 1;
				}
				land_buf[counter * bytes_v + k ]
						= byte_image;
			}
			counter ++;
			bit_mask >>= 1;
		}
	}
	bw = bytes_v;
	h = (int) bitmap->w;
	w = (int) bitmap->h;
	nb = land_buf;

    	if ( bw > line_buf_size ) {
		if ( line_buf != NULL )
			free ( line_buf );
		if ((b = line_buf = (unsigned char *)malloc(bw +1)) == NULL)
	    		error("line buffer");	/* exit */
		line_buf_size = bw + 1;
    	}
    	else {
		b = line_buf;
	}

	for (j=0; j<h; j++) {
	    bcopy((nb+j*bw),b,bw);			/* Copy bits to buf */
	    if (rs != 0) {				/* Shift buf[] >> rs */
		*(b+bw) = (*(b+bw-1)) << (BITS_PAR_CELL-rs);
		for (i=bw-1; i>0; i--) {
		    *(b+i) = ( (*(b+i)) >> rs ) |
				 ( (*(b+i-1) << (BITS_PAR_CELL-rs)) );
		}
		*b = (*b) >> rs;
	    } else {
		*(b+bw) = 0x00;
	    }
	    f = &(frame[xx+(j+y)*FRAME_WIDTH]);
	    for (i=0; i<=bw; i++) {	/* Copy buf[] to frame[] */
					/* Note that it must be i<=bw */
		*(f+i) |= *(b+i);
/*		frame[i+xx+(j+y)*FRAME_WIDTH] |= *(b+i);*/
	}
    }
}

dump_frame()
{
	FILE	*dump;
	register int	i;

	if ((dump=fopen("dump","w")) == NULL)
		perror("dump_frame");

	for (i=0; i<FRAME_SIZE; i++) {
		putc(frame[i],dump);
	}
	fclose(dump);
}

init_lbp()
{
    int err;
    struct lbp_size size;

    if (direct == 1) {
	if (lbp == -1) {
	    lbp = open(NWP_DEV,O_WRONLY);
	    if (lbp < 0) {
		perror("open");
		exit(2);
	    }
	} else if (lbp == -2) {
	    return;
	}
    } else {
	lbp = 1;			/* Standard Out */
    }
/**/err = ioctl(lbp,LBIOCRESET,0);
    err = ioctl(lbp,LBIOCGETSIZE,&size);
    if (err < 0) {
	fprintf(stderr,"dvi2nwp: Can't get frame size\n");
	fprintf(stderr,"dvi2nwp: Aborting\n");
	exit(THROW_AWAY_EXIT);
    }

    FRAME_BIT_WIDTH = size.lbp_x;
    FRAME_BIT_HEIGHT = size.lbp_y;
    FRAME_SIZE = FRAME_BIT_WIDTH * FRAME_BIT_HEIGHT / BITS_PAR_CELL;
    FRAME_WIDTH = FRAME_BIT_WIDTH / BITS_PAR_CELL;
    FRAME_HEIGHT = FRAME_BIT_HEIGHT;

    if ( FRAME_SIZE > frame_size_l ) {	/* guard for using A4 undumped module
					   for B4 output */
      if ( frame != NULL )
	free ( frame );

      frame = (unsigned char *) malloc( (FRAME_SIZE+1) ); /* guard against
							     * overshoot
							     */
      frame_size_l = FRAME_SIZE + 1;
    }

    if (line_buf == NULL) { 	/* If Not undumped load module */
	line_buf = (unsigned char *) malloc( MAXBUF );
	line_buf_size = MAXBUF;
    }
    
    if (land_buf == NULL) {	/* If Not undumped load module */
	land_buf = (unsigned char *) malloc( MAXBUF * MAXBUF );
 	land_buf_size = MAXBUF * MAXBUF;
      }

    if (frame == NULL || line_buf == NULL || land_buf == NULL )  {
	perror("dvi2nwp: malloc buffer");
	exit(THROW_AWAY_EXIT);
    }
}

finish_last_page()
{
    int err;
    int s;

    err = ioctl(lbp,LBIOCSTOP,0);
    if (err < 0) {
	s = check_lbp_stop_status(lbp);
	if (s == RESEND_PREV) {
	    return PRINT_THIS;
	} else
	if (s == READY) {
	    ;
	} else {
	    /* Will never come here */
	    /* Reset the printer for another printing. */
	    ioctl(lbp,LBIOCRESET,0);
	    exit(THROW_AWAY_EXIT);
	}
    }

    return PRINT_SUCCEEDED;
}

end_lbp()
{
    int	err;

    if (direct == 1) {
	close(lbp);
    }
    lbp = -1;
/*    free(frame);*//* Do not free !! We can re-use it. */
}

/*
 * Due to the feature (or bug) of NEWS OS, we can't recover from
 * the following error:
 *	Paper Jam,
 *	No cartridge,
 *	Door open, etc.
 * Therefore, dvi2nwp gives up and aborts when the error is occured.
 *
 * In general, if it is needed to open the door of the lbp to
 * fix an error, then we can't continue the printing because:
 *	1. dvi2nwp issues ioctl(2) to get the current lbp status.
 *	2. If the door is open, the call will always causes 
 *		lbp_command: status error -5
 *	   and fail.
 *	3. It seems that the only way to recover from this situation
 *	   is turning off and then on the power of the lbp.
 *
 * In other words, we can not know whether the door is open or not by
 * the status gotten by ioctl(2). We can specify it by checking whether
 * the call is succeeded or not !!
 *
 * This phenomenon is observed on:
 *	NEWS820, OS 2.2
 */
print_frame(current_page)
int	current_page;
{
    int	err;
    int s;

#ifdef PROFILE
return(0);
#endif

    if ( direct && verbose ) {
	printf("Printing current: %d\n",
	       current_page);
    }

  DO_STOP:    
    /* Flush previous page. */
    err = ioctl(lbp,LBIOCSTOP,0);
    if (err < 0) {
	s = check_lbp_stop_status(lbp);
	if (s == RESEND_PREV) {
	    return PRINT_PREV;
	} else
	if (s == READY) {
	    ;		/* continue */
	} else {
	    /* Will never come here */
	    /* Reset the printer for another printing. */
	    ioctl(lbp,LBIOCRESET,0);
	    exit(2);
	}
    }
    
  DO_WRITE:
    if (lseek(lbp,0L,0) != 0L) {
	perror("dvi2nwp");
	/* Reset the printer for another printing. */
	ioctl(lbp,LBIOCRESET,0);
	exit(2);
    }
    err = write(lbp,(char *)frame,FRAME_SIZE);
    if (err != FRAME_SIZE) {
	perror("dvi2nwp");
	/* Reset the printer for another printing. */
	ioctl(lbp,LBIOCRESET,0);
	exit(2);
    }

  DO_START:    
    err = ioctl(lbp,LBIOCSTART,0);
    if (err < 0) {
	s = check_lbp_start_status(lbp);
	if (s == RESTART_THIS) {
	    goto DO_START;
	} else
	if (s == RESEND_THIS) {
	    goto DO_WRITE;
        } else {
	    /* Will never come here */
	    /* Reset the printer for another printing. */
	    ioctl(lbp,LBIOCRESET,0);
	    exit(2);
	}
    }

#ifdef ACCT
    npages++;
#endif ACCT
    return PRINT_SUCCEEDED;
}

check_lbp_stop_status(lbp)
/* Return:
 * READY 		lbp is ready.
 * REPRINT_PREV 	Reprinting of the previous page is needed.
 */
  int	lbp;
{
    struct lbp_stat status;
    int	err;
    int reprints = -1;		/* Is this really needed ? */
    int	need_resend = 0;
    int close_count = 0;

  AGAIN:
    /* Clear status */
    status.stat[0] = status.stat[1] = status.stat[2] = 0;
    
    err = ioctl(lbp,LBIOCSTATUS,&status);
    if (err < 0) {
	fprintf(stderr,"dvi2nwp: Can't get LBP status.\n");
#ifndef CLOSE_AND_RE_OPEN
	fprintf(stderr,"dvi2nwp: Aborting\n");
	fprintf(stderr,"dvi2nwp: Maybe the door is open or a ");
	fprintf(stderr,"paper is jammed.\n");
	fprintf(stderr,"dvi2nwp: To recover, remove the paper if any, ");
	fprintf(stderr,"close the door, and ");
	fprintf(stderr,"turn off and on the LBP.\n");

	/* Abort and do not retry (see the comment just above the 
	 * print_frame())
	 */
	exit(THROW_AWAY_EXIT);
#else /* CLOSE_AND_RE_OPEN will never be defined */
	if (close_count < MAX_LBP_CLOSE_COUNT) {
	    fprintf(stderr,"dvi2nwp: Please turn off and on the LBP\n");
	    fprintf(stderr,"dvi2nwp: Sleeping 60 sec");
	    SLEEP60();
	    fprintf(stderr,"dvi2nwp: Closeing and re-opening LBP...\n");
	    close(lbp);
	    lbp = open(NWP_DEV,O_WRONLY);
	    if (lbp < 0) {
		fprintf(stderr,"dvi2nwp: LBP close and re-open failed\n");
		fprintf(stderr,"dvi2nwp: Aborting\n");
		exit(THROW_AWAY_EXIT);
	    }
	    need_resend = 1;
	    goto AGAIN;
	} else {
	    fprintf(stderr,"dvi2nwp: Too many close & open\n");
	    fprintf(stderr,"dvi2nwp: Aborting\n");
	    fprintf(stderr,"dvi2nwp: Maybe the door is open or a ");
	    fprintf(stderr,"paper is jammed.\n");
	    fprintf(stderr,"dvi2nwp: To recover, remove the paper if any, ");
	    fprintf(stderr,"close the door, and ");
	    fprintf(stderr,"turn off and on the LBP.\n");

	    exit(THROW_AWAY_EXIT);
	}
#endif /* CLOSE_AND_RE_OPEN */
    }
    
    if (status.stat[0] & ST0_CALL) {
	/* Check LBP malfunction */
	if (status.stat[2] & ST2_FIXER) {
	    fprintf(stderr,"dvi2nwp: LBP Fixer malfunction. ");
	}
	if (status.stat[2] & ST2_SCANNER) {
	    fprintf(stderr,"dvi2nwp: LBP Scanner malfunction. ");
	}
	if (status.stat[2] & ST2_MOTOR) {
	    fprintf(stderr,"dvi2nwp: LBP Motor malfunction. ");
	}
	if (((status.stat[2])&0x0fe) != 0) { /*  Mask parity bit */
	    fprintf(stderr,"dvi2nwp: Aborting\n");
	    /* Reset the printer for another printing. */
	    ioctl(lbp,LBIOCRESET,0);
	    exit(THROW_AWAY_EXIT);
	}
	
	/* LBP is OK, but some minor errors occured. */
	if (status.stat[1] & ST1_NO_PAPER) {
	    fprintf(stderr,"dvi2nwp: LBP Out of paper. ");
	    fprintf(stderr,"Sleeping 10 sec.\n");
	    sleep(10);
	    goto AGAIN;
	} else {
	    /* The followings need special care */
	    if (status.stat[1] & ST1_NO_CARTRIGE) {
		fprintf(stderr,"dvi2nwp: LBP No tonner cartrige. ");
	    }
	    if (status.stat[1] & ST1_JAM) {
		fprintf(stderr,"dvi2nwp: LBP Paper jammed. ");
	    }
	    if (status.stat[1] & ST1_OPEN) {
		fprintf(stderr,"dvi2nwp: LBP Door is open. ");
	    }
	    if (status.stat[1] & ST1_TEST) {
		fprintf(stderr,"dvi2nwp: LBP Doing test print. ");
	    }
	    fprintf(stderr,"dvi2nwp: Sleeping 60 sec");
	    SLEEP60();
	    if (reprints < 0) /* Set reprints just once */
	      reprints = NREPRINT(status.stat[3]);
fprintf(stderr,"dvi2nwp: Need reprint:%d\n",reprints);
	    err = ioctl(lbp,LBIOCRESET,0);
	    if (err < 0) {
		fprintf(stderr,"dvi2nwp: Can't reset LBP.\n");
	    }
	    need_resend = 1;
	    goto AGAIN;
	}
    }

    if (status.stat[0] & ST0_WAIT) {
	fprintf(stderr,"dvi2nwp: LBP Not ready, please wait. ");
	fprintf(stderr,"dvi2nwp: Sleeping 10 sec.\n");
	sleep(10);
	goto AGAIN;
    }
    
    /* LBP is now OK. let's retry printing */
    if (need_resend) return RESEND_PREV;
    return READY;
}

check_lbp_start_status(lbp)
/* Return:
 *   RESTART_THIS	Restart printing this page.
 *   RESEND_THIS	Re-send the image and restart printing.
 */
  int	lbp;
{
    struct lbp_stat status;
    int	err;
    int	need_reprint = 0;
    
  AGAIN:
    /* Clear status */
    status.stat[0] = status.stat[1] = status.stat[2] = 0;
    
    err = ioctl(lbp,LBIOCSTATUS,&status);
    if (err < 0) {
	fprintf(stderr,"dvi2nwp: Aborting\n");
	fprintf(stderr,"dvi2nwp: Maybe the door is open or a ");
	fprintf(stderr,"paper is jammed.\n");
	fprintf(stderr,"dvi2nwp: To recover, remove the paper if any, ");
	fprintf(stderr,"close the door, and ");
	fprintf(stderr,"turn off and on the LBP.\n");

	/* Abort and do not retry (see the comment just above the 
	 * print_frame())
	 */
	exit(THROW_AWAY_EXIT);
    }
    
    if (status.stat[0] & ST0_CALL) {
	/* Check LBP malfunction */
	if (status.stat[2] & ST2_FIXER) {
	    fprintf(stderr,"dvi2nwp: LBP Fixer malfunction. ");
	}
	if (status.stat[2] & ST2_SCANNER) {
	    fprintf(stderr,"dvi2nwp: LBP Scanner malfunction. ");
	}
	if (status.stat[2] & ST2_MOTOR) {
	    fprintf(stderr,"dvi2nwp: LBP Motor malfunction. ");
	}
	if (((status.stat[2])&0x0fe) != 0) { /*  Mask parity bit */
	    fprintf(stderr,"dvi2nwp: Aborting\n");
	    /* Reset the printer for another printing. */
	    ioctl(lbp,LBIOCRESET,0);
	    exit(THROW_AWAY_EXIT);
	}
	
	/* LBP is OK, but some minor errors occured. */
	if (status.stat[1] & ST1_NO_PAPER) {
	    fprintf(stderr,"dvi2nwp: LBP Out of paper. ");
	    fprintf(stderr,"Sleeping 10 sec.\n");
	    sleep(10);
	    goto AGAIN;
	} else {
	    /* The followings need special care */
	    if (status.stat[1] & ST1_NO_CARTRIGE) {
		fprintf(stderr,"dvi2nwp: LBP No tonner cartrige. ");
	    }
	    if (status.stat[1] & ST1_JAM) {
		fprintf(stderr,"dvi2nwp: LBP Paper jammed. ");
	    }
	    if (status.stat[1] & ST1_OPEN) {
		fprintf(stderr,"dvi2nwp: LBP Door is open. ");
	    }
	    if (status.stat[1] & ST1_TEST) {
		fprintf(stderr,"dvi2nwp: LBP Doing test print. ");
	    }
	    fprintf(stderr,"dvi2nwp: Sleeping 60 sec.\n");
	    sleep(60);
	    err = ioctl(lbp,LBIOCRESET,0);
	    if (err < 0) {
		fprintf(stderr,"dvi2nwp: Can't reset LBP.\n");
		fprintf(stderr,"dvi2nwp: Aborting\n");
		exit(THROW_AWAY_EXIT);
	    }
	    need_reprint = 1;
	    goto AGAIN;
	}
    }

    if (status.stat[0] & ST0_WAIT) {
	fprintf(stderr,"dvi2nwp: LBP Not ready, please wait. ");
	fprintf(stderr,"dvi2nwp: Sleeping 10 sec.\n");
	sleep(10);
	goto AGAIN;
    }
    
    /* LBP is now OK. let's retry printing */
    if (need_reprint) return RESEND_THIS;
    return RESTART_THIS;
}

