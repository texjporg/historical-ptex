/* texmf.c: Hand-coded routines for TeX or Metafont in C.  Originally
   written by Tim Morgan, drawing from other Unix ports of TeX.  This is
   a collection of miscellany, everything that's easier (or only
   possible) to do in C.
   
   This file is public domain.  */

#define	EXTERN /* Instantiate data from {tex,mf,mp}d.h here.  */

/* This file is used to create texextra.c etc., with this line
   changed to include texd.h, mfd.h, or mpd.h.  The ?d.h file is what
   #defines TeX or MF or MP, which avoids the need for a special
   Makefile rule.  */
#include "texd.h"

#include <kpathsea/c-ctype.h>
#include <kpathsea/line.h>
#include <kpathsea/readable.h>
#include <kpathsea/variable.h>
#include <kpathsea/absolute.h>

#include <time.h> /* For `struct tm'.  */
#if defined (HAVE_SYS_TIME_H)
#include <sys/time.h>
#elif defined (HAVE_SYS_TIMEB_H)
#include <sys/timeb.h>
#endif

#if defined(__STDC__)
#include <locale.h>
#endif

#include <signal.h> /* Catch interrupts.  */

#include <texmfmp-help.h>

/* {tex,mf}d.h defines TeX, MF, INI, and other such symbols.
   Unfortunately there's no way to get the banner into this code, so
   just repeat the text.  */
#ifdef TeX
#if defined (KANJI)
#include "ptexextra.h"
#elif defined (eTeX)
#include <etexdir/etexextra.h>
#elif defined (pdfTeX)
#include <pdftexdir/pdftexextra.h>
#elif defined (pdfeTeX)
#include <pdfetexdir/pdfetexextra.h>
#elif defined (Omega)
#include <omegadir/omegaextra.h>
#else
#define BANNER "This is TeX, Version 3.141592"
#define COPYRIGHT_HOLDER "D.E. Knuth"
#define AUTHOR NULL
#define PROGRAM_HELP TEXHELP
#define DUMP_VAR TEXformatdefault
#define DUMP_LENGTH_VAR formatdefaultlength
#define DUMP_OPTION "fmt"
#define DUMP_EXT ".fmt"
#define INPUT_FORMAT kpse_tex_format
#define INI_PROGRAM "initex"
#define VIR_PROGRAM "virtex"
#endif
#define edit_var "TEXEDIT"
#endif /* TeX */
#ifdef MF
#define BANNER "This is Metafont, Version 2.71828"
#define COPYRIGHT_HOLDER "D.E. Knuth"
#define AUTHOR NULL
#define PROGRAM_HELP MFHELP
#define DUMP_VAR MFbasedefault
#define DUMP_LENGTH_VAR basedefaultlength
#define DUMP_OPTION "base"
#ifdef DOS
#define DUMP_EXT ".bas"
#else
#define DUMP_EXT ".base"
#endif
#define INPUT_FORMAT kpse_mf_format
#define INI_PROGRAM "inimf"
#define VIR_PROGRAM "virmf"
#define edit_var "MFEDIT"
#endif /* MF */
#ifdef MP
#define BANNER "This is MetaPost, Version 0.641"
#define COPYRIGHT_HOLDER "AT&T Bell Laboratories"
#define AUTHOR "John Hobby"
#define PROGRAM_HELP MPHELP
#define DUMP_VAR MPmemdefault
#define DUMP_LENGTH_VAR memdefaultlength
#define DUMP_OPTION "mem"
#define DUMP_EXT ".mem"
#define INPUT_FORMAT kpse_mp_format
#define INI_PROGRAM "inimpost"
#define VIR_PROGRAM "virmpost"
#define edit_var "MPEDIT"
#endif /* MP */

/* The main program, etc.  */

/* What we were invoked as and with.  */
char **argv;
int argc;

/* If the user overrides argv[0] with -progname.  */
static string user_progname;

/* The C version of what might wind up in DUMP_VAR.  */
static const_string dump_name;

/* The C version of the jobname, if given. */
static const_string job_name;

/* Full source file name. */
extern string fullnameoffile;

/* The filename for dynamic character translation, or NULL.  */
string translate_filename;
string default_translate_filename;

/* Needed for --src-specials option. */
static char *last_source_name;
static int last_lineno;
static boolean srcspecialsoption = false;
static void parse_src_specials_option P1H(const_string);

/* The main body of the WEB is transformed into this procedure.  */
extern TEXDLL void mainbody P1H(void);

/* Parsing a first %&-line in the input file. */
static void parse_first_line P1H(const_string);

/* Parse option flags. */
static void parse_options P2H(int, string *);

/* Try to figure out if we have been given a filename. */
static string get_input_file_name P1H(void);

#ifdef MP
/* name of TeX program to pass to makempx */
static string mpost_tex_program = "";
#endif

#ifdef __STDC__
#ifdef WIN32
extern boolean bOem;
string locale_name = ".ACP";
#else
string locale_name = "";
#endif
#endif

/* The entry point: set up for reading the command line, which will
   happen in `topenin', then call the main body.  */

void TEXDLL
maininit P2C(int, ac, string *, av)
{
  string main_input_file;

  /* Save to pass along to topenin.  */
  argc = ac;
  argv = av;

  /* Must be initialized before options are parsed.  */
  interactionoption = 4;
#ifdef KANJI
#ifdef OUTJIS
prockanjicode = JIS;
#endif /* OUTJIS */
#ifdef OUTEUC
prockanjicode = EUC;
#endif /* OUTEUC */
#ifdef OUTSJIS
prockanjicode = SJIS;
#endif /* OUTSJIS */
#endif /* KANJI */

  /* If the user says --help or --version, we need to notice early.  And
     since we want the --ini option, have to do it before getting into
     the web (which would read the base file, etc.).  */
  parse_options (ac, av);
  
#if defined(__STDC__)
  /* Need to delay it because of win32 `-oem' option.  Default value ""
     means: get value from env. var LC_ALL, LC_CTYPE, or LANG */
  setlocale(LC_CTYPE, locale_name);
#endif

  /* Do this early so we can inspect program_invocation_name and
     kpse_program_name below, and because we have to do this before
     any path searching.  */
  kpse_set_program_name (argv[0], user_progname);

  /* FIXME: gather engine names in a single spot. */
  xputenv("engine", TEXMFENGINENAME);
  
  /* Were we given a simple filename? */
  main_input_file = get_input_file_name();

  /* Second chance to activate file:line:error style messages, this
     time from texmf.cnf. */
  if (filelineerrorstylep < 0) {
    filelineerrorstylep = 0;
  } else if (!filelineerrorstylep) {
    string file_line_error_style = kpse_var_value ("file_line_error_style");
    filelineerrorstylep = (file_line_error_style
                           && (*file_line_error_style == 't'
                               || *file_line_error_style == 'y'
                               || *file_line_error_style == '1'));
  }

  /* If no dump default yet, and we're not doing anything special on
     this run, we may want to look at the first line of the main input
     file for a %&<dumpname> specifier.  */
  if (parsefirstlinep < 0) {
    parsefirstlinep = 0;
  } else if (!parsefirstlinep) {
    string parse_first_line = kpse_var_value ("parse_first_line");
    parsefirstlinep = (parse_first_line
                       && (*parse_first_line == 't'
                           || *parse_first_line == 'y'
                           || *parse_first_line == '1'));
  }
  if (parsefirstlinep && (!dump_name || !translate_filename)) {
    parse_first_line (main_input_file);
  }
  /* Check whether there still is no translate_filename known.  If so,
     use the default_translate_filename. */
  /* FIXME: deprecated. */
  if (!translate_filename) {
    translate_filename = default_translate_filename;
  }
  /* If we're preloaded, I guess everything is set up.  I don't really
     know any more, it's been so long since anyone preloaded.  */
  if (readyalready != 314159) {
    /* The `ini_version' variable is declared/used in the change files.  */
    boolean virversion = false;
    if (FILESTRCASEEQ (kpse_program_name, INI_PROGRAM)) {
      iniversion = true;
    } else if (FILESTRCASEEQ (kpse_program_name, VIR_PROGRAM)) {
      virversion = true;
#ifdef TeX
#ifndef Omega
    } else if (FILESTRCASEEQ (kpse_program_name, "mltex")) {
      mltexp = true;
#endif /* !Omega */
#ifdef eTeX  /* For e-TeX compatibility mode... */
#ifdef KANJI
    } else if (FILESTRCASEEQ (kpse_program_name, "iniptex")) {
      iniversion = true;
    } else if (FILESTRCASEEQ (kpse_program_name, "virptex")) {
      virversion = true;
#else /* KANJI */
    } else if (FILESTRCASEEQ (kpse_program_name, "initex")) {
      iniversion = true;
    } else if (FILESTRCASEEQ (kpse_program_name, "virtex")) {
      virversion = true;
#endif /* KANJI */
#endif /* eTeX */
#endif /* TeX */
    }

    if (!dump_name) {
      /* If called as *vir{mf,tex,mpost} use `plain'.  Otherwise, use the
         name we were invoked under.  */
      dump_name = (virversion ? "plain" : kpse_program_name);
    }
  }

#ifdef TeX
#if !defined(Omega) && !defined(eOmega) && !defined(Aleph)
  /* Sanity check: -mltex and -enc only work in combination with -ini. */
  if (!iniversion) {
    if (mltexp) {
      fprintf(stderr, "-mltex only works with -ini\n");
    }
  }
#endif
#endif
  
  /* If we've set up the fmt/base default in any of the various ways
     above, also set its length.  */
  if (dump_name) {
    /* adjust array for Pascal and provide extension */
    DUMP_VAR = concat3 (" ", dump_name, DUMP_EXT);
    DUMP_LENGTH_VAR = strlen (DUMP_VAR + 1);
  } else {
    /* For dump_name to be NULL is a bug.  */
    abort();
  }

  /* Additional initializations.  No particular reason for doing them
     here instead of first thing in the change file; less symbols to
     propagate through Webc, that's all.  */
#ifdef MF
  kpse_set_program_enabled (kpse_mf_format, MAKE_TEX_MF_BY_DEFAULT,
                            kpse_src_compile);
  kpse_set_program_enabled (kpse_base_format, MAKE_TEX_FMT_BY_DEFAULT,
                            kpse_src_compile);
#endif /* MF */
#ifdef MP
  kpse_set_program_enabled (kpse_mem_format, MAKE_TEX_FMT_BY_DEFAULT,
                            kpse_src_compile);
#endif /* MP */
#ifdef TeX
#ifdef Omega
  kpse_set_program_enabled (kpse_ocp_format, MAKE_OMEGA_OCP_BY_DEFAULT,
                            kpse_src_compile);
  kpse_set_program_enabled (kpse_ofm_format, MAKE_OMEGA_OFM_BY_DEFAULT,
                            kpse_src_compile);
  kpse_set_program_enabled (kpse_tfm_format, false, kpse_src_compile);
#else
  kpse_set_program_enabled (kpse_tfm_format, MAKE_TEX_TFM_BY_DEFAULT,
                            kpse_src_compile);
#endif /* !Omega */
  kpse_set_program_enabled (kpse_tex_format, MAKE_TEX_TEX_BY_DEFAULT,
                            kpse_src_compile);
  kpse_set_program_enabled (kpse_fmt_format, MAKE_TEX_FMT_BY_DEFAULT,
                            kpse_src_compile);

  if (shellenabledp < 0) {
    shellenabledp = 0;
  } else if (!shellenabledp) {
    string shell_escape = kpse_var_value ("shell_escape");
    shellenabledp = (shell_escape
                     && (*shell_escape == 't'
                         || *shell_escape == 'y'
                         || *shell_escape == '1'));
  }
  if (!outputcomment) {
    outputcomment = kpse_var_value ("output_comment");
  }
#endif /* TeX */
}

