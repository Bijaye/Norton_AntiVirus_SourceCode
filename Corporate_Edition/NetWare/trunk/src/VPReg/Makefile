# Copyright 2004-2005 Symantec Corporation. All rights reserved.
# Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
# Intent: VPReg Makefile

Root = ../

Macros = SERVER

Includes = $(PScan)\
			  $(LDVP)/include\
			  $(LDVPmisc)\
			  $(DebugNET)/include\
			  $(QuarantineServer)/Shared/Include\
			  $(MD5)/include\
			  $(NetWare)/include\
			  $(AMS)/include\
			  $(SymSafer)/include\
			  $(Linux_Security)/src/symreg\
			  ./

Sources	  = regmain.cpp\
			$(Linux_Security)/src/symreg/ipcmessage.cpp\
			$(Linux_Security)/src/symreg/socketcomm.cpp\

Binary	  = VPReg.bin
Executable = vpreg

include $(Root)Rules.mk


