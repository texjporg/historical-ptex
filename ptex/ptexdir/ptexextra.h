/* ptexextra.h: banner etc. for pTeX.

   This is included by pTeX, from ptexextra.c
*/

string based_prog_name = "TeX";
const_string PTEXHELP[] = {
  "Usage: ptex [option] texfile",
  "     : ptex [option] &format texfile",
  "",
  "-fmt=NAME               use NAME instead of program name or %&format.",
  "-halt-on-error          stop processing at the first error",
  "[-no]-file-line-error   disable/enable file:line:error style messages",
  "-ini                    be iniptex.",
  "-interaction=STRING     set interaction mode (STRING=batchmode|nonstopmode|",
  "                          scrollmode|errorstopmode)",
#ifdef IPC
  "-ipc                    send DVI output to a socket as well as the usual",
  "                          output file",
  "-ipc-start              as -ipc, and also start the server at the other end",
#endif /* IPC */
  "-jobname=STRING         set the job name to STRING",
  "-kanji=STRING           set Japanese encoding (STRING=euc|jis|sjis)",
  "-kpathsea-debug=NUMBER  set path searching debugging flags according to",
  "                          the bits of NUMBER",
  "[-no]-mktex=FMT         disable/enable mktexFMT generation (FMT=tex/tfm)",
  "-mltex                  enable MLTeX extensions such as \\charsubdef",
  "-output-comment=STRING  use STRING for DVI file comment instead of date",
  "-output-directory=DIR   use DIR as the directory to write files to",
  "[-no]-parse-first-line  disable/enable parsing of the first line of the",
  "                          input file",
  "-progname=STRING        set program (and fmt) name to STRING",
  "-recorder               enable filename recorder",
  "[-no]-shell-escape      disable/enable \\write18{SHELL COMMAND}",
  "-src-specials           insert source specials into the DVI file",
  "-src-specials=WHERE     insert source specials in certain places of",
  "                          the DVI file. WHERE is a comma-separated value",
  "                          list: cr display hbox math par parend vbox",
  "-translate-file=TCXNAME use the TCX file TCXNAME",
  "-help                   print this message and exit.",
  "-version                print version information and exit.",
  NULL
};

#define BANNER "This is pTeX, Version 3.141592-p3.1.11"
#define COPYRIGHT_HOLDER "D.E. Knuth"
#define AUTHOR NULL
#define PROGRAM_HELP PTEXHELP
#define BUG_ADDRESS "ptex-staff@ml.asciimw.jp"
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
