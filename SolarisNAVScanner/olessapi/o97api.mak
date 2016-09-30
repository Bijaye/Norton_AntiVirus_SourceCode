# Copyright 1997 Symantec, Peter Norton Product Group
#########################################################################
#
# $Header:   S:/OLESSAPI/VCS/o97api.mav   1.5   26 Mar 1998 18:32:52   DCHI  $
#
# Description:
#  Make file for O97API library.
#
#########################################################################
#
# $Log:   S:/OLESSAPI/VCS/o97api.mav  $
#  
#     Rev 1.5   26 Mar 1998 18:32:52   DCHI
#  Added w97rmtxt.obj
#  
#     Rev 1.4   11 Aug 1997 14:38:40   DCHI
#  Added o97mod.obj
#  
#     Rev 1.3   31 Jul 1997 14:37:44   DCHI
#  Added crc32.lib.
#  
#     Rev 1.2   10 Jul 1997 16:08:16   DCHI
#  Added changes for WIN and DX for SS != DS.
#  
#     Rev 1.1   05 May 1997 14:30:26   DCHI
#  Added olestrnm.h to update dependencies.
#  
#     Rev 1.0   05 May 1997 14:26:58   DCHI
#  Initial revision.
#  
########################################################################

TARGET_NAME=O97API
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
        o97api.obj      \
        o97lznt.obj     \
        o97mod.obj      \
        o97wdapi.obj    \
        o97xlapi.obj    \
        w97rmtxt.obj    \
        crc32.lib

#UPDATE#

o97api.obj:   olessapi.h olestrnm.h o97api.h crc32.h
o97lznt.obj:  olessapi.h o97api.h
o97mod.obj:   olessapi.h o97api.h
o97wdapi.obj: olessapi.h olestrnm.h o97api.h
o97xlapi.obj: olessapi.h olestrnm.h o97api.h
w97rmtxt.obj: olessapi.h o97api.h

#ENDUPDATE#

