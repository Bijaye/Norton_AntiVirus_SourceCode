# unix.mak
# Unix makefile for the df library.

TARGETS = libdf.a
include ../unixmake.inc

CCFLAGS += $(CCSHAREDFLAGS)

OBJS = \
df.o \
dftree.o \
dfzip.o \
if.o

libdf.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
