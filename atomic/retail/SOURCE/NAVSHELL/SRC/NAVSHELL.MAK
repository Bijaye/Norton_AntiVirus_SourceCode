# Copyright 1996-1997 Symantec, Peter Norton Product Group
########################################################################
#
# $Header:   S:/NAVSHELL/VCS/navshell.mav   1.1   30 Apr 1997 14:25:40   JBRENNA  $
#
# Description:
#
########################################################################
# $Log:   S:/NAVSHELL/VCS/navshell.mav  $
#  
#     Rev 1.1   30 Apr 1997 14:25:40   JBRENNA
#  Convert to new CORE_* platform check.
#  
########################################################################

TARGET_TYPE=DLL
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

# ---------------------------------------------------------------------
# Compile N32SCANW (Gryphon 1.0)

%If "$(CORE_WIN32)" != ""
    PROGRAM_NAME= NAVSHELL.$(TARGET_TYPE)
%Else

# Platform is not W32 (required for NT)
BAD_PLATFORM:
    beep
    echo The $(PLATFORM) platform is not supported for the project.
%EndIf

# ---------------------------------------------------------------------

$(PROGRAM_NAME): \
navshell.obj \
navshell.d32 \
ole32.lib \
uuid.lib \
navshell.res

#UPDATE#
NAVSHELL.RES:    navshell.rc resource.h
navshell.obj:    navshell.cpp navshell.h resource.h
#ENDUPDATE#
