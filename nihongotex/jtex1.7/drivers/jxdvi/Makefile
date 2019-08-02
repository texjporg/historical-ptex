#
# Makefile for dvi previewer.
#
# $Header: Makefile,v 1.5 88/09/06 23:31:01 kagotani Exp $

BINDIR	      = /usr/bin/X11

MANDIR	      = /usr/man/mann

HDRS	      =

LDFLAGS	      = -lX11

LIBS	      =

LINKER	      = $(CC) $(CFLAGS)

MAKEFILE      = Makefile

OBJS	      = dv.o \
		font.o \
		x11.o

PROGRAM	      = xdvi

MANUAL        =	xdvi.n

SRCS	      = dv.c \
		font.c \
		x11.c

#CFLAGS	      =	-O
CFLAGS        = -g -DDEBUG

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		@echo -n "Loading $(PROGRAM) ... "
		@$(LINKER) $(OBJS) $(LIBS) $(LDFLAGS) -o $(PROGRAM)
		@echo "done"

clean:;		@rm -f $(OBJS)

install:	install-$(PROGRAM) install-$(MANUAL)

install-$(PROGRAM):	$(PROGRAM)
		install -s $(PROGRAM) $(BINDIR)

install-$(MANUAL):	$(MANUAL)
		install -m 444 $(MANUAL) $(MANDIR)

$(BINDIR)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS)
		@make -f $(MAKEFILE) BINDIR=$(BINDIR) install

###
dv.o: dvi.h pxl.h
font.o: dvi.h pxl.h
x11.o: dvi.h pxl.h