#ifndef WIN32
/* The entry point: set up for reading the command line, which will
   happen in `topenin', then call the main body.  */

int
main P2C(int, ac,  string *, av)
{
#ifdef __EMX__
  _wildcard (&ac, &av);
  _response (&ac, &av);
#endif

  maininit(ac, av);

  /* Call the real main program.  */
  mainbody ();
  return EXIT_SUCCESS;
} 
#endif /* ! WIN32 */

/* This is supposed to ``open the terminal for input'', but what we
   really do is copy command line arguments into TeX's or Metafont's
   buffer, so they can handle them.  If nothing is available, or we've
   been called already (and hence, argc==0), we return with
   `last=first'.  */

void
topenin P1H(void)
{
  int i;

  buffer[first] = 0; /* In case there are no arguments.  */

  if (optind < argc) { /* We have command line arguments.  */
    int k = first;
    for (i = optind; i < argc; i++) {
      char *ptr = &(argv[i][0]);
      /* Don't use strcat, since in Omega the buffer elements aren't
         single bytes.  */
      while (*ptr) {
        buffer[k++] = *(ptr++);
      }
      buffer[k++] = ' ';
    }
    argc = 0;	/* Don't do this again.  */
    buffer[k] = 0;
  }

  /* Find the end of the buffer.  */
  for (last = first; buffer[last]; ++last)
    ;

  /* Make `last' be one past the last non-blank character in `buffer'.  */
  /* ??? The test for '\r' should not be necessary.  */
  for (--last; last >= first
       && ISBLANK (buffer[last]) && buffer[last] != '\r'; --last) 
    ;
  last++;

  /* One more time, this time converting to TeX's internal character
     representation.  */
#ifndef Omega
  for (i = first; i < last; i++)
    buffer[i] = xord[buffer[i]];
#endif
}

/* IPC for TeX.  By Tom Rokicki for the NeXT; it makes TeX ship out the
   DVI file in a pipe to TeXView so that the output can be displayed
   incrementally.  Shamim Mohamed adapted it for Web2c.  */
#if defined (TeX) && defined (IPC)

#include <sys/socket.h>
#include <fcntl.h>
#ifndef O_NONBLOCK /* POSIX */
#ifdef O_NDELAY    /* BSD */
#define O_NONBLOCK O_NDELAY
#else
#ifdef FNDELAY     /* NeXT */
#define O_NONBLOCK O_FNDELAY
#else
what the fcntl? cannot implement IPC without equivalent for O_NONBLOCK.
#endif /* no FNDELAY */
#endif /* no O_NDELAY */
#endif /* no O_NONBLOCK */

#ifndef IPC_PIPE_NAME /* $HOME is prepended to this.  */
#define IPC_PIPE_NAME "/.TeXview_Pipe"
#endif
#ifndef IPC_SERVER_CMD /* Command to run to start the server.  */
#define IPC_SERVER_CMD "open `which TeXview`"
#endif

struct msg
{
  short namelength; /* length of auxiliary data */
  int eof;   /* new eof for dvi file */
#if 0  /* see usage of struct msg below */
  char more_data[0]; /* where the rest of the stuff goes */ 
#endif
};

static char *ipc_name;
static struct sockaddr *ipc_addr;
static int ipc_addr_len;

static int
ipc_make_name P1H(void)
{
  if (ipc_addr_len == 0) {
    string s = getenv ("HOME");
    if (s) {
      ipc_addr = (struct sockaddr*)xmalloc (strlen (s) + 40);
      ipc_addr->sa_family = 0;
      ipc_name = ipc_addr->sa_data;
      strcpy (ipc_name, s);
      strcat (ipc_name, IPC_PIPE_NAME);
      ipc_addr_len = strlen (ipc_name) + 3;
    }
  }
  return ipc_addr_len;
}


static int sock = -1;

static int
ipc_is_open P1H(void)
{
   return sock >= 0;
}


static void
ipc_open_out P1H(void) {
#ifdef IPC_DEBUG
  fputs ("tex: Opening socket for IPC output ...\n", stderr);
#endif
  if (sock >= 0) {
    return;
  }

  if (ipc_make_name () < 0) {
    sock = -1;
    return;
  }

  sock = socket (PF_UNIX, SOCK_STREAM, 0);
  if (sock >= 0) {
    if (connect (sock, ipc_addr, ipc_addr_len) != 0
        || fcntl (sock, F_SETFL, O_NONBLOCK) < 0) {
      close (sock);
      sock = -1;
      return;
    }
#ifdef IPC_DEBUG
    fputs ("tex: Successfully opened IPC socket.\n", stderr);
#endif
  }
}


static void
ipc_close_out P1H(void)
{
#ifdef IPC_DEBUG
  fputs ("tex: Closing output socket ...\n", stderr);
#endif
  if (ipc_is_open ()) {
    close (sock);
    sock = -1;
  }
}


static void
ipc_snd P3C(int, n,  int, is_eof,  char *, data)
{
  struct
  {
    struct msg msg;
    char more_data[1024];
  } ourmsg;

#ifdef IPC_DEBUG
  fputs ("tex: Sending message to socket ...\n", stderr);
#endif
  if (!ipc_is_open ()) {
    return;
  }

  ourmsg.msg.namelength = n;
  ourmsg.msg.eof = is_eof;
  if (n) {
    strcpy (ourmsg.more_data, data);
  }
  n += sizeof (struct msg);
#ifdef IPC_DEBUG
  fputs ("tex: Writing to socket...\n", stderr);
#endif
  if (write (sock, &ourmsg, n) != n) {
    ipc_close_out ();
  }
#ifdef IPC_DEBUG
  fputs ("tex: IPC message sent.\n", stderr);
#endif
}


/* This routine notifies the server if there is an eof, or the filename
   if a new DVI file is starting.  This is the routine called by TeX.
   Omega defines str_start(#) as str_start_ar[# - too_big_char], with
   too_big_char = biggest_char + 1 = 65536 (omstr.ch).*/

void
ipcpage P1C(int, is_eof)
{
  static boolean begun = false;
  unsigned len = 0;
  unsigned i;
  string p = "";

  if (!begun) {
    string name; /* Just the filename.  */
    string cwd = xgetcwd ();
    
    ipc_open_out ();
#ifndef Omega
    len = strstart[outputfilename + 1] - strstart[outputfilename];
#else
    len = strstartar[outputfilename + 1 - 65536L] -
            strstartar[outputfilename - 65536L];
#endif
    name = (string)xmalloc (len + 1);
#ifndef Omega
    strncpy (name, &strpool[strstart[outputfilename]], len);
#else
    for (i=0; i<len; i++)
      name[i] =  strpool[i+strstartar[outputfilename - 65536L]];
#endif
    name[len] = 0;
    
    /* Have to pass whole filename to the other end, since it may have
       been started up and running as a daemon, e.g., as with the NeXT
       preview program.  */
    p = concat3 (cwd, DIR_SEP_STRING, name);
    free (name);
    len = strlen(p);
    begun = true;
  }
  ipc_snd (len, is_eof, p);
  
  if (len > 0) {
    free (p);
  }
}
#endif /* TeX && IPC */

#if defined (TeX) || defined (MF) || defined (MP)
#ifndef Omega  /* TCX and Omega get along like sparks and gunpowder. */

/* Return the next number following START, setting POST to the following
   character, as in strtol.  Issue a warning and return -1 if no number
   can be parsed.  */

