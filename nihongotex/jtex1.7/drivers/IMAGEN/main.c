/*
 * Copyright 1988 dit Co., Ltd.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of dit Co., Ltd. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * Dit Co., Ltd. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * DIT CO., LTD. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL DIT CO., LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/*
 * main.c - TeX82's DVI2 code converter, main routine.
 */

/*
 * $Id: main.c,v 1.22 90/03/27 11:02:50 void Exp $
 */

#include <stdio.h>
#include "config.h"
#include "page.h"
#include "fio.h"

#define DVI_SUFFIX  ".dvi"

visible	FILE	*out;		    /* output file descripter */
visible	FILE	*dvi;		    /* input dvi file descripter */
visible	char	*use_font;	    /* font format flavor */
visible	char	*use_pdl;	    /* page description language */
visible	int	use_landscapemode;  /* landscape mode/ portrait mode */
visible	int	use_resident;	    /* use resident fonts */
visible	int	use_resolution;	    /* printer's resolution, unit `dpi' */
visible int	use_top_margin;	    /* top margin offset */
visible int	use_left_margin;    /* left margin offset */
visible double	use_magnification;  /* global magnification */

visible	int	use_silent;	/* silent flag */
visible int	use_fsck;	/* check existance of font file in postamble */

hidden	char *usage[] = {
    "Usage: dvi2 [-options]",
    " dvi-file",
     NULL
};

hidden	char *help[] = {
    "-s:             Suppress processing page number\n",
    "-l:             Landscape Mode on\n",
    "-c:             Check only if font exists or not\n",
    "-z:             Use printer resident font\n",
    "-x:             Exchange fonts if not exist\n",
    "-o output:      Set name of output file\n",
    "-F fontformat:  Set font file format\n",
    "-L pdl:         Set name of page description language\n",
    "-R resolution:  Set target resolution\n",
    "-m[0h12345]:    Magnificate the entire page\n",
    "-p [pages,...]: Process the specified pages only\n",
    "-r:             Page reversal on\n",
    "-Po:            Process the odd page numbers only\n",
    "-Pe:            Process the even page numbers only\n",
    "-Ho:            Process the physical odd pages only\n",
    "-He:            Process the physical even pages only\n",
#if 0
    "-Ot unit:       Offset the top margin to ??  [cm/pt/in/(dpi)]\n",
    "-Ol unit:       Offset the left margin to ?? [cm/pt/in/(dpi)]\n",
#else
    "-Ot unit:       Offset the top margin to ??  [unit = dpi]\n",
    "-Ol unit:       Offset the left margin to ?? [unit = dpi]\n",
#endif
     NULL
};

extern char	version[];

extern PAGE_LINK    *FirstPage;	/* Structure Pointer to First BOP */
extern PAGE_LINK    *LastPage;	/* Structure Pointer to Last BOP */

extern char	*strcpy(), *strcat(); 

