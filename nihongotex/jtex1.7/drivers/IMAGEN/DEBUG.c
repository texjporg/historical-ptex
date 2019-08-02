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
 * DEBUG.c - decode dvi codes to visible code for debugging.
 */

/*
 * $Id: DEBUG.c,v 1.22 90/03/27 10:50:54 void Exp $
 */

#include <stdio.h>
#include "config.h"
#include "fio.h"
#include "dvicom.h"
#include "pdl.h"
#include "stack.h"
#include "font.h"
#include "amble.h"

hidden Sig4Byte	CurrentFont;
hidden short	family = 10;
hidden short	member = 0;
hidden int	level = 0;

int TopMargin, LeftMargin;

extern FILE	*out;
extern FILE	*dvi;

extern double	conv;
extern STACK	Curr;
#if DECO
extern int	use_landscapemode;
#endif
extern int	use_resolution;
extern Sig2Byte	MaxOfStack;
extern Sig2Byte	TotalPage;
extern Sig4Byte	SizeOfMaxTallPage;
extern Sig4Byte SizeOfMaxWidePage;
extern DIMENSION Post;

extern NewFontFileInfo font_node[];

extern Sig4Byte	SPtoDEV();
extern char	*(*GetFontGlyph)();
extern char	*GetFontByName();

/* ARGSUSED */
hidden
dbGLYPH(i, num, map)
int i;
Sig4Byte num;
char *map;
{
    font_node[num].DownLoad[i] = PrintBGLY(i, num);
#if DUMPFONT
    PrintFont(map, font_node[num].GlyphData[i].width,
	font_node[num].GlyphData[i].height);
#endif
}

#if DUMPFONT
hidden
PrintFont(p, width, height)
register char *p;
register int  width, height;
{
    int i, j;
    int row, column;

    row = height;
    column = (width + 7) / 8;

    for (i = 0; i < row; i++) {
	for (j = 0; j < column; j++)
	    PrintBitMap(p[(i * column) + j]);
	    (void) fprintf(out, "\n");
    }
    (void) fprintf(out, "\n");
}

hidden
PrintBitMap(a)
Sig1Byte a;
{
    register int  i;
    unsigned char mask = (char) 0200;

    for (i = 0; i < 8; i++) {
	if (a & mask)
	    (void) fprintf(out, "@");
	else
	    (void) fprintf(out, ".");
	mask = mask >> 1;
    }
}
#endif

/* ARGSUSED */
hidden int
PrintBGLY(c, num)
int	c;
int	num;
{

    register int    no;

    no = (family << 8) | member;
    if (++member >= 128) {
	member = 0;
	family++;
    }

#if DECO
    (void) fprintf(out, "@BGLY %d %d %d %d% d %d %d %d\n",
    use_landscapemode,                        /* rotation */
    (no >> 8),                            /* family */
    CharMask(no),                         /* member */
    SPtoDEV(font_node[num].TFMwidth[c]),  /* advace */
    font_node[num].GlyphData[c].width,    /* width */
    font_node[num].GlyphData[c].xoffset,  /* left offset */
    font_node[num].GlyphData[c].height,   /* height */
    font_node[num].GlyphData[c].yoffset); /* top offset */
#endif

    return(no);
}

/* ARGSUSED */
hidden
dbFAMILY(c)
int	c;
{
    static int	old_family = UNLOAD;
    register int cur_family;
#if DECO
    register int cur_member;
#endif

    cur_family = c >> 8;
#if DECO
    cur_member = CharMask(c);
#endif

    if (cur_family != old_family) {
#if DECO
	(void) fprintf(out, "@SET_FAMILY %d@", cur_family);
#endif
	old_family = cur_family;
    }
#if DECO
    (void) fprintf(out, "@LIT %d@\n", cur_member);
#endif
}

