/*
 * Copyright 1988 dit Co., Ltd.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of dit Co., Ltd. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * Dit Co., Ltd. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * DIT CO., LTD. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL DIT CO., LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/*
 * special.c - special
 */

/*
 * $Id: special.c,v 1.2 90/03/27 11:03:32 void Exp $
 */

#include <stdio.h>
#include "config.h"
#include "fio.h"
#include "dvicom.h"
#include "special.h"

NEW_SPECIAL SPECIAL;

typedef struct special_code {
    char    *special_name;
    int	 (**special_routine)();
} SPECIAL_CODE;

SPECIAL_CODE special_table[] = {
    {"pn", &(SPECIAL.pn_com)},
    {"fp", &(SPECIAL.fp_com)},
    {"da", &(SPECIAL.da_com)},
    {"dt", &(SPECIAL.dt_com)},
    {"pa", &(SPECIAL.pa_com)},
    {"ar", &(SPECIAL.ar_com)},
    {"sp", &(SPECIAL.sp_com)},
    {"wh", &(SPECIAL.wh_com)},
    {"bk", &(SPECIAL.bk_com)},
    {"sh", &(SPECIAL.sh_com)},
    {"tx", &(SPECIAL.tx_com)},
    {"imPRESS", &(SPECIAL.fl_com)},
    {NULL, NULL}
};

char *eat_blanks(s)
char *s;
{
    while (*s == BLANK || *s == TAB)
	++s;
    return(s);
}

char *get_token(s, token)
char *s;
char *token;
{
    char *p;
    char *eat_blanks();

    p = s = eat_blanks(s);

    if (*s == EOS)
	return(0);

    while (*s != BLANK && *s != TAB && *s != EOS)
	++s;

    *s = EOS;
    (void) strcpy(token, p);
    return(++s);
}

visible
special_com(token)
char *token;
{
    SPECIAL_CODE   *p;
    char    *s;
    char    *get_token();

    s = token;

    while (s = get_token(s, token)) {
	for (p = special_table; p->special_name; ++p) {
	    if (strcmp(p->special_name, token) == 0) {
		(*p->special_routine)(s);
		return;
	    }
	}
    }
    (void) fprintf(stderr, "! Unknown special-code. ignore (%s)\n", token);
    return;
}
