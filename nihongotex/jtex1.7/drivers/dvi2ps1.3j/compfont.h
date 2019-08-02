#include	"jsub.h"

struct jcompfntinfo {
    struct accarg acca;
    char *proto;
    struct fontop *subop;
    struct font_entry jsubs[NJSUBS];
};

#define	jcompfinfo(fe)	((struct jcompfntinfo *)(fe->finfo))
