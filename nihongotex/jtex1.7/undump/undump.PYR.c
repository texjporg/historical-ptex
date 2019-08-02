/*
 * This program was advertised on unix-wizards.  I have had such a large
 * response I'm sending it out to the world.
 * 
 * Here is the source.  It works fine under 4.1bsd, I see no fundamental
 * reason why it shouldn't work on an 11. (Except possibly small format
 * changes in exec header or user structure.)  No documentation yet.
 * Usage is
 *        undump new-a.out-file [old-a.out-file] [core-file]
 * where old-a.out-file and core-file default to "a.out" and "core",
 * respectively.  Probably should have an option to not require
 * old-a.out-file if the core came from a 407 file.
 * 
 * It doesn't preserve open files, and the program is re-entered at main
 * when you run it.  It's used locally to dump a lisp and restart it.
 * 
 * It requires a local subroutine called scanargs, somewhat similar to
 * getopt (I think).  You should be able to easily get around this, though.
 * =Spencer
 *
 *  Scanargs didn't work on the Pyramid, and its function was
 *  too simple in this case to bother with repair, so it was chucked.
 *     -Mike
 */


/*
 * undump.c - Convert a core file to an a.out.
 *
 * Author:	Spencer W. Thomas
 * 		Computer Science Dept.
 * 		University of Utah
 * Date:	Wed Feb 17 1982
 * Copyright (c) 1982 Spencer W. Thomas
 *
 * Usage:
 * undump new-a.out [a.out] [core]
 */

#include <stdio.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <a.out.h>

#define PSIZE	    10240
#define ctob(x)	(x<<PGSHIFT) /* Pyramid */

struct user u;
struct exec hdr, ohdr;

main(argc, argv)
char **argv;
{
    char *new_name, *a_out_name = "a.out", *core_name = "core";
    FILE *new, *a_out, *core;

    /*if (scanargs(argc, argv, "undump new-a.out!s a.out%s core%s",
	    /*&new_name, &a_out_name, &core_name)
		    /*!= 1)
	/*exit(1);*/
    if ((argc > 4)|| (argc < 2)) {
	fprintf(stderr, "Usage: %s new-a.out [old-a.out] [core]\n",argv[0]);
	exit(1);
    }
    new_name = argv[1];
    if (argc > 2) a_out_name = argv[2];
    if (argc > 3) core_name = argv[3];
    if ((a_out = fopen(a_out_name, "r")) == NULL)
    {
	perror(a_out_name);
	exit(1);
    }
    if ((core = fopen(core_name, "r")) == NULL)
    {
	perror(core_name);
	exit(1);
    }
    if ((new = fopen(new_name, "w")) == NULL)
    {
	perror(new_name);
	exit(1);
    }

    read_u(core);
    make_hdr(new, a_out);
    copy_text(new, a_out);
    copy_data(new, core);
    copy_sym(new, a_out);
    fclose(new);
    fclose(core);
    fclose(a_out);
    mark_x(new_name);
}

/*
 * read the u structure from the core file.
 */
read_u(core)
FILE *core;
{
    if ( fread(&u, sizeof u, 1, core) != 1 )
    {
	perror("Couldn't read user structure from core file");
	exit(1);
    }
}

/*
 * Make the header in the new a.out from the header in the old one
 * modified by the new data size.
 */
