# Copyright 2004-2005 Symantec Corporation. All rights reserved.
# Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
# Intent: VPStart Makefile

Root = ../

Macros = START

Includes = $(PScan)\
			  $(LDVP)/include\
			  $(LDVPmisc)\
			  $(QuarantineServer)/Shared/Include\
			  $(MD5)/include\
			  $(License)/include/slic\
			  $(AMS)/include

Sources = $(LDVPmisc)/Des.cpp\
			 $(PScan)/iface.cpp\
			 $(LDVPmisc)/licensehelper.cpp\
			 $(PScan)/InstHelp.cpp\
			 $(LDVPmisc)/Password.cpp\
			 $(PScan)/misc.cpp\
			 $(PScan)/nlm.cpp\
			 $(PScan)/trnsport.cpp\
			 $(PScan)/uimisc.cpp\
			 initnlm.cpp\
			 setupui.cpp\
			 start.cpp\
			 rss.cpp\
			 ../Rtvscan/nlmmem.cpp

Binary = VPStart.bin

include $(Root)Rules.mk
