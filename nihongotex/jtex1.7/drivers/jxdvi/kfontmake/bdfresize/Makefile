BINDIR	      = /usr/bin/X11

MANDIR	      = /usr/man/mann

HDRS	      =

LDFLAGS	      =

LIBS	      =

LINKER	      = $(CC) $(CFLAGS)

MAKEFILE      = Makefile

OBJS	      = bdfresize.o \
		charresize.o

PROGRAM	      = bdfresize

MANUAL        =	bdfresize.n

SRCS	      = bdfresize.c \
		charresize.c

CFLAGS	      =	-O

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		@echo -n "Loading $(PROGRAM) ... "
		@$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
		@echo "done"

clean:;		@rm -f $(OBJS)

install:	$(PROGRAM) $(MANUAL)
		install -s $(PROGRAM) $(BINDIR)
		install -m 444 $(MANUAL) $(MANDIR)

$(BINDIR)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS)
		@make -f $(MAKEFILE) BINDIR=$(BINDIR) install
###
