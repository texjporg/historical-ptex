#include "config.h"
#include "kanji.h"

/*** lib/usage.c:  Output a help message (from help.h). ***/
void
usage P2C(int, status,  const_string, str)
{
  extern string this_bug_address;  /* from maintain.h */
  if (status == 0) {
    fputs (str, stdout);
    putchar ('\n');
    printf("Email bug reports to %s.\n", this_bug_address);
  } else {
    fprintf (stderr, "Try `%s --help' for more information.\n", str);
  }
  uexit (status);
}

/*** lib/printversion.c:  Output for the option --version. ***/
void
printversionandexit P3C(const_string, banner,
                        const_string, copyright_holder,  const_string, author)
{
  extern const_string local_maintainer;   /* from maintain.h */
  extern DllImport string versionstring;  /* from web2c/lib/version.c */
  extern DllImport string kpathsea_version_string;/* from kpathsea/version.c */
  string prog_name, prog_version;
  string based_prog_name, based_prog_version;
  string work_banner, token_start, token_end;
  unsigned len;

#if TERM_CODE == JIS
  string term_code="JIS";
#elif TERM_CODE == SJIS
  string term_code="SJIS";
#else
  string term_code="EUC";
#endif

  len = strlen(banner);
  work_banner = xmalloc(len + 1);
  strcpy(work_banner, banner);
  work_banner[len+1] = 0;

  token_start = work_banner;
  token_end = strchr(token_start, ',');
  len = token_end - token_start - sizeof("This is");
  prog_name = xmalloc(len + 1);
  strncpy(prog_name, token_start + sizeof("This is"), len);
  prog_name[len] = 0;

  token_start = token_end + sizeof(" ");
  token_end = strchr(token_start, ',');
  len = token_end - token_start - sizeof("Version");
  prog_version = xmalloc(len + 1);
  strncpy(prog_version, token_start + sizeof("Version"), len);
  prog_version[len] = 0;

  token_start = token_end + sizeof(" ");
  token_end = strchr(token_start, ',');
  len = token_end - token_start - sizeof("based on");
  based_prog_name = xmalloc(len + 1);
  strncpy(based_prog_name, token_start + sizeof("based on"), len);
  based_prog_name[len] = 0;

  token_start = token_end + sizeof(" ");
  token_end = strrchr(token_start, 0);
  len = token_end - token_start - sizeof("Version");
  based_prog_version = xmalloc(len + 1);
  strncpy(based_prog_version, token_start + sizeof("Version"), len);
  based_prog_version[len] = 0;

  /* The Web2c version string starts with a space.  */
  printf ("%s%s %s (%s), based on %s %s\n",
     prog_name, versionstring, prog_version, term_code,
     based_prog_name, based_prog_version);
  puts (kpathsea_version_string);

  if (copyright_holder) {
    printf ("Copyright (C) 1997 %s.\n", copyright_holder);
    if (!author)
      author = copyright_holder;
  }

  puts ("There is NO warranty.  You may redistribute this software");
  fputs ("under the terms of the GNU General Public License", stdout);
  /* DVIcopy is GPL'd, so no additional words needed. */
  if (copyright_holder && !STREQ (based_prog_name, "DVIcopy")) {
    printf ("\nand the %s copyright", prog_name);
  }
  puts (".");
  puts ("For more information about these matters, see the files");
  printf ("named COPYING and the %s source.\n", prog_name);
  printf ("Nihongo l10n maintainer: %s.\n", local_maintainer);

  uexit (0);
}
