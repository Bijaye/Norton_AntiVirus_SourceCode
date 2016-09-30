# unixmake.inc
# This file is included by all NAV unix.mak files.
# This implementation is for Solaris, the GNU compiler, and GNU make.
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

# Load the platform specific makefile
ifdef PLATFORM
    include ../$(PLATFORM)_make.inc
endif

# Prepend .cpp to the suffix list.
.SUFFIXES:
.SUFFIXES: .cpp $(SUFFIXES)

# Add rule for building .o files from .cpp files.
.cpp.o: 
	$(CCC) $(CCFLAGS) -c $*.cpp

# Set the compiler flags specific to NAV on all platforms
CCFLAGS += -DSYM_UNIX -DNO_TCSAPI
CCFLAGS += -I../inc.avcore -I../inc_english.avcore -I../inc.core

ifdef ASDEBUG
    CCFLAGS += -DSYM_DEBUG
endif

# Set the compiler flags for Posix thread support.
CCFLAGS += -D_REENTRANT -D_POSIX_PTHREAD_SOURCE=199506L

# Set the linker flags that are specific to the 'debug' and 'release' targets.
ifdef ASDEBUG
    LDFLAGS += -L../bin.debug -g
else
    LDFLAGS += -L../bin.release
endif

# Set the linker library flags for dynamic linking (dlopen(), etc.).
#SYSLIBS += -ldl

# Set the linker library flags for for Posix thread support.
# This library must be LAST on the link line, after the .o's and other libs.
#SYSLIBS += -lpthread


# Set the compiler flags for position indepenent code.
# This is required when building shared libraries (.so).
# Always set it, in case a static library (.a) is used in a shared library.
CCFLAGS += -fPIC

# Set the output directory
ifdef ASDEBUG
    OUTPUTDIR = ../bin.debug
else
    OUTPUTDIR = ../bin.release
endif

# Rules for the standard targets

all: $(TARGETS)
	cp $? $(OUTPUTDIR)

#release: all
#
#debug:
#	@ASDEBUG=1 $(MAKE) -f unix.mak all

clean:
	rm -f *.o $(TARGETS)
