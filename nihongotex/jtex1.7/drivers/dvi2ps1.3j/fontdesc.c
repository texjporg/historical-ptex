#include	<stdio.h>
#include	"defs.h"
#include	"global.h"

extern struct fontop
    pkop,	/* pkfont.c */
    gfop,	/* gffont.c */
    pxlop,	/* pxlfont.c */
    tfmop,	/* bifont.c */
    jstfmop,	/* bifont.c */
    jfmop,	/* bifont.c */
    bitexop,	/* bifont.c */
    jcompop;	/* compfont.c */
struct fontop *fontops[] = {
    &pkop,
    &gfop,
    &pxlop,
    &tfmop,
    &jstfmop,
    &jfmop,
    &bitexop,
    NULL
};

extern struct confop
    fontcop,	/* fontdesc.c */
    jfontcop,	/* fontdesc.c */
    pdcop,	/* psrast.c */
    bkcop,	/* psbi.c */
    includecop,	/* fontdesc.c */
    substcop;	/* fontdesc.c */
struct confop *confops[] = {
    &fontcop,
    &jfontcop,
    &pdcop,
    &bkcop,
    &includecop,
    &substcop,
    NULL
};


int fdline;

read_fontdesc(fdname)
char *fdname;
{
    register int c;
    register FILE *fd;
    char field[STRSIZE];
    struct confop *findconfop();

    if ((fd = BINARYOPEN(fdname, "r")) == NULL)
	Fatal("cannot open fontdesc file %s", fdname);
    for (fdline = 1; (c = getc(fd)) != EOF; fdline++) {
	if (c == '#' || c == '\n') {
	    skipline(c, fd);
	    continue;
	}
	c = getfield(field, c, fd);
	(*(findconfop(field)->co_get))(c, fd);
    }
    fclose(fd);
}

void
skipline(c, f)
register int c;
register FILE *f;
{
    for (; c != '\n'; c = getc(f))
	;
}

getfield(field, c, f)
char *field;
int c;
FILE *f;
{
    char *fend;

    for (; c == ' ' || c == '\t'; c = getc(f))
	;
    if (c == '\n')
	Fatal("fontdesc: illegal line %d", fdline);
    for (fend = field+STRSIZE-1;
	 c != ' ' && c != '\t' && c != '\n'; c = getc(f))
	if (field < fend)
	    *field++ = c;
	else
	    Fatal("fontdesc: field too long (line %d)", fdline);
    *field = '\0';
    return (c);
}

struct confop skipcop = {
    "",
    skipline
};

struct confop *
findconfop(field)
char *field;
{
    struct confop **co;

    for (co = confops; *co != NULL; co++)
	if (STREQ((*co)->co_name, field))
	    return (*co);
    Warning("fontdesc: %s illegal (line %d)", field, fdline);
    return (&skipcop);
}

void getfontdesc();
struct confop fontcop = {
    "font",
    getfontdesc
};
void getjfontdesc();
struct confop jfontcop = {
    "jfont",
    getjfontdesc
};

struct fontdesc {
    struct fontop *fd_op, *fd_subop;
    char *fd_spec;	/* specifier */ /* not used */
    int	fd_sub;		/* font substitution */
    char *fd_path;	/* prototype path */
    struct fontdesc *fd_next;
};
struct fontdesc *fontdescs = NULL;
struct fontdesc **nextfd = &fontdescs;

void
getfontdesc(c, f)
int c;
FILE *f;
{
    getfd(c, f, FALSE);
}

void
getjfontdesc(c, f)
int c;
FILE *f;
{
    getfd(c, f, TRUE);
}

