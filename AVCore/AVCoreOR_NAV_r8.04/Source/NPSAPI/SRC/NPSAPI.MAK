#########################################################################
# $Header:   S:/NPSAPI/VCS/npsapi.mav   1.2   17 Apr 1998 11:54:18   mdunn  $ */
#
#########################################################################
# $Log:   S:/NPSAPI/VCS/npsapi.mav  $ */
#  
#     Rev 1.2   17 Apr 1998 11:54:18   mdunn
#  Now using MFC.
#  
#     Rev 1.1   05 Jun 1997 18:08:30   KSACKIN
#  Added npsinit.cpp to the file dependency list.
#
#     Rev 1.0   29 May 1997 00:42:58   ksackin
#  Initial revision.
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
TARGET_TYPE=LIB
USE_MFC_DLL=TRUE
USE_CRT_DLL=TRUE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

TARGET_NAME=NPSAPI

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

OBJS     = npsevnts.obj npsinit.obj npsstr.obj


LIBS     =  $(Q_SYMDEBUG).lib \
            $(Q_SYMKRNL).lib \

$(PROGRAM_NAME): $(OBJS) \
                 $(RES_FILE) \
                 $(LIBS)

#UPDATE#
npsevnts.obj:  npsevnts.cpp npserr.h npsstb.h service.h platform.h
npsinit.obj:   npsinit.cpp npserr.h npsstb.h service.h platform.h
npsstr.obj:    npsstr.cpp npssvc.h npserr.h service.h platform.h

#ENDUPDATE#
