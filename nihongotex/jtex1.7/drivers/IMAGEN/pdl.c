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
 * pdl.c - Page Description Language Initialize Routine.
 */

/*
 * $Id: pdl.c,v 1.12 90/03/27 11:03:16 void Exp $
 */

#include <stdio.h>
#include "config.h"
#include "pdl.h"

#if TPIC
#include "special.h"
#endif

NEW_PDL PDL;

#if TPIC
NEW_SPECIAL SPECIAL;
#endif

extern int  use_fsck;
extern int  use_silent;

typedef struct pdltype {
    char    *PDLName;
    int	    (*init_PDL)();
#if TPIC
    int	    (*init_SPECIAL)();
#endif
    char    *DRVName;
} PDLTYPE;

/*
 * Driver Independent Function.
 */

extern nop();
extern illegal();
extern undefined();

extern GetFontDef1();
extern GetFontDef2();
extern GetFontDef3();
extern GetFontDef4();

extern SkipFontDef1();
extern SkipFontDef2();
extern SkipFontDef3();
extern SkipFontDef4();

extern CheckFontDef1();
extern CheckFontDef2();
extern CheckFontDef3();
extern CheckFontDef4();

/*
 * If you make a New PDL Driver Function, set up as follows.
 */

extern Make_imPRESS();
extern Make_DEBUG();
extern Make_PS();
#if TPIC
extern Make_imPRESS_SPECIAL();
extern Make_DEBUG_SPECIAL();
extern Make_PS_SPECIAL();
#endif
/*
extern Make_LBP();
 */

PDLTYPE PDLTable[] = {
#if TPIC
    { "imPRESS",    Make_imPRESS,   Make_imPRESS_SPECIAL, "imPRESS"},
    { "DEBUG",	    Make_DEBUG,	    Make_DEBUG_SPECIAL,   "DEBUG"},
    { "PS",         Make_PS,        Make_PS_SPECIAL,      "PS"},
#else
    { "imPRESS",    Make_imPRESS,   "imPRESS"},
    { "DEBUG",	    Make_DEBUG,	    "DEBUG"},
    { "PS",	    Make_PS,	    "PS"},
#endif
/*
    { "LBP",	    Make_LBP,	    "LBP"},
 */
#if TPIC
    { NULL,	    NULL,	    NULL,  NULL}
#else
    { NULL,	    NULL,	    NULL}
#endif
};

/*
 * DVI Code Table. Initialize the all 256 CODE, including undefined codes.
 */

