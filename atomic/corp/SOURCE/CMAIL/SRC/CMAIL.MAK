## Copyright 1996 Symantec Corporation
########################################################################
#
# $Header:   S:/CMAIL/VCS/cmail.mav   1.0   18 Apr 1997 13:47:24   DSACKIN  $
#
# Description:
#
########################################################################
# $Log:   S:/CMAIL/VCS/cmail.mav  $
#  
#     Rev 1.0   18 Apr 1997 13:47:24   DSACKIN
#  Ported from Tesla
#  
#     Rev 1.0   13 Mar 1997 15:09:00   RFULLER
#  Initial revision
#  
#     Rev 1.3   20 Nov 1996 21:56:24   DSACKIN
#  Removed UNICODE options that didn't have any real function.
#
#     Rev 1.2   20 Nov 1996 19:36:40   DSACKIN
#  Added more MFC UNICODE support.
#
#     Rev 1.1   20 Nov 1996 18:46:38   DSACKIN
#  Set CStrings into UNICODE mode.
#
#     Rev 1.0   20 Nov 1996 14:28:32   DSACKIN
#  Initial revision.
#
########################################################################
TARGET_TYPE=LIB
TARGET_NAME=CMAIL

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

PROGRAM_NAME = $(TARGET_NAME).LIB

UNICODE = -DUNICODE -D_UNICODE
DEF += $(UNICODE) -GX

OBJS  = CMAIL.OBJ
OBJS += CSMTP.OBJ

$(PROGRAM_NAME): $(OBJS)

#UPDATE#
CMAIL.OBJ:     CMAIL.H CMAIL.CPP
#ENDUPDATE#
