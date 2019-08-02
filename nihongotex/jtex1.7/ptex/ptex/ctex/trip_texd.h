#define	TRIP
#undef	TRAP
#define	STAT
#undef	DEBUG
#include "tex.h"
#define memmax 3999L 
#define memmin 0 
#define bufsize 4096 
#define errorline 64 
#define halferrorline 32 
#define maxprintline 72 
#define stacksize 200 
#define maxinopen 6 
#define fontmax 75 
#define fontmemsize 20000L 
#define paramsize 60 
#define nestsize 40 
#define maxstrings 3000 
#define stringvacancies 8000 
#define poolsize 32000L 
#define savesize 600 
#define triesize 8000 
#define dvibufsize 16384 
#define filenamesize 1024 
#define poolname "ptex.pool" 
#define memtop 3999L 
typedef unsigned char ASCIIcode  ; 
typedef ASCIIcode xchar  ; 
typedef unsigned char eightbits  ; 
typedef integer poolpointer  ; 
typedef integer strnumber  ; 
typedef integer scaled  ; 
typedef integer nonnegativeinteger  ; 
typedef schar smallnumber  ; 
typedef unsigned char quarterword  ; 
typedef unsigned short halfword  ; 
typedef schar twochoices  ; 
typedef schar fourchoices  ; 
typedef halfword KANJIcode  ; 
#include "memory.h"
typedef schar glueord  ; 
typedef struct {
    short modefield ; 
  schar dirfield, adjdirfield ; 
  scaled pdispfield ; 
  halfword headfield, tailfield, pnodefield, lastjchrfield ; 
  integer pgfield, auxfield, mlfield ; 
} liststaterecord  ; 
typedef schar groupcode  ; 
typedef struct {
    quarterword statefield, indexfield ; 
  halfword startfield, locfield, limitfield, namefield ; 
} instaterecord  ; 
typedef integer internalfontnumber  ; 
typedef integer dviindex  ; 
typedef integer triepointer  ; 
typedef short hyphpointer  ; 
EXTERN integer bad  ; 
EXTERN char nameoffile[filenamesize + 1], realnameoffile[filenamesize + 1]  ; 
EXTERN integer namelength  ; 
EXTERN ASCIIcode buffer[bufsize + 1]  ; 
EXTERN integer first  ; 
EXTERN integer last  ; 
EXTERN integer maxbufstack  ; 
EXTERN ASCIIcode strpool[poolsize + 1]  ; 
EXTERN poolpointer strstart[maxstrings + 1]  ; 
EXTERN poolpointer poolptr  ; 
EXTERN strnumber strptr  ; 
EXTERN poolpointer initpoolptr  ; 
EXTERN strnumber initstrptr  ; 
#ifdef INITEX
EXTERN alphafile poolfile  ; 
#endif /* INITEX */
EXTERN alphafile logfile  ; 
EXTERN schar selector  ; 
EXTERN schar dig[23]  ; 
EXTERN integer tally  ; 
EXTERN integer termoffset  ; 
EXTERN integer fileoffset  ; 
EXTERN ASCIIcode trickbuf[errorline + 1]  ; 
EXTERN integer trickcount  ; 
EXTERN integer firstcount  ; 
EXTERN schar trickbuf2[errorline + 1]  ; 
EXTERN schar kcodepos  ; 
EXTERN schar interaction  ; 
EXTERN boolean deletionsallowed  ; 
EXTERN schar history  ; 
EXTERN schar errorcount  ; 
EXTERN strnumber helpline[6]  ; 
EXTERN schar helpptr  ; 
EXTERN boolean useerrhelp  ; 
EXTERN integer interrupt  ; 
EXTERN boolean OKtointerrupt  ; 
EXTERN boolean aritherror  ; 
EXTERN scaled remainder  ; 
EXTERN halfword tempptr  ; 
EXTERN memoryword 
#define zmem (zzzaa - (int)(memmin))
  zzzaa[memmax - memmin + 1]  ; 