static int
tcx_get_num P4C(int, upb,
                unsigned, line_count,
                string, start,
                string *, post)
{
  int num = strtol (start, post, 0);
  assert (post && *post);
  if (*post == start) {
    /* Could not get a number. If blank line, fine. Else complain.  */
    string p = start;
    while (*p && ISSPACE (*p))
      p++;
    if (*p != 0)
      fprintf (stderr, "%s:%d: Expected numeric constant, not `%s'.\n",
               translate_filename, line_count, start);
    num = -1;
  } else if (num < 0 || num > upb) {
    fprintf (stderr, "%s:%d: Destination charcode %d <0 or >%d.\n",
             translate_filename, line_count, num, upb);
    num = -1;
  }  

  return num;
}

/* Update the xchr, xord, and xprn arrays for TeX, allowing a
   translation table specified at runtime via an external file.
   Look for the character translation file FNAME along the same path as
   tex.pool.  If no suffix in FNAME, use .tcx (don't bother trying to
   support extension-less names for these files).  */

/* FIXME: A new format ought to be introduced for these files. */

void
readtcxfile P1H(void)
{
  string orig_filename;
  if (!find_suffix (translate_filename)) {
    translate_filename = concat (translate_filename, ".tcx");
  }
  orig_filename = translate_filename;
  translate_filename
    = kpse_find_file (translate_filename, kpse_web2c_format, true);
  if (translate_filename) {
    string line;
    unsigned line_count = 0;
    FILE *translate_file = xfopen (translate_filename, FOPEN_R_MODE);
    while (line = read_line (translate_file)) {
      int first;
      string start2;
      string comment_loc = strchr (line, '%');
      if (comment_loc)
        *comment_loc = 0;

      line_count++;

      first = tcx_get_num (255, line_count, line, &start2);
      if (first >= 0) {
        string start3;
        int second;
        int printable;
        
        second = tcx_get_num (255, line_count, start2, &start3);
        if (second >= 0) {
            /* I suppose we could check for nonempty junk following the
               "printable" code, but let's not bother.  */
          string extra;
            
          /* If they mention a second code, make that the internal number.  */
          xord[first] = second;
          xchr[second] = first;

          printable = tcx_get_num (1, line_count, start3, &extra);
          if (printable == -1)
            printable = 0;
          /* Don't allow the 7bit ASCII set to become unprintable. */
          if (32 <= second && second <= 126)
            printable = 1;
        } else {
          second = first; /* else make internal the same as external */
          /* If they mention a charcode, call it printable.  */
          printable = 1;
        }

        xprn[second] = printable;
      }
      free (line);
    }
    xfclose(translate_file, translate_filename);
  } else {
    WARNING1 ("Could not open char translation file `%s'", orig_filename);
  }
}
#endif /* !Omega */
#endif /* TeX || MF || MP [character translation] */

/* Normalize quoting of filename -- that is, only quote if there is a space,
   and always use the quote-name-quote style. */
string
normalize_quotes P2C(const_string, name, const_string, mesg)
{
    boolean quoted = false;
    boolean must_quote = (strchr(name, ' ') != NULL);
    /* Leave room for quotes and NUL. */
    string ret = (string)xmalloc(strlen(name)+3);
    string p;
    const_string q;
    p = ret;
    if (must_quote)
        *p++ = '"';
    for (q = name; *q; q++) {
        if (*q == '"')
            quoted = !quoted;
        else
            *p++ = *q;
    }
    if (must_quote)
        *p++ = '"';
    *p = '\0';
    if (quoted) {
        fprintf(stderr, "! Unbalanced quotes in %s %s\n", mesg, name);
        uexit(1);
    }
    return ret;
}

/* Getting the input filename. */
string
get_input_file_name P1H(void)
{
  string input_file_name = NULL;

  if (argv[optind] && argv[optind][0] != '&' && argv[optind][0] != '\\') {
    /* Not &format, not \input, so assume simple filename. */    
    string name = normalize_quotes(argv[optind], "argument");
    boolean quoted = (name[0] == '"');
    if (quoted) {
        /* Overwrite last quote and skip first quote. */
        name[strlen(name)-1] = '\0';
        name++;
    }
    input_file_name = kpse_find_file(name, INPUT_FORMAT, false);
    if (quoted) {
        /* Undo modifications */
        name[strlen(name)] = '"';
        name--;
    }
    argv[optind] = name;
  }
  return input_file_name;
}

/* Reading the options.  */

/* Test whether getopt found an option ``A''.
   Assumes the option index is in the variable `option_index', and the
   option table in a variable `long_options'.  */
#define ARGUMENT_IS(a) STREQ (long_options[option_index].name, a)

/* SunOS cc can't initialize automatic structs, so make this static.  */
static struct option long_options[]
  = { { DUMP_OPTION,                 1, 0, 0 },
      { "help",                      0, 0, 0 },
      { "ini",                       0, &iniversion, 1 },
      { "interaction",               1, 0, 0 },
      { "halt-on-error",             0, &haltonerrorp, 1 },
      { "kpathsea-debug",            1, 0, 0 },
      { "progname",                  1, 0, 0 },
      { "version",                   0, 0, 0 },
      { "recorder",                  0, &recorder_enabled, 1 },
#ifdef TeX
#ifdef IPC
      { "ipc",                       0, &ipcon, 1 },
      { "ipc-start",                 0, &ipcon, 2 },
#endif /* IPC */
#if !defined(Omega) && !defined(eOmega) && !defined(Aleph)
      { "mltex",                  0, &mltexp, 1 },
#endif /* !Omega && !eOmega && !Aleph */
      { "output-comment",            1, 0, 0 },
      { "output-directory",          1, 0, 0 },
      { "shell-escape",              0, &shellenabledp, 1 },
      { "no-shell-escape",           0, &shellenabledp, -1 },
      { "debug-format",              0, &debugformatfile, 1 },
      { "src-specials",              2, 0, 0 },
#endif /* TeX */
#if defined (TeX) || defined (MF) || defined (MP)
      { "file-line-error-style",     0, &filelineerrorstylep, 1 },
      { "no-file-line-error-style",  0, &filelineerrorstylep, -1 },
      /* Shorter option names for the above. */
      { "file-line-error",           0, &filelineerrorstylep, 1 },
      { "no-file-line-error",        0, &filelineerrorstylep, -1 },
      { "jobname",                   1, 0, 0 },
      { "parse-first-line",          0, &parsefirstlinep, 1 },
      { "no-parse-first-line",       0, &parsefirstlinep, -1 },
#if !defined(Omega) && !defined(eOmega) && !defined(Aleph)
      { "translate-file",            1, 0, 0 },
      { "default-translate-file",    1, 0, 0 },
      { "8bit",                      0, &eightbitp, 1 },
#endif /* !Omega && !eOmega && !Aleph */
#endif /* TeX || MF || MP */
#if defined (TeX) || defined (MF)
      { "mktex",                     1, 0, 0 },
      { "no-mktex",                  1, 0, 0 },
#endif /* TeX or MF */
#ifdef MP
      { "T",                         0, &troffmode, 1 },
      { "troff",                     0, &troffmode, 1 },
      { "tex",                       1, 0, 0 },
#endif /* MP */
#ifdef KANJI
      { "kanji",                  1, 0, 0 },
#endif /* KANJI */
      { 0, 0, 0, 0 } };


