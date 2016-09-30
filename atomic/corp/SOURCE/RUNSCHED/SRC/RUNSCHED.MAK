# Copyright 1996 Symantec, Peter Norton Product Group
########################################################################
#
# $Header:   S:/RUNSCHED/VCS/runsched.mav   1.1   30 Apr 1997 14:46:12   JBRENNA  $
#
# Description:
#       This is the make file for the Norton AntiVirus for Windows
#       Clinic program.
########################################################################
# $Log:   S:/RUNSCHED/VCS/runsched.mav  $
#  
#     Rev 1.1   30 Apr 1997 14:46:12   JBRENNA
#  Convert to new CORE_* platform spec.
#  
#     Rev 1.0   06 Feb 1997 21:08:30   RFULLER
#  Initial revision
#  
#     Rev 1.1   22 Jun 1996 08:57:12   TIVANOV
#  options to compile the resources
#
#     Rev 1.0   19 Jun 1996 16:13:10   TIVANOV
#  Initial revision.
########################################################################

TARGET_TYPE=EXE
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

# ---------------------------------------------------------------------
# Compile RUNSCHED (Gryphon 1.0)

%If "$(CORE_WIN32)" != ""
    PROGRAM_NAME= runsched.$(TARGET_TYPE)
%Else

# Platform is not W32 (required for NT)
BAD_PLATFORM:
    beep
    echo The $(PLATFORM) platform is not supported for the project.

%EndIf

RES_FILE     =RUNSCHED.RES
RES_SRC      =RUNSCHED.RC
OBJS         =runsched.obj
NAVUTILITY = XNTXUTIL

LIBS= \
 $(NAVUTILITY).LIB

# ---------------------------------------------------------------------

$(PROGRAM_NAME): RUNSCHED.obj \
 $(LIBS)\
 $(RES_FILE)

$(RES_FILE): $(RES_SRC)  RESource.h

#UPDATE#
RUNSCHED.obj:   resource.H apscomm.h
#ENDUPDATE#
