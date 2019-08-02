#include <stdio.h>
#include <sys/param.h>
#include <sys/file.h>
#include <aouthdr.h>
#include <sys/ptrace.h>

struct ptrace_user u;


main(argc,argv) 
int argc;
char *argv[];
{

	int fd;

	if (argc < 2) {
		printf("give me a break, and a file...\n");
		exit(0);
		}

	fd = open(argv[1],O_RDONLY);
	read(fd,&u,sizeof (struct ptrace_user));
	printf("sizeof ptrace:  %o\n",sizeof(struct ptrace_user));
	printf("dsize:		%lo\n",u.pt_dsize);
	printf("ssize:		%lo\n",u.pt_ssize);
	printf("magic:		%o\n",u.pt_aouthdr.magic);
	printf("vstamp:		%o\n",u.pt_aouthdr.vstamp);
	printf("tsize:		%lo\n",u.pt_aouthdr.tsize);
	printf("dsize:		%lo\n",u.pt_aouthdr.dsize);
	printf("bsize:		%lo\n",u.pt_aouthdr.bsize);
	printf("msize:		%lo\n",u.pt_aouthdr.msize);
	printf("mod_start:	%lo\n",u.pt_aouthdr.mod_start);
	printf("entry:		%lo\n",u.pt_aouthdr.entry);
	printf("text_start:	%lo\n",u.pt_aouthdr.text_start);
	printf("data_start:	%lo\n",u.pt_aouthdr.data_start);
	printf("entry_mod:	%lo\n",u.pt_aouthdr.entry_mod);
	printf("flags:		%o\n",u.pt_aouthdr.flags);
	printf("\n");
	close(fd);
	}
		