static void
parse_options P2C(int, argc,  string *, argv)
{
  int g;   /* `getopt' return code.  */
  int option_index;
  int version_switch = false;

  for (;;) {
    g = getopt_long_only (argc, argv, "+", long_options, &option_index);

    if (g == -1) /* End of arguments, exit the loop.  */
      break;

    if (g == '?') { /* Unknown option.  */
      /* FIXME: usage (argv[0]); replaced by continue. */
      continue;
    }

    assert (g == 0); /* We have no short option names.  */

    if (ARGUMENT_IS ("kpathsea-debug")) {
      kpathsea_debug |= atoi (optarg);

    } else if (ARGUMENT_IS ("progname")) {
      user_progname = optarg;

    } else if (ARGUMENT_IS ("jobname")) {
      job_name = normalize_quotes(optarg, "jobname");
      
    } else if (ARGUMENT_IS (DUMP_OPTION)) {
      dump_name = optarg;
      if (!user_progname) user_progname = optarg;
      dumpoption = true;

    } else if (ARGUMENT_IS ("output-directory")) {
      output_directory = optarg;
      
#ifdef TeX
    } else if (ARGUMENT_IS ("output-comment")) {
      unsigned len = strlen (optarg);
      if (len < 256) {
        outputcomment = optarg;
      } else {
        WARNING2 ("Comment truncated to 255 characters from %d. (%s)",
                  len, optarg);
        outputcomment = (string)xmalloc (256);
        strncpy (outputcomment, optarg, 255);
        outputcomment[255] = 0;
      }

#ifdef IPC
    } else if (ARGUMENT_IS ("ipc-start")) {
      ipc_open_out ();
      /* Try to start up the other end if it's not already.  */
      if (!ipc_is_open ()) {
        if (system (IPC_SERVER_CMD) == 0) {
          unsigned i;
          for (i = 0; i < 20 && !ipc_is_open (); i++) {
            sleep (2);
            ipc_open_out ();
          }
        }
      }
#endif /* IPC */
    } else if (ARGUMENT_IS ("src-specials")) {
       last_source_name = xstrdup("");
       /* Option `--src" without any value means `auto' mode. */
       if (optarg == NULL) {
         insertsrcspecialeverypar = true;
         insertsrcspecialauto = true;
         srcspecialsoption = true;
         srcspecialsp = true;
       } else {
          parse_src_specials_option(optarg);
       }
#endif /* TeX */
#if defined (TeX) || defined (MF) || defined (MP)
#ifndef Omega
    } else if (ARGUMENT_IS ("translate-file")) {
      translate_filename = optarg;
    } else if (ARGUMENT_IS ("default-translate-file")) {
      default_translate_filename = optarg;
#if defined(WIN32) && defined(OEM)
    } else if (ARGUMENT_IS ("oem")) {
      /* This should switch the locale to the current OEM code page */
      locale_name = ".OCP";
      bOem = true;
#endif
#endif /* !Omega */
#endif /* TeX || MF || MP */

#if defined (TeX) || defined (MF)
    } else if (ARGUMENT_IS ("mktex")) {
      kpse_maketex_option (optarg, true);

    } else if (ARGUMENT_IS ("no-mktex")) {
      kpse_maketex_option (optarg, false);
#endif /* TeX or MF */
#if defined (MP)
    } else if (ARGUMENT_IS ("tex")) {
      mpost_tex_program = optarg;
#endif /* MP */
    } else if (ARGUMENT_IS ("interaction")) {
        /* These numbers match @d's in *.ch */
      if (STREQ (optarg, "batchmode")) {
        interactionoption = 0;
      } else if (STREQ (optarg, "nonstopmode")) {
        interactionoption = 1;
      } else if (STREQ (optarg, "scrollmode")) {
        interactionoption = 2;
      } else if (STREQ (optarg, "errorstopmode")) {
        interactionoption = 3;
      } else {
        WARNING1 ("Ignoring unknown argument `%s' to --interaction", optarg);
      }
#if defined(KANJI)
    } else if (ARGUMENT_IS ("kanji")) {
        /* These numbers match @d's in *.ch */
      if (STREQ (optarg, "jis")) {
        prockanjicode = JIS;
      } else if (STREQ (optarg, "euc")) {
        prockanjicode = EUC;
      } else if (STREQ (optarg, "sjis")) {
        prockanjicode = SJIS;
      } else {
        WARNING1 ("Ignoring unknown argument `%s' to --kanji", optarg);
      }
#endif /* KANJI */
      
    } else if (ARGUMENT_IS ("help")) {
        usagehelp (PROGRAM_HELP, BUG_ADDRESS);

    } else if (ARGUMENT_IS ("version")) {
		version_switch = true;

    } /* Else it was a flag; getopt has already done the assignment.  */
  }
  if (version_switch) {
    printversionandexit (BANNER, COPYRIGHT_HOLDER, AUTHOR);
  }
}

#if defined(TeX)
void 
parse_src_specials_option P1C(const_string, opt_list)
{
  char * toklist = xstrdup(opt_list);
  char * tok;
  insertsrcspecialauto = false;
  tok = strtok (toklist, ", ");
  while (tok) {
    if (strcmp (tok, "everypar") == 0
        || strcmp (tok, "par") == 0
        || strcmp (tok, "auto") == 0) {
      insertsrcspecialauto = true;
      insertsrcspecialeverypar = true;
    } else if (strcmp (tok, "everyparend") == 0
               || strcmp (tok, "parend") == 0)
      insertsrcspecialeveryparend = true;
    else if (strcmp (tok, "everycr") == 0
             || strcmp (tok, "cr") == 0)
      insertsrcspecialeverycr = true;
    else if (strcmp (tok, "everymath") == 0
             || strcmp (tok, "math") == 0)
      insertsrcspecialeverymath = true;
    else if (strcmp (tok, "everyhbox") == 0
             || strcmp (tok, "hbox") == 0)
      insertsrcspecialeveryhbox = true;
    else if (strcmp (tok, "everyvbox") == 0
             || strcmp (tok, "vbox") == 0)
      insertsrcspecialeveryvbox = true;
    else if (strcmp (tok, "everydisplay") == 0
             || strcmp (tok, "display") == 0)
      insertsrcspecialeverydisplay = true;
    else if (strcmp (tok, "none") == 0) {
      /* This one allows to reset an option that could appear in texmf.cnf */
      insertsrcspecialauto = insertsrcspecialeverypar = 
        insertsrcspecialeveryparend = insertsrcspecialeverycr = 
        insertsrcspecialeverymath =  insertsrcspecialeveryhbox =
        insertsrcspecialeveryvbox = insertsrcspecialeverydisplay = false;
    } else {
      WARNING1 ("Ignoring unknown argument `%s' to --src-specials", tok);
    }
    tok = strtok(0, ", ");
  }
  free(toklist);
  srcspecialsp=insertsrcspecialauto | insertsrcspecialeverypar |
    insertsrcspecialeveryparend | insertsrcspecialeverycr |
    insertsrcspecialeverymath |  insertsrcspecialeveryhbox |
    insertsrcspecialeveryvbox | insertsrcspecialeverydisplay;
  srcspecialsoption = true;
}
#endif

/* If the first thing on the command line (we use the globals `argv' and
   `optind') is a normal filename (i.e., does not start with `&' or
   `\'), and if we can open it, and if its first line is %&FORMAT, and
   FORMAT is a readable dump file, then set DUMP_VAR to FORMAT.
   Also call kpse_reset_program_name to ensure the correct paths for the
   format are used.  */
static void
parse_first_line P1C(const_string, filename)
{
  FILE *f = filename ? fopen (filename, FOPEN_R_MODE) : NULL;
  if (f) {
    string first_line = read_line (f);
    xfclose (f, filename);

    /* We deal with the general format "%&fmt --translate-file=tcx" */
    /* The idea of using this format came from Wlodzimierz Bzyl
       <matwb@monika.univ.gda.pl> */
    if (first_line && first_line[0] == '%' && first_line[1] == '&') {
      /* Parse the first line into at most three space-separated parts. */
      char *s;
      char *part[4];
      int npart;
      char **parse;

      for (s = first_line+2; ISBLANK(*s); ++s)
        ;
      npart = 0;
      while (*s && npart != 3) {
        part[npart++] = s;
        while (*s && *s != ' ') s++;
        while (*s == ' ') *s++ = '\0';
      }
      part[npart] = NULL;
      parse = part;
      /* Look at what we've got.  Very crude! */
      if (*parse && **parse != '-') {
        /* A format name */
        if (dump_name) {
          /* format already determined, do nothing. */
        } else {
          string f_name = concat (part[0], DUMP_EXT);
          string d_name = kpse_find_file (f_name, DUMP_FORMAT, false);
          if (d_name && kpse_readable_file (d_name)) {
            dump_name = xstrdup (part[0]);
            kpse_reset_program_name (dump_name);
            /* Tell TeX/MF/MP we have a %&name line... */
            dumpline = true;
          }
          free (f_name);
        }
        parse++;
      }
      /* The tcx stuff, if any.  Should we support the -translate-file
         form as well as --translate-file?  */
      if (*parse) {
        if (translate_filename) {
          /* TCX file already set, do nothing. */
        } else if (STREQ (*parse, "--translate-file")) {
          s = *(parse+1);
        } else if (STREQ (*parse, "-translate-file")) {
          s = *(parse+1);
        } else if (STRNEQ (*parse, "--translate-file=", 17)) {
          s = *parse+17;
        } else if (STRNEQ (*parse, "-translate-file=", 16)) {
          s = *parse+16;
        }
        /* Just set the name, no sanity checks here. */
        /* FIXME: remove trailing spaces. */
        if (s && *s) {
          translate_filename = xstrdup(s);
        }
      }
    }
    if (first_line)
      free (first_line);
  }
}

/* Return true if FNAME is acceptable as a name for \openout, \openin, or
   \input.  */

static boolean
opennameok P3C(const_string, fname, const_string, check_var,
               const_string, default_choice)
{
  /* We distinguish three cases:
     'a' (any)        allows any file to be opened.
     'r' (restricted) means disallowing special file names.
     'p' (paranoid)   means being really paranoid: disallowing special file
                      names and restricting output files to be in or below
                      the working directory or $TEXMFOUTPUT, while input files
                      must be below the current directory, $TEXMFOUTPUT, or
                      (implicitly) in the system areas.
     We default to "paranoid".  The error messages from TeX will be somewhat
     puzzling...
     This function contains several return statements...  */

  char  mess[8];
  const_string open_choice = kpse_var_value (check_var);

  if (strcmp(check_var, "openin_any") == 0)
    strcpy(mess, "reading");
  else if (strcmp(check_var, "openout_any") == 0)
    strcpy(mess, "writing");
  else
    mess[0] = '\0';

  if (!open_choice) open_choice = default_choice;

  if (*open_choice == 'a' || *open_choice == 'y' || *open_choice == '1')
    return true;

#if defined (unix) && !defined (MSDOS)
  {
    const_string base = xbasename (fname);
    /* Disallow .rhosts, .login, etc.  Allow .tex (for LaTeX).  */
    if (base[0] == 0 ||
        (base[0] == '.' && !IS_DIR_SEP(base[1]) && !STREQ (base, ".tex"))) {
      fprintf(stderr, "%s: Not %s to %s (%s = %s).\n",
              program_invocation_name, mess, fname, check_var, open_choice);
      return false;
    }
  }
#else
  /* Other OSs don't have special names? */
#endif

  if (*open_choice == 'r' || *open_choice == 'n' || *open_choice == '0')
    return true;

  /* Paranoia supplied by Charles Karney...  */
  if (kpse_absolute_p (fname, false)) {
    const_string texmfoutput = kpse_var_value ("TEXMFOUTPUT");
    /* Absolute pathname is only OK if TEXMFOUTPUT is set, it's not empty,
       fname begins the TEXMFOUTPUT, and is followed by / */
    if (!texmfoutput || *texmfoutput == '\0'
        || fname != strstr (fname, texmfoutput)
        || !IS_DIR_SEP(fname[strlen(texmfoutput)])) {
      fprintf(stderr, "%s: Not %s to %s (%s = %s).\n",
              program_invocation_name, mess, fname, check_var, open_choice);
      return false;
    }
  }
  /* For all pathnames, we disallow "../" at the beginning or "/../"
     anywhere.  */
  if (fname[0] == '.' && fname[1] == '.' && IS_DIR_SEP(fname[2])) {
    fprintf(stderr, "%s: Not %s to %s (%s = %s).\n",
            program_invocation_name, mess, fname, check_var, open_choice);
    return false;
  } else {
    const_string dotpair = strstr (fname, "..");
    /* If dotpair[2] == DIR_SEP, then dotpair[-1] is well-defined. */
    if (dotpair && IS_DIR_SEP(dotpair[2]) && IS_DIR_SEP(dotpair[-1])) {
      fprintf(stderr, "%s: Not %s to %s (%s = %s).\n",
              program_invocation_name, mess, fname, check_var, open_choice);
      return false;
    }
  }

  /* We passed all tests.  */
  return true;
}

