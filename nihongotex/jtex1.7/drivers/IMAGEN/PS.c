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
 * PS.c - decode dvi codes to PS codes.
 */

/*
 * $Id: PS.c,v 1.3 90/03/27 10:51:13 void Exp $
 */

#include <stdio.h>
#include "config.h"
#include "fio.h"
#include "dvicom.h"
#include "pdl.h"
#include "stack.h"
#include "font.h"

#define MACROFILE         "tex.ps"
#define PS_BOP            "@BeginingOfPage"
#define PS_EOP            "@EndOfPage"
#define PS_START          "@BeginingOfPrinting"
#define PS_SETFONT        "@SetFont"
#define PS_NEWFONT        "@NewFont"
#define PS_DOWNFONT       "@DownBitmap"
#define PS_END            "@EndOfPrinting"
#define PS_RULE           "@Rule"
#define PS_SHOW           "show"
#define PS_MOVETO         "moveto"
#define PS_START_SPECIAL  "@BeginSpecial"
#define PS_END_SPECIAL    "@EndSpecial"
#define PS_SETUP_SPECIAL  "@SetSpecial"

#define READ_OK		4

#if MSDOS
#define DELIMITER	';'			/* Don't specify ':' */
#define IsPathChar(c)	((c) == '/' || (c) == '\\')
#else
#define DELIMITER	':'
#define IsPathChar(c)	((c) == '/')
#endif

#define CurrentNode	font_node[CurrentFont]

extern FILE	*out;
extern FILE	*dvi;
extern char     *StrDup();
extern char     *getenv();
extern char     *index();

int TopMargin, LeftMargin;

hidden short family = 0; /* hidden */
hidden short member = 1; /* hidden */

hidden Sig4Byte	H_System = -1;
hidden Sig4Byte	V_System = -1;

#define FIRST  0
#define SECOND 1
hidden int NewPage = YES;
hidden long BopPos;
hidden int PassLevel = FIRST;

hidden Sig4Byte CurrentFont;

#if TATE
hidden Sig1Byte D_System = YOKO_DIR;
extern GLYPH_DATA gf_rot;
#endif

extern double   conv;
extern STACK    Curr;
extern int      use_landscapemode;
extern int      use_top_margin;
extern int      use_left_margin;
extern int      use_resolution;

extern NewFontFileInfo font_node[];

extern char       *(*GetFontGlyph)();
extern int        (*GetFontIndex)();
extern Sig4Byte   SPtoDEV();
extern            Push(), Pop();
extern            SkipFontDef1();
extern            SkipFontDef2();
extern            SkipFontDef3();
extern            SkipFontDef4();

hidden short
DownBGLY(c, num)
int	c;
int	num;
{
    register short    no;
    register NewFontFileInfo	*node = &font_node[num];
    register GLYPH_DATA		*glyph;

    no = (use_landscapemode ? (1 << 14) : 0) | (family << 7) | member;

    glyph = &(node->GlyphData[c]);

    (void) fprintf(out, "%d ", ((glyph->width + 7) >> 3) << 3);
    (void) fprintf(out, "%d ", glyph->height);
    (void) fprintf(out, "%d ", glyph->xoffset);
    (void) fprintf(out, "%d ", glyph->height - glyph->yoffset - 1);
    (void) fprintf(out, "%d ", (Sig2Byte) SPtoDEV(node->TFMwidth[c]));
    (void) fprintf(out, "] ");
    (void) fprintf(out, "%d ", member);
    (void) fprintf(out, "%s\n", PS_DOWNFONT);

    return (no);
}

hidden
psFAMILY(c)
int	c;
{
    static   int    old_family = -1;
    register int    cur_family, cur_member;

    psPOSITION();

    cur_family = (c >> 7) & 0177;
    cur_member = c & 0177;

    if (cur_family != old_family || NewPage == YES) {
	(void) fprintf(out, "f%d %s\n", cur_family, PS_SETFONT);
	old_family = cur_family;
	NewPage = NO;
    }
    (void) fprintf(out, "(\\%03o) %s\n", cur_member, PS_SHOW);  /* LIT */
}

