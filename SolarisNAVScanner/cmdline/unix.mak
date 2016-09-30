# unix.mak
# Unix makefile for the cmdline test program.
#
# Copy virus definition files to the program directory before running it.
# Run cmdline -HELP for options.

TARGETS = cmdline
include ../unixmake.inc

OBJS = \
clcb.o \
clnavex.o \
cmdhelp.o \
cmdline.o \
cmdparse.o \
cmdstr.o \
ext.o \
filefind.o \
helpstr.o \
msgstr.o \
pathname.o \
report.o \
scanfile.o \
scaninit.o \
scanmem.o \
virlist.o 
# scanboot.o is not used in Unix

cmdline: $(OBJS) 
	$(CCC) $(LDFLAGS) $(OBJS) -lavenge -lpam -lsymkunix $(SYSLIBS) -o $@
