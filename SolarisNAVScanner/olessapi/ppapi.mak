# Copyright 1997 Symantec, Peter Norton Product Group
#########################################################################
#
# $Header:   S:/OLESSAPI/VCS/ppapi.mav   1.0   24 Nov 1997 17:36:42   DCHI  $
#
# Description:
#  Make file for PPAPI library.
#
#########################################################################
#
# $Log:   S:/OLESSAPI/VCS/ppapi.mav  $
#  
#     Rev 1.0   24 Nov 1997 17:36:42   DCHI
#  Initial revision.
#  
########################################################################

TARGET_NAME=PPAPI
PROGRAM_NAME=$(TARGET_NAME).LIB

%If ("$[u,$(PLATFORM)]" == ".DOS") || \
    ("$[u,$(PLATFORM)]" == ".DX")
        MODEL = L
%EndIf

%If ("$[u,$(PLATFORM)]" == ".DX")
        XOPTS += -Alfw
%ElseIf ("$[u,$(PLATFORM)]" == ".WIN")
        XOPTS += -Alnw
%EndIf

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME):        \
        ppapi.obj

#UPDATE#

ppapi.obj:    olessapi.h ppapi.h

#ENDUPDATE#