EXTERN halfword lomemmax  ; 
EXTERN halfword himemmin  ; 
EXTERN integer varused, dynused  ; 
EXTERN halfword avail  ; 
EXTERN halfword memend  ; 
EXTERN halfword rover  ; 
#ifdef DEBUG
EXTERN boolean 
#define freearr (zzzab - (int)(memmin))
  zzzab[memmax - memmin + 1]  ; 
EXTERN boolean 
#define wasfree (zzzac - (int)(memmin))
  zzzac[memmax - memmin + 1]  ; 
EXTERN halfword wasmemend, waslomax, washimin  ; 
EXTERN boolean panicking  ; 
#endif /* DEBUG */
EXTERN integer fontinshortdisplay  ; 
EXTERN integer depththreshold  ; 
EXTERN integer breadthmax  ; 
EXTERN liststaterecord nest[nestsize + 1]  ; 
EXTERN integer nestptr  ; 
EXTERN integer maxneststack  ; 
EXTERN liststaterecord curlist  ; 
EXTERN short shownmode  ; 
EXTERN schar oldsetting  ; 
EXTERN memoryword zeqtb[7011]  ; 
EXTERN quarterword 
#define xeqlevel (zzzad -5785)
  zzzad[714]  ; 
EXTERN twohalves 
#define hash (zzzae -258)
  zzzae[3267]  ; 
