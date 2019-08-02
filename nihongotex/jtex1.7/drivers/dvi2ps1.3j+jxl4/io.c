#include	<stdio.h>
#include	"defs.h"
#include	"global.h"

#define	outc(c)	putc(c,outfp)

/*-->getbytes*/
/**********************************************************************/
/*****************************  getbytes  *****************************/
/**********************************************************************/

void
getbytes(fp, cp, n)	/* get n bytes from file fp */
register FILE *fp;	/* file pointer	 */
register char *cp;	/* character pointer */
register int n;		/* number of bytes  */
{
    while (n--)
	*cp++ = getc(fp);
}


/*-->getuint*/
/**********************************************************************/
/***************************  getuint  ********************************/
/**********************************************************************/

int
getuint(fp, n)		/* return n byte quantity from file fd */
register FILE *fp;	/* file pointer    */
register int n;		/* number of bytes */
{
    register int x;	/* number being constructed */

    x = 0;
    while (n--)  {
	x <<= 8;
	x |= getc(fp);
    }
    return(x);
}


/*-->getint*/
/**********************************************************************/
/****************************  getint  ********************************/
/**********************************************************************/

int
getint(fp, n)		/* return n byte quantity from file fd */
register FILE *fp;	/* file pointer    */
register int n;		/* number of bytes */
{
    int n1;		/* number of bytes	    */
    register int x;	/* number being constructed */

    x = getc(fp);	/* get first (high-order) byte */
    n1 = n--;
    while (n--)  {
	x <<= 8;
	x |= getc(fp);
    }

    /* NOTE: This code assumes that the right-shift is an arithmetic, rather
    than logical, shift which will propagate the sign bit right.   According
    to Kernighan and Ritchie, this is compiler dependent! */

    x<<=32-8*n1;
    x>>=32-8*n1;  /* sign extend */

#ifdef DEBUG
    if (Debug)
    {
	fprintf(stderr,"\tgetint(fp,%d)=%X\n",n1,x);
    }
#endif
    return(x);
}


/* formatted i/o was killing us, so build some tables */
char    *digit = "0123456789ABCDEF";

/*-->putint*/
/**********************************************************************/
/*****************************  putint  *******************************/
/**********************************************************************/

void
putint(n)               /* output an integer followed by a space */
register int n;
{
    char buf[10];
    register char *b;

    if( n == 0 )
	outc('0'); 
    else {
	if( n < 0 ) {
	    outc('-');
	    n = -n;
	    }
    
	for(b=buf;  n>0;  ) {
	    *b++ = digit[n%10];
	    n /= 10;
	    }
    
	for( ; b>buf; )
	    outc(*--b);
	}

    outc(' ');
}


/*-->putoct*/
/**********************************************************************/
/*****************************  putoct  *******************************/
/**********************************************************************/

void
putoct(n)               /* output an 3 digit octal number preceded by a "\" */
register int n;
{
    outc('\\');
    outc(digit[(n&0300)>>6]);
    outc(digit[(n&0070)>>3]);
    outc(digit[n&0007]);
}


/*-->copyfile*/   /* copy a file straight through to output */
/*********************************************************************/
/***************************** copyfile ******************************/
/*********************************************************************/

void
copyfile( str )
char    *str;
{
	FILE    *spfp;
	int     t;

	if( (spfp=fopen(str,"r")) == NULL ) {
		fprintf(stderr,"Unable to open file %s\n", str );
		return;
		}
	if( !G_quiet ) fprintf(stderr," [%s", str);
	while( (t = getc(spfp)) != EOF ) {
		if ( t != '\004' )
		    outc(t);
		}              
	fclose(spfp);
	if( !G_quiet ) fprintf(stderr,"]");
}
