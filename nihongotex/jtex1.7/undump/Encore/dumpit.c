#include <stdio.h>
#include <sys/file.h>
#include <filehdr.h>
#include <aouthdr.h>
#include <scnhdr.h>


#define MAXSCNS 16
struct filehdr hdr;
AOUTHDR       shdr;
struct scnhdr scns[MAXSCNS];
int nscns; 


main(argc,argv)
int argc;
char *argv[];
{
	
	int fd,i;
	
	if (argc < 2) {
		printf("give me a break, and a file...\n");
		exit(0);
		}
	fd = open(argv[1],O_RDONLY);
	read(fd,&hdr,sizeof(struct filehdr));
	nscns = hdr.f_nscns;
	printf("magic: 		%o\n",hdr.f_magic);
	printf("sections:	%o\n",hdr.f_nscns);
	printf("timestamp:	%lo\n",hdr.f_timdat);
	printf("sym ptr:	%lo\n",hdr.f_symptr);
	printf("num sym ents:  	%lo\n",hdr.f_nsyms);
	printf("size of syshdr:	%o\n",hdr.f_opthdr);
	printf("flags:		%o\n",hdr.f_flags);
	printf("\n");
	if (hdr.f_opthdr != 0) {
		read(fd,&shdr,sizeof(AOUTHDR));
		printf("magic:		%o\n",shdr.magic);
		printf("vstamp:		%o\n",shdr.vstamp);
		printf("tsize:		%lo\n",shdr.tsize);
		printf("dsize:		%lo\n",shdr.dsize);
		printf("bsize:		%lo\n",shdr.bsize);
		printf("msize:		%lo\n",shdr.msize);
		printf("mod_start:	%lo\n",shdr.mod_start);
		printf("entry:		%lo\n",shdr.entry);
		printf("text_start:	%lo\n",shdr.text_start);
		printf("data_start:	%lo\n",shdr.data_start);
		printf("entry_mod:	%lo\n",shdr.entry_mod);
		printf("flags:		%o\n",shdr.flags);
		printf("\n");
		}
        for(i=0;(i != nscns);i++) {
	  	read(fd,&scns[i],sizeof(struct scnhdr));
		}
	for(i=0;(i != nscns);i++) {
		printf("name:		%s\n",scns[i].s_name);
		printf("paddr:		%lo\n",scns[i].s_paddr);
		printf("vaddr:		%lo\n",scns[i].s_vaddr);
		printf("size:		%lo\n",scns[i].s_size);
		printf("scnptr:		%lo\n",scns[i].s_scnptr);
		printf("relptr:		%lo\n",scns[i].s_relptr);
		printf("lnnoptr:	%lo\n",scns[i].s_lnnoptr);
		printf("nreloc:		%o\n",scns[i].s_nreloc);
		printf("nlnno:		%o\n",scns[i].s_nlnno);
		printf("flags:		%lo\n",scns[i].s_flags);
		printf("symptr:		%lo\n",scns[i].s_symptr);
		printf("modno:		%o\n",scns[i].s_modno);
		printf("\n");
		}
	close(fd);
	}
		

