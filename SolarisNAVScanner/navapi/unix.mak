# unix.mak
# Unix makefile for the navapi library and apitest program.

TARGETS = libnavapi.a apitest
include ../unixmake.inc

CCFLAGS += -DAVAPI_MT

PAMOBJS = \
cache.o \
cover.o \
cpu.o \
datafile.o \
exclude.o \
exec.o \
flags.o \
instr.o \
instr2.o \
page.o \
pamapi.o \
repair.o \
search.o \
sigex.o

OBJS = \
avapi.o \
boot.o \
file.o \
info.o \
init.o \
memory.o \
navcb.o \
static.o \
virtcb.o 

# The following are not used for Unix
#apenable.o \
#navapinw.o \
#navl311.o \
#navl312.o \
#nlmstuff.o \

TESTOBJS = \
test.o

libnavapi.a: $(PAMOBJS) $(OBJS) 
	$(AR) $(ARFLAGS) $@ $(PAMOBJS) $(OBJS)

apitest: $(TESTOBJS) libnavapi.a
	$(CCC) $(LDFLAGS) $(TESTOBJS) libnavapi.a -lavenge -lsymkunix $(SYSLIBS) -o $@
