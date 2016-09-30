## Copyright 1997 Symantec Corporation
##****************************************************************************
##
## $Header:   S:/CDSTART/VCS/cdstart.mav   1.2   29 Jul 1998 14:58:30   DBuches  $
##
##****************************************************************************
## $Log:   S:/CDSTART/VCS/cdstart.mav  $
#  
#     Rev 1.2   29 Jul 1998 14:58:30   DBuches
#  Added splash.obj.
#
#     Rev 1.1   20 Jul 1997 15:43:06   GWESTER
#  Changes for Roswell & Tesla.
#
#     Rev 1.0   18 Apr 1997 13:45:16   DSACKIN
#  Ported from Tesla
#
#     Rev 1.0   13 Mar 1997 15:08:02   RFULLER
#  Initial revision
#
#     Rev 1.0   31 Dec 1996 15:47:52   TSmith
#  Initial revision.
##****************************************************************************
PROGRAM_NAME=CDSTART.EXE
MODEL=L
TARGET_TYPE=EXE
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

LIBS += VFW32.LIB

CDSTART.EXE: CDSTART.OBJ Splash.obj CDSTART.RES

