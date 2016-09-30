# unix.mak
# Unix makefile for the naveng (engine15) library and engtest program

TARGETS = libnaveng.so engtest
include ../unixmake.inc

CCFLAGS += -DNAVEX15 -DUSE_HEUR $(CCSHAREDFLAGS)

OBJS = \
callfake.o \
datadir.o \
datafile.o \
inifile.o \
navexent.o \
navexrb.o \
navexrf.o \
navexrp.o \
navexsb.o \
navexsf.o \
navexshr.o \
navexsm.o \
navex15.o \
navexsp.o \
query.o \
strscan.o 

# Not used in Unix:
#naventry.o \

TESTOBJS = \
engtest.o 

#libnaveng.a: $(OBJS)
#	$(AR) $(ARFLAGS) $@ $(OBJS)

#engtest: $(TESTOBJS) libnaveng.so
#	$(CCC) $(LDFLAGS) $(TESTOBJS) libnaveng.a -lheur $(SYSLIBS) -o $@

libnaveng.so: $(OBJS)
	$(CCC) $(LDFLAGS) $(LDSHAREDFLAGS) $(OBJS) -lheur $(SYSLIBS) -o $@

engtest: $(TESTOBJS) libnaveng.so
	$(CCC) $(LDFLAGS) $(TESTOBJS) libnaveng.so $(SYSLIBS) -o $@
