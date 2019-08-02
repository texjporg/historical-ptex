/*
 *	config.h
 *
 *	This file contains information needed to configure
 *	    jxdvi/dvi2nwp.
 *	Read through this file and edit definition appropriately.
 *
 *	$Header: config.h,v 1.1 89/07/31 12:31:45 kono Locked $
 */  

/*
 * NWF Kanji font to be used. (See also README.jis)
 *    Define one of the following:
 *      NWF_604
 *      NWF_605
 *    If you have neither of them, leave none of them defined.
 */
/* #define NWF_604 */

#define NWF_605

/*
 * NWF Outline Kanji font (See README.jis)
 *    If you want to use NWF-605 Outline font, 
 *    define V_NWF and give some names.
 */
#define V_NWF
#define V_NWF_FONT_DIR	"/usr/sony/lib/font/devnwp533"
#define M_V_FONT "Mincho.vfont"
#define G_V_FONT "Gothic.vfont"

/*
 * DN Kanji font to be used. ( see also README.jis )
 * Define DNF if you use the DaiNippon Kanji font,
 * V_NWF also can be defined simultaneously, if you want to use 
 * the V_NWF font for some sizes of DNF fonts that not found.
 */

#define DNF
#define	DNF_FONT_DIR	"/usr/lib/tex/fonts"

/*
 * file directories
 */
#define PXL_FONT_DIR	"/usr/lib/tex/fonts"
#define PK_FONT_DIR	"/usr/lib/tex/fonts"
#define JFM_DIR		"/usr/lib/tex/fonts"
#define NWF_FONT_DIR	"/usr/sony/lib/font/devnwp533"
#define NWFADJ_DIR	"/usr/lib/tex/fonts"


/*
 * Default DPI value: (effective only for jxdvi)
 *	118, 300, 400, etc.
 */
#define DEFAULT_DPI	400

/*
 * Default Shrink Factor: (effective only for jxdvi)
 */
#define DEFAULT_SHRINK	5

/*
 * Default X and Y offset on paper: (effective only for dvi2nwp)
 */
#define DEFAULT_X_OFFSET (113)
#define DEFAULT_Y_OFFSET (60)

/*
 * Accounting:
 *   If accounting information is needed, define
 *      ACCT
 */
/* #define ACCT */

