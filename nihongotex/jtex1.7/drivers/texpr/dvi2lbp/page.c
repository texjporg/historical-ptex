/*
 *	Copyright (c) Japan TeX Users Group, 1986 1987
 */

#include <stdio.h>
#include "../common/dvicom.h"
#include "../common/def.h"

int	h, v, w, x, y, z, f;
int	dh, dv;
int	stack[];
FILE	*dvi;
extern int	t_margin, l_margin;
extern float	mag;

/*	put pages	*/

put_pages()
{
	register int *sp, cmd;
	register a, b;

	for( ; ; ) {
		switch(cmd = get_uint(dvi, 1)) {

		case BOP:
			fseek(dvi, 44, 1);
			w = x = y = z = 0;
			v = (float)t_margin/(mag*(DPI/(65536*72.27)));
			h = (float)l_margin/(mag*(DPI/(65536*72.27)));
			sp = stack;  f = -1;
			dh = dv = 0;
			continue;

		case EOP:
			flush_page();
			continue;

		case SET1: case SET2: case SET3: case SET4:
			set_char(get_uint(dvi, cmd-SET1+1), 1);
			continue;

		case PUT1: case PUT2: case PUT3: case PUT4:
			set_char(get_uint(dvi, cmd-PUT1+1), 0);
			continue;

		case SET_RULE: case PUT_RULE:
			a = get_int(dvi, 4);  b = get_int(dvi, 4);
			set_rule(a, b, cmd-PUT_RULE);
			continue;

		case RIGHT1: case RIGHT2: case RIGHT3: case RIGHT4:
			h += get_int(dvi, cmd-RIGHT1+1);
			continue;

		case DOWN1: case DOWN2: case DOWN3: case DOWN4:
			v += get_int(dvi, cmd-DOWN1+1);
			continue;

		case W1: case W2: case W3: case W4:
			 w = get_int(dvi, cmd-W1+1);
		case W0: h += w;
			 continue;

		case X1: case X2: case X3: case X4:
			 x = get_int(dvi, cmd-X1+1);
		case X0: h += x;
			 continue;

		case Y1: case Y2: case Y3: case Y4:
			 y = get_int(dvi, cmd-Y1+1);
		case Y0: v += y;
			 continue;

		case Z1: case Z2: case Z3: case Z4:
			 z = get_int(dvi, cmd-Z1+1);
		case Z0: v += z;
			 continue;

		case FNT1: case FNT2: case FNT3: case FNT4:
			f = get_uint(dvi, cmd-FNT1+1);
			selectfont(f);
			continue;

		case FNT_DEF1: case FNT_DEF2: case FNT_DEF3: case FNT_DEF4:
			fseek(dvi, cmd-FNT_DEF1+13, 1);	/* alredy processed */
			fseek(dvi, get_uint(dvi,1)+get_uint(dvi,1), 1);
			continue;

		case XXX1: case XXX2: case XXX3: case XXX4:
			fseek(dvi, get_uint(dvi, cmd-XXX1+1), 1);
			continue;

		case PUSH:		/* no overflow */
			*sp++ = h; *sp++ = v; *sp++ = w;
			*sp++ = x; *sp++ = y; *sp++ = z;
			continue;

		case POP:
			z = *--sp; y = *--sp; x = *--sp;
			w = *--sp; v = *--sp; h = *--sp;
			continue;

		case NOP:
			continue;

		case POST:
			break;

		default:	
			if(cmd <= SET_CHAR_127)
				set_char(cmd, 1);
			else if(cmd >= FNT_NUM_0 && cmd <= FNT_NUM_63){
				f = cmd-FNT_NUM_0;
				selectfont(f);
			}else
				error("illegal dvi command %d.\n", cmd);
			continue;
		}
		break;
	}
}