/* ARGSUSED */
hidden
SetUp_Job(name)
char *name;
{
    (void) fprintf(out, "DEBUGGING-MODE (ala dvitype):\n");
    (void) fprintf(out, "This document was printed to \"%s\"\n", name);
    (void) fprintf(out, "  resolution = %d pixles per inch\n", use_resolution);
    (void) fprintf(out,
	"  maxv = %d, maxh = %d, maxstackdepth = %d, totalpages = %d\n",
	   SizeOfMaxTallPage, SizeOfMaxWidePage, MaxOfStack, TotalPage);
    (void) fprintf(out,"  numerator/denominator = %d/%d\n", Post.Num, Post.Den);
}

hidden
EndUp_Job()
{
    (void) fprintf(out, "EndOfJob:\n");   /* End of Document */
}

hidden
SetUp_Page()
{
    (void) fprintf(out, "BeginNewPage:\n"); /* Set H and V to 0 */
}

hidden
EndUp_Page()
{
    (void) fprintf(out, "EndOfPage:\n");   /* Print the current Page */
}

hidden
FntNum_cmd(c)
int	c;
{
    CurrentFont = c - 171; /* FNT_NUM_0 */
    (void) fprintf(out, "fntnum%d  current font is %s\n",
	CurrentFont, GetFontByName(CurrentFont));
}

/* ARGSUSED */
hidden
Set1_cmd(c)
int    c;
{
    char *map;
    UnSig1Byte set_byte;

    set_byte = Get1Byte(dvi);

    (void) fprintf(out, "set1 %d\n", set_byte);

    if (font_node[CurrentFont].DownLoad[(int)set_byte] == UNLOAD) {
        map = (*GetFontGlyph)((int)set_byte, CurrentFont);
	dbGLYPH((int)set_byte, CurrentFont, map);
    }

    dbFAMILY(font_node[CurrentFont].DownLoad[(int)set_byte]);

    (void)SPtoDEV(font_node[CurrentFont].TFMwidth[(int)set_byte]);

    Curr.Horizontal += font_node[CurrentFont].TFMwidth[(int)set_byte];

    DumpCoordinate();
}

/* ARGSUSED */
hidden
Set2_cmd(c)
int    c;
{
    UnSig2Byte kanji;
    register int high, low;
    register int suffix;
    char *map;

    kanji = Get2Byte(dvi);

    kanji -= 0x2121;
    high = CharMask(kanji >> 8);
    low  = CharMask(kanji);

    suffix = high * 94 + low;

    if (font_node[CurrentFont].DownLoad[suffix] == UNLOAD) {
        map = (*GetFontGlyph)(suffix, CurrentFont);
	dbGLYPH(suffix, CurrentFont, map);
    }

    dbFAMILY(font_node[CurrentFont].DownLoad[suffix]);

    (void)SPtoDEV(font_node[CurrentFont].TFMwidth[suffix]);

    Curr.Horizontal += font_node[CurrentFont].TFMwidth[suffix];

    (void) fprintf(out, "set2 %d ", suffix);
#if 0
    (void) fprintf(out, "suffix = %d\n", suffix);
    (void) fprintf(out, "font_node[%d].DownLoad[%d] = %d\n",
	CurrentFont, suffix, font_node[CurrentFont].DownLoad[suffix]);
#endif
    DumpCoordinate();
}

/* ARGSUSED */
hidden
Set3_cmd(c)
int	c;
{
    Sig3Byte set_byte;

    set_byte = Get3Byte(dvi);

    (void) fprintf(out, "set3 %d ", (int)set_byte);

    Set_Parse((Sig4Byte) set_byte);
}

/* ARGSUSED */
hidden
Set4_cmd(c)
int	c;
{
    Sig4Byte set_byte;

    set_byte = Get4Byte(dvi);

    (void) fprintf(out, "set4 %d ", (int) set_byte);

    Set_Parse((Sig4Byte) set_byte);
}

