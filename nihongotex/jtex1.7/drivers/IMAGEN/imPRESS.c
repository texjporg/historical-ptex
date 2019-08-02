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
 * imPRESS.c - decode dvi codes to imPRESS codes.
 */

/*
 * $Id: imPRESS.c,v 1.27 90/03/27 11:02:27 void Exp $
 */

#include <stdio.h>
#include <imagen/imPcodes.h>
#include "config.h"
#include "fio.h"
#include "dvicom.h"
#include "pdl.h"
#include "stack.h"
#include "font.h"

#define CurrentNode	font_node[CurrentFont]

extern FILE	*out;
extern FILE	*dvi;

int TopMargin, LeftMargin;

short family = 10; /* hidden */
short member = 0;  /* hidden */

hidden Sig4Byte	H_System = -1;
hidden Sig4Byte	V_System = -1;

#if TATE
hidden Sig1Byte D_System = YOKO_DIR;
extern GLYPH_DATA gf_rot;
#define TATE_imPRESS 0x02
#define YOKO_imPRESS 0x00
#endif

hidden Sig4Byte	CurrentFont;

extern double	conv;
extern STACK	Curr;
extern int  use_landscapemode;
extern int  use_top_margin;
extern int  use_left_margin;
extern NewFontFileInfo font_node[];

extern char	*(*GetFontGlyph)();
extern Sig4Byte	SPtoDEV();
extern		Push(), Pop();


hidden short
DownBGLY(c, num)
int	c;
int	num;
{
    register short    no;
    register NewFontFileInfo	*node = &font_node[num];
    register GLYPH_DATA		*glyph;

#if TATE
    if (use_landscapemode == YES || Curr.Direction == TATE_DIR) {
	no = (1 << 14) | (family << 7) | member;
    	if (++member >= 128) {
	    member = 0;
	    family++;
    	}
    	glyph = &(node->GlyphData[c]);
    	(void) Put1Byte(iBGLY, out);
    	(void) Put2Byte(no, out);
    	(void) Put2Byte((Sig2Byte)SPtoDEV(node->TFMwidth[c]), out);
    	(void) Put2Byte(gf_rot.width, out);
    	(void) Put2Byte(gf_rot.xoffset, out);
    	(void) Put2Byte(gf_rot.height, out);
	(void) Put2Byte(gf_rot.yoffset, out);

	return (no);
    }
    else {
	no = 0 | (family << 7) | member;
    	if (++member >= 128) {
	    member = 0;
	    family++;
    	}
    	glyph = &(node->GlyphData[c]);
    	(void) Put1Byte(iBGLY, out);
    	(void) Put2Byte(no, out);
    	(void) Put2Byte((Sig2Byte)SPtoDEV(node->TFMwidth[c]), out);
    	(void) Put2Byte(glyph->width, out);
    	(void) Put2Byte(glyph->xoffset, out);
    	(void) Put2Byte(glyph->height, out);
	(void) Put2Byte(glyph->yoffset, out);

	return (no);
    }
#else
    no = (use_landscapemode ? (1 << 14) : 0) | (family << 7) | member;

    if (++member >= 128) {
	member = 0;
	family++;
    }
    glyph = &(node->GlyphData[c]);
    (void) Put1Byte(iBGLY, out);	    /* imPRESS @BGLY */
    (void) Put2Byte(no, out);		    /* rotation, family, member  */
    (void) Put2Byte((Sig2Byte)SPtoDEV(node->TFMwidth[c]), out);/* advance */
    (void) Put2Byte(glyph->width, out);	    /* width  */
    (void) Put2Byte(glyph->xoffset, out);   /* left offset */
    (void) Put2Byte(glyph->height, out);    /* height */
    (void) Put2Byte(glyph->yoffset, out);   /* top offset */

    return (no);
#endif
}

hidden
imFAMILY(c)
int	c;
{
    static int	    old_family = -1;
    register int    cur_family, cur_member;

    imPOSITION();

    cur_family = (c >> 7) & 0177;
    cur_member = c & 0177;

    if (cur_family != old_family) {
	(void) Put1Byte(iSET_FAMILY, out);
	(void) Put1Byte(cur_family,  out);
	old_family = cur_family;
    }
    (void) Put1Byte(cur_member, out);		/* LIT */
}

