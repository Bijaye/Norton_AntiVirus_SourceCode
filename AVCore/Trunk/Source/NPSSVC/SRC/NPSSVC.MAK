#########################################################################
# $Header:   S:/NPSSVC/VCS/npssvc.mav   1.4   17 Apr 1998 14:47:04   mdunn  $ */
#
#########################################################################
# $Log:   S:/NPSSVC/VCS/npssvc.mav  $ */
#  
#     Rev 1.4   17 Apr 1998 14:47:04   mdunn
#  Now using MFC.
#
#     Rev 1.3   10 Sep 1997 18:33:54   KSACKIN
#  Ported changes from QAKG.
#
#     Rev 1.2.1.1   14 Aug 1997 10:35:38   KSACKIN
#  Fixed the path to the service.lib to have $(Projvers) in it.
#
#     Rev 1.2.1.0   12 Aug 1997 22:54:22   DALVIDR
#  Branch base for version QAKG
#
#     Rev 1.2   02 Jun 1997 01:30:54   KSACKIN
#  Updated make file to use the NPSAPI versus local version of the API.
#
#     Rev 1.1   19 May 1997 00:53:08   KSACKIN
#  Added SymDebug.lib into this makefile for our SYM_ASSERT.
#
#     Rev 1.0   19 May 1997 00:12:34   KSACKIN
#  Initial revision.
#
#     Rev 1.0   09 Apr 1996 11:37:18   DSACKING
#  Initial revision.
#
#########################################################################
TARGET_TYPE=EXE
CONSOLE_MODE=YES
USE_MFC_DLL=TRUE
USE_CRT_DLL=TRUE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

TARGET_NAME=NPSSVC

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

RES_FILE = npssvc.res
RES_SRC  = npssvc.rc

OBJS     = npssvc.obj npsdo.obj filemon.obj

UNILIBS  = $(PROJPRE)LIB$(PROJVERS).$(CORE_PLAT)$(CORE_BMODE)U\service.lib \
           $(PROJPRE)LIB$(PROJVERS).$(CORE_PLAT)$(CORE_BMODE)U\LinkSrv.lib

LIBS     =  NPSAPI.LIB \
            $(UNILIBS) \
            $(Q_SYMDEBUG).lib \
            $(Q_SYMKRNL).lib \

XOPTS    += -GX                         # enable stack unwinding for C++ exceptions

$(PROGRAM_NAME): $(OBJS) \
                 $(RES_FILE) \
                 $(LIBS)

#UPDATE#
npssvc.obj:    npssvc.cpp npssvc.h npssvstr.h service.h platform.h filemon.h
npsdo.obj:     npsdo.cpp npssvc.h npssvstr.h service.h platform.h
filemon.obj:   filemon.cpp filemon.h

npssvc.res:    npssvc.rc npsmsg.rc npssvstr.h
npsmsg.rc:     npsmsg.mc
#ENDUPDATE#
