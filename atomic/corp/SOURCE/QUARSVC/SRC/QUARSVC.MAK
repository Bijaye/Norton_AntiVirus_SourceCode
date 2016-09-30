# Copyright 1998 Symantec Corporation.
#*************************************************************************
# QuarSvc.mak - created 3/5/98 10:59:33 AM
#
# $Header:   S:/QUARSVC/VCS/quarsvc.mav   1.3   22 Apr 1998 18:34:26   DBuches  $
#
# Description: Make file for the Norton Alert Service Redirector.
#              QuarSvc.EXE is a service which replaces NAV Alert service.
#              it does nothing. The NAV AP needs to start a service named
#              NAV Alert. The Norton Alert Manager will handle the
#              alerting task.
#
#*************************************************************************
# $Log:   S:/QUARSVC/VCS/quarsvc.mav  $
#  
#     Rev 1.3   22 Apr 1998 18:34:26   DBuches
#  Hooked up stale file handle monitor.
#
#     Rev 1.2   21 Apr 1998 19:25:22   SEDWARD
#  Added EResource, FileMon and FHList modules.
#
#     Rev 1.1   06 Apr 1998 13:58:28   SEDWARD
#  Added FILESRV.LIB.
#
#     Rev 1.0   05 Mar 1998 13:02:22   JALLEE
#  Initial revision.
#*************************************************************************
TARGET_NAME=QuarSvc
TARGET_TYPE=EXE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)



RES_FILE=QuarSvc.RES

XOPTS += /GX                            # Enable C++ exception handling
XOPTS += /DBUILDING_E_RESOURCE          # export the EResource class

UNILIBS += $(FAMILYPRE)\LIB$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)U\service.lib
UNILIBS += $(FAMILYPRE)\LIB$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)U\svcctrl.lib
UNILIBS += $(FAMILYPRE)\LIB$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)U\LinkSrv.lib

LIBS += ole32.lib
LIBS += QUARADD.LIB
LIBS += $(Q_SYMKRNL).LIB
LIBS += S32DEBUG.LIB


OBJS += QuarSvc.obj
OBJS += EResource.obj
OBJS += FileMon.obj
OBJS += FHList.obj
OBJS += ISLink.obj
OBJS += ISLink_s.obj


$(PROGRAM_NAME): $(RES_FILE) $(OBJS) $(LIBS) $(UNILIBS)

#UPDATE#
QuarSvc.obj:         QuarSvc.h QuarSvc.cpp QuarStr.h
EResource.obj:       EResource.h EResource.cpp
FileMon.obj:         FileMon.h FileMon.cpp
FHList.obj:          FHList.h FHList.cpp
QuarSvc.res:         QuarSvc.rc QuarStr.h
#ENDUPDATE#
