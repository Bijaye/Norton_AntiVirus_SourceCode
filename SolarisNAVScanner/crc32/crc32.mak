# Copyright 1997 Symantec, Peter Norton Product Group
#########################################################################
#
# $Header:   S:/CRC32/VCS/crc32.mav   1.1   10 Jul 1997 16:05:10   DCHI  $
#
# Description:
#  Make file for CRC32 library.
#
#########################################################################
#
# $Log:   S:/CRC32/VCS/crc32.mav  $
#  
#     Rev 1.1   10 Jul 1997 16:05:10   DCHI
#  Added SS != DS mods.
#  
#     Rev 1.0   10 Jun 1997 13:23:50   DCHI
#  Initial revision.
#  
########################################################################

TARGET_NAME=CRC32
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

$(PROGRAM_NAME):        crc32.obj

#UPDATE#

crc32.obj: crc32.h

#ENDUPDATE#

