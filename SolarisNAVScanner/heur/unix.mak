# unix.mak
# Unix makefile for the heur library and heurtest program.

TARGETS = libheur.a heurtest
include ../unixmake.inc

CCFLAGS += -DVC20 -DUSE_CALLFAKE $(CCSHAREDFLAGS)

OBJS = \
ba.o \
behav.o \
cpu.o \
crc32.o \
datafile.o \
exec.o \
flags.o \
heur2.o \
heurint.o \
instr.o \
instr2.o \
page.o \
pamapi.o \
sigex.o

TESTOBJS = \
callfake.o \
main.o 
# Not used in Unix:
#pathname.o \
#filefind.o \

libheur.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

heurtest: $(TESTOBJS) libheur.a
	$(CCC) $(LDFLAGS) $(TESTOBJS) libheur.a $(SYSLIBS) -o $@
