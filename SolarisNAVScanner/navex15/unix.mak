# unix.mak
# Unix makefile for the navex15 library.

TARGETS = libnavex15.so
include ../unixmake.inc

CCFLAGS += -DNAVEX15 -DNDEBUG $(CCSHAREDFLAGS)

OBJS = \
	query.o \
	navexsf.o \
	navexsb.o \
	navexsp.o \
	navexsm.o \
	navexrf.o \
	navexrb.o \
	navexrp.o \
	navexrh.o \
	navexshr.o \
	nvxbtshr.o \
	navexent.o \
	booscan.o \
	bootype.o \
	datadir.o \
	inifile.o \
	trojscan.o \
	pespscan.o \
	aolpw.o

LIBS = \
	-lcrc32 \
	-ldf

MACROLIBS = \
	-lolessapi \
	-lwd7api \
	-lo97api \
	-lxl5api \
	-lmacroheu \
	-lacsapi \
	-lppapi

MSXOBJS = \
	msx.o \
	msxadd.o \
	msxcrc.o \
	msxdb.o \
	msxl1.o \
	msxl2.o

MACROOBJS = \
	wddecsig.o \
	ldmcrsig.o \
	mcrscn15.o \
	mcrrpr15.o \
	offcsc15.o \
	wdapsg15.o \
	wdavsg15.o \
	wdscan15.o \
	wdrepr15.o \
	wdsigutl.o \
	mcrsgutl.o \
	wd7scn15.o \
	o97scn15.o \
	xl97rpr.o \
	o97rpr15.o \
	olessos.o \
	mcrheusc.o \
	wd7heusc.o \
	mvp.o \
	copy.o \
	builtin.o \
	mcrxclud.o \
	w97heusc.o \
	w97obj.o \
	w97str.o \
	xl5scn.o \
	xl5rpr.o \
	xl4scn.o \
	xl4rpr.o \
	xlrprsht.o \
	a2scn.o \
	a97scn.o \
	acsos.o \
	w7h2sght.o \
	w7h2scan.o \
	excelheu.o \
	xlheusc.o \
	dfos.o \
	ppscan.o \
	nvx15inf.o \
	$(MSXOBJS)

NOMACROOJBS = \
	ldmcrsig.o \
	dfos.o

NEPEOBJS = \
	nepescan.o \
	nepeshr.o \
	neperep.o \
	remotex.o \
	winsig.o 

JAVAOBJS = \
	javascan.o \
	javashr.o \
	javasig.o

ENTRYOBJS = \
	navex15.o

# Not used in Unix:
#    naventry.o

#libnavex15.a: $(OBJS) $(MACROOBJS) $(NEPEOBJS) $(JAVAOBJS) $(ENTRYOBJS)
#	$(AR) $(ARFLAGS) $@ $(OBJS) $(MACROOBJS) $(NEPEOBJS) $(JAVAOBJS) $(ENTRYOBJS)

libnavex15.so: $(OBJS) $(MACROOBJS) $(NEPEOBJS) $(JAVAOBJS) $(ENTRYOBJS)
	$(CCC) $(LDFLAGS) $(LDSHAREDFLAGS) $(OBJS) $(MACROOBJS) $(NEPEOBJS) $(JAVAOBJS) $(ENTRYOBJS) $(LIBS) $(MACROLIBS) -o $@
