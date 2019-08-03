/* ptexextra.h: banner etc. for pTeX.

   This is included by pTeX, from ptexextra.c
*/

string based_prog_name = "TeX";
const_string PTEXHELP[] = {
"Usage: ptex [option] texfile",
"       ptex [option] &format texfile",
"",
"  -fmt=NAME            use NAME instead of program name or %&format.",
"  -progname=NAME       pretend to be program NAME.",
"                       (this affects both format and search path)",
"  -ini                 be iniptex.",
"  -interaction={batchmode/nonstopmode/scrollmode/errorstopmode}",
"  -shell-escape        enable the \\write18{shell command} construct.",
"  -src-specials        insert source specials into the DVI file.",
"  -kanji={euc|sjis|jis}",
"",
"  -version             print version information and exit.",
"  -help                print this message and exit.",
NULL };

#define BANNER "This is pTeX, Version 3.14159-p3.1.4"
#define COPYRIGHT_HOLDER "D.E. Knuth"
#define AUTHOR NULL
#define PROGRAM_HELP PTEXHELP
#define DUMP_VAR TEXformatdefault
#define DUMP_LENGTH_VAR formatdefaultlength
#define DUMP_OPTION "fmt"
#define DUMP_EXT ".fmt"
#define INPUT_FORMAT kpse_tex_format
#define INI_PROGRAM "iniptex"
#define VIR_PROGRAM "virptex"

#ifdef Xchr
#undef Xchr
#define Xchr(x) (x)
#endif /* Xchr */

