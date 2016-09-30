# Copyright 1997 Symantec, Peter Norton Product Group
#########################################################################
#
# $Header:   S:/DF/VCS/df.mav   1.1   26 Nov 1997 16:56:38   DCHI  $
#
# Description:
#  Make file for DF (deflation/inflation) library.
#
#########################################################################
#
# $Log:   S:/DF/VCS/df.mav  $
#  
#     Rev 1.1   26 Nov 1997 16:56:38   DCHI
#  Added definition of NDEBUG.
#  
#     Rev 1.0   11 Nov 1997 16:21:22   DCHI
#  Initial revision.
#  
########################################################################

TARGET_NAME=DF
PROGRAM_NAME=$(TARGET_NAME).LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

%If ("$[u,$(PLATFORM)]" == ".DOS") || \
    ("$[u,$(PLATFORM)]" == ".DX")
        MODEL = L
%EndIf

%If ("$[u,$(PLATFORM)]" == ".DX")
        XOPTS += -Alfw
%ElseIf ("$[u,$(PLATFORM)]" == ".WIN")
        XOPTS += -Alnw
%EndIf

XOPTS += -DNDEBUG

$(PROGRAM_NAME):        \
        df.obj          \
        dftree.obj      \
        dfzip.obj       \
        if.obj

#UPDATE#

df.obj:     df.h
dftree.obj: df.h
dfzip.obj:  df.h dfzip.h
if.obj:     df.h

#ENDUPDATE#

