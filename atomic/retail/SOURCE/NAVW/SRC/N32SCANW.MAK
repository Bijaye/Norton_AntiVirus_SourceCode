# Copyright 1996 Symantec, Peter Norton Product Group
#************************************************************************
#
# $Header:   S:/NAVW/VCS/n32scanw.mav   1.1   30 Apr 1997 19:45:54   JBRENNA  $
#
# Description:
#   Makefile for N32SCANW.EXE -- Simple exe which maps the old
#   Norton AntiVirus Scanner for NT name with the new one. This app is for
#   keeping the link with rest of the Norton applications which are still
#   calling n32scanw instead the new name navwnt.
#
# Contains:
#
# See Also:
#************************************************************************
# $Log:   S:/NAVW/VCS/n32scanw.mav  $
#  
#     Rev 1.1   30 Apr 1997 19:45:54   JBRENNA
#  Convert to new CORE_* platform specification.
#  
#     Rev 1.0   06 Feb 1997 21:01:26   RFULLER
#  Initial revision
#  
#     Rev 1.3   12 Sep 1996 16:11:16   PVENKAT
#  Removed UNICODE definitions, as this needs to start in WIN95 and gracefully
#  exit.
#  
#     Rev 1.2   06 Sep 1996 15:22:46   RPULINT
#  Moved changes over from Gryphon: Added n32scanw.rc reference
#  
#     Rev 1.1   21 Aug 1996 15:00:18   DALLEE
#  Added header and updated dependencies.
#************************************************************************

TARGET_TYPE=EXE
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

# ---------------------------------------------------------------------
# Compile N32SCANW (Gryphon 1.0)

#DEF=-DUNICODE -D_UNICODE

%If ("$(CORE_WIN32)" != "")
    PROGRAM_NAME=N32SCANW.$(TARGET_TYPE)
%Else

# Platform is not W32 (required for NT)
BAD_PLATFORM:
    beep
    echo The $(PLATFORM) platform is not supported for the project.

%EndIf

OBJS         =N32SCANW.obj
RES_FILE     =N32SCANW.RES
RES_SRC      =N32SCANW.RC
DEF          += /DNAVWNT

# ---------------------------------------------------------------------

$(PROGRAM_NAME): N32SCANW.obj \
$(Q_SYMGUI).LIB \
$(RES_FILE)

$(RES_FILE): $(RES_SRC)

#UPDATE#
n32scanw.obj:	platform.h dbcs_str.h dosvmm.h 
#ENDUPDATE#
