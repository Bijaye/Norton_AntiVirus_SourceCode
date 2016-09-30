# solaris_make.inc
# This file is included by the more general unixmake.inc.
# This implementation is for Solaris and the GNU compiler.
#


# Set the C++ compiler.
CCC = gcc

# Set the library (archive) manager.
AR = /usr/xpg4/bin/ar

# Set the compiler flags specific to NAV.
CCFLAGS += -DBIG_ENDIAN -DSYM_SOLARIS

# Flags that are enabled by -O/-O1
O1OPTFLAGS = \
-fthread-jumps \
-fdefer-pop \
-fdelayed-branch \
-fomit-frame-pointer

# Flags that are enabled by -O2
O2OPTFLAGS = $(O1OPTFLAGS) \
-fstrength-reduce \
-fcse-follow-jumps \
-fcse-skip-blocks \
-frerun-cse-after-loop \
-felide-constructors \
-fexpensive-optimizations \
-fschedule-insns \
-fschedule-insns2 \
-ffloat-store \
-fdefault-inline \
-fforce-addr \
-fforce-mem \
-fcaller-saves \
-ffast-math \
-ffunction-cse \
-fpeephole \
-fkeep-inline-functions

# Flags that are enabled by -O3
O3OPTFLAGS = $(O1OPTFLAGS) $(O2OPTFLAGS) -finline-functions


# Set the compiler flags that are specific to the 'debug' and 'release' targets
# -O1 or -O2 combined with -fpic or -fPIC cause bugs in navapi/search.cpp
# and elsewhere, so instead -Os and $(O2OPTFLAGS) are used.
ifdef ASDEBUG
    CCFLAGS += -ggdb3
else
    # We are keeping optimizatins off for the moment until we can
    # figure out which one is causing problems (specifically for
    # superman)
    #CCFLAGS += $(O2OPTFLAGS)
endif

# Set the linker flags required to build shared libraries.
# This should be appended to LDFLAGS in makefiles that build a shared library.
LDSHAREDFLAGS = -shared -symbolic -z defs -z text -lc

# Set the linker flags for dynamic linking and Posix thread support.
SYSLIBS += -ldl -lpthread 