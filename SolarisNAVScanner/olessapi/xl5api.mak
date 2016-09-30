# Copyright 1997 Symantec, Peter Norton Product Group
#########################################################################
#
# $Header:   S:/OLESSAPI/VCS/xl5api.mav   1.0   02 Oct 1997 17:30:24   DCHI  $
#
# Description:
#  Make file for XL5API library.
#
#########################################################################
#
# $Log:   S:/OLESSAPI/VCS/xl5api.mav  $
#  
#     Rev 1.0   02 Oct 1997 17:30:24   DCHI
#  Initial revision.
#  
########################################################################

TARGET_NAME=XL5API
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
        xl5api.obj      \
        crc32.lib

#UPDATE#

xl5api.obj:   olessapi.h xl5api.h

#ENDUPDATE#

