# Copyright 2004-2005 Symantec Corporation. All rights reserved.
# Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
# Intent: Basic GNU Make variables and rules for RTVScan Linux port

Top = $(Root)../../../..

AntiVirus		  = $(Top)/Norton_AntiVirus
AMS				  = $(Release)/AMS
AVCore			  = $(CorporateEdition)/Win32/src/AVCore
Common			  = $(CorporateEdition)/Common/src
CorporateEdition = $(AntiVirus)/Corporate_Edition
Dec3				  = $(Linux_Security)/dev/dec3
DefUtils			  = $(Release)/DefUtils
ECOM				  = $(Release)/ECOM
InternetSecurity = $(Release)/Norton_Internet_Security
LDVP				  = $(Top)/Core_Technology/LDVP_Shared/src
LDVPmisc			  = $(LDVP)/Common/Misc
License			  = $(Release)/SLIC
Linux				  = $(CorporateEdition)/Linux/src
Linux_Security	  = $(Top)/Symantec_Client_Security/Linux_Security
MD5				  = $(AntiVirus)/Shared/MD5
MD5Util			  = $(MD5)/MD5Util
MakeMsgs			  = $(Common)/MakeMsgs
NavGlue			  = $(Common)/NAV_Glue
NetWare			  = $(CorporateEdition)/NetWare/src
NetwareSDK		  = $(Top)/Tools/NDK1004/nwsdk
PScan				  = $(Common)/PScan
QuarantineServer = $(AntiVirus)/QuarantineServer
Release			  = $(Top)/Release
SESA_Collector	  = $(Release)/SESA_Collector_Framework/r1.01
SESA_Connector	  = $(Top)/Enterprise_Systems_SESACollectors/Collector_Framework_Plugins/Common/Connector
SymSync			  = $(Common)/Utils/sym_synch
TCSAPI			  = $(Release)/tcsapi
VPReg				  = $(NetWare)/VPReg

SHELL = /bin/sh

CPP			= cpp
CC				= gcc -x c
CXX			= gcc -x c++
AR				= ar -rv
LD				= gcc
INSTALL		= install
INSTALLDATA = install -m 644
LN				= ln -s
LS				= @-ls -l
RM				= @rm -f
CP				= cp -f
ACCESS		= @chmod go=rX

ifdef DEBUG
	libDir  = debug
	LibDir  = Debug
	LIBDIR  = $(Linux_Security)/src/lib.ida
	BINDIR  = $(Linux_Security)/src/bin.ida
	DFLAGS  = -g
	OFLAGS  = -O
	LFLAGS  =
else
	libDir  = release
	LibDir  = Release
	LIBDIR  = $(Linux_Security)/src/lib.ira
	BINDIR  = $(Linux_Security)/src/bin.ira
	macros += NDEBUG
	DFLAGS  =
	OFLAGS  = -O3
	LFLAGS  = -s
endif

ifdef CMD
	LFLAGS += -v
endif
ifdef MAP
	LFLAGS += -Wl,-M
endif

