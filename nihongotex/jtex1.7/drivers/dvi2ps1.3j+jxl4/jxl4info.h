#define	NFNTCHARS	256
#define	NONEXISTANT	-1	/* offset for raster files not found */



struct jxl4fntinfo {

    /* int font_space;	/* computed from FNT_DEF s parameter	*/
    /* int font_mag;	/* computed from FNT_DEF s and d parameters */
    /* int designsize;	/* design size read from raster file	*/
    float corrfact;
    int nfntchars;
    int dictform;	/* form of dev_font dictionary		*/
    struct JXL4      *jxl4;
    struct X_INFO    *xinfo;
    struct Y_INFO    *yinfo;
    struct PXL_DIR   *pxldir;
    struct TFM_TABLE *tfmtable;
    struct rastchar_entry *ch;	/* character information	*/
};

#define	jxl4finfo(fe)	((struct jxl4fntinfo *)(fe->finfo))

/* used to pass information from access to readfontinfo */


struct jxl4fntinfo *alloc_jxl4finfo();
/* int pktype_access(); /* jxl4 font only pxltype access. */
/* int pxltype_access(); */
int jxl4type_access();
/* void dev_init_rastfont(); /* where is this function */
extern int resolution;
