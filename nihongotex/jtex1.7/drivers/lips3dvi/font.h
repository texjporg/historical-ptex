/*
 *	font.h -- font managing structure	
 *
 *	Written by H. Nagahara on 2/10/92 08:58:25.
 *	Copyright (C) 1990,1991 by ASCII Corporation.  All rights reserved.
 *		SCCS memo: font.h 10.1
 */
/* font type code */
#define	TFM		0	/* TFM character set not donwloaded */
#define	JFM		2	/* JFM character set not downloaded */
#define	JFMT		3	/* JFM tategaki character set */

#define	JFMID		11	/* JFM file ID */
#define	JFMTID		9	/* tategaki JFM file ID */

#define	YOKO		0	/* yokogaki */
#define	TATE		1	/* rotateed */

/* structer for Japanese font metric information */
struct chartype {
    uint4	width;		/* character width */
    uint4	hight;		/* character hight */
    uint4	depth;		/* character depth */
    uint4	nglue;		/* natural glue befor this char */
};

struct jfm {
    uint2	kcode;		/* kanji code */
    struct chartype *cinfo;	/* pointer of character-type table */
};

/* structrer for font metric information */
struct metric {
    char	*name;		/* font name */
    struct metric *next;	/* next font metirc info */
    int		begin;		/* least char/type code */
    int		end;		/* last char/type code */
    uint4	sum;		/* check sum in tfm/jfm */
    uint4	design;		/* design size in tfm/jfm */
    int1	type;		/* JFM or TFM */
    int		cnum;		/* number of characters in the this list */
    union{
	uint4		*width;	/* array of the char width */
	struct jfm	*klist;	/* pointer to the kanji char list */
    }		cc;
};
    
/* Japanese internal font attribute list */
struct jfatr {
    char	*name;		/* font name pattern */
    int1	face;		/* font face # */
    int1	weight;		/* font stroke weight */
    int1	direction;	/* font direction */
    int1	fillpattern;	/* fill pattern # */
    int1	shadowpattern;	/* shadow pattern # */
    int1	outline;	/* outline dot */
    struct jfatr *next;		/* list */
};

/* font info in printer */
struct prinfo {
    char	download;	/* dowonloaded flag */
    int2	prn_no;		/* font # */
    int2	graphset;	/* graphic set # */
    int4	datacount;	/* font data bytes */
    char	*name;		/* downloaded font name */
    struct grif	*pattern20;	/* bitmap pattern for 0x20 code */
    struct jfatr *attrib;	/* font attribute */
};

/* structure for grif infomation */
struct grif {
    uint4	code;		/* character code */
    uint2	width;		/* bitmap width */
    uint2	hight;		/* bitmap hight */
    int2	hrefp;		/* horizontal reference point */
    int2	vrefp;		/* vertical reference point */
#if	USEEMS
    int2	emspage;	/* EMS logical page + 1 */
#endif
    uint1	*bitmap;	/* pointer to the bitmap data */
};				/* bitmap area has the size :
				   ((width +7)/8) * hight */

/* structure for JXL font_dir data */
struct fdir {
    int2	code;		/* character code */
    int2	width;		/* bitmap width */
    int2	hight;		/* bitmap height */
    int2	h_offset;	/* horizontal offset of ref. point */
    int2	v_offset;	/* vertical offset of ref. point */
#if	USEEMS
    int2	emspage;	/* EMS logical page + 1 */
#endif
    uint1	*bitmap;	/* bitmap data */
    struct font	*fontnode;	/* belonging font */
    struct fdir	*samenext;	/* link through same font */
    struct fdir *sameprev;	/* link through same font */
    struct fdir	*prev;		/* previous bitmap in cache */
    struct fdir	*next;		/* next bitmap in cache */
};

/* structure for JXL fontdir info */
struct jxlinfo {
    FILE	*jxlfp;		/* JXL file descripter */
    char	*jxlname;	/* JXL file name */
    int4	xinfo;		/* xinfo offset of jxl file */
    int4	yinfo;		/* yinfo offset of jxl file */
    int4	dirptr0;	/* Hi-Kanji fontdir offset */
    struct fdir	*jgrif0;	/* Hi-Kanji fontdir list */
    int4	dirptr1;	/* Level 1 Kanji fontdir offset */
    struct fdir	*jgrif1;	/* Level 1 Kanji fontdir list */
    int4	dirptr2;	/* level 2 Kanji fontdir offset */
    struct fdir	*jgrif2;	/* Level 2 Kanji fontdir list */
};

/* structure for the elements of the font-list */
struct font {
    int		fno;		/* font # */
    struct font	*prev,		/* link for font list */
		*next;
    char	type;		/* font type TFM or JFM */
    uint4	sum;		/* check sum */
    uint4	scale;		/* magnification */
    uint4	design;		/* design size */
    struct prinfo *prn;		/* printed dependent infomation */
    struct metric *info;	/* metric information */
    union {
	struct grif	*pattern;	/* bitmap pattern for alphabet */
	struct jxlinfo	*finfo;		/* font dir for JXL font */
    } gdata;
};
