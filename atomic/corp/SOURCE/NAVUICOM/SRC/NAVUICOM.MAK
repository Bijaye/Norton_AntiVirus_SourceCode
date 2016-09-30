# Copyright 1992-1997 Symantec, Peter Norton Product Group
########################################################################
#
# $Header:   S:/NAVUICOM/VCS/NAVUICOM.MAv   1.0   25 Mar 1997 13:41:54   DBUCHES  $
#  MAKEFILE for NAVUICOM.LIB
#
#
########################################################################
# $Log:   S:/NAVUICOM/VCS/NAVUICOM.MAv  $
#  
#     Rev 1.0   25 Mar 1997 13:41:54   DBUCHES
#  Initial revision.

TARGET_NAME=NAVUICOM
TARGET_TYPE=DLL
PROGRAM_NAME=$(TARGET_NAME).LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK


OBJS=regobj.obj

$(PROGRAM_NAME): $(OBJS)

#UPDATE#
regobj.obj:    regobj.h
#ENDUPDATE#


