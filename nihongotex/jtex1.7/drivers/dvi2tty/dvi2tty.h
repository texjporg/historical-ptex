#define Copyright "dvi2tty.c  Copyright (C) 1984, 1985, 1986 Svante Lindahl.\n\
Copyright (C) 1988 M.J.E. Mol"

#include <stdio.h>

#define TRUE        1
#define FALSE       0
#define nil         NULL

#define EUC	1
#define SJIS	2
#define JIS	3

/*
 * ERROR CODES , don't start with 0
 */

#define illop    1              /* illegal op-code                   */
#define stkof    2              /* stack over-flow                   */
#define stkuf    3              /* stack under-flow                  */
#define stkrq    4              /* stack requirement                 */
#define badid    5              /* id is not right                   */
#define bdsgn    6              /* signature is wrong                */
#define fwsgn    7              /* too few signatures                */
#define nopre    8              /* no pre-amble where expected       */
#define nobop    9              /* no bop-command where expected     */
#define nopp    10              /* no postpost where expected        */
#define bdpre   11              /* unexpected preamble occured       */
#define bdbop   12              /* unexpected bop-command occured    */
#define bdpst   13              /* unexpected post-command occured   */
#define bdpp    14              /* unexpected postpost               */
#define nopst   15              /* no post-amble where expected      */
#define illch   16              /* character code out of range       */
#define filop   17              /* cannot access file                */
#define filcr   18              /* cannot creat file                 */
#define pipcr   19              /* cannot creat pipe                 */

/*---------------------------------------------------------------------------*/

typedef char bool;

typedef struct prlistptr {      /* list of pages selected for output         */
    int       pag;                      /* number of pages                   */
    bool      all;                      /* pages in intervall selected       */
    struct prlistptr *prv;              /* previous item in list             */
    struct prlistptr *nxt;              /* next item in list                 */
    int       adr;                      /* nr of byte in file where bop is   */
} printlisttype;

/*---------------------------------------------------------------------------*/

extern bool   outputtofile;            /* output to file or stdout(dvi2tty.c)*/
extern bool   pageswitchon;            /* user-set pages to print(dvistuff.c)*/
extern bool   sequenceon;              /* not TeX pagenrs (dvistuff.c)       */
extern bool   scascii;                 /* Scand. nat. chars (dvistuff.c)     */
extern bool   noffd;                   /* output ^L or formfeed (dvistuff.c) */

extern printlisttype *currentpage;     /* current page to print (dvi2tty.c)  */
extern printlisttype *firstpage;       /* first page selected (dvi2tty.c)    */
extern printlisttype *lastpage;        /* last page selected (dvi2tty.c)     */

extern int            ttywidth;        /* screen width (dvi2tty.c)           */
extern int            foo;             /* temporary 'register' (dvi2tty.c)   */
extern int            opcode;          /* dvi opcode (dvistuff.c)            */
extern int	      OType;		/* Output Kanji code EUC/SJIS/JIS */

extern FILE *DVIfile;                  /* dvi file (dvi2tty.c)               */
extern FILE *output;                   /* output file (dvi2tty.c)            */

/*---------------------------------------------------------------------------*/

/* dvi2tty.c */
void errorexit();

/* dvistuff.c */
void dvimain();

extern char KANJI_IN[], KANJI_OUT[];

