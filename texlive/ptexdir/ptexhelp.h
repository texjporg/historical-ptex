/* ptexhelp.h: for the --help option"
*/
#ifndef PTEXHELP_H
#define PTEXHELP_H
#define HELP_H

#ifdef TeX
/*  ptexextra.h */
#undef TEXPOOLNAME
#define TEXPOOLNAME "ptex.pool"
#endif /* TeX */

#ifdef TFTOPL
string based_prog_name = "TFtoPL";
const_string PTEXTFTOPLHELP[] = {
"Usage: tftopl [option] tfmfile [plfile]",
"",
"  -verbose               output progress reports.",
"  -version               print version information and exit.",
"  -help                  print this message and exit.",
"  -kanji={jis|euc|sjis}  plfile kanji code.",
NULL };
#endif /* TFTOPL */

#ifdef PLTOTF
string based_prog_name = "PLtoTF";
const_string PTEXPLTOTFHELP[] = {
"Usage: pltotf [option] plfile [tfmfile]",
"",
"  -verbose               output progress reports.",
"  -version               print version information and exit.",
"  -help                  print this message and exit.",
"  -kanji={jis|euc|sjis}  plfile kanji code.",
NULL };
#endif /* PLTOTF */

#ifdef BIBTEX
string based_prog_name = "BibTeX";
const_string JBIBTEXHELP[] = {
"Usage: jbibtex [option] auxfile",
"",
"  -min-crossrefs=INTEGER     minimum number of cross-refs required",
"                             for automatic cite_list inclusion (default 2).",
"  -terse                     do silently.",
"  -version                   print version information and exit.",
"  -help                      print this message and exit.",
"  -kanji={jis|euc|sjis}      kanji code for output-file.",
NULL };
#endif /* BIBTEX */

#ifdef DVITYPE
string based_prog_name = "DVItype";
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
