# unix.mak
# Unix makefile for the pam (emulator) library.

TARGETS = libpam.a
include ../unixmake.inc

OBJS = \
cache.o \
cover.o \
cpu.o \
datafile.o \
exclude.o \
exec.o \
flags.o \
instr.o \
instr2.o \
opcodele.o \
page.o \
pamapi.o \
repair.o \
search.o \
sigex.o
 
libpam.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
