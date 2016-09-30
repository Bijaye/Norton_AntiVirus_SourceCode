# Copyright 2004-2005 Symantec Corporation. All rights reserved.
# Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
# Intent: Rtvscan Makefile

Root = ../

Macros = SERVER WS_32 NO_VSNPRINTF LIC=NO
#ENFORCE_LICENSING

Includes = $(PScan)\
			  $(LDVP)/include\
			  $(LDVPmisc)\
			  $(QuarantineServer)/Shared/Include\
			  $(MD5)/include\
			  $(Dec3)/include\
			  $(DefUtils)/include\
			  $(SESA_Collector)/Include\
			  $(InternetSecurity)/include\
			  $(SESA_Connector)\
			  $(License)/include/slic\
			  $(SymSync)/include\
			  $(AMS)/include\
			  $(Linux_Security)/src/include

Sources = $(PScan)/getfinalaction.cpp\
			 $(PScan)/nlm.cpp\
			 $(PScan)/delha.cpp\
			 $(PScan)/snode.cpp\
			 $(SESA_Connector)/LogFile.cpp\
			 $(PScan)/rcopy.cpp\
			 $(SESA_Connector)/Status.cpp\
			 $(SESA_Connector)/Directory.cpp\
			 $(PScan)/pattman.cpp\
			 $(PScan)/FrwdThrd.cpp\
			 $(PScan)/FrwdLogs.cpp\
			 $(PScan)/FrwdLogFile.cpp\
			 $(PScan)/FrwdStatus.cpp\
			 $(PScan)/action.cpp\
			 $(PScan)/misc.cpp\
			 $(PScan)/ForwardEvents.cpp\
			 $(PScan)/afteract.cpp\
			 $(PScan)/DecomposerSDK.cpp\
			 $(MD5Util)/md5c.c\
			 $(LDVPmisc)/Des.cpp\
			 $(LDVPmisc)/Password.cpp\
			 $(PScan)/csv.cpp\
			 $(PScan)/dataset.cpp\
			 $(PScan)/ThreatCatSettings.cpp\
			 $(PScan)/filestor.cpp\
			 $(PScan)/aceslist.cpp\
			 $(PScan)/altpass.cpp\
			 $(PScan)/callback.cpp\
			 $(PScan)/checksum.cpp\
			 $(PScan)/client.cpp\
			 $(PScan)/decomp.cpp\
			 $(PScan)/findnode.cpp\
			 $(PScan)/grc.cpp\
			 $(PScan)/hsm.cpp\
			 $(PScan)/iface.cpp\
			 $(PScan)/is.cpp\
			 $(PScan)/log.cpp\
			 $(PScan)/mngdprod.cpp\
			 $(PScan)/process.cpp\
			 $(PScan)/pushmult.cpp\
			 $(PScan)/pscan.cpp\
			 $(PScan)/storage.cpp\
			 $(PScan)/readwrit.cpp\
			 $(PScan)/valert.cpp\
			 $(PScan)/vbinmon.cpp\
			 $(PScan)/walker.cpp\
			 $(PScan)/filetime.cpp\
			 nlmmain.cpp\
			 $(PScan)/vbin.cpp\
			 $(PScan)/sortedvbinenumerator.cpp\
			 $(Linux)/ntsStubs.cpp\
			 $(Linux)/pscan_L.cpp\
			 $(Linux)/ndkStartConsoleScreen.cpp\
			 $(LDVPmisc)/Dprintf.cpp

Dec3Libs = ctypes dec2 dec2amg dec2arj dec2as dec2gzip dec2id dec2lha dec2lib dec2lz\
			  dec2mb3 dec2ole1 dec2rar dec2rtf dec2ss dec2tar dec2text dec2tnef dec2zip\
			  decsdk grenleaf keymgr lib95 logmgr rec2 rec2lib symamg32 symgzip symlha\
			  symlz symoless symrar symtnef symzlib unarj

LinkPaths = $(Dec3)/lib\
				$(DefUtils)/LibLinux$(LibDir) $(TCSAPI)/lib/$(libDir)\
				$(ECOM)/BinLinux$(LibDir)

StaticLinkLibs       = NAV_Glue $(Dec3Libs) defupdate defutils tcsapi 
StaticDependentLibs += NAV_Glue
LinkLibs             = ecomlodrlin

Binary	  = RTVScan.bin
Executable = rtvscan

include $(Root)Rules.mk

$(PScan)/nlm.o : Macros += DO_NOT_INCLUDE_FINDFIRSTFILE
