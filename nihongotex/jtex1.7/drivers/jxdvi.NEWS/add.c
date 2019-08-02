 /*
 *   add.c
 *	$Header: add.c,v 1.9 88/02/23 14:59:44 atsuo Exp $
 *	written by Atsuo, K. (ISIR)
 */

#include "atsuo-copyright.h"

#include <stdio.h>
#include <X/Xlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <newsiop/lbp.h>
#include "pxl.h"
#include "add.h"

char	tmp_file[64];
int	direct = 0;
int	verbose = 0;
int	double_side = 0;
int	x_offset = 0, y_offset = 0;
int	odd_pages = 0, even_pages = 0;
int	begin_page_num = 0, end_page_num = 9999;
int	noprint = 0;

extern	current_page;

unsigned char frame[FRAME_SIZE+1];	/* guard against overshoot */
int	lbp = -1;

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
 *	1. bitmap->nbits contains no garbage bits in padding bits.
 *	   That is, pad bits are all zeros.
 */

put_bitmap_to_image(bitmap, x, y)
	register struct bitmap *bitmap;
	long x;
	register long y;
{
	register int	i, j;
	register int	bw;
	int	rs;
	int	w, h, xx;
	unsigned char	buf[128]; /* Must be unsigned !! 128 bytes may be enough */
	register unsigned char	*b=buf;
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
	nb = (unsigned char *) bitmap->nbits;

	if ( x<0 || y<0 ) return;
        if ( (x+w)>FRAME_BIT_WIDTH || (y+h)>FRAME_BIT_HEIGHT ) return;

	xx = (int) x/BITS_PAR_CELL;
	rs = (int) x%BITS_PAR_CELL;			/* 8 is bits width of char */

	for (j=0; j<h; j++) {
	    bcopy((nb+j*bw),b,bw);			/* Copy nbits to buf */
	    if (rs != 0) {				/* Shift buf[] >> rs */
		*(b+bw) = (*(b+bw-1)) << (BITS_PAR_CELL-rs);
		for (i=bw-1; i>0; i--) {
		    *(b+i) = ( (*(b+i)) >> rs ) | ( (*(b+i-1) << (BITS_PAR_CELL-rs)) );
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
	if (direct == 1) {
		if (lbp == -1) {
			lbp = open(NWP_DEV,O_WRONLY);
			if (lbp < 0) {
				perror("open");
				lbp = -2;
			}
		} else if (lbp == -2) {
			return;
		}
	} else {
		lbp = 1;			/* Standard Out */
	}
}

end_lbp()
{
	int	err;
AGAIN:
	err = ioctl(lbp,LBIOCSTOP,0);
	if (err < 0) {
		if (check_lbp_status(lbp) == RETRY) goto AGAIN;
		else exit(2);
	}

	if (direct == 1) {
		close(lbp);
	}
}

restart_lbp()
{
	int	err;
	
AGAIN:
printf("Restarting LBP\n");
	err = ioctl(lbp,LBIOCSTART,0);
	if (err < 0) {
		if (check_lbp_status(lbp) == RETRY) goto AGAIN;
		else exit(2);
	}
	err = ioctl(lbp,LBIOCSTOP,0);
	if (err < 0) {
	/* If lbp is ready, restart lbp before printing new frame. */
		if (check_lbp_status(lbp) == RETRY) restart_lbp();
		else exit(2);
	}
}

print_frame(current_page)
int	current_page;
{
	int	err;
#ifdef PROFILE
return(0);
#endif
	if ( direct && verbose ) {
		printf("Printing current: %d\n",
				current_page);
	}

	/* Flush previous page. */
	err = ioctl(lbp,LBIOCSTOP,0);
	if (err < 0) {
		/* If REPRINT needed, restart lbp. */
		if (check_lbp_status(lbp) == RESTART) restart_lbp();
	}

AGAIN:
	if (lseek(lbp,0L,0) != 0L) {
		perror("dvi2nwp");
		exit(2);
	}
	
	err = write(lbp,(char *)frame,FRAME_SIZE);
	if (err != FRAME_SIZE) {
		perror("dvi2nwp");
		exit(2);
	}

	err = ioctl(lbp,LBIOCSTART,0);
	if (err < 0) {
		if (check_lbp_status(lbp) == RETRY) goto AGAIN;
		else exit(2);
	}
#ifdef ACCT
	npages++;
#endif ACCT
}

check_lbp_status(lbp)
/* 
 * Return RETRY if LBP is ready. 
 * Return RESTART if reprint needed.
 */
int	lbp;
{
	struct lbp_stat status;
	int	err;
	
AGAIN:
/* Clear status */
	status.stat[0] = status.stat[1] = status.stat[2] = 0;
	
	err = ioctl(lbp,LBIOCSTATUS,&status);
	if (err < 0) {
		fprintf("dvi2nwp:Can't get LBP status. Aborting\n");
		exit(2);
	}
	
	if (status.stat[0] & ST0_CALL) {
		/* Check LBP malfunction */

		if (status.stat[2] & ST2_FIXER) {
			fprintf(stderr,"LBP Fixer malfunction. ");
		}
		if (status.stat[2] & ST2_SCANNER) {
			fprintf(stderr,"LBP Scanner malfunction. ");
		}
		if (status.stat[2] & ST2_MOTOR) {
			fprintf(stderr,"LBP Motor malfunction. ");
		}
		if (((status.stat[2])&0x0fe) != 0) { /*  Mask parity bit */
			fprintf(stderr,"dvi2nwp:Aborting\n");
			exit(2);
		}

		/* LBP is OK, but some minor errors occured. */
		if (status.stat[1] & ST1_NO_CARTRIGE) {
			fprintf(stderr,"LBP No tonner cartrige. ");
		}
		if (status.stat[1] & ST1_NO_PAPER) {
			fprintf(stderr,"LBP Out of paper. ");
		}
		if (status.stat[1] & ST1_JAM) {
			fprintf(stderr,"LBP Paper jammed. ");
		}
		if (status.stat[1] & ST1_OPEN) {
			fprintf(stderr,"LBP Door is opened. ");
		}
		if (status.stat[1] & ST1_TEST) {
			fprintf(stderr,"LBP Doing test print. ");
		}
		if (((status.stat[1])&0x0fe) != 0) { /*  Mask parity bit */
			fprintf(stderr,"dvi2nwp is sleeping 60 sec.\n");
			sleep(60);
			goto AGAIN;
		}
	}
	if (status.stat[0] & ST0_WAIT) {
		fprintf(stderr,"LBP Not ready, please wait. ");
		fprintf(stderr,"dvi2nwp is sleeping 60 sec.\n");
		sleep(60);
		goto AGAIN;
	}

	/* LBP is now OK. let's retry printing */
	if (status.stat[0] & ST0_REPRINT_REQ) {
printf("Need reprint:%d\n",NREPRINT(status.stat[3]));
		return RESTART;
	}
	return RETRY;
}