getfd(c, f, jcomp)
int c;
FILE *f;
int jcomp;
{
    char field_type[STRSIZE];
    char field_spec[STRSIZE];
    char field_sub[STRSIZE];
    char field_path[STRSIZE];
    struct fontdesc *fd;
    struct fontop *fop;
    struct fontop *findfontop();

    c = getfield(field_type, c, f);
    if ((fop = findfontop(field_type)) == NULL) {
	Warning("fontdesc: illegal font type %s", field_type);
	skipline(c, f);
	return;
    }
    c = getfield(field_spec, c, f);
    c = getfield(field_sub, c, f);
    c = getfield(field_path, c, f);
    skipline(c, f);
    fd = NEW(struct fontdesc, "fontdesc");
    if (jcomp) {
	fd->fd_op = &jcompop;
	fd->fd_subop = fop;
    } else {
	fd->fd_op = fop;
	fd->fd_subop = NULL;
    }
    fd->fd_spec = strsave(field_spec);
    fd->fd_sub = atoi(field_sub);
    fd->fd_path = strsave(field_path);
    fd->fd_next = NULL;
    *nextfd = fd;
    nextfd = &(fd->fd_next);
}

struct fontop *
findfontop(type)
register char *type;
{
    register struct fontop **fo;
    
    for (fo = fontops; *fo != NULL; fo++)
	if (STREQ((*fo)->fo_type, type))
	    return (*fo);
    return (NULL);
}

void getinclude();
struct confop includecop = {
    "include",
    getinclude
};

struct includefile {
    char *if_file;
    struct includefile *if_next;
};
struct includefile *includefiles = NULL;
struct includefile **nextif = &includefiles;

void
getinclude(c, f)
int c;
FILE *f;
{
    char field_file[STRSIZE];
    register struct includefile *inf;

    c = getfield(field_file, c, f);
    skipline(c, f);
    inf = NEW(struct includefile, "includefile");
    inf->if_file = strsave(field_file);
    inf->if_next = NULL;
    *nextif = inf;
    nextif = &(inf->if_next);
}

do_include(inclop)
void (*inclop)();
{
    register struct includefile *inf;

    for (inf = includefiles; inf != NULL; inf = inf->if_next)
	(*inclop)(inf->if_file);
}

void getsubst();
struct confop substcop = {
    "subst",
    getsubst
};

struct fontsubst {
    char *fs_font;
    int fs_len;
    int fs_reqmag;
    int fs_submag;
    struct fontsubst *fs_next;
};
struct fontsubst *fontsubsts = NULL;
struct fontsubst **nextfs = &fontsubsts;

void
getsubst(c, f)
int c;
FILE *f;
{
    char field_font[STRSIZE];
    char field_reqmag[STRSIZE];
    char field_submag[STRSIZE];
    struct fontsubst *fs;;

    c = getfield(field_font, c, f);
    c = getfield(field_reqmag, c, f);
    c = getfield(field_submag, c, f);
    skipline(c, f);
    fs = NEW(struct fontsubst, "fontsubst");
    fs->fs_font = strsave(field_font);
    fs->fs_len = strlen(field_font);
    fs->fs_reqmag = atoi(field_reqmag);
    fs->fs_submag = atoi(field_submag);
    fs->fs_next = NULL;
    *nextfs = fs;
    nextfs = &(fs->fs_next);
}


/*
 * read_fontinfo
 */
read_fontinfo(fe)
struct font_entry *fe;
{
    struct accarg acca;
    float rawmagfact, newmagfact;
    register struct fontdesc *fd;
    struct fontsubst *fs;
    int i, next;
    int null_fontdict(), null_setchar(), null_setstring();

    acca.rawmagfact = rawmagfact = newmagfact = dev_fontmag(fe);
    acca.pv_name = fe->n;
    /* We don't treat the case fe->a != 0 */

    acca.acc_mode = ACC_EXACT;
    acca.actmagfact = actfact(acca.rawmagfact);
    for (fd = fontdescs; fd != NULL; fd = fd->fd_next) {
	if ((*(fd->fd_op->fo_access))(fd->fd_path, fe, &acca,
				      fd->fd_subop)) {
	    (*(fd->fd_op->fo_readfontinfo))(fe);
	    return;
	}
    }

