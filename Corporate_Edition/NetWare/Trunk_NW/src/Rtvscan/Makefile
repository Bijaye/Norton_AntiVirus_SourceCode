# Copyright 2004-2005 Symantec Corporation. All rights reserved.
# Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
# Intent: Rtvscan Makefile

Root = ../

Macros = SERVER WS_32 NO_VSNPRINTF LIC=NO
#ENFORCE_LICENSING


Includes = $(PScan)\
			  $(DebugNET)/include\
			  $(SymSafer)/include\
			  $(LDVP)/include\
			  $(LDVPmisc)\
			  $(LDVPscoms)\
			  $(QuarantineServer)/Shared/Include\
			  $(MD5)/include\
			  $(Dec3)/include\
			  $(SESA_Collector)/Include\
			  $(InternetSecurity)/include\
			  $(SESA_Connector)\
			  $(License)/include/slic\
			  $(SymSync)/include\
			  $(AMS)/include\
			  $(Linux_Security)/src/include\
			  $(NetWare)/include\
			  $(Common)/include\
			  $(Linux_Security)/src/symreg\
			  $(Linux_Security)/src/savtray/Common\
			  $(DefUtils)/include\
			  $(SavUtil)/include\


SKIP := \
			 $(LDVPmisc)/ScsCommsUtils.cpp\

GoodSources += \
			 $(LDVPmisc)/RegUtils.cpp\
			 $(LDVPmisc)/SIDUtils.cpp\
			 $(LDVPmisc)/vpstrutils.cpp\

SKIP += \
			 $(LDVPmisc)/RoleVector.cpp\
			 $(LDVPmisc)/SecureCommsEventInfo.cpp\

GoodSources += \
			 $(LDVPmisc)/EventCustomInfo.cpp\

SKIP += \
			 $(PScan)/IssueCerts.cpp\
			 $(PScan)/mynewop.cpp\

GoodSources += \
			 $(LDVPmisc)/EventBlockWrapper.cpp\
			 $(LDVPmisc)/OSEventLogger.cpp\

SKIP += \
			 $(PScan)/Checkin.cpp\
			 $(PScan)/OpState.cpp\
			 $(PScan)/OpStateMap.cpp\
			 $(PScan)/ClientTrack.cpp\
			 $(PScan)/ClientTrackCheckin.cpp\
			 $(PScan)/ClientTrackData.cpp\
			 $(PScan)/ClientTrackQueue.cpp\
			 $(PScan)/InitIIDS.cpp\

GoodSources += \
			 $(SESA_Connector)/LogFile.cpp\
			 $(SESA_Connector)/Status.cpp\
			 $(SESA_Connector)/Directory.cpp\
			 $(PScan)/FrwdThrd.cpp\
			 $(PScan)/FrwdLogs.cpp\
			 $(PScan)/FrwdLogFile.cpp\
			 $(PScan)/FrwdStatus.cpp\
			 $(PScan)/ForwardEvents.cpp\
			 $(PScan)/CVirusEventSinkNoCC.cpp\
			 $(PScan)/DecProc.cpp\
			 $(PScan)/DecomposerSDKNoCC.cpp\
			 $(MD5Util)/md5c.c\
#			 $(LDVPmisc)/Des.cpp\
#			 $(LDVPmisc)/Password.cpp\
			 $(PScan)/csv.cpp\
			 $(PScan)/dataset.cpp\
			 $(PScan)/ThreatCatSettings.cpp\

SKIP += \
			 $(PScan)/LegacyInfectionHandler.cpp\

GoodSources += \
			 $(PScan)/action.cpp\
			 $(PScan)/rcopy.cpp\

SKIP += \
			 $(PScan)/AccessChecker.cpp\

GoodSources += \
			 $(PScan)/afteract.cpp\
			 $(PScan)/altpass.cpp\

SKIP += \
			 $(PScan)/ams.cpp\

GoodSources += \
			 $(PScan)/callback.cpp\
			 $(PScan)/checksum.cpp\
			 $(PScan)/client.cpp\

SKIP += \
			 $(PScan)/conn.cpp\

GoodSources += \
			 $(PScan)/delha.cpp\

SKIP += \
			 $(PScan)/decomp.cpp\
			 $(PScan)/netware/discover.cpp\

GoodSources += \
			 $(PScan)/filestor.cpp\
			 $(PScan)/findnode.cpp\

SKIP += \
			 $(PScan)/ftp.cpp\

GoodSources += \
			 $(PScan)/grc.cpp\
			 $(PScan)/hsm.cpp\
			 $(PScan)/iface.cpp\