boolean openinnameok P1C(const_string, fname)
{
    /* For input default to all. */
    return opennameok (fname, "openin_any", "a");
}

boolean openoutnameok P1C(const_string, fname)
{
    /* For output, default to paranoid. */
    return opennameok (fname, "openout_any", "p");
}

/* All our interrupt handler has to do is set TeX's or Metafont's global
   variable `interrupt'; then they will do everything needed.  */
#ifdef WIN32
/* Win32 doesn't set SIGINT ... */
BOOL WINAPI
catch_interrupt (DWORD arg)
{
  switch (arg) {
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
    interrupt = 1;
    return TRUE;
  default:
    /* No need to set interrupt as we are exiting anyway */
    return FALSE;
  }
}
#else /* not WIN32 */
static RETSIGTYPE
catch_interrupt P1C (int, arg)
{
  interrupt = 1;
#ifdef OS2
  (void) signal (SIGINT, SIG_ACK);
#else
  (void) signal (SIGINT, catch_interrupt);
#endif /* not OS2 */
}
#endif /* not WIN32 */

/* Besides getting the date and time here, we also set up the interrupt
   handler, for no particularly good reason.  It's just that since the
   `fix_date_and_time' routine is called early on (section 1337 in TeX,
   ``Get the first line of input and prepare to start''), this is as
   good a place as any.  */

void
get_date_and_time P4C(integer *, minutes,  integer *, day,
                      integer *, month,  integer *, year)
{
  time_t clock = time ((time_t *) 0);
  struct tm *tmptr = localtime (&clock);

  *minutes = tmptr->tm_hour * 60 + tmptr->tm_min;
  *day = tmptr->tm_mday;
  *month = tmptr->tm_mon + 1;
  *year = tmptr->tm_year + 1900;

  {
#ifdef SA_INTERRUPT
    /* Under SunOS 4.1.x, the default action after return from the
       signal handler is to restart the I/O if nothing has been
       transferred.  The effect on TeX is that interrupts are ignored if
       we are waiting for input.  The following tells the system to
       return EINTR from read() in this case.  From ken@cs.toronto.edu.  */

    struct sigaction a, oa;

    a.sa_handler = catch_interrupt;
    sigemptyset (&a.sa_mask);
    sigaddset (&a.sa_mask, SIGINT);
    a.sa_flags = SA_INTERRUPT;
    sigaction (SIGINT, &a, &oa);
    if (oa.sa_handler != SIG_DFL)
      sigaction (SIGINT, &oa, (struct sigaction *) 0);
#else /* no SA_INTERRUPT */
#ifdef WIN32
    SetConsoleCtrlHandler(catch_interrupt, TRUE);
#else /* not WIN32 */
    RETSIGTYPE (*old_handler) P1H(int);
    
    old_handler = signal (SIGINT, catch_interrupt);
    if (old_handler != SIG_DFL)
      signal (SIGINT, old_handler);
#endif /* not WIN32 */
#endif /* no SA_INTERRUPT */
  }
}

/*
  Generating a better seed numbers
  */
integer
getrandomseed()
{
#if defined (HAVE_GETTIMEOFDAY)
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_usec + 1000000 * tv.tv_usec);
#elif defined (HAVE_FTIME)
  struct timeb tb;
  ftime(&tb);
  return (tb.millitm + 1000 * tb.time);
#else
  time_t clock = time ((time_t*)NULL);
  struct tm *tmptr = localtime(&clock);
  return (tmptr->tm_sec + 60*(tmptr->tm_min + 60*tmptr->tm_hour));
#endif
}

/* Read a line of input as efficiently as possible while still looking
   like Pascal.  We set `last' to `first' and return `false' if we get
   to eof.  Otherwise, we return `true' and set last = first +
   length(line except trailing whitespace).  */

boolean
input_line P1C(FILE *, f)
{
  int i;

#ifdef KANJI
/* new line input method */
  static int injis=0;
#endif

  /* Recognize either LF or CR as a line terminator.  */
  last = first;
#ifdef KANJI
/* new line input method */
  while (last < bufsize-3 && (i = getc (f)) != EOF && i != '\n' && i != '\r') {
    int j;
    if (i == '\033') { /* ESC */
      if ((i = getc(f)) == '$') { /* Kanji-In */
        i = getc(f);
        if (i == '@' || i == 'B') {
          injis = 1;
        } else {
          buffer[last++] = '\033';
          buffer[last++] = '$';
          if(i == EOF) {
             break;
          } else if(i == '\n') {
             break;
          } else if(i == '\r') {
             j = getc(f);
             if(j != '\n') ungetc(j, f);
             else i = j;
             break;
          } else {
             buffer[last++] = i;
	  }
        }
      } else if (i == '(') { /* Kanji-out */
        i = getc(f);
        if (i == 'J' || i == 'B' || i == 'H') {
          injis = 0;
        } else {
          buffer[last++] = '\033';
          buffer[last++] = '(';
          if(i == EOF) {
             break;
          } else if(i == '\n') {
             break;
          } else if(i == '\r') {
             j = getc(f);
             if(j != '\n') ungetc(j, f);
             else i = j;
             break;
	  } else {
             buffer[last++] = i;
	  }
        }
      } else {
        buffer[last++] = '\033';
        if(i == EOF) {
           break;
        } else if(i == '\n') {
           break;
        } else if(i == '\r') {
           j = getc(f);
           if(j != '\n') ungetc(j, f);
           else i = j;
           break;
	} else {
           buffer[last++] = i;
	}
      }
    } else { /* rather than ESC */
      if (injis == 0) {
        buffer[last++] = i;
      } else {
        j = getc(f);
        if(j == EOF) {
           buffer[last++] = i;
           i = j;
           break;
        } else if(j == '\n') {
           buffer[last++] = i;
           i = j;
           break;
	} else if(j == '\r') {
           buffer[last++] = i;
           i = j;
           j = getc(f);
           if(j != '\n') ungetc(j, f);
           else i = j;
           break;
        /* JIS encoding */
        } else {
           if (prockanjicode==SJIS) {
              i = JIStoSJIS(i << 8 | j);
	   } else {
              i = JIStoEUC(i << 8 | j);
           }
           buffer[last++] = (i >> 8) & 0xff;
           buffer[last++] = i & 0xff;
	}
      }
    }
  }

  if(i == EOF || i == '\n' || i == '\r')
    injis = 0;
#else /* KANJI */
  while (last < bufsize && (i = getc (f)) != EOF && i != '\n' && i != '\r')
    buffer[last++] = i;
#endif /* KANJI */

  if (i == EOF && errno != EINTR && last == first)
    return false;

  /* We didn't get the whole line because our buffer was too small.  */
  if (i != EOF && i != '\n' && i != '\r') {
    fprintf (stderr, "! Unable to read an entire line---bufsize=%u.\n",
                     (unsigned) bufsize);
    fputs ("Please increase buf_size in texmf.cnf.\n", stderr);
    uexit (1);
  }

  buffer[last] = ' ';
  if (last >= maxbufstack)
    maxbufstack = last;

  /* If next char is LF of a CRLF, read it.  */
  if (i == '\r') {
    while ((i = getc (f)) == EOF && errno == EINTR)
      ;
    if (i != '\n')
      ungetc (i, f);
  }
  
  /* Trim trailing whitespace.  */
  while (last > first && ISBLANK (buffer[last - 1]))
    --last;

  /* Don't bother using xord if we don't need to.  */
#ifndef Omega
  for (i = first; i <= last; i++)
     buffer[i] = xord[buffer[i]];
#endif

#ifdef KANJI
  for (i = last+1; (i < last + 5 && i < bufsize) ; i++)
    buffer[i] = '\0';
#endif

  return true;
}

/* This string specifies what the `e' option does in response to an
   error message.  */ 
static char *edit_value = EDITOR;

/* This procedure originally due to sjc@s1-c.  TeX & Metafont call it when
   the user types `e' in response to an error, invoking a text editor on
   the erroneous source file.  FNSTART is how far into FILENAME the
   actual filename starts; FNLENGTH is how long the filename is.  */
   
