## Copyright 1996 Symantec Corporation
########################################################################
#
# $Header:   S:/QUEUEMGR/VCS/queuemgr.mav   1.0   18 Apr 1997 14:05:14   DSACKIN  $
#
# Description:
#
########################################################################
# $Log:   S:/QUEUEMGR/VCS/queuemgr.mav  $
#  
#     Rev 1.0   18 Apr 1997 14:05:14   DSACKIN
#  Ported from Tesla
#  
#     Rev 1.0   24 Mar 1997 16:47:20   DSACKIN
#  Initial revision.
#
#     Rev 1.0   13 Mar 1997 15:09:00   RFULLER
#  Initial revision
#
########################################################################
TARGET_TYPE=LIB
TARGET_NAME=QUEUEMGR

CHAR_SET=U                              # Set Unicode
USE_MFC_DLL=TRUE                        # Link MFC dynamically
USE_CRT_DLL=TRUE                        # Link C run time library dynamically

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

PROGRAM_NAME = $(TARGET_NAME).LIB

UNICODE = -DUNICODE -D_UNICODE
DEF += $(UNICODE) -GX

OBJS  = QUEUEMGR.OBJ
OBJS += QMGRPROF.OBJ

$(PROGRAM_NAME): $(OBJS)

#UPDATE#
QUEUEMGR.OBJ:     QUEUEMGR.CPP QUEUEMGR.H
QMGRPROF.OBJ:     QMGRPROF.CPP QUEUEMGR.H
#ENDUPDATE#
