#include "config.h"

/*** lib/usage.c:  Output a help message (from help.h). ***/
void
usage P1C(const_string, str)
{
  fprintf (stderr, "Try `%s --help' for more information.\n", str);
  uexit (1);
}

/* Call usage if the program exits by printing the help message.
   MESSAGE is an NULL-terminated array or strings which make up the
   help message.  Each string is printed on a separate line.
   We use arrays instead of a single string to work around compiler
   limitations (sigh).
*/
void
usagehelp P2C(const_string*, message, const_string, bug_email)
{
  if (!bug_email)
    bug_email = "www-ptex@ascii.co.jp";
  while (*message) {
    fprintf(stdout, "%s\n", *message);
    ++message;
  }
  fprintf(stdout, "\nEmail bug reports to %s.\n", bug_email);
  uexit(0);
}
