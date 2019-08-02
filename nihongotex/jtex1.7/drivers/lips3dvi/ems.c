/*
 *	ems.c -- EMS memory mapping routine (for MSDOS)
 *
 *	Written by A. Iwamoto on 10/15/91 15:54:02.
 *	Modified by H. Nagahara on 2/10/92 08:58:17
 *	Copyright (C) 1991-1992 by ASCII Corporation.
 *		SCCS memo: ems.c 10.1
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<dos.h>
#include	<signal.h>
#include	<memory.h>
#include	"config.h"
#include	"font.h"

#if	USEEMS


#define EMS_FRAME_SIZE 0x4000		/* 16K bytes/frame */

static int	current_map_page	= -1;	/*current page on the segment*/
static int	last_map_page		= 0;	/* offset of free page */

static int	EMS_OK 			= 0;	/* EMS is working */
static int	EMS_get_pages		= 0;	/* enable EMS pages */

static int	ems_handle		= -1;	/* EMS handle of this proc */

static char	*ems_start;		/* ems_start address */
static char	*ems_end;		/* ems_end address */
static char	*alloc_ptr;		/* current allocating position */


#ifdef EMS_DEBUG
#define disp_ems_error(r) fprintf(stderr,"line =  %d |ah = %d|\n",__LINE__,r)
#else
#define disp_ems_error(r)
#endif

void open_EMS(void);
void close_EMS(void);

#ifdef EMS_DEBUG
main()
{
	open_EMS();
	printf("ptr = %lx\n",ems_start);
	printf("ptr = %lx\n",ems_end);
	map_EMS(0);
	strcpy(ems_start,"page1");
	map_EMS(1);
	strcpy(ems_start,"page2");

	map_EMS(0);
	printf("%s\n",ems_start);
	map_EMS(1);
	printf("%s\n",ems_start);

	close_EMS();
	exit(0);
}
#endif

/*
 * try to open EMS
 *
 * set flag EMS_OK on success
 *
 */

void
open_EMS()
{
	union	REGS	regs;
	struct	SREGS	sregs;
	char		*vector;

	/* getvector */
	regs.x.ax = 0x3567;
	intdosx(&regs, &regs, &sregs);
	vector = (char *)((unsigned long)(sregs.es) << 16L | 10L);

	/* exist EMS? */

	if (memcmp("EMMXXXX0", vector, 8) != 0) {
		return;
	}


	/* EMS ready? */
	regs.h.ah = 0x40;
	int86(0x67, &regs, &regs);

	if (regs.h.ah != 0) {
		disp_ems_error(regs.h.ah);
		return;
	}

	/* Version is greater than 3.2 ? */
	regs.h.ah = 0x46;
	int86 (0x67, &regs, &regs);

	if ((regs.h.ah != 0) || (regs.h.al < 0x32)) {
		disp_ems_error(0);
		return;
	}

	/*  Get page frame address */
	regs.h.ah = 0x41;
	int86 (0x67, &regs, &regs);

	if (regs.h.ah != 0) {
		disp_ems_error(regs.h.ah);
		return;
	}

	FP_SEG(ems_start) = regs.x.bx;
	FP_OFF(ems_start) = 0;
	alloc_ptr = ems_start;
	ems_end = ems_start + EMS_FRAME_SIZE;

	/* Get the number of free pages */

	regs.h.ah = 0x42;
	int86(0x67, &regs, &regs);

	if (regs.h.ah != 0) {
		disp_ems_error(regs.h.ah);
		return;
    	}

	EMS_get_pages = regs.x.bx;

	if ( EMS_get_pages <= 0 ) return;

	regs.h.ah = 0x43;
	regs.x.bx = EMS_get_pages;
	int86(0x67, &regs, &regs);

	if (regs.h.ah != 0) {
		disp_ems_error(regs.h.ah);
		return;
    	}

	ems_handle = regs.x.dx;

	set_signal();
	EMS_OK++;
}

/*
 * free EMS handle
 */

void
close_EMS()
{
	union REGS	regs;

	if ( !EMS_OK || ems_handle < 0 ) return;
	regs.h.ah = 0x45;
	regs.x.dx = ems_handle;
	int86(0x67, &regs, &regs);
}

/*
 * map the page
 */

int
map_EMS(page)
	int page;
{
	union REGS		regs;

	if ( !EMS_OK ) return -1;

	if ( page == current_map_page )
		return 0;

	regs.h.ah = 0x44;
	regs.h.al = 0;		/* map on phisical 0 page */
	regs.x.bx = page;
	regs.x.dx = ems_handle;

	int86(0x67, &regs, &regs);

	if (regs.h.ah != 0) {
		disp_ems_error(regs.h.ah);
		EMS_OK = !EMS_OK;
		return -1;
    	}
	
	current_map_page = page;

	return 0;
}

/*
 * allocate memory on EMS
 */

char *ems_alloc(len)
int	len;
{
	if ( !EMS_OK || len > EMS_FRAME_SIZE ) NULL;

	if ( alloc_ptr + len >= ems_end ) {
		if (last_map_page < EMS_get_pages - 1) {
			last_map_page++;
			alloc_ptr = ems_start;
		} else {
			return NULL;
		}
	}

	map_EMS(last_map_page);
	alloc_ptr += len;
	return alloc_ptr - len;
}

/*
 * allocate memory for struct grif bitmap
 */

void
alloc_grif_bitmap(map,len)
	struct grif *map;
	int len;
{
	char *ptr;

	if (!EMS_OK || (ptr = ems_alloc(len)) == NULL ) {
		ptr = malloc(len);
		map->emspage = 0;
		map->bitmap = ptr;
		return;
	}

	map->emspage = current_map_page + 1;
	map->bitmap = ptr;
}

/*
 * allocate memory for struct grif bitmap ( fill with \0 )
 */

void
calloc_grif_bitmap(map,len,size)
	struct grif *map;
	int len,size;
{
	alloc_grif_bitmap(map,len*size);
	if ( map->bitmap != 0 ) {
		memset(map->bitmap,'\0',len*size);
	}
}


/*
 * free memory on struct grif bitmap (DO NOTHING on EMS bitmap)
 */

void
free_grif_bitmap(map)
	struct grif *map;
{
	if ( map->emspage == 0 ) {
		free(map->bitmap);
	}
}

/*
 * map memory on the segment
 */

void
map_grif_bitmap(map)
	struct grif *map;
{
	if ( map->emspage ) {
		map_EMS(map->emspage - 1);
	}
}

/*
 * allocate memory for struct fdir bitmap
 */

void
alloc_fdir_bitmap(map,len)
	struct fdir *map;
	int len;
{
	char *ptr;

	if (!EMS_OK || (ptr = ems_alloc(len)) == NULL ) {
		ptr = malloc(len);
		map->emspage = 0;
		map->bitmap = ptr;
		return;
	}

	map->emspage = current_map_page + 1;
	map->bitmap = ptr;
}

/*
 * free memory on struct grif bitmap (DO NOTHING on EMS bitmap)
 */

void
free_fdir_bitmap(map)
	struct fdir *map;
{
	if ( map->emspage == 0 ) {
		free(map->bitmap);
	}
}

/*
 * map memory on the segment
 */

void
map_fdir_bitmap(map)
	struct fdir *map;
{
	if ( map->emspage ) {
		map_EMS(map->emspage - 1);
	}
}

/*
 * abnormal end
 */

void
abort_end()
{
	close_EMS();
	exit(3);
}

void
set_signal()
{
	signal(SIGINT, abort_end);
	signal(SIGABRT, abort_end);
	atexit(close_EMS);
}


#endif	/* USEEMS */