hidden short
psGLYPH(i, num, map)
int	i;
Sig4Byte    num;
char	    *map;
{
    register NewFontFileInfo	*node = &font_node[num];
    short    n;
    register int x1, x2, x3, x4;
    unsigned char *out_char;

    if (member == 1)
	(void) fprintf(out, "1.000 256 /f%d %s\n", family, PS_NEWFONT);
/*
	(void) fprintf(out, "1.000 128 /f%d %s\n", family, PS_NEWFONT);
*/

    (void) fprintf(out, "f%d %s\n", family, PS_SETFONT);
    (void) fprintf(out, "[<");

    x3 = (RoundUp(node->GlyphData[i].width, 8) >> 3);
    x4 = 1;

#if TATE
    if (Curr.Direction == TATE_DIR)
	x1 = gf_rot.height - 1;
    else
	x1 = node->GlyphData[i].height - 1;
    for (; x1 >= 0; --x1, ++x4) {
#else
    for (x1 = node->GlyphData[i].height - 1; x1 >= 0; --x1, ++x4) {
#endif
	out_char = (unsigned char *) map + (x1 * x3);
	for (x2 = 0; x2 < x3; ++x2, ++out_char) {
	    (void) fprintf(out, "%X", ((*out_char >> 4) & 0xf));
	    (void) fprintf(out, "%X", (*out_char & 0xf));
	}
	if (x4 > 5) {
	    x4 = 0;
	    (void) fprintf(out, "\n");
	    (void) fprintf(out, "  ");
	}
	else
	    (void) fprintf(out, " ");
    }

    (void) fprintf(out, "\n");
    (void) fprintf(out, "> ");

#if TATE
    if (CurrentNode.pxl_data.pxl_id == PXLID)
	n = ((short (*)[2])(node->DownLoad))[i][Curr.Direction] = DownBGLY(i, num);
    else
	n = node->DownLoad[i] = DownBGLY(i, num);
#else
    n = node->DownLoad[i] = DownBGLY(i, num);
#endif

    if (++member >= 128) {
	member = 1;
	family++;
    }

    return (n);
}

hidden
psPOSITION()
{
    register Sig4Byte	h = Curr.Horizontal;
    register Sig4Byte	v = Curr.Vertical;
#if TATE
    register Sig1Byte    d = Curr.Direction;

    if (D_System != d) {
	D_System = d;
	(void) fprintf(out, "%4d ", (Sig2Byte) (SPtoDEV(h)));
	(void) fprintf(out, "%4d ", (Sig2Byte) (SPtoDEV(v)));
	(void) fprintf(out, "%s ", PS_MOVETO);
    }
#endif

    if (H_System != h)
	H_System = h;
    if (V_System != v)
	V_System = v;

    (void) fprintf(out, "%4d ", (Sig2Byte) (SPtoDEV(h)));
    (void) fprintf(out, "%4d ", (Sig2Byte) (SPtoDEV(v)));
    (void) fprintf(out, "%s ", PS_MOVETO);
}

hidden
SetUp_Job(name)
char *name;
{
    FILE *fp;
    char  macrofile[BUFSIZ];
    char  buf[BUFSIZ];
    register char   *env = getenv(MACRODIRENV);
    register char   *p, *lp;

    TopMargin = use_top_margin;
    LeftMargin = use_left_margin;

    (void) fprintf(out, "%%! PS-Adobe-2.0\n");
    (void) fprintf(out, "%%%%Creator: %s\n", name);
    (void) fprintf(out, "%%%%Title: dvi2\n");
    (void) fprintf(out, "%%%%Pages: (atend)\n");
    (void) fprintf(out, "%%%%EndComments\n");

    if (env == NULL)
	(void) sprintf(macrofile, "./%s", MACROFILE);
    else {
	env = StrDup(env);
	for (lp = env; (p = index(lp, DELIMITER)) != NULL; lp = p) {
	    *p++ = EOS;
	    (void) sprintf(macrofile, "%s/%s", lp, MACROFILE);
	    if (access(macrofile, READ_OK) == SUCCESS) {
		(void) free(env);
		goto label;
	    }
	}
	(void) fprintf(stderr, "can't find macro file for PS.\n");
	exit(1);
    }

label:

    if ((fp = fopen(macrofile, "r")) == NULL) {
	(void) fprintf(stderr, "can't open macro file for PS.\n");
	exit(1);
    }

    for (;;) {
	if (fgets(buf, BUFSIZ, fp) == (char *) NULL)
	    break; 
	fputs(buf, out);
    }

    fclose(fp);

    (void) fprintf(out, "\n");
    (void) fprintf(out, "TeXhax@Dictionary begin\n");
    (void) fprintf(out, "/@Resolution %d def\n", use_resolution);
    (void) fprintf(out, "%s\n", PS_START);
}

hidden int
EndUp_Job(byte)
int byte;
{
    (void) fprintf(out, "%s", PS_END);
    return (byte);
}

hidden int
SetUp_Page(byte)
int byte;
{
    register int i;

    H_System = V_System = -1;

    /*
     * (10L * 4L + 4L + 1L) is
     *  page_numbers * 4Byte + 4Bytes counter + 1Byte BOP.
     */

    if (PassLevel == FIRST) {
	BopPos = (long) ftell(dvi) - (10L * 4L + 4L + 1L); /* debug */
    }
    else {
	(void) Get4Byte(dvi);
	for (i = 0; i < 10; ++i)
	    (void) Get4Byte(dvi);
	ResetStack();
	(void) fprintf(out, "%s\n", PS_BOP);
	NewPage = YES;
    }

    return (byte);
}

hidden int
EndUp_Page(byte)
int byte;
{
    /*
     * Print the current Page
     */
    if (PassLevel == FIRST) {
	PassLevel = SECOND;
	(void) fseek(dvi, (long) BopPos, 0);
	return (-1); /* usagi */
    }
    else {
	PassLevel = FIRST;
	(void) fprintf(out, "%s\n", PS_EOP);
	(void) fprintf(out, "%%%%\n");
	return (byte);
    }
}

hidden int
FntNum_cmd(c)
int	c;
{
    CurrentFont = c - FNT_NUM_0; /* 171 */
    return (c);
}

hidden int
Set1_cmd(c)
int	c;
{
    char           *map;
    register int    set_byte;
    register short  dl;
    register NewFontFileInfo *node = &CurrentNode;

    set_byte = Get1Byte(dvi);

    dl = node->DownLoad[set_byte];

    if (PassLevel == SECOND)
	psFAMILY(dl);
    else {
	if (dl == UNLOAD) {
	    map = (*GetFontGlyph)(set_byte, CurrentFont);
	    dl = psGLYPH(set_byte, CurrentFont, map);
	    (void) free(map);
	}
	return (c);
    }

#if TATE
    if (Curr.Direction == TATE_DIR) {
	Curr.Vertical += node->TFMwidth[set_byte];
	V_System = Curr.Vertical;
    } else {
	Curr.Horizontal += node->TFMwidth[set_byte];
	H_System = Curr.Horizontal;
    }
#else
    H_System = (Curr.Horizontal += node->TFMwidth[set_byte]);
#endif
    return (c);
}

hidden int
Set2_cmd(c)	/* JIS Kanji Handling Routine, ASCII version */
int	c;
{
    register UnSig2Byte kanji;
    register int    suffix;
    char           *map;
    register short  dl;
    register NewFontFileInfo *node = &CurrentNode;

    kanji = GetUs2Byte(dvi) - 0x2121;

    suffix = (CharMask(kanji >> 8) * 94) + CharMask(kanji);

    dl = node->DownLoad[suffix];

    if (PassLevel == SECOND)
	psFAMILY(dl);
    else {
	if (dl == UNLOAD) {
	    map = (*GetFontGlyph) (suffix, CurrentFont);
	    dl = psGLYPH(suffix, CurrentFont, map);
	    (void) free(map);
	}
	return (c);
    }

#if TATE
    if (Curr.Direction == TATE_DIR) {
	Curr.Vertical += node->TFMwidth[suffix];
	V_System = Curr.Vertical;
    } else {
	Curr.Horizontal += node->TFMwidth[suffix];
	H_System = Curr.Horizontal;
    }
#else
    H_System = (Curr.Horizontal += node->TFMwidth[suffix]);
#endif
    return (c);
}

hidden int
Set3_cmd(c)
int	c;
{
    Set_Parse((Sig4Byte) Get3Byte(dvi));
    return (c);
}

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
    char           *map;
    register short  dl;
    register NewFontFileInfo *node = &CurrentNode;

    if ((dl = node->DownLoad[(int) byte]) == UNLOAD) {
	map = (*GetFontGlyph) ((int) byte, CurrentFont);
	dl = psGLYPH((int) byte, CurrentFont, map);
	(void) free(map);
    }
    psFAMILY(dl);

#if TATE
    if (Curr.Direction == TATE_DIR) {
	Curr.Vertical += node->TFMwidth[byte];
	V_System = Curr.Vertical;
    } else {
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

hidden int
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

    if (PassLevel == FIRST)
	return(c);

    h = SPtoDEV(Height);
    w = SPtoDEV(Width);

    if ((double) h < (Height * conv))
	h++;
    if ((double) w < (Width  * conv))
	w++;

    psPOSITION();

    switch (c) {
    case PUT_RULE :	/* PUT_RULE doesn't move the reference point. */
	break;
    case SET_RULE :
	Curr.Horizontal += Width;
	break;
    }

    (void) fprintf(out, "%d ", (Sig2Byte) w);		/* width */
    (void) fprintf(out, "%d ", (Sig2Byte) h);		/* height */
    (void) fprintf(out, "%s\n", PS_RULE);
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
    if (PassLevel == FIRST)
	return;

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
    if (PassLevel == FIRST)
	return(c);

#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Vertical += Curr.Woffset;
    else
	Curr.Horizontal += Curr.Woffset;
#else
    Curr.Horizontal += Curr.Woffset;
#endif
    return(c);
}

hidden int
W1_cmd(c)
int	c;
{
    W_Parse((Sig4Byte) Get1Byte(dvi));
    return(c);
}

hidden int
W2_cmd(c)
int	c;
{
    W_Parse((Sig4Byte) Get2Byte(dvi));
    return(c);
}

hidden int
W3_cmd(c)
int	c;
{
    W_Parse((Sig4Byte) Get3Byte(dvi));
    return(c);
}

hidden int
W4_cmd(c)
int	c;
{
    W_Parse(Get4Byte(dvi));
    return(c);
}

hidden
W_Parse(byte)
Sig4Byte	byte;
{
    if (PassLevel == FIRST)
	return;

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
    if (PassLevel == FIRST)
	return(c);

#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Vertical += Curr.Xoffset;
    else
	Curr.Horizontal += Curr.Xoffset;
#else
    Curr.Horizontal += Curr.Xoffset;
#endif
    return(c);
}

hidden int
X1_cmd(c)
int	c;
{
    X_Parse((Sig4Byte) Get1Byte(dvi));
    return(c);
}

hidden int
X2_cmd(c)
int	c;
{
    X_Parse((Sig4Byte) Get2Byte(dvi));
    return(c);
}

hidden int
X3_cmd(c)
int	c;
{
    X_Parse((Sig4Byte) Get3Byte(dvi));
    return(c);
}

hidden int
X4_cmd(c)
int	c;
{
    X_Parse(Get4Byte(dvi));
    return(c);
}

hidden
X_Parse(byte)
Sig4Byte	byte;
{
    if (PassLevel == FIRST)
	return;

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
    if (PassLevel == FIRST)
	return(c);

#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Horizontal -= Curr.Yoffset;
    else
	Curr.Vertical += Curr.Yoffset;
#else
    Curr.Vertical += Curr.Yoffset;
#endif
    return(c);
}

hidden int
Y1_cmd(c)
int	c;
{
    Y_Parse((Sig4Byte) Get1Byte(dvi));
    return(c);
}

hidden int
Y2_cmd(c)
int	c;
{
    Y_Parse((Sig4Byte) Get2Byte(dvi));
    return(c);
}

hidden int
Y3_cmd(c)
int	c;
{
    Y_Parse((Sig4Byte) Get3Byte(dvi));
    return(c);
}

hidden int
Y4_cmd(c)
int	c;
{
    Y_Parse(Get4Byte(dvi));
    return(c);
}

hidden
Y_Parse(byte)
Sig4Byte	byte;
{
    if (PassLevel == FIRST)
	return;

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
    if (PassLevel == FIRST)
	return(c);

#if TATE
    if (Curr.Direction == TATE_DIR)
	Curr.Horizontal -= Curr.Zoffset;
    else
	Curr.Vertical += Curr.Zoffset;
#else
    Curr.Vertical += Curr.Zoffset;
#endif
    return(c);
}

hidden int
Z1_cmd(c)
int	c;
{
    Z_Parse((Sig4Byte) Get1Byte(dvi));
    return(c);
}

hidden int
Z2_cmd(c)
int	c;
{
    Z_Parse((Sig4Byte) Get2Byte(dvi));
    return(c);
}

hidden int
Z3_cmd(c)
int	c;
{
    Z_Parse((Sig4Byte) Get3Byte(dvi));
    return(c);
}

hidden int
Z4_cmd(c)
int	c;
{
    Z_Parse(Get4Byte(dvi));
    return(c);
}

hidden
Z_Parse(byte)
Sig4Byte	byte;
{
    if (PassLevel == FIRST)
	return;

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
    return(c);
}

hidden int
Down2_cmd(c)
int	c;
{
    Down_Parse((Sig4Byte) Get2Byte(dvi));
    return(c);
}

hidden int
Down3_cmd(c)
int	c;
{
    Down_Parse((Sig4Byte) Get3Byte(dvi));
    return(c);
}

hidden int
Down4_cmd(c)
int	c;
{
    Down_Parse(Get4Byte(dvi));
    return(c);
}

hidden
Down_Parse(byte)
Sig4Byte	byte;
{
    if (PassLevel == FIRST)
	return;

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
    return(c);
}

hidden int
XXX2_cmd(c)
int	c;
{
    XXX_Parse((Sig4Byte) Get2Byte(dvi));
    return(c);
}

hidden int
XXX3_cmd(c)
int	c;
{
    XXX_Parse((Sig4Byte) Get3Byte(dvi));
    return(c);
}

hidden int
XXX4_cmd(c)
int	c;
{
    XXX_Parse(Get4Byte(dvi));
    return(c);
}

hidden
XXX_Parse(i)
Sig4Byte	i;
{
    char    *command;
    char    *GetString();
    char    buf[BUFSIZ];
    register int    count, fd;

    /*
     * Get command option as strings.
     */
    command = GetString(dvi, i);

    if (PassLevel == FIRST)
	return;

    strcpy(buf, command);

    if ((fd = open(command, READMODE)) == -1) {
	(void) fprintf(stderr, "! Can't open the (%s) file for \\special...ignore\n", command);
	return;
    }

    (void) fprintf(out, "\n%s\n", PS_START_SPECIAL);
    (void) fprintf(out, "\n%s\n", PS_SETUP_SPECIAL);

    while ((count = read(fd, buf, BUFSIZ)) > 0)
	(void) fwrite(buf, sizeof(char), count, out);

    (void) close(fd);

    (void) fprintf(out, "\n%s\n", PS_END_SPECIAL);
}

hidden int
Set_Char(c)
register int	c;
{
    char           *map;
    register short  dl;
    register NewFontFileInfo *node = &CurrentNode;

#if TATE
    dl = ((short (*)[2]) (node->DownLoad))[c][Curr.Direction];
#else
    dl = node->DownLoad[c];
#endif
    if (PassLevel == SECOND)
	psFAMILY(dl);
    else {
	if (dl == UNLOAD) {
	    map = (*GetFontGlyph) (c, CurrentFont);
	    dl = psGLYPH(c, CurrentFont, map);
	    (void) free(map);
	}
	return (c);
    }

#if TATE
    if (Curr.Direction == TATE_DIR) {
	Curr.Vertical += node->TFMwidth[c];
	V_System = Curr.Vertical;
    } else {
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

hidden int
ComFontDef1(c)
int c;
{
    ComFontDef_Parse(Get1Byte(dvi));
    return (c);
}

hidden int
ComFontDef2(c)
int c;
{
    ComFontDef_Parse(Get2Byte(dvi));
    return (c);
}

hidden int
ComFontDef3(c)
int c;
{
    ComFontDef_Parse(Get3Byte(dvi));
    return (c);
}

hidden int
ComFontDef4(c)
int c;
{
    ComFontDef_Parse(Get4Byte(dvi));
    return (c);
}

hidden
ComFontDef_Parse(def_byte)
Sig4Byte   def_byte;
{
    if (PassLevel == FIRST)
	(*GetFontIndex)(def_byte);
    else
	SkipFontDefinition();
}

hidden
SkipFontDefinition()
{
    register int i;

    for (i = 0; i < 3; ++i)
	(void) Get4Byte(dvi);

    (void) GetString(dvi, (int) (Get1Byte(dvi) + Get1Byte(dvi)));
}

#if TATE
hidden int
Dir_cmd(c)
int     c;
{
    register int direction;

    direction = Get1Byte(dvi);

    if (PassLevel == FIRST)
	return(c);

    switch (direction) {
	case TATE_DIR:
	case YOKO_DIR:
	    Curr.Direction = direction;
	    break;
	default:
	    error("Illegal direction"); /* exit */
    }
}
#endif

visible
Make_PS(PS)
NEW_PDL *PS;
{
    PS->SetChar = Set_Char;
    PS->Set1 = Set1_cmd;
    PS->Set2 = Set2_cmd;
    PS->Set3 = Set3_cmd;
    PS->Set4 = Set4_cmd;
    PS->SetRule = Rule_cmd;
    PS->Put1 = Put1_cmd;
    PS->Put2 = Put2_cmd;
    PS->Put3 = Put3_cmd;
    PS->Put4 = Put4_cmd;
    PS->PutRule = Rule_cmd;
    PS->Push = Push;
    PS->Pop = Pop;
    PS->Right1 = Right1_cmd;
    PS->Right2 = Right2_cmd;
    PS->Right3 = Right3_cmd;
    PS->Right4 = Right4_cmd;
    PS->W0 = W0_cmd;
    PS->W1 = W1_cmd;
    PS->W2 = W2_cmd;
    PS->W3 = W3_cmd;
    PS->W4 = W4_cmd;
    PS->X0 = X0_cmd;
    PS->X1 = X1_cmd;
    PS->X2 = X2_cmd;
    PS->X3 = X3_cmd;
    PS->X4 = X4_cmd;
    PS->Y0 = Y0_cmd;
    PS->Y1 = Y1_cmd;
    PS->Y2 = Y2_cmd;
    PS->Y3 = Y3_cmd;
    PS->Y4 = Y4_cmd;
    PS->Z0 = Z0_cmd;
    PS->Z1 = Z1_cmd;
    PS->Z2 = Z2_cmd;
    PS->Z3 = Z3_cmd;
    PS->Z4 = Z4_cmd;
    PS->Down1 = Down1_cmd;
    PS->Down2 = Down2_cmd;
    PS->Down3 = Down3_cmd;
    PS->Down4 = Down4_cmd;
    PS->FntNum = FntNum_cmd;
    PS->Fnt1 = Fnt1_cmd;
    PS->Fnt2 = Fnt2_cmd;
    PS->Fnt3 = Fnt3_cmd;
    PS->Fnt4 = Fnt4_cmd;

    PS->FntDef1 = ComFontDef1;
    PS->FntDef2 = ComFontDef2;
    PS->FntDef3 = ComFontDef3;
    PS->FntDef4 = ComFontDef4;
    
    PS->XXX1 = XXX1_cmd;
    PS->XXX2 = XXX2_cmd;
    PS->XXX3 = XXX3_cmd;
    PS->XXX4 = XXX4_cmd;
#if TATE
    PS->Dir = Dir_cmd;
#endif
    PS->SetUpJob = SetUp_Job;
    PS->EndUpJob = EndUp_Job;
    PS->SetUpPage = SetUp_Page;
    PS->EndUpPage = EndUp_Page;
    PS->Bop = SetUp_Page;
    PS->Eop = EndUp_Page;
}
