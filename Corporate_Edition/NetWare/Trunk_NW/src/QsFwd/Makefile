# Copyright 2004-2005 Symantec Corporation. All rights reserved.
# Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
# Intent: QsFwd Makefile

Root = ../

Macros = SERVER

Includes = Include\
			  $(QuarantineServer)/Include\
			  $(QuarantineServer)/QsPak32\
			  $(QuarantineServer)/Shared/Include\
			  $(LDVP)/include\
			  $(AMS)/include

Sources = Src/QSFwd.cpp\
			 $(QuarantineServer)/QsPak32/package.cpp\
			 $(QuarantineServer)/QsInfo/QsInfo.cpp\
			 $(QuarantineServer)/QsComm32/TLI.CPP

Binary = QSFwd.bin

include $(Root)Rules.mk
