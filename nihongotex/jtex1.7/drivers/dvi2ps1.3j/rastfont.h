#define	NFNTCHARS	256
#define	NONEXISTANT	-1	/* offset for raster files not found */

struct rastchar_entry {		/* character entry */
    unsigned short width, height;/* width and height in pixels */
    short xOffset, yOffset;	/* x offset and y offset in pixels */
    short dev_font, dev_char;
    unsigned short nbpl;	/* # of bytes per pixel data line */
    union {
	int fileOffset;
	char *pixptr;
    } where;
    int tfmw;			/* TFM width */
};

struct rastfntinfo {
    /* int font_space;	/* computed from FNT_DEF s parameter	*/
    /* int font_mag;	/* computed from FNT_DEF s and d parameters */
    /* int designsize;	/* design size read from raster file	*/
    float corrfact;
    int nfntchars;
    int dictform;	/* form of dev_font dictionary		*/
    struct rastchar_entry ch[1];	/* character information	*/
};

#define	rastfinfo(fe)	((struct rastfntinfo *)(fe->finfo))

/* used to pass information from access to readfontinfo */
struct passrastfntinfo {
    /* int font_mag; */
    float corrfact;
};

struct rastfntinfo *alloc_rastfinfo();
int pktype_access();
int pxltype_access();
void dev_init_rastfont();
extern int resolution;
