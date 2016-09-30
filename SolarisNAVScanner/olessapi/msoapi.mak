# Copyright 1998 Symantec, SABU
#########################################################################
#
# $Header:   S:/OLESSAPI/VCS/msoapi.mav   1.0   17 Nov 1998 11:48:26   DCHI  $
#
# Description:
#  Make file for MSOAPI library.
#
#########################################################################
#
# $Log:   S:/OLESSAPI/VCS/msoapi.mav  $
#  
#     Rev 1.0   17 Nov 1998 11:48:26   DCHI
#  Initial revision.
#  
########################################################################

TARGET_NAME=MSOAPI
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
        msoapi.obj

#UPDATE#

msoapi.obj: msoapi.h df.h o97api.h crc32.h

#ENDUPDATE#