GoodSources += \
			 $(PScan)/is.cpp\

SKIP += \
			 $(PScan)/license.cpp\
			 $(PScan)/licensealloc.cpp\
			 $(LDVPmisc)/licensehelper.cpp\

GoodSources += \
			 $(PScan)/log.cpp\
			 $(PScan)/misc.cpp\
			 $(PScan)/mngdprod.cpp\
			 $(NetWare)/common/nlm.cpp\
			 $(PScan)/pattman.cpp\

SKIP += \
			 $(PScan)/process.cpp\
			 $(Common)/common/ThreadBase.cpp\
			 $(Common)/common/ThreadStaticSize.cpp\

GoodSources += \
			 $(PScan)/pscan.cpp\
			 $(PScan)/pushmult.cpp\
			 $(PScan)/snode.cpp\
			 $(PScan)/storage.cpp\
			 $(PScan)/readwrit.cpp\
			 $(PScan)/ScanStatus.cpp\
			 $(PScan)/uimisc.cpp\

SKIP += \
			 $(PScan)/trnsport.cpp\

GoodSources += \
			 $(PScan)/valert.cpp\
			 $(PScan)/vbin.cpp\
			 $(PScan)/vbinmon.cpp\
			 $(PScan)/walker.cpp\
			 $(PScan)/filetime.cpp\
			 nlmmain.cpp\

SKIP += \
			 nlmmem.cpp\
			 nui.cpp\
			 pscan_n.cpp\
			 scrnsave.cpp\

GoodSources += \
			 $(PScan)/getfinalaction.cpp\
			 $(PScan)/SortedVBinEnumerator.cpp\
			 $(Linux)/ntsStubs.cpp\

GoodSources += \
			 $(Linux)/pscan_L.cpp\
			 $(SymSafer)/src/SymSaferRegistry.cpp\
			 $(Linux_Security)/src/savtray/Common/MessageService.cpp\
			 $(Linux)/TrayIPC.cpp\

FirstSources = \


#$(NetWare)/common/NetwareSymSaferRegistry.cpp

Sources = $(FirstSources) $(GoodSources)

Dec3Libs = ctypes dec2 dec2amg dec2arj dec2as dec2gzip dec2id dec2lha dec2lib dec2lz\
			  dec2mb3 dec2ole1 dec2rar dec2rtf dec2ss dec2tar dec2text dec2tnef dec2zip\
			  decsdk grenleaf keymgr lib95 logmgr rec2 rec2lib symamg32 symgzip symlha\
			  symlz symoless symrar symtnef symzlib unarj

export LD_LIBRARY_PATH += $(ECOM)/BinLinux$(LibDir)

LinkPaths = $(Dec3)/lib/LinuxIntel/$(libDir)\
				$(ECOM)/BinLinux$(LibDir)\
				$(TCSAPI)/lib/$(libDir)\
				$(DefUtils)/LibLinux$(LibDir)\
				$(Linux_Security)/tools/certc/lib\
				$(Linux_Security)/tools/cryptoc/lib

StaticLinkLibs       = NAV_Glue $(Dec3Libs) defupdate defutils tcsapi symgfs certc bsafe certcsp
StaticDependentLibs += NAV_Glue
LinkLibs             = ecomlodrlin

Binary	  = rtvscand.bin
Executable = rtvscand


testSources		= $(PScan)/testSuite.cpp
cxxtestSources	= $(wildcard $(PScan)/test-*.h)
testDependents	= $(testSources:%.cpp=%.d)
testObjects		= $(testSources:%.cpp=%.o)
testBinaries	= $(testSources:%.cpp=%.bin)
tests			= $(testSources:%.cpp=%.tst)

Precious = $(testDependents) $(testObjects) $(testBinaries)
List		= $(testSources)

include $(Root)Rules.mk

$(PScan)/nlm.o : Macros += DO_NOT_INCLUDE_FINDFIRSTFILE

#test : LD_LIBRARY_PATH += $(ECOM)/BinLinux$(LibDir)
test : TEST ?= -c
test : $(tests)

$(PScan)/testSuite.bin: Includes += $(CxxTest)
$(PScan)/testSuite.bin: $(PScan)/testSuite.o
	$(LD) $(LDFLAGS) -o $@ $+ $(LDPATHS) $(LDLIBS) $(Modules) $(LinkObjects)
	@-cp -f $(ECOM)/BinLinux$(LibDir)/libecomlodrlin.so $(LIBDIR)
	$(ACCESS) $@

$(PScan)/testSuite.cpp: $(cxxtestSources)
	$(CxxTest)/cxxtestgen.pl --error-printer -o $@ $^ 
