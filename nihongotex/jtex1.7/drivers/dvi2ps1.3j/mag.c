/* borrowed from Beebe's dvi driver
 * modified actfact to use binary search  (sakurai)
 */

#include	<stdio.h>
#include	"defs.h"
#include	"global.h"

/***********************************************************************
  sqrt(1.2)**I	(I = -16,16)
***********************************************************************/

float mag_table[] = {
0.23256803936137783874,
0.25476552262595201888,
0.27908164723365340649,
0.30571862715114242265,
0.33489797668038408779,
0.36686235258137090718,
0.40187757201646090535,
0.44023482309764508862,
0.48225308641975308642,
0.52828178771717410634,
0.57870370370370370370,
0.63393814526060892761,
0.69444444444444444444,
0.76072577431273071313,
0.83333333333333333333,
0.91287092917527685576,
1.00000000000000000000,
1.09544511501033222690,
1.20000000000000000000,
1.31453413801239867230,
1.44000000000000000000,
1.57744096561487840680,
1.72800000000000000000,
1.89292915873785408810,
2.07360000000000000000,
2.27151499048542490570,
2.48832000000000000000,
2.72581798858250988690,
2.98598400000000000000,
3.27098158629901186430,
3.58318080000000000000,
3.92517790355881423710,
4.29981696000000000000,
};

int mag_index;		/* set by apprfact */
#define	MAGTABSIZE	(sizeof(mag_table) / sizeof(float))
int magtabsize = MAGTABSIZE;


/*-->actfact*/
/**********************************************************************/
/****************************** actfact *******************************/
/**********************************************************************/

float
actfact(magfact)
float magfact;

/***********************************************************************
Compute the actual size factor given the integer approximation unmodsize
= (magnification  factor)*1000.
***********************************************************************/

{
    register unsigned int unmodsize;
    register int k;
    register unsigned int tab_entry;
    register int left, right;

    unmodsize = MAGSIZE(magfact);
    for (left = 0, right = MAGTABSIZE; left <= right; ) {
	k = (left+right)/2;
	tab_entry = MAGSIZE(mag_table[k]);	/* round entry */
	if (unmodsize == tab_entry)
	    return ((float)mag_table[k]);
	else if (unmodsize < tab_entry)
	    right = k-1;
	else /* if (unmodsize > tab_entry) */
	    left = k+1;
    }
#ifdef STATS
    if (Stats)
	fprintf(stderr, "Non-standard mag %d\n", unmodsize);
#endif
    return (magfact);
}

float
apprfact(magfact)
float magfact;

/***********************************************************************
Compute the actual size factor given the integer approximation unmodsize
= (magnification  factor)*1000.   Values  not found  in  the  table  are
rounded to the nearest table  entry; this ensures that rounding  errors,
or  user  magnification  parameter  input  errors  result  in  something
reasonable.  mag_table[] has a  wider range of magnifications  available
than most sites will have, and can be used to find a nearest match  font
when one is missing.
***********************************************************************/

{
    register unsigned int unmodsize;
    register int k;
    register unsigned int tab_entry;
    register int left, right;

    unmodsize = MAGSIZE(magfact);
    for (left = 0, right = MAGTABSIZE; ;) {
	k = (left+right)/2;
	if (left+1 == right)
	    break;
	tab_entry = MAGSIZE(mag_table[k]);	/* round entry */
	if (unmodsize == tab_entry)
	    break;
	else if (unmodsize < tab_entry)
	    right = k;
	else /* if (unmodsize > tab_entry) */
	    left = k;
    }
    return((float)mag_table[mag_index = k]);
}