Slibs		+= stdc++ gcc
ifneq ($(wildcard /usr/lib/gcc-lib/i386-redhat-linux/*/libgcc_eh.a),)
	Slibs += gcc_eh
endif

macros	+= LINUX UNIX SYM_LINUX SYM_UNIX POSIX LINUX_DEMO LINUX_DISABLE_SLIC _MBCS\
				_REENTRANT __stdcall= __cdecl=
includes += $(Linux)/include
paths		+= $(LIBDIR)
slibs		+= ndkFunctions $(Slibs)
dlibs		+= pthread m

ifneq ($(slibs),)
	StaticDependentLibs += ndkFunctions
else
	DependentLibs		  += ndkFunctions
endif

# The $(Linux)/include directory *MUST* be first in the include search path. For parts of
# the code that we do not have permission to edit, we have to override their include
# files. For instance, we have a number of empty include files to handle the situation
# when the non-editable files includes files that do not exist on Linux.

CFLAGS	 =
CXXFLAGS	 =
GCCFLAGS	 = $(DFLAGS) $(OFLAGS) -w
CPPFLAGS	 = $(addprefix -D,$(Macros) $(macros)) $(addprefix -I,$(includes) $(Includes))

LDFLAGS	 = $(LFLAGS)
LDPATHS	 = $(addprefix -L,$(paths) $(LinkPaths))

# run -v, edit collect2 command line to remove: -lstdc++ -lgcc -lgcc_s after -Bdynamic
# to get them removed from the ldd list

LDLIBS  = -Wl,-Bstatic\
			 -Wl,--start-group $(addprefix -l,$(slibs) $(StaticLinkLibs)) -Wl,--end-group\
			 -Wl,-Bdynamic $(addprefix -l,$(dlibs) $(LinkLibs))

define preprocess
	$(CPP) -o $@ $(CPPFLAGS) $<
endef

define compile
	$($(1)) -MD $($(2)FLAGS) $(GCCFLAGS) -c $< $(CPPFLAGS) -o $@
endef

define mkStaticLibrary
	$(AR) $@ $+
endef

define mkDynamicLibrary
	$(LD) $(LDFLAGS) -shared -o $@ $+
endef

define mkBinary
	$(LD) $(LDFLAGS) -o $@ $+ $(LDPATHS) $(LDLIBS) $(Modules)
	$(ACCESS) $@
endef

define copy
	$(CP) $< $@
endef

define mkObjects
	$(patsubst %.c,%.o,$(patsubst %.C,%.o,\
	$(patsubst %.cpp,%.o,$(patsubst %.CPP,%.o,\
	$(1)))))
endef

Init				= cases
Dependents		= $(Objects:%.o=%.d)
Objects			= $(call mkObjects,$(Sources))
StaticObjects	= $(call mkObjects,$(StaticSources))
StaticLibrary	= $(StaticLib:%=lib%.a)
DynamicObjects = $(call mkObjects,$(DynamicSources))
DynamicLibrary = $(DynamicLib:%=lib%.so)
Lists				= $(strip $(Sources) $(StaticSources) $(DynamicSources) $(List))

LIBDIRdependentlibs = $(addprefix $(LIBDIR)/lib,$(addsuffix .a,$(StaticDependentLibs)))\
							 $(addprefix $(LIBDIR)/lib,$(addsuffix .so,$(DependentLibs)))
LIBDIRlibs			  = $(addprefix $(LIBDIR)/,$(StaticLibrary) $(DynamicLibrary))
BINDIRexecutable	  = $(addprefix $(BINDIR)/,$(Executable))

targets = all clean install uninstall test list dlist

all		 : target =
clean		 : target = clean
install	 : target = install
uninstall : target = uninstall
test		 : target = test
list		 : target = list
dlist		 : target = dlist

.PHONY	 : $(targets) $(Directories) $(Init) $(Phony)
.PRECIOUS : $(Dependents) $(Objects) $(StaticObjects) $(DynamicObjects)\
				$(LIBDIRdependentlibs)\
				$(Binary) $(Binaries) $(Precious)
.SUFFIXES :

$(targets) : $(Directories)

$(Directories) :
	$(MAKE) -C $@ $(target)

cases : $(Linux)/include/CBA.h $(Linux)/include/NTS.h

$(Linux)/include/CBA.h :
	cd $(dir $@); $(LN) cba.h $(notdir $@)

$(Linux)/include/NTS.h :
	cd $(dir $@); $(LN) nts.h $(notdir $@)

%.o : %.c
	$(call compile,CC,C)

%.o : %.C
	$(call compile,CC,C)

%.o : %.cpp
	$(call compile,CXX,CXX)

%.o : %.CPP
	$(call compile,CXX,CXX)

%.a : $(StaticObjects)
	$(call mkStaticLibrary)

%.so : $(DynamicObjects)
	$(call mkDynamicLibrary)

%.bin : $(Main) $(Objects) $(LIBDIRdependentlibs)
	$(call mkBinary)

%.tst : %.bin
	./$< $(TEST)

$(BINDIRexecutable) : $(Binary)
	$(call copy)

ifneq ($(Dependents),)
-include $(Dependents)
endif

all : $(Init) $(BINDIRexecutable) $(Binary) $(Targets) $(LIBDIRlibs)

ifneq ($(Lists),)
list : $(Lists)
	$(LS) $^
endif

dlist :
ifneq ($(Dependents),)
	@egrep -h '^[[:space:]]*\.\./' $(Dependents) $(Dlist) |\
	sed -e 's/[[:space:]]*\\//' | egrep -i '\.(h|hpp)'
endif

clean :
	$(RM) $(Dependents) $(Objects) $(StaticObjects) $(DynamicObjects)\
			$(StaticLibrary) $(DynamicLibrary) $(LIBDIRlibs)\
			$(Binary) $(BINDIRexecutable) $(Binaries) $(Precious)\
			core.[0-9]* $(Clean)
