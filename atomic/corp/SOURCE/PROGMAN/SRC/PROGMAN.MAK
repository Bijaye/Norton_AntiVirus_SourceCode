# Copyright 1996 Symantec Corporation
#############################################################################
#
# $Header:   S:/progman/VCS/progman.mav   1.0   06 Feb 1997 21:08:22   RFULLER  $
#
# Description:
#
#############################################################################
#
# $Log:   S:/progman/VCS/progman.mav  $
#  
#     Rev 1.0   06 Feb 1997 21:08:22   RFULLER
#  Initial revision
#  
#     Rev 1.0   10 Jun 1996 11:34:56   jmillar
#  Initial revision.
#  
#############################################################################
TARGET_NAME=PROGMAN
PROGRAM_NAME=$(TARGET_NAME).LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

OBJS =  PROGMAN.OBJ

$(PROGRAM_NAME):        $(OBJS)

#UPDATE#
#ENDUPDATE#