main(argc, argv)
int	argc;
char	**argv;
{
    char            dvifile[128];
    char            output[128];
    char            pagestring[128];

    register int    c;
    Sig4Byte        postptr;
    PAGE_LINK      *chain;
    int		    pageopt;

    Sig4Byte        GetPostPtr();
    Sig4Byte        GetPostamble();
    int             CheckSuffix();

    /*
     * force stderr to non buffering mode 
     */
    (void) setbuf(stderr, NULL);

    /*
     * check the arguments. 
     */
    if (argc == 1) {
	PutMsg(usage);
	exit(1);
    }

    /*
     * setup default value. 
     */
    use_silent = NO;
    use_fsck = 0;
    use_font = DEFAULTFONTS;
    use_pdl = DEFAULTPDL;
    use_landscapemode  = DEFAULTPRINTMODE;
    use_resident = USINGRESIDENTFONT;
    use_resolution = DEFAULTRESOLUTION;
    use_magnification = 1.0;
    use_left_margin = use_top_margin = use_resolution;
    output[0] = EOS;
    pagestring[0] = EOS;
    pageopt = STRAIGHT; /* straightforward */

    /*
     * check the options. 
     */
    while (--argc > 0 && **(++argv) == '-') {
	c = *++*argv;
	switch (c) {
	case 'c':
	    use_fsck = 1;
	    break;
	case 'x':
	    use_fsck = 2;
	    break;
	case 'l':
	    use_landscapemode = YES;
	    break;
	case 's':
	    use_silent = YES;
	    break;
	case 'z':
	    use_resident = YES;
	    break;
	case 'r':
	    pageopt = REVERSE;
	    use_fsck = 2;
	    break;
	case 'o':
	    if (--argc <= 0)
		error("No output file specified");	/* exit */
	    (void) strcpy(output, *(++argv));
	    break;
	case 'O':
	    switch (*++*argv) {
	    case 't':
		if (--argc <= 0)
		    error("No magrin size specified");	/* exit */
		use_top_margin = atoi(*(++argv));
		break;
	    case 'l':
		if (--argc <= 0)
		    error("No magrin size specified");	/* exit */
		use_left_margin = atoi(*(++argv));
		break;
	    default:
		error("Must be 't', 'l' after 'O'");	/* exit */
	    }
	    break;
	case 'R':
	    if (--argc <= 0)
		error("No resolution specified");	/* exit */
	    else
		use_resolution = atoi(*(++argv));		
	    break;
	case 'L':
	    if (--argc <= 0)
		error("No pdl specified");	/* exit */
	    else
		use_pdl = *(++argv);
	    break;
	case 'F':
	    if (--argc <= 0)
		error("No fontformat specified");	/* exit */
	    else
		use_font = *(++argv);
	    break;
	case 'h':
	    (void) fprintf(stderr, "%s\n\n", version);
	    PutMsg(help);
	    exit(1);
	    break;
	case 'p':
	    if (--argc <= 0)
		error("Please specify ranges");	/* exit */
	    else {
		(void) strcpy(pagestring, *(++argv));
	    	use_fsck = 2;
	    }
	    break;
	case 'P':
	    switch (*++*argv) {
	    case 'o':
		pageopt = ODD;
		use_fsck = 2;
		break;
	    case 'e':
		pageopt = EVEN;
		use_fsck = 2;
		break;
	    default:
		error("Must be [Po, Pe]");	/* exit */
	    }
	    break;
	case 'H':
	    switch (*++*argv) {
	    case 'o':
		pageopt = ODD;
		use_fsck = 2;
		break;
	    case 'e':
		pageopt = EVEN;
		use_fsck = 2;
		break;
	    default:
		error("Must be [Ho, He]");	/* exit */
	    }
	    break;
	case 'm':
	    switch (*++*argv) {
	    case '0':
		use_magnification = 1.0;
		break;
	    case 'h':
		use_magnification = 1.0954451;
		break;
	    case '1':
		use_magnification = 1.2;
		break;
	    case '2':
		use_magnification = 1.44;
		break;
	    case '3':
		use_magnification = 1.728;
		break;
	    case '4':
		use_magnification = 2.0736;
		break;
	    case '5':
		use_magnification = 2.48832;
		break;
	    default:
		error("Unkown magnification size [0h12345]");	/* exit */
		break;
	    }
	    break;
	default:
	    PutMsg(usage);
	    exit(1);
	}
    }

    /*
     * set suffix & open the specified correct files.
     */
    if (argc != 1) {
	PutMsg(usage);
	exit(1);
    }

    (void) strcpy(dvifile, *argv);

    if (CheckSuffix(dvifile, DVI_SUFFIX) == NO)
	(void) strcat(dvifile, DVI_SUFFIX);

    if ((dvi = fopen(dvifile, FREADMODE)) == NULL)
	cant_open(dvifile);	/* exit */

    if (*output == EOS) {
	out = stdout;
	(void) strcpy(output, "stdout");
    } else if ((out = fopen(output, FWRITEMODE)) == NULL)
	cant_open(output);	/* exit */

    /*
     * make table for PDL & set up for font-routines.
     */
    InitPDLTable(use_pdl);
    InitFontRoutine(use_font);

    /*
     * check preamble definition before the postamble.
     */
    GetPreamble();

    /*
     * go to the postamble part.
     */
    if ((postptr = GetPostPtr()) == NULL)
	error("This file doesn't contain the postamble");	/* exit */
    (void) fseek(dvi, (long) postptr, 0);

    /*
     * check the postamble & set up page linkage information.
     */
    SetPageLink(pageopt, GetPostamble(), pagestring);
    InitStack();
    StartJob(output);

    /*
     * process page.
     */
    if (pageopt == REVERSE) {
	for (chain = LastPage;  chain->Prev != NULL; chain = chain->Prev)
	    ScanOnePage(chain->Bop);
    }
    else {
	for (chain = FirstPage; chain->Next != NULL; chain = chain->Next)
	    ScanOnePage(chain->Bop);
    }

    LastJob();
    (void) fclose(dvi);
    (void) fclose(out);
}
