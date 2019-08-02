#define	HALFTFMCHAR	127
#define	LASTTFMCHAR	255
#define	LWFSIZE		64

struct tfmchar_entry {		/* character entry */
   short dev_font, dev_char;
   int tfmw;			/* TFM width */
};

struct tfmfntinfo {
   int lastfntchar;
   struct tfmchar_entry ch[LASTTFMCHAR+1];	/* character information */
};

#define	tfmfinfo(fe)	((struct tfmfntinfo *)(fe->finfo))


struct jstfmchar_entry {	/* character entry */
   short dev_ku, dev_ten;
   int tfmw;			/* TFM width */
};

struct jstfmfntinfo {
   int lastfntchar;
   struct jstfmchar_entry ch[LASTTFMCHAR+1];	/* character information */
   short dev_font;
   int *dictopen;
   char *dev_name;
};

#define	jstfmfinfo(fe)	((struct jstfmfntinfo *)(fe->finfo))


#define	JFM_ID		11

struct jfmtype_entry {		/* character type entry */
   int jfm_code;
   int jfm_type;
};

struct jfmchar_entry {		/* character entry */
   int tfmw;			/* TFM width */
};

struct jfmfntinfo {
   int nctype;
   struct jfmtype_entry *ctype;	/* character type information */
   int lasttypecode;
   struct jfmchar_entry *ch;	/* character information indexed by type */
   char *dev_name;
};

#define	jfmfinfo(fe)	((struct jfmfntinfo *)(fe->finfo))


struct bitexfntinfo {
   int lastfntchar;
   struct tfmchar_entry ch[HALFTFMCHAR+1];	/* character information */
   char dev_name[LWFSIZE];
};

#define	bitexfinfo(fe)	((struct bitexfntinfo *)(fe->finfo))


char *dev_bikanjiname();
