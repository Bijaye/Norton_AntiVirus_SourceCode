# Copyright 2004-2005 Symantec Corporation. All rights reserved.
# Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
# Intent: NAV_Glue Makefile

Root = ../

Macros = SERVER WS_32 NO_VSNPRINTF

Includes = \
			  $(SymSafer)/include\
			  $(PScan)\
			  $(NavGlue)\
			  $(LDVP)/include\
			  $(LDVP)/include/SecureComms\
			  $(LDVPmisc)\
			  $(DebugNET)/include\
			  $(QuarantineServer)/Shared/Include\
			  $(NetWare)/include\
			  $(Linux_Security)/src/include\
			  $(MD5)/include\
			  $(Dec3)/include\
			  $(DefUtils)/include\
			  $(AVCore)/include/src\
			  $(ECOM)/include\
			  $(AMS)/include

Sources = \
			 $(NavGlue)/Misc3.cpp \
			 $(NavGlue)/EventSinkNoCC.cpp \
			 $(PScan)/DecomposerSDKNoCC.cpp \
			 $(NavGlue)/NavDecmp.cpp \
			 $(NavGlue)/debug.cpp \
			 $(NavGlue)/debug2.cpp \
			 $(NavGlue)/glue.cpp \
			 $(NavGlue)/glue2.cpp \
			 $(NavGlue)/glue3.cpp \
			 $(NavGlue)/misc.cpp \
			 $(NavGlue)/misc2.cpp \
			 $(NavGlue)/TrckAllc.cpp \
			 $(NavGlue)/decomp.cpp \
			 $(LDVPmisc)/vdbversion.cpp \
			 $(PScan)/filetime.cpp \
			 $(PScan)/csv.cpp \
			 $(PScan)/dataset.cpp \
			 $(PScan)/ThreatCatSettings.cpp \
			 $(NavGlue)/gluemem.cpp \
			 $(NavGlue)/SavVirusInfo.cpp


StaticSources = $(Sources)
StaticLib	  = NAV_Glue

include $(Root)Rules.mk
