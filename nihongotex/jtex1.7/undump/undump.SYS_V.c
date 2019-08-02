/*
 * undump - resurrect a core file into a running program.
 *
 *	for UNIX System V on a 3Bx
 *	that uses the Common Object File Format
 *
 * Author:
 *	Lou Salkind
 *	New York University
 *	Tue Mar  3 13:18:25 EST 1987
 *
 * Adapted from:
 *	 Spencer Thomas's undump and the file unexec.c in GNU emacs
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/psw.h>
#include <sys/pcb.h>
#include <sys/signal.h>
#include <sys/fs/s5dir.h>
#include <sys/user.h>

#include <stdio.h>
#include <sys/stat.h>

#include <aouthdr.h>
#include <filehdr.h>
#include <scnhdr.h>
#include <syms.h>

#define	PAGE_SIZE	NBPC

struct filehdr fh;
AOUTHDR aout;
struct scnhdr tsc;
struct scnhdr dsc;
struct scnhdr bsc;

long bias;
long lnnoptr;
long text_scnptr;
long data_scnptr;
long symlocptr;

main(argc, argv)
	char **argv;
{
	FILE *afp, *cfp, *nfp;
	struct user u;
	long off;
	long size;
	struct scnhdr sc;
	int i, n;
	char *a_out_name = "a.out";
	char *core_name = "core";
	char *new_name;

	if (argc < 2 || argc > 4) {
		fprintf(stderr, "usage: %s new [a.out [core]]\n", argv[0]);
		exit(1);
	}
	new_name = argv[1];
	if (argc > 2)
		a_out_name = argv[2];
	if (argc > 3)
		core_name = argv[3];
	afp = fopen(a_out_name, "r");
	if (afp == 0)
		Perror(a_out_name);
	cfp = fopen(core_name, "r");
	if (cfp == 0)
		Perror(core_name);
	nfp = fopen(new_name, "w");
	if (nfp == 0)
		Perror(new_name);

	if (fread(&fh, sizeof fh, 1, afp) != 1)
		Perror("fh read");
	if (fread(&aout, sizeof aout, 1, afp) != 1)
		Perror("aout read");

	for (i = 0; i < fh.f_nscns; i++) {
		if (fread(&sc, sizeof(sc), 1, afp) != 1)
			Perror("read");
		if (strcmp(sc.s_name, ".text") == 0) {
			tsc = sc;
		} else if (strcmp(sc.s_name, ".data") == 0) {
			dsc = sc;
		} else if (strcmp(sc.s_name, ".bss") == 0) {
			bsc = sc;
		}
	}

	if (fread(&u, sizeof u, 1, cfp) != 1)
		Perror("core read");
	if (u.u_exdata.ux_tsize != aout.tsize ||
	    u.u_exdata.ux_dsize != aout.dsize ||
	    u.u_exdata.ux_bsize != aout.bsize) {
		fprintf("mismatch between %s and %s sizes\n", a_out_name, core_name);
		exit(1);
	}

	off = USIZE*PAGE_SIZE;
	size = u.u_dsize *PAGE_SIZE;

	fh.f_flags |= F_RELFLG | F_EXEC;
	aout.dsize = size;
	aout.bsize = 0;
	tsc.s_size = aout.tsize;
	tsc.s_scnptr = sizeof(fh) + sizeof(aout);
	tsc.s_scnptr += fh.f_nscns * sizeof (struct scnhdr);
	text_scnptr = tsc.s_scnptr;
	lnnoptr = tsc.s_lnnoptr;
	symlocptr = fh.f_symptr;

	dsc.s_paddr = dsc.s_vaddr = aout.data_start;
	dsc.s_size = aout.dsize;
	dsc.s_scnptr = tsc.s_scnptr + tsc.s_size;
	data_scnptr = dsc.s_scnptr;

	bsc.s_paddr = bsc.s_vaddr = aout.data_start + aout.dsize;
	bsc.s_size = aout.bsize;
	bsc.s_scnptr = 0L;
	bias = dsc.s_scnptr + dsc.s_size - lnnoptr;

	if (fh.f_symptr > 0L)
		fh.f_symptr += bias;
	if (tsc.s_lnnoptr > 0L)
		tsc.s_lnnoptr += bias;

	if (fwrite(&fh, sizeof(fh), 1, nfp) != 1)
		Perror("fh write");
	if (fwrite(&aout, sizeof(aout), 1, nfp) != 1)
		Perror("aout write");
	if (fwrite(&tsc, sizeof(tsc), 1, nfp) != 1)
		Perror("ts write");
	if (fwrite(&dsc, sizeof(dsc), 1, nfp) != 1)
		Perror("ds write");
	if (fwrite(&bsc, sizeof(bsc), 1, nfp) != 1)
		Perror("bs write");
	fseek(nfp, (long)text_scnptr, 0);
	copy(afp, nfp, aout.tsize);
	fseek(cfp, off, 0);
	fseek(nfp, (long)data_scnptr, 0);
	copy(cfp, nfp, size);
	copy_syms(afp, nfp);
	fclose(nfp);
	fclose(afp);
	fclose(cfp);
	mark_x(new_name);
	exit(0);
}

copy_syms(afp, nfp)
	register FILE *afp, *nfp;
{
	char page[BUFSIZ];
	register int n;
	register int nsyms;
	struct syment symentry;
	AUXENT auxentry;

	/* if there are line numbers, copy them */
	if (lnnoptr) {
		if (fseek(afp, lnnoptr, 0) == -1L)
			Perror("ln fseek");
		copy(afp, nfp, symlocptr - lnnoptr);
	}

	/* now write the symbol table */
	if (fseek(nfp, fh.f_symptr, 0) == -1L)
		Perror("fh fseek");
	for (nsyms = 0; nsyms < fh.f_nsyms; nsyms++) {
		if (fread(&symentry, SYMESZ, 1, afp) != 1)
			Perror("sym fread");
		if (fwrite(&symentry, SYMESZ, 1, nfp) != 1)
			Perror("sym fwrite");
		/*
		 * adjust relative offsets of line numbers for
		 * function definitions
		 */
		if (symentry.n_numaux) {
			if (fread(&auxentry, AUXESZ, 1, afp) != 1)
				Perror("aux fread");
			nsyms++;
			if (ISFCN (symentry.n_type))
				auxentry.x_sym.x_fcnary.x_fcn.x_lnnoptr += bias;
			if (fwrite(&auxentry, AUXESZ, 1, nfp) != 1)
				Perror("aux fwrite");
		}
	}

	/* finally write the string table, if any */
	while ((n = fread(page, 1, sizeof page, afp)) > 0) {
		if (fwrite(page, 1, n, nfp) != n)
			Perror("sym write");
	}
	if (n < 0)
		Perror("sym read");
}

/*
 * After succesfully building the new a.out, mark it executable
 */
mark_x(name)
char *name;
{
    struct stat sbuf;
    int um;

    um = umask(777);
    umask(um);
    if (stat(name, &sbuf) == -1)
    {
	perror ("Can't stat new a.out");
	fprintf(stderr, "Setting protection to %o\n", 0777 & ~um);
	sbuf.st_mode = 0777;
    }
    sbuf.st_mode |= 0111 & ~um;
    if (chmod(name, sbuf.st_mode) == -1)
	perror("Couldn't change mode of new a.out to executable");

}

copy(a, b, size)
	register FILE *a, *b;
	long size;
{
	char buf[BUFSIZ];
	register int i, n;

	while (size > 0) {
		i = size;
		if (i > sizeof buf)
			i = sizeof buf;
		if ((n = fread(buf, 1, i, a)) <= 0)
			Perror("copy read");
		if (fwrite(buf, 1, n, b) != n)
			Perror("copy write");
		size -= n;
	}
}

Perror(s)
	char *s;
{
	perror(s);
	exit(1);
}