int ((**DviCode[])()) = {

/*
 * 0--127
 * set_char_[0-127]
 */
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,
    &PDL.SetChar, &PDL.SetChar, &PDL.SetChar,

/*
 * 128--131
 * set[1-4]
 */
    &PDL.Set1, &PDL.Set2, &PDL.Set3, &PDL.Set4,

/*
 * 132
 * set_rule
 */
    &PDL.SetRule,

/*
 * 133--136
 * put[1-4]
 */
    &PDL.Put1, &PDL.Put2, &PDL.Put3, &PDL.Put4,

/*
 * 137
 * put_rule
 */
    &PDL.PutRule,

/*
 * 138
 * nop
 */
    &PDL.Nop,

/*
 * 139
 * bop
 */
    &PDL.Bop,

/*
 * 140
 * eop
 */
    &PDL.Eop,

/*
 * 141--142
 * push, pop
 */ 
    &PDL.Push, &PDL.Pop,

/*
 * 143--146
 * right[1-4]
 */
    &PDL.Right1, &PDL.Right2, &PDL.Right3, &PDL.Right4,

/*
 * 147--151
 * w[0-4]
 */
    &PDL.W0, &PDL.W1, &PDL.W2, &PDL.W3, &PDL.W4,

/*
 * 152--156
 * x[0-4]
 */
    &PDL.X0, &PDL.X1, &PDL.X2, &PDL.X3, &PDL.X4,

/*
 * 157--160
 * down[1-4]
 */
    &PDL.Down1, &PDL.Down2, &PDL.Down3, &PDL.Down4,

/*
 * 161--165
 * y[0-4]
 */
    &PDL.Y0, &PDL.Y1, &PDL.Y2, &PDL.Y3, &PDL.Y4,

/*
 * 166--170
 * z[0-4]
 */
    &PDL.Z0, &PDL.Z1, &PDL.Z2, &PDL.Z3, &PDL.Z4,

/*
 * 171--234
 * fnt_num_[0-63]
 */
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum,
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum,
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum,
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum,
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum,
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum,
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum,
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum,
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum,
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum,
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum,
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum,
    &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, &PDL.FntNum, 

/*
 * 235--238
 * fnt[1-4]
 */
    &PDL.Fnt1, &PDL.Fnt2, &PDL.Fnt3, &PDL.Fnt4,

/*
 * 239--242
 * xxx[1-4]
 */
    &PDL.XXX1, &PDL.XXX2, &PDL.XXX3, &PDL.XXX4,

/*
 * 243--246
 * fnt_def[1-4]
 */
    &PDL.FntDef1, &PDL.FntDef2, &PDL.FntDef3, &PDL.FntDef4,

/*
 * 247
 * pre
 */
    &PDL.Preamble,

/*
 * 248
 * post
 */
    &PDL.Postamble,

/*
 * 249
 * post_post
 */
    &PDL.PostPostamble,

/*
 * 250--251
 * begin_reflect, end_reflect
 */

    &PDL.Undefined, &PDL.Undefined,
/*
 * 252--255
 */
    &PDL.Undefined, &PDL.Undefined, &PDL.Undefined
#if TATE
    ,&PDL.Dir
#else
    ,
#endif
};

/*
 * InitPDLTable - Find the Target PDL Name, and Initialize the DviCode table
 *	       for that.
 */
visible
InitPDLTable(target)
char	*target;
{
    PDLTYPE *p;

    MakeIndept();
    for (p = PDLTable; p->PDLName; ++p) {
	if (strcmp(p->PDLName, target) == 0) {
	    (*p->init_PDL)(&PDL);
#if TPIC
	    (*p->init_SPECIAL)(&SPECIAL);
#endif
	    return;
	}
    }
    error("Unknown page description language");	/* exit */
}

/*
 * MakeIndept - Initialize the Device Independent function.
 */
hidden
MakeIndept()
{
    PDL.Nop =  nop;
    PDL.Bop =  illegal;
    PDL.Eop =  illegal;
    PDL.Preamble = illegal;
    PDL.Postamble = illegal;
    PDL.PostPostamble = illegal;
    PDL.Undefined = undefined;
    PDL.Illegal = illegal;

    switch (use_fsck) {
    case 0 :
	PDL.FntDef1 = GetFontDef1;
	PDL.FntDef2 = GetFontDef2;
	PDL.FntDef3 = GetFontDef3;
	PDL.FntDef4 = GetFontDef4;
    break;
    case 1 :
	PDL.FntDef1 = CheckFontDef1; /* nop */
	PDL.FntDef2 = CheckFontDef2;
	PDL.FntDef3 = CheckFontDef3;
	PDL.FntDef4 = CheckFontDef4;
    break;
    case 2 :
	PDL.FntDef1 = SkipFontDef1;
	PDL.FntDef2 = SkipFontDef2;
	PDL.FntDef3 = SkipFontDef3;
	PDL.FntDef4 = SkipFontDef4;
    
    break;
    }
}

visible
StartJob(name)
char	*name;
{
    (*PDL.SetUpJob)(name);
}

visible
LastJob()
{
    (*PDL.EndUpJob)();

    if (!use_silent)
	(void) fprintf(stderr, "\n");
}

visible
SetPage()
{
    (*PDL.SetUpPage)();
}

visible
EndPage()
{
    (*PDL.EndUpPage)();
}
