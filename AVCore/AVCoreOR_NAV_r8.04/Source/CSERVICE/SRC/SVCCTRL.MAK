#########################################################################
# $Header:   S:/CSERVICE/VCS/svcctrl.mav   1.1   12 Dec 1997 14:48:54   TSmith  $
#
#########################################################################
# $Log:   S:/CSERVICE/VCS/svcctrl.mav  $
#  
#     Rev 1.1   12 Dec 1997 14:48:54   TSmith
#  Removed symkrnl linkage.
#  
#     Rev 1.0   09 May 1997 17:50:34   ksackin
#  Initial revision.
#  
#     Rev 1.1   18 Apr 1997 13:48:56   DSACKIN
#  Ported from Tesla
#  
#     Rev 1.0   13 Mar 1997 15:09:26   RFULLER
#  Initial revision
#  
#     Rev 1.0   08 Nov 1996 11:46:52   DKEEFER
#  Initial revision
#  
#     Rev 1.2   20 Jun 1996 02:21:16   DSACKIN
#  Removed the DEBUG_PROFILE information from here.
#
#     Rev 1.1   20 Jun 1996 01:51:14   DSACKIN
#  Added DEBUG_PROFILE define.
#
#     Rev 1.0   24 Apr 1996 17:26:18   DSACKIN
#  Initial revision.
#
#
#########################################################################
PROGRAM_NAME=SVCCTRL.LIB
TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

DEF=-DUNICODE -D_UNICODE


$(PROGRAM_NAME): \
        svcctrl.obj \
        svcprof.obj \
        timercli.lib

#UPDATE#
svcctrl.obj:     platform.h
#ENDUPDATE#
