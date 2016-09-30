# Copyright 1997 Symantec, Peter Norton Product Group
#########################################################################
#
# $Header:   S:/OLESSAPI/VCS/wd7api.mav   1.3   26 Mar 1998 18:32:42   DCHI  $
#
# Description:
#  Make file for WD7API library.
#
#########################################################################
#
# $Log:   S:/OLESSAPI/VCS/wd7api.mav  $
#  
#     Rev 1.3   26 Mar 1998 18:32:42   DCHI
#  Added wd7rmtxt.obj
#  
#     Rev 1.2   31 Jul 1997 14:37:36   DCHI
#  Added crc32.lib.
#  
#     Rev 1.1   10 Jul 1997 16:08:20   DCHI
#  Added changes for WIN and DX for SS != DS.
#  
#     Rev 1.0   05 May 1997 13:59:00   DCHI
#  Initial revision.
#  
########################################################################

TARGET_NAME=WD7API
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
        wd7api.obj      \
        wd7encdc.obj    \
        wd7rmtxt.obj    \
        crc32.lib

#UPDATE#

wd7api.obj:   olessapi.h wd7api.h crc32.h
wd7encdc.obj: olessapi.h wd7api.h
wd7rmtxt.obj: olessapi.h wd7api.h

#ENDUPDATE#