make_hdr(new, a_out)
FILE *new, *a_out;
{
    if (fread(&hdr, sizeof hdr, 1, a_out) != 1)
    {
	perror("Couldn't read header from a.out file");
	exit(1);
    }
    ohdr = hdr;
    if N_BADMAG(hdr)
    {
	fprintf(stderr, "a.out file doesn't have legal magic number\n");
	exit(1);
    }
    if (hdr.a_magic != u.u_exdata.ux_mag ||
	hdr.a_text != u.u_exdata.ux_tsize ||
	hdr.a_data != u.u_exdata.ux_dsize ||
	hdr.a_entry != u.u_exdata.ux_entloc)
    {
	fprintf(stderr, "Core file didn't come from this a.out\n");
fprintf(stderr, "magic: %o %o", hdr.a_magic, u.u_exdata.ux_mag);
fprintf(stderr, "text: %o %o", hdr.a_text, u.u_exdata.ux_tsize);
fprintf(stderr, "data: %o %o", hdr.a_data, u.u_exdata.ux_dsize);
fprintf(stderr, "entry: %o %o", hdr.a_entry, u.u_exdata.ux_entloc);
	exit(1);
    }

    printf("Data segment size was %u", hdr.a_data);
    hdr.a_data = ctob(u.u_dsize);
    hdr.a_bss = 0;			/* all data is inited now! */
    printf(" now is %u\n", hdr.a_data);
    if (fwrite(&hdr, sizeof hdr, 1, new) != 1)
    {
	perror("Couldn't write header to new a.out file");
	exit(1);
    }
}

/*
 * Copy the text from the a.out to the new a.out
 */
copy_text(new, a_out)
FILE *new, *a_out;
{
    char page[PSIZE];
    int txtcnt = hdr.a_text;

    fseek(new, N_TXTOFF(hdr), 0);
    fseek(a_out, N_TXTOFF(hdr), 0);
    if (hdr.a_magic == OMAGIC)
    {
	printf("a.out file is not shared text, getting text from core file\n");
	fseek(a_out, hdr.a_text, 1);	/* skip over text */
	return;
    }
    while (txtcnt >= PSIZE)
    {
	if (fread(page, PSIZE, 1, a_out) != 1)
	{
	    perror("Read failure on a.out text");
	    exit(1);
	}
	if (fwrite(page, PSIZE, 1, new) != 1)
	{
	    perror("Write failure in text segment");
	    exit(1);
	}
	txtcnt -= PSIZE;
    }
    if (txtcnt)
    {
	if (fread(page, txtcnt, 1, a_out) != 1)
	{
	    perror("Read failure on a.out text");
	    exit(1);
	}
	if (fwrite(page, txtcnt, 1, new) != 1)
	{
	    perror("Write failure in text segment");
	    exit(1);
	}
    }
}

/*
 * copy the data from the core file to the new a.out
 */
copy_data(new, core)
FILE *new, *core;
{
    char page[PSIZE];
    int datacnt = ctob(u.u_dsize);

    if (hdr.a_magic == OMAGIC)
	datacnt += u.u_tsize;
    fseek(core, ctob(UPAGES), 0);
    while (datacnt >= PSIZE)
    {
	if (fread(page, PSIZE, 1, core) != 1)
	{
	    perror("Read failure on core data");
	    exit(1);
	}
	if (fwrite(page, PSIZE, 1, new) != 1)
	{
	    perror("Write failure in data segment");
	    exit(1);
	}
	datacnt -= PSIZE;
    }
    if (datacnt)
    {
	if (fread(page, datacnt, 1, core) != 1)
	{
	    perror("Read failure on core data");
	    exit(1);
	}
	if (fwrite(page, datacnt, 1, new) != 1)
	{
	    perror("Write failure in data segment");
	    exit(1);
	}
    }
}

/*
 * Copy the relocation information and symbol table from the a.out to the new
 */
copy_sym(new, a_out)
FILE *new, *a_out;
{
    char page[PSIZE];
    int n;

    fseek(a_out, ohdr.a_data, 1);	/* skip over data segment */
    while ((n = fread(page, 1, PSIZE, a_out)) > 0)
    {
	if (fwrite(page, 1, n, new) != n)
	{
	    perror("Error writing symbol table to new a.out");
	    fprintf(stderr, "new a.out should be ok otherwise\n");
	    return;
	}
    }
    if (n < 0)
    {
	perror("Error reading symbol table from a.out");
	fprintf(stderr, "new a.out should be ok otherwise\n");
    }
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

