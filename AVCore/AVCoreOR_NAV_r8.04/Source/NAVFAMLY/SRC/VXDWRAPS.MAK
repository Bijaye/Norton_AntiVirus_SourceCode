#  Copyright 1994 Symantec Corporation
#============================================================================
#  $Header:   S:\navfamly\vcs\vxdwraps.mav   1.1   15 Sep 1994 17:32:30   CAREY  $
#
#  Description:
#       This is a make file for NAVCore VxD wraps.
#
#  See Also:
#
#----------------------------------------------------------------------------
#  $Log:   S:\navfamly\vcs\vxdwraps.mav  $
#  
#     Rev 1.1   15 Sep 1994 17:32:30   CAREY
#  No change.
#  
#     Rev 1.0   16 Aug 1994 22:03:40   RSTANEV
#  Initial revision.
#
#----------------------------------------------------------------------------

TARGET_NAME=NAVCORE
TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

PROGRAM_NAME=NAVCORE.LIB

$(PROGRAM_NAME): vxdwraps.obj

#UPDATE#
vxdwraps.obj: platform.inc vnavcore.inc
#ENDUPDATE#
