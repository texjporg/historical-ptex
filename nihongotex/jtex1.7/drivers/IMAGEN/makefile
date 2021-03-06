#
#	makefile for `dvi2'
#					$Date: 89/08/11 11:08:42 $
#

DEST	      = /usr/local/bin

EXTHDRS	      = /usr/include/ctype.h \
		/usr/include/fcntl.h \
		/usr/include/floatingpoint.h \
		/usr/include/imagen/imPcodes.h \
		/usr/include/math.h \
		/usr/include/stdio.h \
		/usr/include/sys/fcntlcom.h \
		/usr/include/sys/file.h \
		/usr/include/sys/ieeefp.h \
		/usr/include/sys/sysmacros.h \
		/usr/include/sys/types.h

HDRS	      = amble.h \
		config.h \
		defs.h \
		dvicom.h \
		fio.h \
		font.h \
		jxl4.h \
		page.h \
		pdl.h \
		site.h \
		special.h \
		stack.h

CFLAGS	      = -DJXL
#CFLAGS	      = -DJXL -g -O -traditional -f68881

LDFLAGS	      =
#LDFLAGS      = -g # -f68881

CC	      = cc
#CC	      = gcc

LINKER	      = cc
#LINKER	      = gcc

LIBS	      =

MAKEFILE      = makefile

OBJS	      = DEBUG.o \
		PS.o \
		abspath.o \
		check.o \
		code.o \
		dbSPECIAL.o \
		fio.o \
		fntdef.o \
		fontglyph.o \
		fontindex.o \
		fontinit.o \
		fontopen.o \
		fontsuffix.o \
		imPRESS.o \
		imSPECIAL.o \
		jxl4.o \
		lib.o \
		link.o \
		main.o \
		malloc.o \
		page.o \
		pdl.o \
		psSPECIAL.o \
		special.o \
		stack.o \
		unit.o \
		version.o

PRINT	      = pr

PROGRAM	      = dvi2

SRCS	      = DEBUG.c \
		PS.c \
		abspath.c \
		check.c \
		code.c \
		dbSPECIAL.c \
		fio.c \
		fntdef.c \
		fontglyph.c \
		fontindex.c \
		fontinit.c \
		fontopen.c \
		fontsuffix.c \
		imPRESS.c \
		imSPECIAL.c \
		jxl4.c \
		lib.c \
		link.c \
		main.c \
		malloc.c \
		page.c \
		pdl.c \
		psSPECIAL.c \
		special.c \
		stack.c \
		unit.c \
		version.c

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		@echo -n "Loading $(PROGRAM) ... "
		@$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
		@echo "done"

clean:;		@rm -f $(OBJS)

depend:;	@mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

index:;		@ctags -wx $(HDRS) $(SRCS)

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		@install -s $(PROGRAM) $(DEST)

print:;		@$(PRINT) $(HDRS) $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS); @ctags $(HDRS) $(SRCS)

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install
###
DEBUG.o: /usr/include/stdio.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h dvicom.h pdl.h \
	stack.h font.h amble.h
PS.o: /usr/include/stdio.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h dvicom.h pdl.h \
	stack.h font.h
abspath.o: /usr/include/stdio.h config.h defs.h site.h
check.o: /usr/include/stdio.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h dvicom.h amble.h
code.o: /usr/include/stdio.h dvicom.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h stack.h
dbSPECIAL.o: /usr/include/stdio.h /usr/include/imagen/imPcodes.h config.h \
	defs.h site.h fio.h /usr/include/fcntl.h /usr/include/sys/fcntlcom.h \
	dvicom.h special.h
fio.o: /usr/include/stdio.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h
fntdef.o: /usr/include/stdio.h dvicom.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h font.h
fontglyph.o: /usr/include/stdio.h /usr/include/sys/file.h \
	/usr/include/sys/fcntlcom.h dvicom.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h font.h jxl4.h stack.h
fontindex.o: /usr/include/stdio.h /usr/include/sys/file.h \
	/usr/include/sys/fcntlcom.h dvicom.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h font.h jxl4.h
fontinit.o: /usr/include/stdio.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h
fontopen.o: /usr/include/stdio.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h
fontsuffix.o: /usr/include/stdio.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h
imPRESS.o: /usr/include/stdio.h /usr/include/imagen/imPcodes.h config.h \
	defs.h site.h fio.h /usr/include/fcntl.h /usr/include/sys/fcntlcom.h \
	dvicom.h pdl.h stack.h font.h
imSPECIAL.o: /usr/include/stdio.h /usr/include/math.h \
	/usr/include/floatingpoint.h /usr/include/sys/ieeefp.h \
	/usr/include/imagen/imPcodes.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h dvicom.h special.h
jxl4.o: /usr/include/stdio.h config.h defs.h site.h
lib.o: /usr/include/stdio.h config.h defs.h site.h
link.o: /usr/include/stdio.h /usr/include/ctype.h dvicom.h config.h defs.h \
	site.h fio.h /usr/include/fcntl.h /usr/include/sys/fcntlcom.h page.h
main.o: /usr/include/stdio.h config.h defs.h site.h page.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h
malloc.o: /usr/include/stdio.h /usr/include/sys/types.h \
	/usr/include/sys/sysmacros.h config.h defs.h site.h
page.o: /usr/include/stdio.h dvicom.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h
pdl.o: /usr/include/stdio.h config.h defs.h site.h pdl.h special.h
psSPECIAL.o: /usr/include/stdio.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h dvicom.h special.h
special.o: /usr/include/stdio.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h dvicom.h special.h
stack.o: /usr/include/stdio.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h dvicom.h stack.h
unit.o: /usr/include/stdio.h config.h defs.h site.h fio.h \
	/usr/include/fcntl.h /usr/include/sys/fcntlcom.h
