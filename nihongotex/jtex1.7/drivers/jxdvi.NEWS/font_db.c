/*
 *	Kanji Font Database for SONY NEWS/NWF-604
 *	$Header: font_db.c,v 1.2 88/02/08 01:04:32 atsuo Exp $
 *		written by Atsuo, K.
 *		on 27/12/87
 */

#include "atsuo-copyright.h"  

#include <stdio.h>
#include "font.h"

struct kanji_font_data kanji_font_db[] = {
/*    font name, height, width, byte wides	*/
        { "min7.500nwf",  10, 16, 2 },          /* M10   7pt (100dpi) */
        { "min8.500nwf",  11, 16, 2 },          /* M11   8pt (100dpi) */
        { "min9.500nwf",  12, 16, 2 },          /* M12   9pt (100dpi) */
        { "min10.500nwf", 14, 16, 2 },          /* M14  10pt (100dpi) */
        { "min10.600nwf", 17, 24, 3 },          /* M17  12pt (100dpi) */
	{ "min10.720nwf", 17, 24, 3 },		/* M17	12pt (wrong) */
	{ "min10.864nwf", 17, 24, 3 },		/* M17	12pt (wrong) */
        { "min12.500nwf", 17, 24, 3 },          /* M17  12pt (100dpi) */

	{ "min7.2000nwf", 39, 40, 5 },		/* M39	 7pt */
	{ "min8.2000nwf", 44, 48, 6 },		/* M44	 8pt */
	{ "min9.2000nwf", 50, 56, 7 },		/* M50	 9pt */

	{ "min10.2000nwf", 55, 56, 7 },		/* M55	10pt */
	{ "min10.2191nwf", 60, 60, 8 },		/* M60	11pt */ /* made by Nishioka */
	{ "min10.2400nwf", 66, 72, 9 },		/* M66	12pt */
	{ "min10.2880nwf", 77, 77, 10 },	/* M77	14t (wrong) */
	{ "min10.3456nwf", 88, 88, 11 },	/* M88	17pt (wrong) */
	{ "min10.4147nwf", 88, 88, 11 },	/* M88	17pt (wrong) */
	{ "min10.4977nwf", 88, 88, 11 },	/* M88	17pt (wrong) */

	{ "min11.2000nwf", 60, 60, 8 },		/* M60	11pt */ /* made by Nishioka */
	{ "min12.2000nwf", 66, 72, 9 },		/* M66	12pt */
	{ "min14.2000nwf", 77, 77, 10 },	/* M77	14pt */ /* made by Nishioka */
	{ "min16.2000nwf", 88, 88, 11 },	/* M88	16pt */ /* made by Nishioka */

        { "goth7.500nwf",  10, 16, 2 },         /* G10   7pt (100dpi) */
        { "goth8.500nwf",  11, 16, 2 },         /* G11   8pt (100dpi) */
        { "goth9.500nwf",  12, 16, 2 },         /* G12   9pt (100dpi) */
        { "goth10.500nwf", 14, 16, 2 },         /* G14  10pt (100dpi) */
        { "goth10.600nwf", 17, 24, 3 },         /* G17  12pt (100dpi) */
        { "goth10.720nwf", 17, 24, 3 },         /* G17  12pt (wrong) */
        { "goth10.864nwf", 17, 24, 3 },         /* G17  12pt (wrong) */
        { "goth12.500nwf", 17, 24, 3 },         /* G17  12pt (100dpi) */

	{ "goth7.2000nwf", 39, 40, 5 },		/* G39	 7pt */
	{ "goth8.2000nwf", 44, 48, 6 },		/* G44	 8pt */
	{ "goth9.2000nwf", 50, 56, 7 },		/* G50	 9pt */
	{ "goth10.2000nwf", 55, 56, 7 },	/* G55	10pt */
	{ "goth10.2400nwf", 66, 72, 9 },	/* G55	12pt */
	{ "goth10.2880nwf", 66, 72, 9 },	/* G66	12pt (wrong) */
	{ "goth10.3456nwf", 66, 72, 9 },	/* G55	12pt (wrong) */
	{ "goth12.2000nwf", 66, 72, 9 },	/* G66	12pt */
	{ "goth14.2000nwf", 88, 88, 11 },	/* G88	14pt */ /* Made by Nishioka */
	{ (char *)NULL, 0, 0, 0 }
};
