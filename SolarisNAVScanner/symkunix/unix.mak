# unix.mak
# Unix makefile for the symkunix (SymKernal for Unix) library.
# This only implemements the Core library routines needed for navapi, etc.

TARGETS = libsymkunix.a
include ../unixmake.inc

OBJS = \
unixfile.o \
unixmem.o \
unixxapi.o

libsymkunix.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
