# unix.mak
# Unix makefile for the crc32 library.

TARGETS = libcrc32.a
include ../unixmake.inc

CCFLAGS += $(CCSHAREDFLAGS)

OBJS = crc32.o
 
libcrc32.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
