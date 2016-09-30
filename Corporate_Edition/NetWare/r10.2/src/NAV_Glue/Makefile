# Copyright 2004-2005 Symantec Corporation. All rights reserved.
# Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
# Intent: NAV_Glue Makefile

Root = ../

Macros = SERVER WS_32 NO_VSNPRINTF

Includes = $(PScan)\
			  $(NavGlue)\
			  $(LDVP)/include\
			  $(LDVPmisc)\
			  $(QuarantineServer)/Shared/Include\
			  $(MD5)/include\
			  $(Dec3)/include\
			  $(DefUtils)/include\
			  $(AVCore)/include/src\
			  $(ECOM)/include\
			  $(AMS)/include

Sources = $(NavGlue)/Misc3.cpp\
			 $(NavGlue)/EventSink.cpp\
			 $(PScan)/DecomposerSDK.cpp\
			 $(NavGlue)/NavDecmp.cpp\
			 $(NavGlue)/debug.cpp\
			 $(NavGlue)/debug2.cpp\
			 $(NavGlue)/glue.cpp\
			 $(NavGlue)/glue3.cpp\
			 $(NavGlue)/misc2.cpp\
			 $(NavGlue)/SavVirusInfo.cpp\
			 $(NavGlue)/TrckAllc.cpp\
			 $(NavGlue)/decomp.cpp\
			 $(LDVPmisc)/vdbversion.cpp\
			 $(NavGlue)/glue2.cpp\
			 $(NavGlue)/misc.cpp

StaticSources = $(Sources)
StaticLib	  = NAV_Glue

include $(Root)Rules.mk
