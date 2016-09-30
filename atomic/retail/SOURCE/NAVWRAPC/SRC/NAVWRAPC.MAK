## Copyright (C) 1998 Symantec Corporation
##*****************************************************************************
## $Header:   S:/NAVWRAPC/VCS/NAVWRAPC.MAv   1.1   23 Jun 1998 15:13:46   mdunn  $
##
## Description: Builds NAVWRAPC DLL naming the final module according to what
##              target it is being built for.
##
##*****************************************************************************
## $Log:   S:/NAVWRAPC/VCS/NAVWRAPC.MAv  $
#  
#     Rev 1.1   23 Jun 1998 15:13:46   mdunn
#  Added navwrapc.res.
#  
#     Rev 1.0   29 May 1998 14:34:56   DBuches
#  Initial revision.
#  
#     Rev 1.1   01 Apr 1998 22:08:32   TSmith
#  Removed target related ifdefs. Build only one target now.
#  
#     Rev 1.0   30 Mar 1998 16:02:18   TSmith
#  Initial revision.
##*****************************************************************************
TARGET_TYPE=DLL
USE_CRT_DLL=YES
USE_MFC_DLL=YES
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

LIBS += N32XUTIL.LIB
LIBS += N32EXCLU.LIB
LIBS += N32ALOG.LIB

PROGRAM_NAME = N32WRAPC.DLL

CLOPTS += /D$(TARGET)

$(PROGRAM_NAME) : NAVWRAPC.OBJ $(Q_SYMKRNL).LIB $(Q_SYMDEBUG).LIB $(LIBS) \
                  navwrapc.res

#UPDATE#
#ENDUPDATE#

