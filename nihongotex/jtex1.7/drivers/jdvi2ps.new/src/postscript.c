/*-->DoPostScriptBox*/
/**********************************************************************/
/*************************  DoPostScriptBox  **************************/
/**********************************************************************/

#include	"../fonts/public.h"
#include	"../fonts/common.h"
#include	"jdvi2kps.h"

#define		Min(a, b)	((a) > (b) ? (b) : (a))
#define		Abs(n)		(((n) >= 0) ? (n) : -(n))

int	framemode	= False;	/* Framemarker compatibility mode.  */
int	skip_postscript	= False;	/* input PostScript data flad */

/*
 * DoPostScriptBox:
 *  This is where the postscriptbox is handled.
 *  Note that the width and height here are in real points (1/72.27 inch)
 *  as generated from TeX, as opposed to the silly 1/72 inch PS points.
 */

void
DoPostScriptBox(str)
char *str;
{
	register FILE *spfp;
	float width;
	float height;
	float x1, y1, x2, y2;
	float bbx, bby, bbw, bbh;
	float ho, vo, hsc, vsc;
	int foundsize;
	char name[STRSIZE];
	char dummy[10];
	char line[STRSIZE];
	char fullname[STRSIZE];

	/*
	 * Read args and open file.
	 */
	if(sscanf(str, "postscriptbox { %fpt }{ %fpt }{ %[^ {}] }",
	  &width, &height, name) != 3)
	{
		Warning("badly formed postscriptbox command `%s'.", str);
		return;
	}

	/*
	 * Open the PS file.
	 */
	if((spfp = fopenp(Option.ps_path, name, fullname, "r")) == NULL)
	{
		strcat(name, ".ps");
		if((spfp = fopenp(Option.ps_path, name, fullname, "r")) == NULL)
		{
			Warning("unable to open %s for postscriptbox cmd.",
			  name);
			return;
		}
	}

	/*
	 * Search for BoundingBox or Frame, and rewind.
	 */
	foundsize = False;
	while(fgets(line, STRSIZE, spfp))
	{
		if(sscanf(line, " %%%% BoundingBox : %f %f %f %f ",
		  &x1, &y1, &x2, &y2) == 4)
		{
			bbx = Min(x1, x2);
			bby = Min(y1, y2);
			bbw = Abs(x2 - x1);
			bbh = Abs(y2 - y1);
			foundsize = True;
			break;
		}
		if(framemode &&
		  sscanf(line, " %f %f %f %f %[C] ",
		    &x1, &y1, &x2, &y2, &dummy[0]) == 5)
		{
			bbx = Min(x1, x2);
			bby = Min(y1, y2);
			bbw = Abs(x2 - x1);
			bbh = Abs(y2 - y1);
			foundsize = True;
			break;
		}
	}
	fseek(spfp, 0L, 0);
	if(! foundsize)
	{
		Warning("no BoundingBox found: assuming unit square.");
		bbx = 0.0;
		bby = 0.0;
		bbw = 1.0;
		bbh = 1.0;
	}

	/*
	 * Emit preamble.
	 *  The PS box thinks it will print in bbw PSpoints, or bbw/72 inches.
	 *  TeX wants it to fit into width real points, or width/72.27 inches.
	 *  The current scale has been reset to 72 dots per inch.
	 * Scale = (width/72.27) / (bbw/72).
	 */
	hsc = (width/72.27) * (72.0/bbw);
	vsc = (height/72.27) * (72.0/bbh);
	ho = -bbx * hsc;
	vo = -bby * vsc;
	if(! skip_postscript)
	{
		fprintf(stderr, " (%s", name); fflush(stderr);
		fprintf(outfp, "%% \\postscriptbox{%f}{%f}{%s}\n",
		  width, height, name);
		fprintf(outfp, "%f @hoffset %f @voffset\n", ho, vo);
		fprintf(outfp, "%f @hscale %f @vscale\n", hsc, vsc);

		/*
		 * Copy file.
		 */
		fprintf(outfp, "@setspecial\n");
		copy_file(spfp, fullname);
		fprintf (stderr, ")"); fflush(stderr);
	}
	else
		fprintf(stderr, " (%s skipped)", name); fflush(stderr);

	fclose (spfp);
}
