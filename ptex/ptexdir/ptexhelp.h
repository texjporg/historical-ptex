/* ptexhelp.h: for the --help option"
*/
#ifndef PTEXHELP_H
#define PTEXHELP_H
#define HELP_H

#ifdef TeX
#define PTEXHELP "\
Usage: ptex [option] texfile\n\
       ptex [option] &format texfile\n\n\
  -fmt=NAME            use NAME instead of program name or %&format.\n\
  -progname=NAME       pretend to be program NAME.\n\
                       (this affects both format and search path)\n\
  -ini                 be iniptex.\n\
  -interaction={batchmode/nonstopmode/scrollmode/errorstopmode}\n\
  -shell-escape        enable the \\write18{shell command} construct.\n\
  -verbose             output progress reports.\n\
  -version             print version information and exit.\n\
  -help                print this message and exit.\n"
#endif /* TeX */

#ifdef TFTOPL
#define TFTOPLHELP "\
Usage: tftopl [option] tfmfile [plfile]\n\n\
  -verbose             output progress reports.\n\
  -version             print version information and exit.\n\
  -help                print this message and exit.\n"
#endif /* TFTOPL */

#ifdef PLTOTF
#define PLTOTFHELP "\
Usage: pltotf [option] plfile [tfmfile]\n\n\
  -verbose             output progress reports.\n\
  -version             print version information and exit.\n\
  -help                print this message and exit.\n"
#endif /* PLTOTF */

#ifdef BIBTEX
#define BIBTEXHELP "\
Usage: jbibtex [option] auxfile\n\n\
  -min-crossrefs=INTEGER     minimum number of cross-refs required\n\
                             for automatic cite_list inclusion (default 2).\n\
  -terse                     do silently.\n\
  -version                   print version information and exit.\n\
  -help                      print this message and exit.\n"
#endif /* BIBTEX */

#ifdef DVITYPE
#define DVITYPEHELP "\
Usage: pdvitype [option] dvifile\n\n\
  -dpi=REAL                  assumed device resolution in pixels per inch\n\
                             (default 300.0).\n\
  -magnification=INTEGER     overrride the postamble's magnification.\n\
  -max-pages=INTEGER         maximum number of pages (default 1000000).\n\
  -output-level=1..4         verbosity level.\n\
                                 1: terse, 2: medium-level, 3: verbose,\n\
                                 4: plus check for postamble(default).\n\
  -page-start=PAGE           type, e.g., `1.*.-5' to specify the first page\\n
                             with \\count0=1, \\count2=4.\n\
                             (see the TeX book chapter 15).\n\
  -show-opcodes              show opcodes in dicimal.\n\
  -version                   print version information and exit.\n\
  -help                      print this message and exit.\n"
#endif /* DVITYPE */
#endif /* PTEXHELP_H */
