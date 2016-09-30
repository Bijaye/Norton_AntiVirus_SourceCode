#************************************************************************
# Copyright 1992-1993 Symantec, Peter Norton Product Group
#************************************************************************
# NAVEXLNK.MAK
#
# $Header:   S:/NAVEX/VCS/navexlnk.mav   1.3   26 Dec 1996 15:23:20   AOONWAL  $
#
# Description:
#   This file contains the dependencies and rules required to build the
#   NAVEX DOS linker.
#
# Notes:
#   The updated linker should be copied to the appropriate TOOLS
#   so that the build system has access to it through the PATH.
#
#************************************************************************
# $Log:   S:/NAVEX/VCS/navexlnk.mav  $
#  
#     Rev 1.3   26 Dec 1996 15:23:20   AOONWAL
#  No change.
#  
#     Rev 1.2   02 Dec 1996 14:03:16   AOONWAL
#  No change.
#  
#     Rev 1.1   29 Oct 1996 13:00:30   AOONWAL
#  No change.
#  
#     Rev 1.0   19 Oct 1995 14:01:20   DCHI
#  Initial revision.
#  
#************************************************************************

PROGRAM_NAME    = NAVEXLNK.EXE
TARGET_TYPE     = EXE
.include $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME): navexlnk.obj
        link /STACK:4096 navexlnk, navexlnk.exe;;;

navexlnk.obj:   navexlnk.c