void
calledit P4C(packedASCIIcode *, filename,
             poolpointer, fnstart,
             integer, fnlength,
             integer, linenumber)
{
  char *temp, *command;
  char c;
  int sdone, ddone, i;

  sdone = ddone = 0;
  filename += fnstart;

  /* Close any open input files, since we're going to kill the job.  */
  for (i = 1; i <= inopen; i++)
    xfclose (inputfile[i], "inputfile");

  /* Replace the default with the value of the appropriate environment
     variable or config file value, if it's set.  */
  temp = kpse_var_value (edit_var);
  if (temp != NULL)
    edit_value = temp;

  /* Construct the command string.  The `11' is the maximum length an
     integer might be.  */
  command = (string) xmalloc (strlen (edit_value) + fnlength + 11);

  /* So we can construct it as we go.  */
  temp = command;

  while ((c = *edit_value++) != 0)
    {
      if (c == '%')
        {
          switch (c = *edit_value++)
            {
	    case 'd':
	      if (ddone)
                FATAL ("call_edit: `%%d' appears twice in editor command");
              sprintf (temp, "%ld", linenumber);
              while (*temp != '\0')
                temp++;
              ddone = 1;
              break;

	    case 's':
              if (sdone)
                FATAL ("call_edit: `%%s' appears twice in editor command");
              for (i =0; i < fnlength; i++)
		*temp++ = Xchr (filename[i]);
              sdone = 1;
              break;

	    case '\0':
              *temp++ = '%';
              /* Back up to the null to force termination.  */
	      edit_value--;
	      break;

	    default:
	      *temp++ = '%';
	      *temp++ = c;
	      break;
	    }
	}
      else
	*temp++ = c;
    }

  *temp = 0;

  /* Execute the command.  */
#ifdef WIN32
  /* Win32 reimplementation of the system() command
     provides opportunity to call it asynchronously */
  if (win32_system(command, true) != 0 )
#else
  if (system (command) != 0)
#endif
    fprintf (stderr, "! Trouble executing `%s'.\n", command);

  /* Quit, since we found an error.  */
  uexit (1);
}

/* Read and write dump files.  As distributed, these files are
   architecture dependent; specifically, BigEndian and LittleEndian
   architectures produce different files.  These routines always output
   BigEndian files.  This still does not guarantee them to be
   architecture-independent, because it is possible to make a format
   that dumps a glue ratio, i.e., a floating-point number.  Fortunately,
   none of the standard formats do that.  */

#if !defined (WORDS_BIGENDIAN) && !defined (NO_DUMP_SHARE) /* this fn */

/* This macro is always invoked as a statement.  It assumes a variable
   `temp'.  */
   
#define SWAP(x, y) temp = (x); (x) = (y); (y) = temp


/* Make the NITEMS items pointed at by P, each of size SIZE, be the
   opposite-endianness of whatever they are now.  */

static void
swap_items P3C(char *, p,  int, nitems,  int, size)
{
  char temp;

  /* Since `size' does not change, we can write a while loop for each
     case, and avoid testing `size' for each time.  */
  switch (size)
    {
    /* 16-byte items happen on the DEC Alpha machine when we are not
       doing sharable memory dumps.  */
    case 16:
      while (nitems--)
        {
          SWAP (p[0], p[15]);
          SWAP (p[1], p[14]);
          SWAP (p[2], p[13]);
          SWAP (p[3], p[12]);
          SWAP (p[4], p[11]);
          SWAP (p[5], p[10]);
          SWAP (p[6], p[9]);
          SWAP (p[7], p[8]);
          p += size;
        }
      break;

    case 8:
      while (nitems--)
        {
          SWAP (p[0], p[7]);
          SWAP (p[1], p[6]);
          SWAP (p[2], p[5]);
          SWAP (p[3], p[4]);
          p += size;
        }
      break;

    case 4:
      while (nitems--)
        {
          SWAP (p[0], p[3]);
          SWAP (p[1], p[2]);
          p += size;
        }
      break;

    case 2:
      while (nitems--)
        {
          SWAP (p[0], p[1]);
          p += size;
        }
      break;

    case 1:
      /* Nothing to do.  */
      break;

    default:
      FATAL1 ("Can't swap a %d-byte item for (un)dumping", size);
  }
}
#endif /* not WORDS_BIGENDIAN and not NO_DUMP_SHARE */


/* Here we write NITEMS items, each item being ITEM_SIZE bytes long.
   The pointer to the stuff to write is P, and we write to the file
   OUT_FILE.  */

void
do_dump P4C(char *, p,  int, item_size,  int, nitems,  FILE *, out_file)
{
#if !defined (WORDS_BIGENDIAN) && !defined (NO_DUMP_SHARE)
  swap_items (p, nitems, item_size);
#endif

  if (fwrite (p, item_size, nitems, out_file) != nitems)
    {
      fprintf (stderr, "! Could not write %d %d-byte item(s).\n",
               nitems, item_size);
      uexit (1);
    }

  /* Have to restore the old contents of memory, since some of it might
     get used again.  */
#if !defined (WORDS_BIGENDIAN) && !defined (NO_DUMP_SHARE)
  swap_items (p, nitems, item_size);
#endif
}


/* Here is the dual of the writing routine.  */

void
do_undump P4C(char *, p,  int, item_size,  int, nitems,  FILE *, in_file)
{
  if (fread (p, item_size, nitems, in_file) != nitems)
    FATAL2 ("Could not undump %d %d-byte item(s)", nitems, item_size);

#if !defined (WORDS_BIGENDIAN) && !defined (NO_DUMP_SHARE)
  swap_items (p, nitems, item_size);
#endif
}

/* Look up VAR_NAME in texmf.cnf; assign either the value found there or
   DFLT to *VAR.  */

void
setupboundvariable P3C(integer *, var,  const_string, var_name,  integer, dflt)
{
  string expansion = kpse_var_value (var_name);
  *var = dflt;

  if (expansion) {
    integer conf_val = atoi (expansion);
    /* It's ok if the cnf file specifies 0 for extra_mem_{top,bot}, etc.
       But negative numbers are always wrong.  */
    if (conf_val < 0 || (conf_val == 0 && dflt > 0)) {
      fprintf (stderr,
               "%s: Bad value (%ld) in texmf.cnf for %s, keeping %ld.\n",
               program_invocation_name,
               (long) conf_val, var_name + 1, (long) dflt);
    } else {
      *var = conf_val; /* We'll make further checks later.  */
    }
    free (expansion);
  }
}

/* FIXME -- some (most?) of this can/should be moved to the Pascal/WEB side. */
#if defined(TeX) || defined(MP) || defined(MF)
static void
checkpoolpointer (poolpointer poolptr, size_t len)
{
  if (poolptr + len >= poolsize) {
    fprintf (stderr, "\nstring pool overflow [%i bytes]\n", 
            (int)poolsize); /* fixme */
    exit(1);
  }
}

#if !defined(pdfTeX) && !defined(pdfeTeX)
static int
maketexstring(const_string s)
{
  size_t len;
  assert (s != 0);
  len = strlen(s);
  checkpoolpointer (poolptr, len);
  while (len-- > 0)
    strpool[poolptr++] = *s++;
  return (makestring());
}
#endif

strnumber
makefullnamestring()
{
  return maketexstring(fullnameoffile);
}

strnumber
getjobname()
{
    strnumber ret = curname;
    if (job_name != NULL)
      ret = maketexstring(job_name);
    return ret;
}
#endif

#if defined(TeX)
int
compare_paths P2C(const_string, p1, const_string, p2)
{
  int ret;
  while (
#ifdef MONOCASE_FILENAMES
                (((ret = (toupper(*p1) - toupper(*p2))) == 0) && (*p2 != 0))
#else
         (((ret = (*p1 - *p2)) == 0) && (*p2 != 0))
#endif
                || (IS_DIR_SEP(*p1) && IS_DIR_SEP(*p2))) {
       p1++, p2++;
  }
  ret = (ret < 0 ? -1 : (ret > 0 ? 1 : 0));
  return ret;
}

string
gettexstring P1C(strnumber, s)
{
  poolpointer i, len;
  string name;
#ifndef Omega
  len = strstart[s + 1] - strstart[s];
#else
  len = strstartar[s + 1 - 65536L] - strstartar[s - 65536L];
#endif
  name = (string)xmalloc (len + 1);
#ifndef Omega
  strncpy (name, (string)&strpool[strstart[s]], len);
#else
  /* Don't use strncpy.  The strpool is not made up of chars. */
  for (i=0; i<len; i++) name[i] =  strpool[i+strstartar[s - 65536L]];
#endif
  name[len] = 0;
  return name;
}

boolean
isnewsource P2C(strnumber, srcfilename, int, lineno)
{
  char *name = gettexstring(srcfilename);
  return (compare_paths(name, last_source_name) != 0 || lineno != last_lineno);
}

void
remembersourceinfo P2C(strnumber, srcfilename,
                                          int, lineno)
{
  if (last_source_name)
       free(last_source_name);
  last_source_name = gettexstring(srcfilename);
  last_lineno = lineno;
}

poolpointer
makesrcspecial P2C(strnumber, srcfilename,
                                  int, lineno)
{
  poolpointer oldpoolptr = poolptr;
  char *filename = gettexstring(srcfilename);
  char buf[40];
  size_t len = strlen(filename);
  char * s = buf;  

  /* Always put a space after the number, which makes things easier
   * to parse.
   */
  sprintf (buf, "src:%d ", lineno);

  if (poolptr + strlen(buf) + strlen(filename) >= poolsize) {
       fprintf (stderr, "\nstring pool overflow\n"); /* fixme */
       exit (1);
  }
  s = buf;
  while (*s)
    strpool[poolptr++] = *s++;

  s = filename;
  while (*s)
    strpool[poolptr++] = *s++;
       
  return (oldpoolptr);
}
#endif

#ifdef MP
/* Invoke makempx (or troffmpx) to make sure there is an up-to-date
   .mpx file for a given .mp file.  (Original from John Hobby 3/14/90)  */