hidden
Set_Parse(byte)
Sig4Byte	byte;
{
    char *map;

    if (font_node[CurrentFont].DownLoad[(int)byte] == UNLOAD) {
	map = (*GetFontGlyph)((int)byte, CurrentFont);
	dbGLYPH((int)byte, CurrentFont, map);
    }
    dbFAMILY(font_node[CurrentFont].DownLoad[(int)byte]);

    (void)SPtoDEV(font_node[CurrentFont].TFMwidth[(int)byte]);

    Curr.Horizontal += font_node[CurrentFont].TFMwidth[(int)byte];

    DumpCoordinate();
}

/* ARGSUSED */
hidden
Put1_cmd(c)
int	c;
{
    UnSig1Byte	put_byte;

    put_byte = Get1Byte(dvi);

    (void) fprintf(out, "put1 %d ", put_byte);

    Put_Parse((Sig4Byte) put_byte);
}

/* ARGSUSED */
hidden
Put2_cmd(c)
int	c;
{
    Sig2Byte	put_byte;

    put_byte = Get2Byte(dvi);

    (void) fprintf(out, "put2 %d ", put_byte);

    Put_Parse((Sig4Byte) put_byte);
}

/* ARGSUSED */
hidden
Put3_cmd(c)
int	c;
{
    Sig3Byte	put_byte;

    put_byte = Get3Byte(dvi);

    (void) fprintf(out, "put3 %d ", put_byte);

    Put_Parse((Sig4Byte) put_byte);
}

/* ARGSUSED */
hidden
Put4_cmd(c)
int	c;
{
    Sig4Byte	put_byte;

    put_byte = Get4Byte(dvi);

    (void) fprintf(out, "put4 %d ", put_byte);

    Put_Parse(put_byte);
}

/* ARGSUSED */
hidden
Put_Parse(byte)
Sig4Byte	byte;
{
    DumpCoordinate();
}

hidden
Rule_cmd(c)
int	c;
{
    Sig4Byte    Height, Width;
    register int h, w;

    Height = Get4Byte(dvi);
    Width  = Get4Byte(dvi);

    h = Height * conv;
    w = Width  * conv;

    if ((double)h < (Height * conv))
	h++;
    if ((double)w < (Width  * conv))
	w++;

    switch (c) {
    case PUT_RULE :	/* PUT_RULE doesn't move the reference point. */
	(void) fprintf(out, "put_rule height = %d width = %d\n", Height, Width);
	break;
    case SET_RULE :
	Curr.Horizontal += Width;
	(void) fprintf(out, "set_rule height = %d width = %d\n", Height, Width);
	break;
    }
    DumpCoordinate();
}

hidden
Stack_cmd(c)
int c;
{
    switch (c) {
    case PUSH :
	Push();
	(void) fprintf(out, "push\nlevel %d:", level++);
	break;
    case POP :
	Pop();
	(void) fprintf(out, "pop\nlevel %d:", --level);
	break;
    }
    StackContent();
}

/* ARGSUSED */
hidden
Right1_cmd(c)
int	c;
{
    Sig1Byte  right_byte;

    right_byte = Get1Byte(dvi);

    (void) fprintf(out, "right1 %d\n", right_byte);

    Right_Parse((Sig4Byte) right_byte);
}

/* ARGSUSED */
hidden
Right2_cmd(c)
int	c;
{
    Sig2Byte right_byte;

    right_byte = Get2Byte(dvi);

    (void) fprintf(out, "right2 %d\n", right_byte);

    Right_Parse((Sig4Byte) right_byte);
}

/* ARGSUSED */
hidden
Right3_cmd(c)
int	c;
{
    Sig3Byte  right_byte;

    right_byte = Get3Byte(dvi);

    (void) fprintf(out, "right3 %d\n", right_byte);

    Right_Parse((Sig4Byte) right_byte);
}

/* ARGSUSED */
hidden
Right4_cmd(c)
int	c;
{
    Sig4Byte right_byte;

    right_byte = Get4Byte(dvi);

    (void) fprintf(out, "right4 %d\n", right_byte);

    Right_Parse(right_byte);
}

