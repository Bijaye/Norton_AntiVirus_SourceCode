# unix.mak
# Unix makefile for the symkunix (SymKernal for Unix) library.
# This only implemements the Core library routines needed for navapi, etc.

TARGETS = libnavvdl.a
include ../unixmake.inc

OBJS = \
navvdl.o

libnavvdl.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
