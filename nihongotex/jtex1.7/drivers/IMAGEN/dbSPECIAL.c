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
 * dbSPECIAL.c - for SPECIAL debugging
 */

/*
 * $Id: dbSPECIAL.c,v 1.2 90/03/27 10:51:38 void Exp $
 */

#include <stdio.h>
#include <imagen/imPcodes.h>
#include "config.h"
#include "fio.h"
#include "dvicom.h"
#include "special.h"

extern FILE	*out;
extern FILE	*dvi;

hidden
db_PEN(s)
char *s;
{
    (void) fprintf(out, "SPECIAL> pn: %s\n", s);
}

hidden
db_PATH(s)
char *s;
{
    (void) fprintf(out, "SPECIAL> pa %s\n", s);
}
hidden
db_DRAW_DOT(s)
char *s;
{
    (void) fprintf(out, "SPECIAL> dd %s\n", s);
}
hidden
db_DRAW_DASH(s)
char *s;
{
    (void) fprintf(out, "SPECIAL> dt: %s\n", s);
}

hidden
db_ARC(s)
char *s;
{
    (void) fprintf(out, "SPECIAL> ar: %s\n", s);
}

hidden
db_SPLINE(s)
char *s;
{
    (void) fprintf(out, "SPECIAL> sp: %s\n", s);
}

hidden
db_SHADE(s)
char *s;
{
    (void) fprintf(out, "SPECIAL> sh: %s\n", s);
}

hidden
db_WHITEN(s)
char *s;
{
    (void) fprintf(out, "SPECIAL> wh: %s\n", s);
}

hidden
db_BLACKEN(s)
char *s;
{
    (void) fprintf(out, "SPECIAL> bk: %s\n", s);
}

hidden
db_TEXTURE(s)
char *s;
{
    (void) fprintf(out, "SPECIAL> tx: %s\n", s);
}

hidden
db_FLUSH(s)
char *s;
{
    (void) fprintf(out, "SPECIAL> fs: %s\n", s);
}

visible
Make_DEBUG_SPECIAL(DEBUG)
NEW_SPECIAL *DEBUG;
{
    DEBUG->pn_com = db_PEN;
    DEBUG->fp_com = db_FLUSH;
    DEBUG->da_com = db_DRAW_DASH;
    DEBUG->dt_com = db_DRAW_DOT;
    DEBUG->pa_com = db_PATH;
    DEBUG->ar_com = db_ARC;
    DEBUG->sp_com = db_SPLINE;
    DEBUG->wh_com = db_WHITEN;
    DEBUG->bk_com = db_BLACKEN;
    DEBUG->sh_com = db_SHADE;
    DEBUG->tx_com = db_TEXTURE;
}