/* ARGSUSED */
hidden
Right_Parse(right)
Sig4Byte	right;
{
    Curr.Horizontal += right;

    DumpCoordinate();
}

/* ARGSUSED */
hidden
W0_cmd(c)
int	c;
{
    Curr.Horizontal += Curr.Woffset;

    (void) fprintf(out, "w0 %d\n", Curr.Woffset);
    DumpCoordinate();
}

/* ARGSUSED */
hidden
W1_cmd(c)
int	c;
{
    UnSig1Byte	w_byte;

    w_byte = Get1Byte(dvi);

    (void) fprintf(out, "w1 %d\n", w_byte);

    W_Parse((Sig4Byte) w_byte);
}

/* ARGSUSED */
hidden
W2_cmd(c)
int	c;
{
    Sig2Byte	w_byte;

    w_byte = Get2Byte(dvi);

    (void) fprintf(out, "w2 %d\n", w_byte);

    W_Parse((Sig4Byte) w_byte);
}

/* ARGSUSED */
hidden
W3_cmd(c)
int	c;
{
    Sig3Byte	w_byte;

    w_byte = Get3Byte(dvi);

    (void) fprintf(out, "w3 %d\n", w_byte);

    W_Parse((Sig4Byte) w_byte);
}

/* ARGSUSED */
hidden
W4_cmd(c)
int	c;
{
    Sig4Byte	w_byte;

    w_byte = Get4Byte(dvi);

    (void) fprintf(out, "w4 %d\n", w_byte);

    W_Parse(w_byte);
}

hidden
W_Parse(byte)
Sig4Byte	byte;
{
    Curr.Woffset = byte;
    Curr.Horizontal += byte;

    DumpCoordinate();
}

/* ARGSUSED */
hidden
X0_cmd(c)
int	c;
{
    (void) fprintf(out, "x0 %d\n", Curr.Xoffset);

    Curr.Horizontal += Curr.Xoffset;

    DumpCoordinate();
}

/* ARGSUSED */
hidden
X1_cmd(c)
int	c;
{
    UnSig1Byte	x_byte;

    x_byte = Get1Byte(dvi);

    (void) fprintf(out, "x1 %d\n", x_byte);

    X_Parse((Sig4Byte) x_byte);
}

/* ARGSUSED */
hidden
X2_cmd(c)
int	c;
{
    Sig2Byte	x_byte;

    x_byte = Get2Byte(dvi);

    (void) fprintf(out, "x2 %d\n", x_byte);

    X_Parse((Sig4Byte) x_byte);
}

/* ARGSUSED */
hidden
X3_cmd(c)
int	c;
{
    Sig3Byte	x_byte;

    x_byte = Get3Byte(dvi);

    (void) fprintf(out, "x3 %d\n", x_byte);

    X_Parse((Sig4Byte) x_byte);
}

/* ARGSUSED */
hidden
X4_cmd(c)
int	c;
{
    Sig4Byte	x_byte;

    x_byte = Get4Byte(dvi);

    (void) fprintf(out, "x4 %d\n", x_byte);

    X_Parse(x_byte);
}

hidden
X_Parse(byte)
Sig4Byte	byte;
{
    Curr.Xoffset = byte;
    Curr.Horizontal += byte;

    DumpCoordinate();
}

/* ARGSUSED */
hidden
Y0_cmd(c)
int	c;
{
    (void) fprintf(out, "y0 %d\n", Curr.Yoffset);

    Curr.Vertical += Curr.Yoffset;

    DumpCoordinate();
}

/* ARGSUSED */
hidden
Y1_cmd(c)
int	c;
{
    UnSig1Byte	y_byte;

    y_byte = Get1Byte(dvi);

    (void) fprintf(out, "y1 %d\n", y_byte);

    Y_Parse((Sig4Byte) y_byte);
}