    acca.acc_mode = ACC_SUBST;
    for (fs = fontsubsts; fs != NULL; fs = fs->fs_next)
	if (strncmp(fe->n, fs->fs_font, fs->fs_len) == 0) {
	    acca.submag = fs->fs_submag;
	    acca.reqmag = fs->fs_reqmag;
	    for (fd = fontdescs; fd != NULL; fd = fd->fd_next) {
		if ((fd->fd_sub&ACC_SUBST) == 0)
		    continue;
		if ((*(fd->fd_op->fo_access))(fd->fd_path, fe, &acca,
					      fd->fd_subop)) {
		    (*(fd->fd_op->fo_readfontinfo))(fe);
		    Warning("Font file for %s: font mag %d replaced by %d",
			    fe->n, fs->fs_reqmag, fs->fs_submag);
		    return;
		}
	    }
	}

    acca.acc_mode = ACC_MAGSTEP;
    apprfact(rawmagfact);
    for (i = 0; ; i = next) {
	acca.stepmagfact = newmagfact = mag_table[mag_index+i];
	for (fd = fontdescs; fd != NULL; fd = fd->fd_next) {
	    if ((fd->fd_sub&ACC_MAGSTEP) == 0)
		continue;
	    if ((*(fd->fd_op->fo_access))(fd->fd_path, fe, &acca,
					  fd->fd_subop)) {
		(*(fd->fd_op->fo_readfontinfo))(fe);
		Warning("Font file for %s: magnification %d replaced by %d",
			fe->n, MAGSIZE(rawmagfact), MAGSIZE(newmagfact));
		return;
	    }
	}
	if (i >= 0) {
	    if (mag_index+(next = -i-1) < 0)
		if (mag_index+(next = i+1) >= magtabsize)
		    goto notfound;
	} else {
	    if (mag_index+(next = -i) >= magtabsize)
		if (mag_index+(next = i-1) < 0)
		    goto notfound;
	}
    }

 notfound:	/* allow missing FNT files */
    Warning("No font file for %s in magnification %d",
	    fe->n, MAGSIZE(rawmagfact));
    fe->dev_fontdict = null_fontdict;
    fe->dev_setchar = null_setchar;
    fe->dev_setstring = null_setstring;
    sprintf(fe->name, "null:%s:%d", fe->n, MAGSIZE(rawmagfact));
}

null_fontdict(fe, c)
struct font_entry *fe;
int c;
{
    return (NONFONT);
}

null_setchar(c)
int c;
{
    return (0);
}

null_setstring(s, len)
char *s;
int len;
{
    return (0);
}

/*
 * Turn a prototype path into a fullpath.
 */
pave(path, proto, acca)
char *path, *proto;
struct accarg *acca;
{
    register char *p, *s, *t;
    char *pend;
    int len;
    char buf[32];

    for (p = path, s = proto, pend = path+PATHLEN-1; *s != '\0'; s++)
	if (*s == '%') {
	    switch (*++s) {
	    case 'f':
	    case 'n':
	    case 's':
		t = acca->pv_name;
		break;
	    case 'd':
	    case 'm':
		sprintf(t = buf, "%d", acca->pv_mag);
		break;
	    case 'F':
		t = acca->pv_fam;
		break;
	    case 'D':
		sprintf(t = buf, "%d", acca->pv_ds);
		break;
	    case 'j':
		t = acca->pv_jsub;
		break;
	    default:
		*(t = buf) = *s;  *(t+1) = '\0';
		break;
	    }
	    if (p + (len = strlen(t)) <= pend) {
		strncpy(p, t, len);
		p += len;
	    } else
		Fatal("font path too long %s", proto);
	} else if (p < pend) {
	    *p++ = *s;
	} else
	    Fatal("font path too long %s", proto);
    *p = '\0';
}
