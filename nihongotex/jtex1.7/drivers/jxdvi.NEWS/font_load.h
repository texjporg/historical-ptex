/*
**	"font_load.h"
**		written by Atsuo, K. 
**		on 88/01/12
*/

#include "atsuo-copyright.h"  

struct code_range {
	short	start;			/* start of jis code */
	short	end;			/* end of jis code */
};

struct fldent {
	char	fontname[128];			/* name of font */
	int		sc;						/* scaled size */
	int		d_sc;					/* design size */
	int		count;					/* number of ranges */
	struct code_range *ranges;		/* list of code ranges */
};
