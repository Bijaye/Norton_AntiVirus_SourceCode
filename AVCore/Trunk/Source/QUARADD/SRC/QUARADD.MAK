# Copyright (C) 1997 Symantec Corporation
##############################################################################
# $Header:   S:/QUARADD/VCS/QUARADD.MAv   1.0   26 May 1998 14:27:46   DBuches  $
#
#
##############################################################################
# $Log:   S:/QUARADD/VCS/QUARADD.MAv  $
#  
#     Rev 1.0   26 May 1998 14:27:46   DBuches
#  Initial revision.
#  
#     Rev 1.1   09 Apr 1998 20:02:22   JTaylor
#  Removed a bad compiler switch
#
#     Rev 1.0   26 Feb 1998 11:06:48   jtaylor
#  Initial revision.
#
#     Rev 1.0   26 Nov 1997 11:57:36   DBuches
#  Initial revision.
##############################################################################

TARGET_NAME=QUARADD
TARGET_TYPE=LIB
PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME):  QUARADD.OBJ

#UPDATE#
#ENDUPDATE#

