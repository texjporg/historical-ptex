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
usagehelp P1C(const_string*, message)
{
    while (*message) {
        fputs(*message, stdout);
        putchar('\n');
        ++message;
    }
    putchar('\n');
	puts("Email bug reports to www-ptex@ascii.co.jp");
    uexit(0);
}