hidden short
imGLYPH(i, num, map)
int	i;
Sig4Byte    num;
char	    *map;
{
    register NewFontFileInfo	*node = &font_node[num];
    register short    n;

#if TATE
    if (CurrentNode.pxl_data.pxl_id == PXLID)
	n = ((short (*)[2])(node->DownLoad))[i][Curr.Direction] = DownBGLY(i, num);
    else
	n = node->DownLoad[i] = DownBGLY(i, num);
#else
    n = node->DownLoad[i] = DownBGLY(i, num);
#endif

#if TATE
    if (use_landscapemode == YES || Curr.Direction == TATE_DIR) {
    	(void) fwrite(map, sizeof(char),
		 (RoundUp(gf_rot.width, 8) >> 3) * gf_rot.height, out);
    }
    else {
    	(void) fwrite(map, sizeof(char),
		 (RoundUp(node->GlyphData[i].width, 8) >> 3) *
		  node->GlyphData[i].height, out);
    }
#else
    (void) fwrite(map, sizeof(char),
		 (RoundUp(node->GlyphData[i].width, 8) >> 3) *
		  node->GlyphData[i].height, out);
#endif
    return (n);
}

hidden
imPOSITION()
{
    register Sig4Byte	h = Curr.Horizontal;
    register Sig4Byte	v = Curr.Vertical;

#if TATE
    register Sig1Byte   d = Curr.Direction;

    if (D_System != d) {
	(void) Put1Byte(iSET_ADV_DIRS, out);

	if (d == TATE_DIR)
	    (void) Put1Byte(TATE_imPRESS, out);
	else
	    (void) Put1Byte(YOKO_imPRESS, out);

	D_System = d;

	(void) Put1Byte(iABS_H, out);
	(void) Put2Byte((Sig2Byte)(SPtoDEV(h) + LeftMargin), out);
	H_System = h;

	(void) Put1Byte(iABS_V, out);
	(void) Put2Byte((Sig2Byte)(SPtoDEV(v) + TopMargin), out);
	V_System = v;
    }

#endif

    if (H_System != h) {
	(void) Put1Byte(iABS_H, out);
	(void) Put2Byte((Sig2Byte)(SPtoDEV(h) + LeftMargin), out);
	H_System = h;
    }
    if (V_System != v) {
	(void) Put1Byte(iABS_V, out);
	(void) Put2Byte((Sig2Byte)(SPtoDEV(v) + TopMargin), out);
	V_System = v;
    }
}

hidden
SetUp_Job(name)
char *name;
{
    TopMargin = use_top_margin;
    LeftMargin = use_left_margin;

    (void) fprintf(out, "@document(language imPRESS, name \"%s\")", name);

    if (use_landscapemode) {
	(void) Put1Byte(iSET_HV_SYSTEM, out);
	(void) Put1Byte(0x55, out);	/* Origin = ulc, h:v = 90, h:x = 90 */
	(void) Put1Byte(iSET_ADV_DIRS, out);
	(void) Put1Byte(0x00, out);	/* main = 0(degrees), secondary = 90 */
    }
}

hidden
EndUp_Job()
{
    /* End of Document */
    (void) Put1Byte(iEOF, out);
}

hidden
SetUp_Page()
{
    H_System = V_System = -1;
#if TATE
    D_System = YOKO_DIR;
#endif
    (void) Put1Byte(iPAGE, out);
}

hidden
EndUp_Page()
{
    /* Print the current Page */
    (void) Put1Byte(iENDPAGE, out);
}

hidden int
EndUpOfPage(c)
int c;
{
    /* Print the current Page */
    (void) Put1Byte(iENDPAGE, out);
    return(c);
}

hidden
FntNum_cmd(c)
int	c;
{
    CurrentFont = c - FNT_NUM_0; /* 171 */
}