#include <kpathsea/concatn.h>

#ifndef MPXCOMMAND
#define MPXCOMMAND "makempx"
#endif

boolean
callmakempx P2C(string, mpname,  string, mpxname)
{
  int ret;
  string cnf_cmd = kpse_var_value ("MPXCOMMAND");
  
  if (cnf_cmd && STREQ (cnf_cmd, "0")) {
    /* If they turned off this feature, just return success.  */
    ret = 0;

  } else {
    /* We will invoke something. Compile-time default if nothing else.  */
    string cmd;
    string qmpname = normalize_quotes(mpname, "mpname");
    string qmpxname = normalize_quotes(mpxname, "mpxname");
    if (!cnf_cmd)
      cnf_cmd = xstrdup (MPXCOMMAND);

    if (troffmode)
      cmd = concatn (cnf_cmd, " -troff ",
                     qmpname, " ", qmpxname, NULL);
    else if (mpost_tex_program && *mpost_tex_program)
      cmd = concatn (cnf_cmd, " -tex=", mpost_tex_program, " ",
                     qmpname, " ", qmpxname, NULL);
    else
      cmd = concatn (cnf_cmd, " -tex ", qmpname, " ", qmpxname, NULL);

    /* Run it.  */
    ret = system (cmd);
    free (cmd);
    free (qmpname);
    free (qmpxname);
  }

  free (cnf_cmd);
  return ret == 0;
}
#endif /* MP */

/* Metafont/MetaPost fraction routines. Replaced either by assembler or C.
   The assembler syntax doesn't work on Solaris/x86.  */
#ifndef TeX
#if defined (__sun__) || defined (__cplusplus)
#define NO_MF_ASM
#endif
#if defined(WIN32) && !defined(NO_MF_ASM)
#include "lib/mfmpw32.c"
#elif defined (__i386__) && defined (__GNUC__) && !defined (NO_MF_ASM)
#include "lib/mfmpi386.asm"
#else
/* Replace fixed-point fraction routines from mf.web and mp.web with
   Hobby's floating-point C code.  */

/****************************************************************
Copyright 1990 - 1995 by AT&T Bell Laboratories.

Permission to use, copy, modify, and distribute this software
and its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the names of AT&T Bell Laboratories or
any of its entities not be used in advertising or publicity
pertaining to distribution of the software without specific,
written prior permission.

AT&T disclaims all warranties with regard to this software,
including all implied warranties of merchantability and fitness.
In no event shall AT&T be liable for any special, indirect or
consequential damages or any damages whatsoever resulting from
loss of use, data or profits, whether in an action of contract,
negligence or other tortious action, arising out of or in
connection with the use or performance of this software.
****************************************************************/

/**********************************************************
 The following is by John Hobby
 **********************************************************/

#ifndef FIXPT

/* These replacements for takefraction, makefraction, takescaled, makescaled
   run about 3 to 11 times faster than the standard versions on modern machines
   that have fast hardware for double-precision floating point.  They should
   produce approximately correct results on all machines and agree exactly
   with the standard versions on machines that satisfy the following conditions:
   1. Doubles must have at least 46 mantissa bits; i.e., numbers expressible
      as n*2^k with abs(n)<2^46 should be representable.
   2. The following should hold for addition, subtraction, and multiplcation but
      not necessarily for division:
      A. If the true answer is between two representable numbers, the computed
         answer must be one of them.
      B. When the true answer is representable, this must be the computed result.
   3. Dividing one double by another should always produce a relative error of
      at most one part in 2^46.  (This is why the mantissa requirement is
      46 bits instead of 45 bits.)
   3. In the absence of overflow, double-to-integer conversion should truncate
      toward zero and do this in an exact fashion.
   4. Integer-to-double convesion should produce exact results.
   5. Dividing one power of two by another should yield an exact result.
   6. ASCII to double conversion should be exact for integer values.
   7. Integer arithmetic must be done in the two's-complement system.
*/
#define ELGORDO  0x7fffffff
#define TWEXP31  2147483648.0
#define TWEXP28  268435456.0
#define TWEXP16 65536.0
#define TWEXP_16 (1.0/65536.0)
#define TWEXP_28 (1.0/268435456.0)

integer
ztakefraction P2C(integer, p, integer, q)     /* Approximate p*q/2^28 */
{	register double d;
	register integer i;
	d = (double)p * (double)q * TWEXP_28;
	if ((p^q) >= 0) {
		d += 0.5;
		if (d>=TWEXP31) {
			if (d!=TWEXP31 || (((p&077777)*(q&077777))&040000)==0)
				aritherror = true;
			return ELGORDO;
		}
		i = (integer) d;
		if (d==i && (((p&077777)*(q&077777))&040000)!=0) --i;
	} else {
		d -= 0.5;
		if (d<= -TWEXP31) {
			if (d!= -TWEXP31 || ((-(p&077777)*(q&077777))&040000)==0)
				aritherror = true;
			return -ELGORDO;
		}
		i = (integer) d;
		if (d==i && ((-(p&077777)*(q&077777))&040000)!=0) ++i;
	}
	return i;
}

integer
ztakescaled P2C(integer, p, integer, q)		/* Approximate p*q/2^16 */
{	register double d;
	register integer i;
	d = (double)p * (double)q * TWEXP_16;
	if ((p^q) >= 0) {
		d += 0.5;
		if (d>=TWEXP31) {
			if (d!=TWEXP31 || (((p&077777)*(q&077777))&040000)==0)
				aritherror = true;
			return ELGORDO;
		}
		i = (integer) d;
		if (d==i && (((p&077777)*(q&077777))&040000)!=0) --i;
	} else {
		d -= 0.5;
		if (d<= -TWEXP31) {
			if (d!= -TWEXP31 || ((-(p&077777)*(q&077777))&040000)==0)
				aritherror = true;
			return -ELGORDO;
		}
		i = (integer) d;
		if (d==i && ((-(p&077777)*(q&077777))&040000)!=0) ++i;
	}
	return i;
}

/* Note that d cannot exactly equal TWEXP31 when the overflow test is made
   because the exact value of p/q cannot be strictly between (2^31-1)/2^28
   and 8/1.  No pair of integers less than 2^31 has such a ratio.
*/
integer
zmakefraction P2C(integer, p, integer, q)	/* Approximate 2^28*p/q */
{	register double d;
	register integer i;
#ifdef DEBUG
	if (q==0) confusion(47); 
#endif /* DEBUG */
	d = TWEXP28 * (double)p /(double)q;
	if ((p^q) >= 0) {
		d += 0.5;
		if (d>=TWEXP31) {aritherror=true; return ELGORDO;}
		i = (integer) d;
		if (d==i && ( ((q>0 ? -q : q)&077777)
				* (((i&037777)<<1)-1) & 04000)!=0) --i;
	} else {
		d -= 0.5;
		if (d<= -TWEXP31) {aritherror=true; return -ELGORDO;}
		i = (integer) d;
		if (d==i && ( ((q>0 ? q : -q)&077777)
				* (((i&037777)<<1)+1) & 04000)!=0) ++i;
	}
	return i;
}

/* Note that d cannot exactly equal TWEXP31 when the overflow test is made
   because the exact value of p/q cannot be strictly between (2^31-1)/2^16
   and 2^15/1.  No pair of integers less than 2^31 has such a ratio.
*/
integer
zmakescaled P2C(integer, p, integer, q)		/* Approximate 2^16*p/q */
{	register double d;
	register integer i;
#ifdef DEBUG
	if (q==0) confusion(47); 
#endif /* DEBUG */
	d = TWEXP16 * (double)p /(double)q;
	if ((p^q) >= 0) {
		d += 0.5;
		if (d>=TWEXP31) {aritherror=true; return ELGORDO;}
		i = (integer) d;
		if (d==i && ( ((q>0 ? -q : q)&077777)
				* (((i&037777)<<1)-1) & 04000)!=0) --i;
	} else {
		d -= 0.5;
		if (d<= -TWEXP31) {aritherror=true; return -ELGORDO;}
		i = (integer) d;
		if (d==i && ( ((q>0 ? q : -q)&077777)
				* (((i&037777)<<1)+1) & 04000)!=0) ++i;
	}
	return i;
}

#endif /* not FIXPT */
#endif /* not assembler */
#endif /* not TeX, i.e., MF or MP */

#ifdef MF
/* On-line display routines for Metafont.  Here we use a dispatch table
   indexed by the MFTERM or TERM environment variable to select the
   graphics routines appropriate to the user's terminal.  stdout must be
   connected to a terminal for us to do any graphics.  */

#ifdef MFNOWIN
#undef AMIGAWIN
#undef EPSFWIN
#undef HP2627WIN
#undef MFTALKWIN
#undef NEXTWIN
#undef REGISWIN
#undef SUNWIN
#undef TEKTRONIXWIN
#undef UNITERMWIN
#undef WIN32WIN
#undef X11WIN
#endif