EXTERN halfword hashused  ; 
EXTERN boolean nonewcontrolsequence  ; 
EXTERN integer cscount  ; 
EXTERN memoryword savestack[savesize + 1]  ; 
EXTERN integer saveptr  ; 
EXTERN integer maxsavestack  ; 
EXTERN quarterword curlevel  ; 
EXTERN groupcode curgroup  ; 
EXTERN integer curboundary  ; 
EXTERN integer magset  ; 
EXTERN halfword curcmd  ; 
EXTERN halfword curchr  ; 
EXTERN halfword curcs  ; 
EXTERN halfword curtok  ; 
EXTERN KANJIcode curjchr  ; 
EXTERN halfword jflag  ; 
EXTERN instaterecord inputstack[stacksize + 1]  ; 
EXTERN integer inputptr  ; 
EXTERN integer maxinstack  ; 
EXTERN instaterecord curinput  ; 
EXTERN integer inopen  ; 
EXTERN alphafile inputfile[maxinopen + 1]  ; 
EXTERN integer line  ; 
EXTERN integer linestack[maxinopen + 1]  ; 
EXTERN schar scannerstatus  ; 
EXTERN halfword warningindex  ; 
EXTERN halfword defref  ; 
EXTERN halfword paramstack[paramsize + 1]  ; 
EXTERN integer paramptr  ; 
EXTERN integer maxparamstack  ; 
EXTERN integer alignstate  ; 
EXTERN integer baseptr  ; 
EXTERN halfword parloc  ; 
EXTERN halfword partoken  ; 
EXTERN boolean skipmode  ; 
EXTERN boolean forceeof  ; 
EXTERN halfword curmark[5]  ; 
EXTERN schar longstate  ; 
EXTERN halfword pstack[9]  ; 
EXTERN integer curval  ; 
EXTERN schar curvallevel  ; 
EXTERN smallnumber radix  ; 
EXTERN glueord curorder  ; 
EXTERN alphafile readfile[16]  ; 
EXTERN schar readopen[17]  ; 
EXTERN halfword condptr  ; 
EXTERN schar iflimit  ; 
EXTERN smallnumber curif  ; 
EXTERN integer ifline  ; 
EXTERN integer skipline  ; 
EXTERN strnumber curname  ; 
EXTERN strnumber curarea  ; 
EXTERN strnumber curext  ; 
EXTERN poolpointer areadelimiter  ; 
EXTERN poolpointer extdelimiter  ; 
EXTERN ccharpointer TEXformatdefault  ; 
EXTERN boolean nameinprogress  ; 
EXTERN strnumber jobname  ; 
EXTERN boolean logopened  ; 
EXTERN bytefile dvifile  ; 
EXTERN strnumber outputfilename  ; 
EXTERN strnumber logname  ; 
EXTERN bytefile tfmfile  ; 
EXTERN memoryword fontinfo[fontmemsize + 1]  ; 
EXTERN integer fmemptr  ; 
EXTERN internalfontnumber fontptr  ; 
EXTERN fourquarters fontcheck[fontmax + 1]  ; 
EXTERN scaled fontsize[fontmax + 1]  ; 
EXTERN scaled fontdsize[fontmax + 1]  ; 
EXTERN halfword fontparams[fontmax + 1]  ; 
EXTERN strnumber fontname[fontmax + 1]  ; 
EXTERN strnumber fontarea[fontmax + 1]  ; 
EXTERN eightbits fontbc[fontmax + 1]  ; 
EXTERN eightbits fontec[fontmax + 1]  ; 
EXTERN halfword fontglue[fontmax + 1]  ; 
EXTERN schar fontdir[fontmax + 1]  ; 
EXTERN boolean fontused[fontmax + 1]  ; 
EXTERN halfword fontnumext[fontmax + 1]  ; 
EXTERN integer hyphenchar[fontmax + 1]  ; 
EXTERN integer skewchar[fontmax + 1]  ; 
EXTERN integer charbase[fontmax + 1]  ; 
EXTERN integer widthbase[fontmax + 1]  ; 
EXTERN integer heightbase[fontmax + 1]  ; 
EXTERN integer depthbase[fontmax + 1]  ; 
EXTERN integer italicbase[fontmax + 1]  ; 
EXTERN integer ligkernbase[fontmax + 1]  ; 
EXTERN integer kernbase[fontmax + 1]  ; 
EXTERN integer extenbase[fontmax + 1]  ; 
EXTERN integer parambase[fontmax + 1]  ; 
EXTERN integer ctypebase[fontmax + 1]  ; 
EXTERN fourquarters nullcharacter  ; 
EXTERN integer totalpages  ; 
EXTERN scaled maxv  ; 
EXTERN scaled maxh  ; 
EXTERN integer maxpush  ; 
EXTERN integer lastbop  ; 
EXTERN boolean dirused  ; 
EXTERN integer deadcycles  ; 
EXTERN boolean doingleaders  ; 
EXTERN quarterword c, f  ; 
EXTERN scaled ruleht, ruledp, rulewd  ; 
EXTERN halfword g  ; 
EXTERN integer lq, lr  ; 
EXTERN eightbits dvibuf[dvibufsize + 1]  ; 
EXTERN dviindex halfbuf  ; 
EXTERN dviindex dvilimit  ; 
EXTERN dviindex dviptr  ; 
EXTERN integer dvioffset  ; 
EXTERN integer dvigone  ; 
EXTERN halfword downptr, rightptr  ; 
EXTERN scaled dvih, dviv  ; 
EXTERN integer dvidir  ; 
EXTERN scaled curh, curv  ; 
EXTERN integer curdir  ; 
EXTERN internalfontnumber dvif  ; 
EXTERN integer curs  ; 
EXTERN scaled totalstretch[4], totalshrink[4]  ; 
EXTERN halfword adjusttail  ; 
EXTERN scaled lastdisp  ; 
EXTERN halfword curkanjiskip  ; 
EXTERN halfword curxkanjiskip  ; 
EXTERN integer packbeginline  ; 
EXTERN twohalves emptyfield  ; 
EXTERN fourquarters nulldelimiter  ; 
EXTERN halfword curmlist  ; 
EXTERN smallnumber curstyle  ; 
EXTERN smallnumber cursize  ; 
EXTERN scaled curmu  ; 
EXTERN boolean mlistpenalties  ; 
EXTERN internalfontnumber curf  ; 
EXTERN quarterword curc  ; 
EXTERN fourquarters curi  ; 
EXTERN integer magicoffset  ; 
EXTERN halfword curalign  ; 
EXTERN halfword curspan  ; 
EXTERN halfword curloop  ; 
EXTERN halfword alignptr  ; 
EXTERN halfword curhead, curtail  ; 
EXTERN halfword justbox  ; 
EXTERN halfword passive  ; 
EXTERN halfword printednode  ; 
EXTERN halfword passnumber  ; 
EXTERN scaled activewidth[7]  ; 
EXTERN scaled curactivewidth[7]  ; 
EXTERN scaled background[7]  ; 
EXTERN scaled breakwidth[7]  ; 
EXTERN boolean noshrinkerroryet  ; 
EXTERN halfword curp  ; 
EXTERN boolean secondpass  ; 
EXTERN integer threshold  ; 
EXTERN boolean chain  ; 
EXTERN scaled minimaldemerits[4]  ; 
EXTERN scaled minimumdemerits  ; 
EXTERN halfword bestplace[4]  ; 
EXTERN halfword bestplline[4]  ; 
EXTERN scaled discwidth  ; 
EXTERN halfword easyline  ; 
EXTERN halfword lastspecialline  ; 
EXTERN scaled firstwidth  ; 
EXTERN scaled secondwidth  ; 
EXTERN scaled firstindent  ; 
EXTERN scaled secondindent  ; 
EXTERN halfword bestbet  ; 
EXTERN integer fewestdemerits  ; 
EXTERN halfword bestline  ; 
EXTERN integer actuallooseness  ; 
EXTERN integer linediff  ; 
EXTERN halfword hc[66]  ; 
EXTERN smallnumber hn  ; 
EXTERN halfword ha, hb  ; 
EXTERN internalfontnumber hf  ; 
EXTERN ASCIIcode hu[64]  ; 
EXTERN integer hyfchar  ; 
EXTERN schar hyf[65]  ; 
EXTERN smallnumber hyphenpassed  ; 
EXTERN twohalves trie[triesize + 1]  ; 
EXTERN smallnumber hyfdistance[256]  ; 
EXTERN smallnumber hyfnum[256]  ; 
EXTERN quarterword hyfnext[256]  ; 
EXTERN strnumber hyphword[308]  ; 
EXTERN halfword hyphlist[308]  ; 
EXTERN hyphpointer hyphcount  ; 
#ifdef INITEX
EXTERN quarterword trieophash[511]  ; 
#endif /* INITEX */
EXTERN quarterword trieopptr  ; 
#ifdef INITEX
EXTERN ASCIIcode triec[triesize + 1]  ; 
EXTERN quarterword trieo[triesize + 1]  ; 
EXTERN triepointer triel[triesize + 1]  ; 
EXTERN triepointer trier[triesize + 1]  ; 
EXTERN triepointer trieptr  ; 
#endif /* INITEX */
#ifdef INITEX
EXTERN triepointer triehash[triesize + 1]  ; 
#endif /* INITEX */
#ifdef INITEX
EXTERN boolean trietaken[triesize + 1]  ; 
EXTERN triepointer triemin  ; 
#endif /* INITEX */
EXTERN triepointer triemax  ; 
EXTERN scaled bestheightplusdepth  ; 
EXTERN halfword pagetail  ; 
EXTERN schar pagecontents  ; 
EXTERN scaled pagemaxdepth  ; 
EXTERN halfword bestpagebreak  ; 
EXTERN integer leastpagecost  ; 
EXTERN scaled bestsize  ; 
EXTERN scaled pagesofar[8]  ; 
EXTERN halfword lastglue  ; 
EXTERN integer lastpenalty  ; 
EXTERN scaled lastkern  ; 
EXTERN integer insertpenalties  ; 
EXTERN boolean outputactive  ; 
EXTERN eightbits pagedir  ; 
EXTERN halfword curbox  ; 
EXTERN halfword aftertoken  ; 
EXTERN boolean longhelpseen  ; 
EXTERN strnumber formatident  ; 
EXTERN wordfile fmtfile  ; 
EXTERN integer readyalready  ; 
EXTERN alphafile writefile[16]  ; 
EXTERN boolean writeopen[18]  ; 
EXTERN halfword writeloc  ; 
EXTERN poolpointer editnamestart  ; 
EXTERN integer editnamelength, editline, tfmtemp  ; 
EXTERN halfword firstchar, lastchar  ; 
EXTERN boolean findfirst  ; 
EXTERN boolean inhibitglueflag  ; 

#include "coerce.h"
