# Copyright 1997, Symantec Corporation, Peter Norton Product Group
#####################################################################
#
#   $Header :   $
#
#   Make file for MODUSR project
#
#####################################################################
#
#   $Log    :   $
#####################################################################
#

TARGET_TYPE=EXE                                     # Executable
USE_MFC_DLL=FALSE                                   # No MFC
USE_CRT_DLL=TRUE                                    # C Run Time Library
CONSOLE_MODE=YES                                    # Console Mode Win32

.INCLUDE    $(QUAKEINC)\PLATFORM.MAK

TARGET_NAME     = MODUSR
PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE)
XOPTS           += -GX

OBJS            = $(TARGET_NAME).OBJ

$(PROGRAM_NAME): $(OBJS)

#UPDATE#
#END#
