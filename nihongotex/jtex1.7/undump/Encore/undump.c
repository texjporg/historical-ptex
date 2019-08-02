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
#include <sys/stat.h>
#include <aouthdr.h>
#include <filehdr.h>
#include <scnhdr.h>
#include <linenum.h>
#include <sys/ptrace.h>

#define PSIZE	    10240
#define MAXSCNS		8

struct ptrace_user u;
AOUTHDR ahdr, oahdr;
struct filehdr fhdr, ofhdr;
struct scnhdr  scns[MAXSCNS];
int    nscns;
long cur_pos, sym_top;


main(argc, argv)
char **argv;
{
    char *new_name, *a_out_name = "a.out", *core_name = "core";
    FILE *new, *a_out, *core;

    if (scanargs(argc, argv, "undump new-a.out!s a.out%s core%s",
	    &new_name, &a_out_name, &core_name)
		    != 1)
	exit(1);

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
	
	int i;

/* sym_top points to the beginning of the line#/sym/str data */

/*
 *  read file header from a.out
 */

    if (fread(&fhdr, sizeof fhdr, 1, a_out) != 1)
    {
	perror("Couldn't read file header from a.out file");
	exit(1);
    }

/*
 *  read system header from a.out COFF format
 */

    if (fhdr.f_opthdr > 0) 
    {
	if (fread(&ahdr, sizeof ahdr, 1, a_out) != 1)
	{
	    perror("Couldn't read system file header from a.out file");
    	    exit(1);
	}

/*
 *  read section headers from a.out COFF format
 */
	
	nscns = fhdr.f_nscns;
	for(i=0;(i != nscns);i++) {
	    if (fread(&scns[i], sizeof(struct scnhdr), 1, a_out) != 1) {
	    	perror("Couldn't read section headers from a.out file");
		exit(1);
	        }
	    }
    }
  

/*    if N_BADMAG(hdr)
    {
	fprintf(stderr, "a.out file doesn't have legal magic number\n");
	exit(1);
    } */



    if (ahdr.magic != u.pt_aouthdr.magic ||
	ahdr.tsize != u.pt_aouthdr.tsize ||
	ahdr.dsize != u.pt_aouthdr.dsize ||
	ahdr.entry != u.pt_aouthdr.entry)
    {
	fprintf(stderr, "Core file didn't come from this a.out\n");
	exit(1);
    }

    printf("Data segment size was %u", ahdr.dsize);
    ahdr.dsize = u.pt_dsize;
    ahdr.bsize = 0;			/* all data is inited now! */
    printf(" now is %u\n", ahdr.dsize);


/*
 *  point to top of old line#/symbol/string info if there is any so 
 *  we can find it later...
 */

    sym_top = 0;
    if (scns[0].s_nlnno != 0) 
	sym_top = scns[0].s_lnnoptr;
    else if (fhdr.f_nsyms != 0)
	sym_top = fhdr.f_symptr;

    scns[1].s_size = u.pt_dsize;
    scns[2].s_size = 0;
    scns[2].s_vaddr = 0;
    scns[2].s_paddr = 0;

    cur_pos = scns[1].s_scnptr + scns[1].s_size;
   
    if (scns[0].s_nlnno != 0) {
	scns[0].s_lnnoptr = cur_pos;
        cur_pos += scns[0].s_nlnno * sizeof(struct lineno);
	}
    if (fhdr.f_nsyms != 0) {
	fhdr.f_symptr = cur_pos;
	}

    


    if (fwrite(&fhdr, sizeof fhdr, 1, new) != 1)
    {
	perror("Couldn't write file header to new a.out file");
	exit(1);
    }
    if (fhdr.f_opthdr > 0) 
    {
	if (fwrite(&ahdr, sizeof ahdr, 1, new) != 1)
	{
	    perror("Couldn't write system file header to new a.out file");
    	    exit(1);
	}
	for(i=0;(i != nscns);i++) {
	    if (fwrite(&scns[i], sizeof(struct scnhdr), 1, new) != 1) {
	    	perror("Couldn't write section headers to new a.out file");
		exit(1);
	        }
	    }
    }
	
}

/*
 * Copy the text from the a.out to the new a.out
 */
copy_text(new, a_out)
FILE *new, *a_out;
{
    char page[PSIZE];
    int txtcnt;

    txtcnt = ahdr.tsize;

    fseek(new, scns[0].s_scnptr, 0);
    fseek(a_out, scns[0].s_scnptr, 0);

/*    if (hdr.a_magic == OMAGIC)
    {
	printf("a.out file is not shared text, getting text from core file\n");
	fseek(a_out, hdr.a_text, 1);	* skip over text *
	return;
    } */


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
    int datacnt;


    datacnt = u.pt_dsize;

/*    if (hdr.a_magic == OMAGIC)
	datacnt += u.u_tsize; */
    fseek(core,(long) sizeof(struct ptrace_user), 0);
    fseek(new, scns[1].s_scnptr, 0);
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

    fseek(a_out, sym_top, 0);	/* skip over data segment */
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

