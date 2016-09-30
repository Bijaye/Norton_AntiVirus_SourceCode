# Copyright 1997 Symantec, Peter Norton Product Group
###########################################################################
#
# $Header:   S:/MACROHEU/VCS/VBAEMSTR.MAv   1.0   19 Aug 1997 17:33:28   DCHI  $
#
# Description:
#  Makefile for VBAEMSTR
#
###########################################################################
#
# $Log:   S:/MACROHEU/VCS/VBAEMSTR.MAv  $
#  
#     Rev 1.0   19 Aug 1997 17:33:28   DCHI
#  Initial revision.
#  
###########################################################################

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

####################################################
#                     D O S                        #
####################################################

%If ("$[u,$(PLATFORM)]" == ".DOS")
TARGET_NAME=VBAEMSTR
PROGRAM_NAME=$(TARGET_NAME).EXE

        MODEL = L
%EndIf

$(PROGRAM_NAME):        \
        vbaemstr.obj