/* ARGSUSED */
hidden
Y2_cmd(c)
int	c;
{
    Sig2Byte	y_byte;

    y_byte = Get2Byte(dvi);

    (void) fprintf(out, "y2 %d\n", y_byte);

    Y_Parse((Sig4Byte) y_byte);
}

/* ARGSUSED */
hidden
Y3_cmd(c)
int	c;
{
    Sig3Byte	y_byte;

    y_byte = Get3Byte(dvi);

    (void) fprintf(out, "y3 %d\n", y_byte);

    Y_Parse((Sig4Byte) y_byte);
}

/* ARGSUSED */
hidden
Y4_cmd(c)
int	c;
{
    Sig4Byte	y_byte;

    y_byte = Get4Byte(dvi);

    (void) fprintf(out, "y4 %d\n", y_byte);

    Y_Parse(y_byte);
}

hidden
Y_Parse(byte)
Sig4Byte	byte;
{
    Curr.Yoffset = byte;
    Curr.Vertical += byte;

    DumpCoordinate();
}

/* ARGSUSED */
hidden
Z0_cmd(c)
int	c;
{
    Curr.Vertical += Curr.Zoffset;

    (void) fprintf(out, "z0 %d\n", Curr.Zoffset);
    DumpCoordinate();
}

/* ARGSUSED */
hidden
Z1_cmd(c)
int	c;
{
    UnSig1Byte	z_byte;

    z_byte = Get1Byte(dvi);

    (void) fprintf(out, "z1 %d\n", z_byte);

    Z_Parse((Sig4Byte) z_byte);
}

/* ARGSUSED */
hidden
Z2_cmd(c)
int	c;
{
    Sig2Byte	z_byte;

    z_byte = Get2Byte(dvi);

    (void) fprintf(out, "z2 %d\n", z_byte);

    Z_Parse((Sig4Byte) z_byte);
}

/* ARGSUSED */
hidden
Z3_cmd(c)
int	c;
{
    Sig3Byte	z_byte;

    z_byte = Get3Byte(dvi);

    (void) fprintf(out, "z3 %d\n", z_byte);

    Z_Parse((Sig4Byte) z_byte);
}

/* ARGSUSED */
hidden
Z4_cmd(c)
int	c;
{
    Sig4Byte	z_byte;

    z_byte = Get4Byte(dvi);

    (void) fprintf(out, "z4 %d\n", z_byte);

    Z_Parse(z_byte);
}

hidden
Z_Parse(byte)
Sig4Byte	byte;
{
    Curr.Zoffset = byte;
    Curr.Vertical += byte;

    DumpCoordinate();
}

/* ARGSUSED */
hidden
Down1_cmd(c)
int	c;
{
    Sig1Byte	down_byte;

    down_byte = Get1Byte(dvi);

    (void) fprintf(out, "down1 %d\n", down_byte);

    Down_Parse((Sig4Byte) down_byte);
}

/* ARGSUSED */
hidden
Down2_cmd(c)
int	c;
{
    Sig2Byte	down_byte;

    down_byte = Get2Byte(dvi);

    (void) fprintf(out, "down2 %d\n", down_byte);

    Down_Parse((Sig4Byte) down_byte);
}

/* ARGSUSED */
hidden
Down3_cmd(c)
int	c;
{
    Sig3Byte	down_byte;

    down_byte = Get3Byte(dvi);

    (void) fprintf(out, "down3 %d\n", down_byte);

    Down_Parse((Sig4Byte) down_byte);
}

/* ARGSUSED */
hidden
Down4_cmd(c)
int	c;
{
    Sig4Byte	down_byte;

    down_byte = Get4Byte(dvi);

    (void) fprintf(out, "down4 %d\n", down_byte);

    Down_Parse(down_byte);
}

hidden
Down_Parse(byte)
Sig4Byte	byte;
{

    Curr.Vertical += byte;

    DumpCoordinate();
}

/*
 * This is the Special commands. `\special{...}'
 * The handling & Specification of this commands is not specified yet.
 * So, We've to skip this command.
 */