hidden int
Set1_cmd(c)
int	c;
{
    char *map;
    register int    set_byte;
    register short    dl;
    register NewFontFileInfo	*node = &CurrentNode;

    set_byte = Get1Byte(dvi);

#if TATE
    if ((dl = ((short (*)[2])(node->DownLoad))[set_byte][Curr.Direction]) == UNLOAD) {
#else
    if ((dl = node->DownLoad[set_byte]) == UNLOAD) {
#endif
	map = (*GetFontGlyph) (set_byte, CurrentFont);
	dl = imGLYPH(set_byte, CurrentFont, map);
	(void) free(map);
    }
    imFAMILY(dl);

#if TATE
    if (Curr.Direction == TATE_DIR) {
	Curr.Vertical += node->TFMwidth[set_byte];
	V_System = Curr.Vertical;
    }
    else {
	Curr.Horizontal += node->TFMwidth[set_byte];
	H_System = Curr.Horizontal;
    }
#else
    H_System = (Curr.Horizontal += node->TFMwidth[set_byte]);
#endif
    return (c);
}

/* ARGSUSED */
hidden int
Set2_cmd(c)	/* JIS Kanji Handling Routine, ASCII version */
int	c;
{
    register UnSig2Byte kanji;
    register int suffix;
    char *map;
    register short    dl;
    register NewFontFileInfo	*node = &CurrentNode;

    kanji = GetUs2Byte(dvi) - 0x2121;

    suffix = (CharMask(kanji >> 8) * 94) + CharMask(kanji);

    if ((dl = node->DownLoad[suffix]) == UNLOAD) {
        map = (*GetFontGlyph) (suffix, CurrentFont);
	dl = imGLYPH(suffix, CurrentFont, map);
	(void) free(map);
    }
    imFAMILY(dl);

#if TATE
    if (Curr.Direction == TATE_DIR) {
	Curr.Vertical += node->TFMwidth[suffix];
	V_System = Curr.Vertical;
    }
    else {
	Curr.Horizontal += node->TFMwidth[suffix];
	H_System = Curr.Horizontal;
    }
#else
    H_System = (Curr.Horizontal += node->TFMwidth[suffix]);
#endif
    return (c);
}

/* ARGSUSED */
hidden int
Set3_cmd(c)
int	c;
{
    Set_Parse((Sig4Byte) Get3Byte(dvi));
    return (c);
}

/* ARGSUSED */
hidden int
Set4_cmd(c)
int	c;
{
    Set_Parse((Sig4Byte) Get4Byte(dvi));
    return (c);
}

hidden
Set_Parse(byte)
register Sig4Byte   byte;
{
    char *map;
    register short    dl;
    register NewFontFileInfo	*node = &CurrentNode;

    if ((dl = node->DownLoad[(int) byte]) == UNLOAD) {
        map = (*GetFontGlyph) ((int) byte, CurrentFont);
	dl = imGLYPH((int) byte, CurrentFont, map);
	(void) free(map);
    }
    imFAMILY(dl);

#if TATE
    if (Curr.Direction == TATE_DIR) {
	Curr.Vertical += node->TFMwidth[byte];
	V_System = Curr.Vertical;
    }
    else {
	Curr.Horizontal += node->TFMwidth[byte];
	H_System = Curr.Horizontal;
    }
#else
    H_System = (Curr.Horizontal += node->TFMwidth[byte]);
#endif
}

hidden int
Put1_cmd(c)
int	c;
{
    Put_Parse((Sig4Byte) Get1Byte(dvi));
    return (c);
}

hidden int
Put2_cmd(c)
int	c;
{
    Put_Parse((Sig4Byte) Get2Byte(dvi));
    return (c);
}

hidden int
Put3_cmd(c)
int	c;
{
    Put_Parse((Sig4Byte) Get3Byte(dvi));
    return (c);
}

hidden int
Put4_cmd(c)
int	c;
{
    Put_Parse(Get4Byte(dvi));
    return (c);
}

/* ARGSUSED */
hidden
Put_Parse(byte)
Sig4Byte	byte;
{
/* DUMMY */
}

hidden int
Rule_cmd(c)
int	c;
{
    Sig4Byte    Height, Width;
    register Sig4Byte    h, w;

    Height = Get4Byte(dvi);
    Width  = Get4Byte(dvi);

    h = SPtoDEV(Height);
    w = SPtoDEV(Width);

    if ((double) h < (Height * conv))
	h++;
    if ((double) w < (Width  * conv))
	w++;

    imPOSITION();

    switch (c) {
    case PUT_RULE :	/* PUT_RULE doesn't move the reference point. */
	break;
    case SET_RULE :
	Curr.Horizontal += Width;
	break;
    }

#if TATE
    if (Curr.Direction == TATE_DIR) {
	(void) Put1Byte(iBRULE, out);
	(void) Put2Byte((Sig2Byte) h, out);
	(void) Put2Byte((Sig2Byte) w, out);
	(void) Put2Byte((Sig2Byte) 0, out);
    }
    else {
	(void) Put1Byte(iBRULE, out);
	(void) Put2Byte((Sig2Byte) w, out);
	(void) Put2Byte((Sig2Byte) h, out);
	(void) Put2Byte((Sig2Byte) (-h + 1), out);
    }
#else
    (void) Put1Byte(iBRULE, out);
    (void) Put2Byte((Sig2Byte) w, out);		/* width */
    (void) Put2Byte((Sig2Byte) h, out);		/* height */
    (void) Put2Byte((Sig2Byte) (-h + 1), out);	/* top offset */
#endif
    return (c);
}

hidden int
Right1_cmd(c)
int	c;
{
    Right_Parse((Sig4Byte) Get1Byte(dvi));
    return (c);
}

hidden int
Right2_cmd(c)
int	c;
{
    Right_Parse((Sig4Byte) Get2Byte(dvi));
    return (c);
}

hidden int
Right3_cmd(c)
int	c;
{
    Right_Parse((Sig4Byte) Get3Byte(dvi));
    return (c);
}

hidden int
Right4_cmd(c)
int	c;
{
    Right_Parse(Get4Byte(dvi));
    return (c);
}

hidden
Right_Parse(right)
Sig4Byte	right;
{
#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Vertical += right;
    else
	Curr.Horizontal += right;
#else
    Curr.Horizontal += right;
#endif
}

hidden int
W0_cmd(c)
int	c;
{
#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Vertical += Curr.Woffset;
    else
	Curr.Horizontal += Curr.Woffset;
#else
    Curr.Horizontal += Curr.Woffset;
#endif
    return (c);
}

hidden int
W1_cmd(c)
int	c;
{
    W_Parse((Sig4Byte) Get1Byte(dvi));
    return (c);
}

hidden int
W2_cmd(c)
int	c;
{
    W_Parse((Sig4Byte) Get2Byte(dvi));
    return (c);
}

hidden int
W3_cmd(c)
int	c;
{
    W_Parse((Sig4Byte) Get3Byte(dvi));
    return (c);
}

hidden int
W4_cmd(c)
int	c;
{
    W_Parse(Get4Byte(dvi));
    return (c);
}

hidden
W_Parse(byte)
Sig4Byte	byte;
{
    Curr.Woffset = byte;
#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Vertical += byte;
    else
	Curr.Horizontal += byte;
#else
    Curr.Horizontal += byte;
#endif
}

hidden int
X0_cmd(c)
int	c;
{
#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Vertical += Curr.Xoffset;
    else
	Curr.Horizontal += Curr.Xoffset;
#else
    Curr.Horizontal += Curr.Xoffset;
#endif
    return (c);
}

hidden int
X1_cmd(c)
int	c;
{
    X_Parse((Sig4Byte) Get1Byte(dvi));
    return (c);
}

hidden int
X2_cmd(c)
int	c;
{
    X_Parse((Sig4Byte) Get2Byte(dvi));
    return (c);
}

hidden int
X3_cmd(c)
int	c;
{
    X_Parse((Sig4Byte) Get3Byte(dvi));
    return (c);
}

hidden int
X4_cmd(c)
int	c;
{
    X_Parse(Get4Byte(dvi));
    return (c);
}

hidden
X_Parse(byte)
Sig4Byte	byte;
{
    Curr.Xoffset = byte;
#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Vertical += byte;
    else
	Curr.Horizontal += byte;
#else
    Curr.Horizontal += byte;
#endif
}

hidden int
Y0_cmd(c)
int	c;
{
#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Horizontal -= Curr.Yoffset;
    else
	Curr.Vertical += Curr.Yoffset;
#else
    Curr.Vertical += Curr.Yoffset;
#endif
    return (c);
}

hidden int
Y1_cmd(c)
int	c;
{
    Y_Parse((Sig4Byte) Get1Byte(dvi));
    return (c);
}

hidden int
Y2_cmd(c)
int	c;
{
    Y_Parse((Sig4Byte) Get2Byte(dvi));
    return (c);
}

hidden int
Y3_cmd(c)
int	c;
{
    Y_Parse((Sig4Byte) Get3Byte(dvi));
    return (c);
}

hidden int
Y4_cmd(c)
int	c;
{
    Y_Parse(Get4Byte(dvi));
    return (c);
}

hidden
Y_Parse(byte)
Sig4Byte	byte;
{
    Curr.Yoffset = byte;
#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Horizontal -= byte;
    else
	Curr.Vertical += byte;
#else
    Curr.Vertical += byte;
#endif
}

hidden int
Z0_cmd(c)
int	c;
{
#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Horizontal -= Curr.Zoffset;
    else
	Curr.Vertical += Curr.Zoffset;
#else
    Curr.Vertical += Curr.Zoffset;
#endif
    return (c);
}

hidden int
Z1_cmd(c)
int	c;
{
    Z_Parse((Sig4Byte) Get1Byte(dvi));
    return (c);
}

hidden int
Z2_cmd(c)
int	c;
{
    Z_Parse((Sig4Byte) Get2Byte(dvi));
    return (c);
}

hidden int
Z3_cmd(c)
int	c;
{
    Z_Parse((Sig4Byte) Get3Byte(dvi));
    return (c);
}

hidden int
Z4_cmd(c)
int	c;
{
    Z_Parse(Get4Byte(dvi));
    return (c);
}

hidden
Z_Parse(byte)
Sig4Byte	byte;
{
    Curr.Zoffset = byte;
#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Horizontal -= byte;
    else
	Curr.Vertical += byte;
#else
    Curr.Vertical += byte;
#endif
}

hidden int
Down1_cmd(c)
int	c;
{
    Down_Parse((Sig4Byte) Get1Byte(dvi));
    return (c);
}

hidden int
Down2_cmd(c)
int	c;
{
    Down_Parse((Sig4Byte) Get2Byte(dvi));
    return (c);
}

hidden int
Down3_cmd(c)
int	c;
{
    Down_Parse((Sig4Byte) Get3Byte(dvi));
    return (c);
}

hidden int
Down4_cmd(c)
int	c;
{
    Down_Parse(Get4Byte(dvi));
    return (c);
}

hidden
Down_Parse(byte)
Sig4Byte	byte;
{
#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Horizontal -= byte;
    else
	Curr.Vertical += byte;
#else
    Curr.Vertical += byte;
#endif
}


/*
 * This is the Special commands. `\special{...}'
 * The handling & Specification of this commands is not specified yet.
 * So, We've to skip this command.
 */

hidden int
XXX1_cmd(c)
int	c;
{
    XXX_Parse((Sig4Byte) Get1Byte(dvi));
    return (c);
}

hidden int
XXX2_cmd(c)
int	c;
{
    XXX_Parse((Sig4Byte) Get2Byte(dvi));
    return (c);
}

hidden int
XXX3_cmd(c)
int	c;
{
    XXX_Parse((Sig4Byte) Get3Byte(dvi));
    return (c);
}

hidden int
XXX4_cmd(c)
int	c;
{
    XXX_Parse(Get4Byte(dvi));
    return (c);
}

hidden
XXX_Parse(i)
Sig4Byte	i;
{
    char    *command;
    char    *GetString();
    char    buf[BUFSIZ];
    register int    count, fd;

#if TPIC
	extern  special_com();
#endif

    /*
     * Get command option as strings.
     */
    command = GetString(dvi, i);
	strcpy(buf, command);

#if TPIC
    imPOSITION();
	special_com(buf);
#else
    if ((fd = open(command, READMODE)) == -1) {
	(void) fprintf(stderr, "! Can't open the (%s) file for \\special...ignore\n", command);
	return;
    }

    while ((count = read(fd, buf, BUFSIZ)) > 0)
	(void) fwrite(buf, sizeof(char), count, out);

    (void) close(fd);
#endif

    /*
     * This is Dummy Routine.
     */
}

hidden int
Set_Char(c)
register int	c;
{
    char *map;
    register short    dl;
    register NewFontFileInfo	*node = &CurrentNode;

#if TATE
    if ((dl = ((short (*)[2])(node->DownLoad))[c][Curr.Direction]) == UNLOAD) {
#else
    if ((dl = node->DownLoad[c]) == UNLOAD) {
#endif
        map = (*GetFontGlyph) (c, CurrentFont);
	dl = imGLYPH(c, CurrentFont, map);
	(void) free(map);
    }
    imFAMILY(dl);

#if TATE
    if (Curr.Direction == TATE_DIR) {
	Curr.Vertical += node->TFMwidth[c];
	V_System = Curr.Vertical;
    }
    else {
	Curr.Horizontal += node->TFMwidth[c];
	H_System = Curr.Horizontal;
    }
#else
    H_System = (Curr.Horizontal += node->TFMwidth[c]);
#endif
    return (c);
}

hidden int
Fnt1_cmd(c)
int	c;
{
    Fnt_Parse((Sig4Byte) Get1Byte(dvi));
    return (c);
}

hidden int
Fnt2_cmd(c)
int	c;
{
    Fnt_Parse((Sig4Byte) Get2Byte(dvi));
    return (c);
}

hidden int
Fnt3_cmd(c)
int	c;
{
    Fnt_Parse((Sig4Byte) Get3Byte(dvi));
    return (c);
}

hidden int
Fnt4_cmd(c)
int	c;
{
    Fnt_Parse(Get4Byte(dvi));
    return (c);
}

hidden
Fnt_Parse(byte)
Sig4Byte	byte;
{
    CurrentFont = byte;
}

#if TATE
hidden int
Dir_cmd(c)
int	c;
{
    register int direction;

    direction = Get1Byte(dvi);

    switch (direction) {
    case TATE_DIR:
    case YOKO_DIR:
	Curr.Direction = direction;
	break;
    default:
	error("Illegal direction"); /* exit */
    }
    return (c);
}
#endif

visible
Make_imPRESS(imPRESS)
NEW_PDL *imPRESS;
{
    imPRESS->SetChar = Set_Char;
    imPRESS->Set1 = Set1_cmd;
    imPRESS->Set2 = Set2_cmd;
    imPRESS->Set3 = Set3_cmd;
    imPRESS->Set4 = Set4_cmd;
    imPRESS->SetRule = Rule_cmd;
    imPRESS->Put1 = Put1_cmd;
    imPRESS->Put2 = Put2_cmd;
    imPRESS->Put3 = Put3_cmd;
    imPRESS->Put4 = Put4_cmd;
    imPRESS->PutRule = Rule_cmd;
    imPRESS->Push = Push;
    imPRESS->Pop = Pop;
    imPRESS->Right1 = Right1_cmd;
    imPRESS->Right2 = Right2_cmd;
    imPRESS->Right3 = Right3_cmd;
    imPRESS->Right4 = Right4_cmd;
    imPRESS->W0 = W0_cmd;
    imPRESS->W1 = W1_cmd;
    imPRESS->W2 = W2_cmd;
    imPRESS->W3 = W3_cmd;
    imPRESS->W4 = W4_cmd;
    imPRESS->X0 = X0_cmd;
    imPRESS->X1 = X1_cmd;
    imPRESS->X2 = X2_cmd;
    imPRESS->X3 = X3_cmd;
    imPRESS->X4 = X4_cmd;
    imPRESS->Y0 = Y0_cmd;
    imPRESS->Y1 = Y1_cmd;
    imPRESS->Y2 = Y2_cmd;
    imPRESS->Y3 = Y3_cmd;
    imPRESS->Y4 = Y4_cmd;
    imPRESS->Z0 = Z0_cmd;
    imPRESS->Z1 = Z1_cmd;
    imPRESS->Z2 = Z2_cmd;
    imPRESS->Z3 = Z3_cmd;
    imPRESS->Z4 = Z4_cmd;
    imPRESS->Down1 = Down1_cmd;
    imPRESS->Down2 = Down2_cmd;
    imPRESS->Down3 = Down3_cmd;
    imPRESS->Down4 = Down4_cmd;
    imPRESS->FntNum = FntNum_cmd;
    imPRESS->Fnt1 = Fnt1_cmd;
    imPRESS->Fnt2 = Fnt2_cmd;
    imPRESS->Fnt3 = Fnt3_cmd;
    imPRESS->Fnt4 = Fnt4_cmd;
    imPRESS->XXX1 = XXX1_cmd;
    imPRESS->XXX2 = XXX2_cmd;
    imPRESS->XXX3 = XXX3_cmd;
    imPRESS->XXX4 = XXX4_cmd;

#if TATE
    imPRESS->Dir  = Dir_cmd;
#endif

    imPRESS->SetUpJob = SetUp_Job;
    imPRESS->EndUpJob = EndUp_Job;
    imPRESS->SetUpPage = SetUp_Page;
    imPRESS->EndUpPage = EndUp_Page;
    imPRESS->Eop = EndUpOfPage;
}
