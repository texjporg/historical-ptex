#include	<stdio.h>
#include	<signal.h>
#include	<ctype.h>
#include	"dvi2ricoh.h"
#include	"commands.h"
#include	"font.h"

#define	MASKSIGNAL \
	(sigmask(SIGINT) | sigmask(SIGHUP) | \
	 sigmask(SIGTERM)| sigmask(SIGKILL)|sigmask(SIGQUIT))
	
int	shouldnl;

int
cmd(dvifp, pass)
	FILE	*dvifp;
{
	register int		op, emitflag, skipflag, val, val2, sp;
	struct fontinfo		*ftptr;
	extern struct fontinfo	*findfont();
	extern void		beginpage();
	extern void		endpage();
	extern void		setchar();
	extern void		setrule();
	extern void		initprinter();
	extern void		termprinter();

	sp = 0;
	if (pass == 0)
		emitflag = FALSE;
	else
		emitflag = TRUE;
	skipflag = FALSE;

	for (;;) {
		switch (op = getuint(dvifp, 1)) {
		case SET1:
		case SET2:
		case SET3:
		case SET4:
			val = getuint(dvifp, op - SET1 + 1);
			if (!skipflag)
				setchar(val, pass, TRUE);
			break;
		case SET_RULE:
			val = getsint(dvifp, 4);
			val2 = getsint(dvifp, 4);
			if (emitflag)
				setrule(val, val2, TRUE);
			break;
		case PUT1:
		case PUT2:
		case PUT3:
		case PUT4:
			val = getuint(dvifp, op - PUT1 + 1);
			if (!skipflag)
				setchar(val, pass, FALSE);
			break;
		case PUT_RULE:
			val = getsint(dvifp, 4);
			val2 = getsint(dvifp, 4);
			if (emitflag)
				setrule(val, val2, FALSE);
			break;
		case NOP:
			break;
		case BOP:
			curpage = ftell(dvifp) - 1;
			for (op = 0; op <= 9; op++)
				count[op] = getsint(dvifp, 4);
			prevpage = getuint(dvifp, 4);

			bzero((char *)&curstat, sizeof(curstat));
			devstat.hh = UNDEFPOS;
			devstat.vv = UNDEFPOS;
			/*
			devfont = NULL;
			*/
			curfont = NULL;

			if (pass == 0)
				emitflag = FALSE;
			else
				emitflag = TRUE;
			skipflag = FALSE;

			if (fpageflag && count[0] < firstpage) {
				skipflag = TRUE;
				emitflag = FALSE;
			}
			if (lpageflag && lastpage < count[0])
				return 3;
			if (!initflag) {
				initprinter();
				initflag = TRUE;
			}
			if (pass != 0)
				beginpage(count[0]);
			break;
		case EOP:
			if (sp != 0)
				fatal("stack is not empty when EOP");
			if (skipflag)
				return 2;
			if (pass != 0)
				endpage();
			return 1;
		case PUSH:
			if (sp >= stacklimit)
				fatal("stack overflow");
			/*
			 * structure assignment
			 */
			stack[sp++] = curstat;
			break;
		case POP:
			--sp;
			if (sp < 0)
				fatal("stack underflow");
			/*
			 * structure assignment
			 */
			curstat = stack[sp];
			break;
		case RIGHT1:
		case RIGHT2:
		case RIGHT3:
		case RIGHT4:
			curstat.h += getsint(dvifp, op - RIGHT1 + 1);
			break;
		case W0:
			curstat.h += curstat.w;
			break;
		case W1:
		case W2:
		case W3:
		case W4:
			curstat.w = getsint(dvifp, op - W1 + 1);
			curstat.h += curstat.w;
			break;
		case X0:
			curstat.h += curstat.x;
			break;
		case X1:
		case X2:
		case X3:
		case X4:
			curstat.x = getsint(dvifp, op - X1 +1);
			curstat.h += curstat.x;
			break;
		case DOWN1:
		case DOWN2:
		case DOWN3:
		case DOWN4:
			curstat.v += getsint(dvifp, op - DOWN1 + 1);
			break;
		case Y0:
			curstat.v += curstat.y;
			break;
		case Y1:
		case Y2:
		case Y3:
		case Y4:
			curstat.y = getsint(dvifp, op - Y1 + 1);
			curstat.v += curstat.y;
			break;
		case Z0:
			curstat.v += curstat.z;
			break;
		case Z1:
		case Z2:
		case Z3:
		case Z4:
			curstat.z = getsint(dvifp, op - Z1 + 1);
			curstat.v += curstat.z;
			break;
		case FNT1:
		case FNT2:
		case FNT3:
		case FNT4:
			if (!skipflag) {
				val = getuint(dvifp, op - FNT1 + 1);
				if ((curfont = findfont(val)) == NULL)
					fatal("unknown font");
			}
			break;
		case XXX1:
		case XXX2:
		case XXX3:
		case XXX4:
			val = getuint(dvifp, op - XXX1 + 1);
			if (emitflag)
				dospecial(dvifp, val);
			else
				skipspecial(dvifp, val);
			break;
		case FNT_DEF1:
		case FNT_DEF2:
		case FNT_DEF3:
		case FNT_DEF4:
			val = getuint(dvifp, op - FNT_DEF1 + 1);
			skipfontdef(dvifp, val);
			break;
		case PRE:
			fatal("PRE occurs within file");
		case POST:
			if (pass != 0 && initflag)
				termprinter();
			return 0;
		case POST_POST:
			fatal("POST_POST with no preceding POST");
		default:
			if (SETC_000 <= op && op <= SETC_127) {
				if (!skipflag)
					setchar(op, pass, TRUE);
			} else if (FONT_00 <= op && op <= FONT_63) {
				if (!skipflag) {
					val = op - FONT_00;
					if ((curfont = findfont(val)) == NULL)
						fatal("unknown font");
				}
			} else
				fatal("%d is an undefined op", op);
			break;
		}
	}
	/* never reached */
}

