# Copyright 1996 Symantec, Peter Norton Product Group
###########################################################################
#
# $Header:   S:/NAVEX/VCS/booparse.mav   1.0   05 Sep 1997 20:35:32   CNACHEN  $
#
# Description:
#  Makefile for DATGEN.EXE
#
###########################################################################
#
# $Log:   S:/NAVEX/VCS/booparse.mav  $
#  
#     Rev 1.0   05 Sep 1997 20:35:32   CNACHEN
#  Initial revision.
#  
#     Rev 1.0   07 Jun 1996 13:01:46   DCHI
#  Initial revision.
#  
###########################################################################

TARGET_NAME=BOOPARSE
PROGRAM_NAME=$(TARGET_NAME).EXE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

####################################################
#                     W 3 2                        #
####################################################

%If ("$[u,$(PLATFORM)]" == ".W32")
# W32 is a Windows 95 console application
        XOPTS += /D "_CONSOLE"
        LINKOPTS += /SUBSYSTEM:console
%EndIf

$(PROGRAM_NAME):        \
        booparse.obj

#UPDATE#

booparse.obj: avtypes.h token.h

#ENDUPDATE#


