##############################################################################
#       Written By CTS
#       This is the Makefile for Emulator.
##############################################################################

OBJS = cache.o callfake.o cover.o cpu.o datafile.o exclude.o exec.o \
	flags.o instr.o instr2.o opcodele.o page.o pamapi.o repair.o \
	search.o sigex.o
INCL = ../include

libemulatorlib.a: $(OBJS)
	ar -rv libemulatorlib.a $(OBJS)
$(OBJS): $$(@F:.o=.cpp)
	CC -O4 -misalign -mt -c -DSYM_UNIX -DBIG_ENDIAN -DUSE_CALLFAKE -I$(INCL) $(@F:.o=.cpp)

clean:
	rm *.o ; rm libemulatorlib.a
