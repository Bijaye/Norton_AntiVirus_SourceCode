# linux_x86_make.inc
# This file is included by the more generla unixmake.inc.
# This implementation is for Linux x86 and the GNU compiler.
#

# Set the C++ compiler.
CCC = gcc

# Set the library (archive) manager.
AR = /usr/bin/ar

# Set the compiler flags specific to Linux x86.
CCFLAGS += -DSYM_BSD_X86

# Set the compiler flags that are specific to the 'debug' and 'release' targets
# -O1 or -O2 combined with -fpic or -fPIC cause bugs in navapi/search.cpp
# and elsewhere, so instead -Os and $(O2OPTFLAGS) are used.
ifdef ASDEBUG
    CCFLAGS += -ggdb
else
    CCFLAGS += -O2
endif

# Set the linker flags required to build shared libraries.
# This should be appended to LDFLAGS in makefiles that build a shared library.
LDSHAREDFLAGS = -shared -Bsymbolic -z defs -z text -lc

SYSLIBS += -pthread