# unixmake.inc
# This file is included by all NAV unix.mak files.
# This implementation is for Solaris and the GNU compiler.
#
# A typical unix.mak file looks like this:
#
#     # unix.mak
#     # Unix makefile for the foo library and footest program.
#
#     TARGETS = libfoo.a testfoo
#     include ../unixmake.inc
#
#     OBJS = foo.o foo2.o
#
#     TESTOBJS = testfo.o
#
#     libfoo.a: $(OBJS)
#         $(AR) $(ARFLAGS) $@ $(OBJS)
#
#     testfoo: $(TESTOBJS) libfoo.a
#         $(CCC) $(LDFLAGS) $(TESTOBJS) libfoo.a ($SYSLIBS) -o $@

# Set the C++ compiler.
CCC = gcc

# Set the library (archive) manager.
AR = /usr/xpg4/bin/ar

# Prepend .cpp to the suffix list.
.SUFFIXES:
.SUFFIXES: .cpp $(SUFFIXES)

# Add rule for building .o files from .cpp files.
.cpp.o: 
	$(CCC) $(CCFLAGS) -c $*.cpp

# Set the compiler flags specific to NAV.
CCFLAGS += -DSYM_UNIX -DBIG_ENDIAN -DSYM_SOLARIS
CCFLAGS += -I../inc.avcomp -I../inc.avcore -I../inc.core -I../inc.tcsapi
debug := CCFLAGS += -DSYM_DEBUG

# Set the compiler flags for Posix thread support.
CCFLAGS += -D_REENTRANT -D_POSIX_PTHREAD_SOURCE=199506L

# Set the compiler flags that are specific to the 'debug' and 'release' targets.
debug := CCFLAGS += -g
release := CCFLAGS += -O2

# Set the linker flags that are specific to the 'debug' and 'release' targets.
debug := LDFLAGS += -L../bin.debug
release := LDFLAGS += -L../bin.release -g

# Set the linker library flags for dynamic linking.
SYSLIBS += -ldl

# Set the linker library flags for for Posix thread support.
# This library must be LAST on the link line, after the .o's and other libs.
SYSLIBS += -lpthread

# Set the compiler and linker flags required to build shared libraries.
# These should be appended to CCFLAGS and LDFLAGS when necessary.
CCSHAREDFLAGS = -fpic
LDSHAREDFLAGS = -shared -symbolic -z defs -z text -lc

# Rules for the standard targets

debug: $(TARGETS)
	cp $? ../libSolarisDebug

release: $(TARGETS)
	cp $? ../libSolarisRelease

clean:
	rm *.o $(TARGETS)