void
beginpage(pg)
	int	pg;
{
	if (verbose) {
		fprintf(stderr, "[%3.d", pg);
		fflush(stderr);
		shouldnl = TRUE;
	}
	(void)sigblock(MASKSIGNAL);
	startpage();
	(void)sigsetmask(sigblock(0) & ~MASKSIGNAL);
}

void
endpage()
{
	if (verbose) {
		fprintf(stderr, "]");
		if (++pageno % 10 == 0) {
			putc('\n', stderr);
			shouldnl = FALSE;
		}
		fflush(stderr);
	}
	/*
	 * feed paper
	 */
	if (ncopies > 99)
		ncopies = 99;
	(void)sigblock(MASKSIGNAL);
	feedpage(ncopies);
	(void)sigsetmask(sigblock(0) & ~MASKSIGNAL);
}

void
mesgend()
{
	if (shouldnl) {
		putc('\n', stderr);
		fflush(stderr);
	}
}

void
setchar(ch, pass, move)
	int	ch;
	int	pass;
	int	move;
{
	register struct chinfo	*cptr;
	register int		ku, ten, code;

	if (isromfont(curfont->flags)) {
		if (pass == 0)
			return;
		(void)sigblock(MASKSIGNAL);
		ku = emitromchar(ch);
		(void)sigsetmask(sigblock(0) & ~MASKSIGNAL);
		if (move)
			curstat.h += ku;
		return;
	}
	code = ch;
	if (isjfmt(curfont->flags)) {
		ku = ch / 0x100 - 0x21;
		ten = (ch & 0xff) - 0x21;
		if (ku < 0 || ku >= MAXKU || ten < 0 || ten >= MAXTEN) {
			warning("ch is out of range");
			return;
		}
		cptr = &curfont->chdir[ku * MAXTEN + ten];
		ch = ten + 0x21;
	} else {
		if (ch > MAXCHAR) {
			warning("ch is too large");
			return;
		}
		cptr = &curfont->chdir[ch];
		ku = -1;
	}
	if (pass == 0) {
		if (++cptr->count== 0)
			--cptr->count;
		return;
	}
	(void)sigblock(MASKSIGNAL);
	if (!isdownloaded(cptr->flags)) {
		ku = emitimage(code, cptr);
	} else {
		setfont(ku);
		ku = emitchar(ch, cptr);
	}
	(void)sigsetmask(sigblock(0) & ~MASKSIGNAL);
	if (move)
		curstat.h += ku;
}

void
setrule(h, w, move)
	register int	h;
	register int	w;
	int		move;
{
	if (h > 0 && w > 0) {
		(void)sigblock(MASKSIGNAL);
		emitrule(w, h);
		(void)sigsetmask(sigblock(0) & ~MASKSIGNAL);
	}
	if (move)
		curstat.h += w;
}

void
initprinter()
{
	(void)sigblock(MASKSIGNAL);
	emitinit();
	(void)sigsetmask(sigblock(0) & ~MASKSIGNAL);
}

void
termprinter()
{
	(void)sigblock(MASKSIGNAL);
	emitterm();
	(void)sigsetmask(sigblock(0) & ~MASKSIGNAL);
}