/* ARGSUSED */
hidden
XXX1_cmd(c)
int	c;
{
    XXX_Parse((Sig4Byte) Get1Byte(dvi));
}

/* ARGSUSED */
hidden
XXX2_cmd(c)
int	c;
{
    XXX_Parse((Sig4Byte) Get2Byte(dvi));
}

/* ARGSUSED */
hidden
XXX3_cmd(c)
int	c;
{
    XXX_Parse((Sig4Byte) Get3Byte(dvi));
}

/* ARGSUSED */
hidden
XXX4_cmd(c)
int	c;
{
    XXX_Parse(Get4Byte(dvi));
}

hidden
XXX_Parse(i)
Sig4Byte	i;
{
    char *command;
    char *GetString();

#if TPIC
    extern special_com();
#endif

    /* Get command option as strings. */
    command = GetString(dvi, i);

    /* We have no ideas to handling the \special command, yet. */

    (void) fprintf(out, "xxx %d ", i);
    (void) fprintf(out, "string = %s\n", command);
#if TPIC
    special_com(command);
#endif
    DumpCoordinate();
}

hidden
Set_Char(c)
register int	c;
{
    char *map;

    if (font_node[CurrentFont].DownLoad[c] == UNLOAD) {
	map = (*GetFontGlyph)(c, CurrentFont);
	dbGLYPH(c, CurrentFont, map);
    }
    dbFAMILY(font_node[CurrentFont].DownLoad[c]);

    (void)SPtoDEV(font_node[CurrentFont].TFMwidth[c]);

    Curr.Horizontal += font_node[CurrentFont].TFMwidth[c];

    (void) fprintf(out, "setchar%d ", c);
    DumpCoordinate();
}

/* ARGSUSED */
hidden
Fnt1_cmd(c)
int	c;
{
    (void) fprintf(out, "fnt1 ");

    Fnt_Parse((Sig4Byte) Get1Byte(dvi));
}

/* ARGSUSED */
hidden
Fnt2_cmd(c)
int	c;
{
    (void) fprintf(out, "fnt2 ");

    Fnt_Parse((Sig4Byte) Get2Byte(dvi));
}

/* ARGSUSED */
hidden
Fnt3_cmd(c)
int	c;
{
    (void) fprintf(out, "fnt3 ");

    Fnt_Parse((Sig4Byte) Get3Byte(dvi));
}

/* ARGSUSED */
hidden
Fnt4_cmd(c)
int	c;
{
    (void) fprintf(out, "fnt4 ");

    Fnt_Parse(Get4Byte(dvi));
}

hidden
Fnt_Parse(byte)
Sig4Byte	byte;
{
    CurrentFont = byte;

    (void) fprintf(out, " current font is %d\n", byte);
}

#if TATE
hidden
Dir_cmd(c)
int	c;
{
    int direction;

    direction = Get1Byte(dvi);

    (void) fprintf(out, "dir = %d: ", direction);

    switch (direction) {
    case TATE_DIR:
	Curr.Direction = TATE_DIR;
	(void) fprintf(out, "tate\n");
	break;
    case YOKO_DIR:
	Curr.Direction = YOKO_DIR;
	(void) fprintf(out, "yoko\n");
	break;
    default:
	(void) fprintf(out, "unkown\n");
    }
}
#endif

