#include "../common/def.h"

#define	AMIN24		(MIN24_NEW | MIN24)
#define	AMIN32		(MIN32_NEW | MIN32)
#define	AMIN40		(MIN40_NEW | MIN40)
#define	ALL24		(MIN24_NEW | GOT24_NEW | MIN24 | GOT24)
#define	ALL32		(MIN32_NEW | GOT32_NEW | MIN32 | GOT32)
#define	ALL40		(MIN40_NEW | GOT40_NEW | MIN40 | GOT40)
#define	FS24		(selfont & ((*face & GOTH)?ALL24:AMIN24))
#define	FS32		(selfont & ((*face & GOTH)?ALL32:AMIN32))
#define	FS40		(selfont & ((*face & GOTH)?ALL40:AMIN40))

#define	GOTH		0x03
#define	NEWJIS		0x04
#define	DSET		0x08

int	selfont=SELJFONT;
int	last_size, old_type_face, dsetf;

extern	int	kmode;
extern	FONT	font[];

calc_pxl_size(size, face)
char	*face;
{
	int	s;

	if((size >= 40*2-4) && FS40)
		s = 40*2;
	else if((size >= 32*2-4) && FS32)
		s = 32*2;
	else if((size >= 24*2-3) && FS24)
		s = 24*2;
	else if((size >= 40-4) && FS40)
		s = 40;
	else if((size >= 32-2) && FS32)
		s = 32;
	else if((size >= 24-1) && FS24)
		s = 24;
	else if((size >= 40/2-1) && DIVF && FS40)
		s = 20;
	else if((size >= 32/2-1) && DIVF && FS32)
		s = 16;
	else if(DIVF && FS24)
		s = 12;
	else s = 32;

	*face &= GOTH;
	switch(s){
	case 12:
	case 24:
	case 48:
		if(*face & GOTH){
			if(selfont & GOT24_NEW){
				*face |= NEWJIS;
				break;
			}
			if(!(selfont & GOT24))
				*face |= DSET;
		}
		if(selfont & MIN24_NEW)
			*face |= NEWJIS;
		break;
	case 16:
	case 32:
	case 64:
		if(*face & GOTH){
			if(selfont & GOT32_NEW){
				*face |= NEWJIS;
				break;
			}
			if(!(selfont & GOT32))
				*face |= DSET;
		}
		if(selfont & MIN32_NEW)
			*face |= NEWJIS;
		break;
	case 20:
	case 40:
	case 80:
		if(*face & GOTH){
			if(selfont & GOT40_NEW){
				*face |= NEWJIS;
				break;
			}
			if(!(selfont & GOT40))
				*face |= DSET;
		}
		if(selfont & MIN40_NEW)
			*face |= NEWJIS;
		break;
	}
	return(s);
}


select_size(n)
{
	int	m, s;

	m = 100;
	switch(n){
		case 24/2:	m = -50;
		case 24*2:	m += 100;
		case 24:	s = 1000;
				break;
		case 32/2:	m = -50;
		case 32*2:	m += 100;
		case 32:
		default:	s = 750;
				break;
		case 40/2:	m = -50;
		case 40*2:	m += 100;
		case 40:	s = 600;
				break;
	}
	printf("\233?%d K\233%d;%d B", s, m, m);
}

selectfont(f)
{
	FONT	*p;

	p = &font[f];
	if(p->id != JFM_ID){
		if(kmode){
			printf("\033(/ 0");/* designate 3840 to G1 */
			kmode = 0;
			if(last_size!=24 && last_size!=32 &&last_size!=40){
				last_size = 0;
				printf("\233100;100 B");
			}
		}
		return;
	}
	if(p->face & NEWJIS){
		printf("\033(,2\017");
		kmode = 1;
	}else{
		printf("\033(,0\017");
		kmode = 1;
	}
	if(last_size != p->size){
		last_size = p->size;
		select_size(last_size);
	}
	if(p->face & GOTH){
		if(p->face & DSET){
			dsetf = p->face & GOTH;
			return;
		}
		if(!old_type_face){
			printf("\23381y");
			old_type_face = 1;
		}
	}else{
		if(old_type_face){
			printf("\23380y");
			old_type_face = 0;
		}
	}
/*
	if(dsetf){
		printf("\23322m");
		dsetf = 0;
	}
*/
}

adjust_yoffset(s)
{
	switch(s){
	case 16:
	case 19:
	case 28:
		return(-1);
	case 34:
	case 25:
	case 37:
	case 76:
	case 53:
		return(-2);
	case 31:
		return(-3);
	case 64:
		return(1);
	default:
		return(0);
	}
}

adjust_xoffset(s)
{
	switch(s){
	case 19:
	case 22:
	case 28:
	case 31:
	case 37:
	case 44:
	case 53:
		return(1);
	case 64:
	case 76:
		return(2);
	default:
		return(0);
	}
}
