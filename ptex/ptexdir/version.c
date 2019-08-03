#include "config.h"
#include "kanji.h"

/*** lib/printversion.c:  Output for the option --version. ***/
void
printversionandexit P3C(const_string, banner,
                        const_string, copyright_holder,  const_string, author)
{
  extern string versionstring;  /* from web2c/lib/version.c */
  extern string based_prog_name;
  extern KPSEDLL string kpathsea_version_string;/* from kpathsea/version.c */
  string prog_name, prog_version;
  string work_banner;
  unsigned len;

#if TERM_CODE == JIS
  string term_code=" (JIS)";
#elif TERM_CODE == SJIS
  string term_code=" (SJIS)";
#else
  string term_code=" (EUC)";
#endif

  work_banner = xstrdup(banner);
                         /* attention!:  sizeof(s) = strlen(s) + 1 */
  prog_name = strtok(work_banner, ",") + sizeof("This is ") - 1;
  prog_version = strtok(NULL, ",") + sizeof(" Version ") - 1;

  /* The Web2c version string starts with a space.  */
  printf ("%s%s %s%s\n", prog_name, versionstring, prog_version, term_code);
  puts (kpathsea_version_string);

  if (copyright_holder) {
    printf ("Copyright (C) 1999 %s.\n", copyright_holder);
    if (!author)
      author = copyright_holder;
  }

  puts ("Kpathsea is copyright (C) 1999 Free Software Foundation, Inc.");

  puts ("There is NO warranty.  Redistribution of this software is");
  fputs ("covered by the terms of ", stdout);
  /* DVIcopy is GPL'd, so no additional words needed. */
  if (/*copyright_holder && */!STREQ (prog_name, "DVIcopy")) {
    printf ("both the %s copyright and\n", prog_name);
  }
  puts ("the GNU General Public License.");
  puts ("For more information about these matters, see the files");
  printf ("named COPYING and the %s source.\n", prog_name);
  printf ("Primary author of %s: %s.\n", based_prog_name, author);
  puts ("Kpathsea written by Karl Berry and others.");

  uexit (0);
}