visible
Make_DEBUG(DEBUG)
NEW_PDL *DEBUG;
{
    DEBUG->SetChar = Set_Char;
    DEBUG->Set1 = Set1_cmd;
    DEBUG->Set2 = Set2_cmd;
    DEBUG->Set3 = Set3_cmd;
    DEBUG->Set4 = Set4_cmd;
    DEBUG->SetRule = Rule_cmd;
    DEBUG->Put1 = Put1_cmd;
    DEBUG->Put2 = Put2_cmd;
    DEBUG->Put3 = Put3_cmd;
    DEBUG->Put4 = Put4_cmd;
    DEBUG->PutRule = Rule_cmd;
    DEBUG->Push = Stack_cmd;
    DEBUG->Pop = Stack_cmd;
    DEBUG->Right1 = Right1_cmd;
    DEBUG->Right2 = Right2_cmd;
    DEBUG->Right3 = Right3_cmd;
    DEBUG->Right4 = Right4_cmd;
    DEBUG->W0 = W0_cmd;
    DEBUG->W1 = W1_cmd;
    DEBUG->W2 = W2_cmd;
    DEBUG->W3 = W3_cmd;
    DEBUG->W4 = W4_cmd;
    DEBUG->X0 = X0_cmd;
    DEBUG->X1 = X1_cmd;
    DEBUG->X2 = X2_cmd;
    DEBUG->X3 = X3_cmd;
    DEBUG->X4 = X4_cmd;
    DEBUG->Y0 = Y0_cmd;
    DEBUG->Y1 = Y1_cmd;
    DEBUG->Y2 = Y2_cmd;
    DEBUG->Y3 = Y3_cmd;
    DEBUG->Y4 = Y4_cmd;
    DEBUG->Z0 = Z0_cmd;
    DEBUG->Z1 = Z1_cmd;
    DEBUG->Z2 = Z2_cmd;
    DEBUG->Z3 = Z3_cmd;
    DEBUG->Z4 = Z4_cmd;
    DEBUG->Down1 = Down1_cmd;
    DEBUG->Down2 = Down2_cmd;
    DEBUG->Down3 = Down3_cmd;
    DEBUG->Down4 = Down4_cmd;
    DEBUG->FntNum = FntNum_cmd;
    DEBUG->Fnt1 = Fnt1_cmd;
    DEBUG->Fnt2 = Fnt2_cmd;
    DEBUG->Fnt3 = Fnt3_cmd;
    DEBUG->Fnt4 = Fnt4_cmd;
    DEBUG->XXX1 = XXX1_cmd;
    DEBUG->XXX2 = XXX2_cmd;
    DEBUG->XXX3 = XXX3_cmd;
    DEBUG->XXX4 = XXX4_cmd;

#if TATE
    DEBUG->Dir = Dir_cmd;
#endif

    DEBUG->SetUpJob = SetUp_Job;
    DEBUG->EndUpJob = EndUp_Job;
    DEBUG->SetUpPage = SetUp_Page;
    DEBUG->EndUpPage = EndUp_Page;
}

hidden
DumpCoordinate()
{
    hidden Sig4Byte temp1 = 0;
    hidden Sig4Byte temp2 = 0;

    if (Curr.Horizontal != temp1) {
	(void) fprintf(out, "h:=%d", temp1);
	if (Curr.Horizontal - temp1 > 0)
	    (void) fprintf(out, "+");
	(void) fprintf(out, "%d=%d\n", Curr.Horizontal - temp1, Curr.Horizontal);
	temp1 = Curr.Horizontal;
    }
    else if (Curr.Vertical != temp2) {
	(void) fprintf(out, "v:=%d", temp2);
	if (Curr.Vertical - temp2 > 0)
	    (void) fprintf(out, "+");
	(void) fprintf(out, "%d=%d\n", Curr.Vertical - temp2, Curr.Vertical);
	temp2 = Curr.Vertical;
    }
}

hidden
StackContent()
{
#if TATE
    (void) fprintf(out, "(h=%d,v=%d,w=%d,x=%d,y=%d,z=%d,d=%d)\n",
	Curr.Horizontal, Curr.Vertical, Curr.Woffset,
	Curr.Xoffset, Curr.Yoffset, Curr.Zoffset, Curr.Direction);
#else
    (void) fprintf(out, "(h=%d,v=%d,w=%d,x=%d,y=%d,z=%d)\n",
	Curr.Horizontal, Curr.Vertical, Curr.Woffset,
	Curr.Xoffset, Curr.Yoffset, Curr.Zoffset);
#endif
}
