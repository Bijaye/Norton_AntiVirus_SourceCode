# Copyright 1998 Symantec, SABU
#########################################################################
#
# $Header:   S:/OLESSAPI/VCS/acsapi.mav   1.1   29 Apr 1998 15:57:26   DCHI  $
#
# Description:
#  Make file for ACSAPI library.
#
#########################################################################
#
# $Log:   S:/OLESSAPI/VCS/acsapi.mav  $
#  
#     Rev 1.1   29 Apr 1998 15:57:26   DCHI
#  Added a2api.obj
#  
#     Rev 1.0   17 Apr 1998 13:40:50   DCHI
#  Initial revision.
#  
########################################################################

TARGET_NAME=ACSAPI
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
        acsapi.obj      \
        a97lznt.obj     \
        a2api.obj

#UPDATE#

acsapi.obj:   acsapi.h
a97lznt.obj:  acsapi.h
a2api.obj:    acsapi.h

#ENDUPDATE#

