## Copyright 1996 Symantec Corporation
########################################################################
#
# $Header:   S:/ALERTQ/VCS/alertq.mav   1.0   24 Mar 1997 16:40:14   DSACKIN  $
#
# Description:
#
########################################################################
# $Log:   S:/ALERTQ/VCS/alertq.mav  $
#  
#     Rev 1.0   24 Mar 1997 16:40:14   DSACKIN
#  Initial revision.
#
#     Rev 1.0   13 Mar 1997 15:09:00   RFULLER
#  Initial revision
#
########################################################################
TARGET_TYPE=LIB
TARGET_NAME=ALERTQ

CHAR_SET=U                              # Set Unicode
USE_MFC_DLL=TRUE                        # Link MFC dynamically
USE_CRT_DLL=TRUE                        # Link C run time library dynamically

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

PROGRAM_NAME = $(TARGET_NAME).LIB

UNICODE = -DUNICODE -D_UNICODE
DEF += $(UNICODE) -GX

OBJS  = ALERTQ.OBJ

$(PROGRAM_NAME): $(OBJS)

#UPDATE#
ALERTQ.OBJ:     ALERTQ.CPP ALERTQ.H QUEUEMGR.H
#ENDUPDATE#