#ifdef AMIGAWIN
extern int mf_amiga_initscreen P1H(void);
extern void mf_amiga_updatescreen P1H(void);
extern void mf_amiga_blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void mf_amiga_paintrow P4H(screenrow, pixelcolor, transspec, screencol);
#endif
#ifdef EPSFWIN
extern int mf_epsf_initscreen P1H(void);
extern void mf_epsf_updatescreen P1H(void);
extern void mf_epsf_blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void mf_epsf_paintrow P4H(screenrow, pixelcolor, transspec, screencol);
#endif
#ifdef HP2627WIN
extern int mf_hp2627_initscreen P1H(void);
extern void mf_hp2627_updatescreen P1H(void);
extern void mf_hp2627_blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void mf_hp2627_paintrow P4H(screenrow, pixelcolor, transspec, screencol);
#endif
#ifdef MFTALKWIN
extern int mf_mftalk_initscreen P1H(void);
extern void mf_mftalk_updatescreen P1H(void);
extern void mf_mftalk_blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void mf_mftalk_paintrow P4H(screenrow, pixelcolor, transspec, screencol);
#endif
#ifdef NEXTWIN
extern int mf_next_initscreen P1H(void);
extern void mf_next_updatescreen P1H(void);
extern void mf_next_blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void mf_next_paintrow P4H(screenrow, pixelcolor, transspec, screencol);
#endif
#ifdef REGISWIN
extern int mf_regis_initscreen P1H(void);
extern void mf_regis_updatescreen P1H(void);
extern void mf_regis_blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void mf_regis_paintrow P4H(screenrow, pixelcolor, transspec, screencol);
#endif
#ifdef SUNWIN
extern int mf_sun_initscreen P1H(void);
extern void mf_sun_updatescreen P1H(void);
extern void mf_sun_blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void mf_sun_paintrow P4H(screenrow, pixelcolor, transspec, screencol);
#endif
#ifdef TEKTRONIXWIN
extern int mf_tektronix_initscreen P1H(void);
extern void mf_tektronix_updatescreen P1H(void);
extern void mf_tektronix_blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void mf_tektronix_paintrow P4H(screenrow, pixelcolor, transspec, screencol);
#endif
#ifdef UNITERMWIN
extern int mf_uniterm_initscreen P1H(void);
extern void mf_uniterm_updatescreen P1H(void);
extern void mf_uniterm_blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void mf_uniterm_paintrow P4H(screenrow, pixelcolor, transspec, screencol);
#endif
#ifdef WIN32WIN
extern int mf_win32_initscreen P1H(void);
extern void mf_win32_updatescreen P1H(void);
extern void mf_win32_blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void mf_win32_paintrow P4H(screenrow, pixelcolor, transspec, screencol);
#endif
#ifdef X11WIN
extern int mf_x11_initscreen P1H(void);
extern void mf_x11_updatescreen P1H(void);
extern void mf_x11_blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void mf_x11_paintrow P4H(screenrow, pixelcolor, transspec, screencol);
#endif
extern int mf_trap_initscreen P1H(void);
extern void mf_trap_updatescreen P1H(void);
extern void mf_trap_blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void mf_trap_paintrow P4H(screenrow, pixelcolor, transspec, screencol);


/* This variable, `mfwsw', contains the dispatch tables for each
   terminal.  We map the Pascal calls to the routines `init_screen',
   `update_screen', `blank_rectangle', and `paint_row' into the
   appropriate entry point for the specific terminal that MF is being
   run on.  */

struct mfwin_sw
{
  char *mfwsw_type;		/* Name of terminal a la TERMCAP.  */
  int (*mfwsw_initscreen) P1H(void);
  void (*mfwsw_updatescrn) P1H(void);
  void (*mfwsw_blankrect) P4H(screencol, screencol, screenrow, screenrow);
  void (*mfwsw_paintrow) P4H(screenrow, pixelcolor, transspec, screencol);
} mfwsw[] =
{
#ifdef AMIGAWIN
  { "amiterm", mf_amiga_initscreen, mf_amiga_updatescreen,
    mf_amiga_blankrectangle, mf_amiga_paintrow },
#endif
#ifdef EPSFWIN
  { "epsf", mf_epsf_initscreen, mf_epsf_updatescreen, 
    mf_epsf_blankrectangle, mf_epsf_paintrow },
#endif
#ifdef HP2627WIN
  { "hp2627", mf_hp2627_initscreen, mf_hp2627_updatescreen,
    mf_hp2627_blankrectangle, mf_hp2627_paintrow },
#endif
#ifdef MFTALKWIN
  { "mftalk", mf_mftalk_initscreen, mf_mftalk_updatescreen, 
     mf_mftalk_blankrectangle, mf_mftalk_paintrow },
#endif
#ifdef NEXTWIN
  { "next", mf_next_initscreen, mf_next_updatescreen,
    mf_next_blankrectangle, mf_next_paintrow },
#endif
#ifdef REGISWIN
  { "regis", mf_regis_initscreen, mf_regis_updatescreen,
    mf_regis_blankrectangle, mf_regis_paintrow },
#endif
#ifdef SUNWIN
  { "sun", mf_sun_initscreen, mf_sun_updatescreen,
    mf_sun_blankrectangle, mf_sun_paintrow },
#endif
#ifdef TEKTRONIXWIN
  { "tek", mf_tektronix_initscreen, mf_tektronix_updatescreen,
    mf_tektronix_blankrectangle, mf_tektronix_paintrow },
#endif
#ifdef UNITERMWIN
   { "uniterm", mf_uniterm_initscreen, mf_uniterm_updatescreen,
     mf_uniterm_blankrectangle, mf_uniterm_paintrow },
#endif
#ifdef WIN32WIN
  { "win32term", mf_win32_initscreen, mf_win32_updatescreen, 
    mf_win32_blankrectangle, mf_win32_paintrow },
#endif
#ifdef X11WIN
  { "xterm", mf_x11_initscreen, mf_x11_updatescreen, 
    mf_x11_blankrectangle, mf_x11_paintrow },
#endif
  
  /* Always support this.  */
  { "trap", mf_trap_initscreen, mf_trap_updatescreen,
    mf_trap_blankrectangle, mf_trap_paintrow },

/* Finally, we must have an entry with a terminal type of NULL.  */
  { NULL, NULL, NULL, NULL, NULL }

}; /* End of the array initialization.  */


/* This is a pointer to the mfwsw[] entry that we find.  */
static struct mfwin_sw *mfwp;


/* The following are routines that just jump to the correct
   terminal-specific graphics code. If none of the routines in the
   dispatch table exist, or they fail, we produce trap-compatible
   output, i.e., the same words and punctuation that the unchanged
   mf.web would produce.  */


/* This returns true if we can do window operations, else false.  */

boolean
initscreen P1H(void)
{
  /* If MFTERM is set, use it.  */
  const_string tty_type = kpse_var_value ("MFTERM");
  
  if (tty_type == NULL)
    { 
#if defined (AMIGA)
      tty_type = "amiterm";
#elif defined (WIN32)
      tty_type = "win32term";
#elif defined (OS2) || defined (__DJGPP__) /* not AMIGA nor WIN32 */
      tty_type = "mftalk";
#else /* not (OS2 or WIN32 or __DJGPP__ or AMIGA) */
      /* If DISPLAY is set, we are X11; otherwise, who knows.  */
      boolean have_display = getenv ("DISPLAY") != NULL;
      tty_type = have_display ? "xterm" : getenv ("TERM");

      /* If we don't know what kind of terminal this is, or if Metafont
         isn't being run interactively, don't do any online output.  */
      if (tty_type == NULL
          || (!STREQ (tty_type, "trap") && !isatty (fileno (stdout))))
        return 0;
#endif /* not (OS2 or WIN32 or __DJGPP__ or AMIGA) */
  }

  /* Test each of the terminals given in `mfwsw' against the terminal
     type, and take the first one that matches, or if the user is running
     under Emacs, the first one.  */
  for (mfwp = mfwsw; mfwp->mfwsw_type != NULL; mfwp++) {
    if (!strncmp (mfwp->mfwsw_type, tty_type, strlen (mfwp->mfwsw_type))
	|| STREQ (tty_type, "emacs"))
      if (mfwp->mfwsw_initscreen)
	return ((*mfwp->mfwsw_initscreen) ());
      else {
        fprintf (stderr, "mf: Couldn't initialize online display for `%s'.\n",
                 tty_type);
        break;
      }
  }
  
  /* We disable X support by default, since most sites don't use it, and
     variations in X configurations seem impossible to overcome
     automatically. Too frustrating for everyone involved.  */
  if (STREQ (tty_type, "xterm")) {
    fputs ("\nmf: Window support for X was not compiled into this binary.\n",
           stderr);
    fputs ("mf: There may be a binary called `mfw' on your system which\n",
           stderr);
    fputs ("mf: does contain X window support.\n\n", stderr);
    fputs ("mf: If you need to recompile, remember to give the --with-x\n",
           stderr);
    fputs ("mf: option to configure\n\n", stderr);
    fputs ("mf: (Or perhaps you just failed to specify the mode.)\n", stderr);
  }

  /* The current terminal type wasn't found in any of the entries, or
     initalization failed, so silently give up, assuming that the user
     isn't on a terminal that supports graphic output.  */
  return 0;
}


/* Make sure everything is visible.  */

void
updatescreen P1H(void)
{
  if (mfwp->mfwsw_updatescrn)
    (*mfwp->mfwsw_updatescrn) ();
}


/* This sets the rectangle bounded by ([left,right], [top,bottom]) to
   the background color.  */

void
blankrectangle P4C(screencol, left, screencol, right,
                   screenrow, top, screenrow, bottom)
{
  if (mfwp->mfwsw_blankrect)
    (*mfwp->mfwsw_blankrect) (left, right, top, bottom);
}


/* This paints ROW, starting with the color INIT_COLOR. 
   TRANSITION_VECTOR then specifies the length of the run; then we
   switch colors.  This goes on for VECTOR_SIZE transitions.  */

void
paintrow P4C(screenrow, row, pixelcolor, init_color,
             transspec, transition_vector, screencol, vector_size)
{
  if (mfwp->mfwsw_paintrow)
    (*mfwp->mfwsw_paintrow) (row, init_color, transition_vector, vector_size);
}
#endif /* MF */
