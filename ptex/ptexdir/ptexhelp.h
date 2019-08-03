/* ptexhelp.h: for the --help option"
*/
#ifndef PTEXHELP_H
#define PTEXHELP_H
#define HELP_H

#ifdef TeX
/*  texmfmp-help.h */
#endif /* TeX */

#ifdef TFTOPL
const_string PTEXTFTOPLHELP[] = {
"Usage: tftopl [option] tfmfile [plfile]",
"",
"  -verbose             output progress reports.",
"  -version             print version information and exit.",
"  -help                print this message and exit.",
NULL };
#endif /* TFTOPL */

#ifdef PLTOTF
const_string PTEXPLTOTFHELP[] = {
"Usage: pltotf [option] plfile [tfmfile]",
"",
"  -verbose             output progress reports.",
"  -version             print version information and exit.",
"  -help                print this message and exit.",
NULL };
#endif /* PLTOTF */

#ifdef BIBTEX
const_string JBIBTEXHELP[] = {
"Usage: jbibtex [option] auxfile",
"",
"  -min-crossrefs=INTEGER     minimum number of cross-refs required",
"                             for automatic cite_list inclusion (default 2).",
"  -terse                     do silently.",
"  -version                   print version information and exit.",
"  -help                      print this message and exit.",
NULL };
#endif /* BIBTEX */

#ifdef DVITYPE
const_string PDVITYPEHELP[] = {
"Usage: pdvitype [option] dvifile",
"",
"  -dpi=REAL                  assumed device resolution in pixels per inch",
"                             (default 300.0).",
"  -magnification=INTEGER     overrride the postamble's magnification.",
"  -max-pages=INTEGER         maximum number of pages (default 1000000).",
"  -output-level=1..4         verbosity level.",
"                                 1: terse, 2: medium-level, 3: verbose,",
"                                 4: plus check for postamble(default).",
"  -page-start=PAGE           type, e.g., `1.*.-5' to specify the first page",
"                             with \\count0=1, \\count2=4.",
"                             (see the TeX book chapter 15).",
"  -show-opcodes              show opcodes in dicimal.",
"  -version                   print version information and exit.",
"  -help                      print this message and exit.",
NULL };
#endif /* DVITYPE */
#endif /* PTEXHELP_H */
