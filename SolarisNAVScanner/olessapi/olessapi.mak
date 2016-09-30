# Copyright 1997 Symantec, Peter Norton Product Group
#########################################################################
#
# $Header:   S:/OLESSAPI/VCS/olessapi.mav   1.3   26 Mar 1998 18:33:10   DCHI  $
#
# Description:
#  Make file for OLESSAPI library.
#
#########################################################################
#
# $Log:   S:/OLESSAPI/VCS/olessapi.mav  $
#  
#     Rev 1.3   26 Mar 1998 18:33:10   DCHI
#  Added summary.obj
#  
#     Rev 1.2   10 Jul 1997 16:08:00   DCHI
#  Added changes for WIN and DX for SS != DS.
#  
#     Rev 1.1   05 May 1997 14:24:10   DCHI
#  Added olestrnm.cpp to list of sources.
#  
#     Rev 1.0   23 Apr 1997 15:09:54   DCHI
#  Initial revision.
#  
########################################################################

TARGET_NAME=OLESSAPI
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
        olessapi.obj    \
        olestrnm.obj    \
        summary.obj

#UPDATE#

olessapi.obj: olessapi.h ssbstype.h
olestrnm.obj: ssbstype.h olestrnm.h
summary.obj:  olessapi.h olestrnm.h ssbstype.h

#ENDUPDATE#

