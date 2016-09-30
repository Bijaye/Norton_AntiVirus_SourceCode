# Copyright (C) 1997 Symantec Corporation
##############################################################################
# $Header:   S:/SCANDLVR/VCS/ScanDlvr.mav   1.20   23 Jun 1998 15:33:36   mdunn  $
#
#
##############################################################################
# $Log:   S:/SCANDLVR/VCS/ScanDlvr.mav  $
#  
#     Rev 1.20   23 Jun 1998 15:33:36   mdunn
#  Added scandlvr.res
#  
#     Rev 1.19   22 Jun 1998 13:53:52   sedward
#  Removed 'CleanPg.obj' from dependency list.
#
#     Rev 1.18   08 Jun 1998 20:32:04   jtaylor
#  Removed the resource dll.  It is now dynamically loaded.
#
#     Rev 1.17   03 Jun 1998 14:04:32   jtaylor
#  Added Scan and Deliver resource DLL library.
#
#     Rev 1.16   03 Jun 1998 14:03:42   jtaylor
#  Removed the resources from the DLL.
#
#     Rev 1.15   01 May 1998 22:21:20   jtaylor
#  Added progress dialog support.
#
#     Rev 1.14   27 Apr 1998 02:09:32   SEDWARD
#  Added defannty.lib.
#
#     Rev 1.13   25 Apr 1998 18:52:12   jtaylor
#  Added sarc lib.
#
#     Rev 1.12   24 Apr 1998 18:23:52   SEDWARD
#  Added FOTDPg.obj.
#
#     Rev 1.11   23 Apr 1998 18:35:48   jtaylor
#  added 2 more SARC libraries.
#
#     Rev 1.10   23 Apr 1998 16:47:34   jtaylor
#  Changed the name of the SARC lib.
#
#     Rev 1.9   23 Apr 1998 16:31:36   jtaylor
#  Changed lib specs to support Alpha.
#
#     Rev 1.8   20 Apr 1998 18:09:52   SEDWARD
#  Added new modules.
#
#     Rev 1.4   13 Apr 1998 15:28:36   JTaylor
#  Added two com .libraries
#
#     Rev 1.3   13 Apr 1998 15:24:02   JTaylor
#  added a .obj
#
#     Rev 1.2   13 Apr 1998 15:21:40   JTaylor
#  Added a few new object files.
#
#     Rev 1.1   11 Apr 1998 13:59:10   SEDWARD
#  Use renamed wizard panel modules.
#
#     Rev 1.0   06 Apr 1998 19:26:12   SEDWARD
#  Initial revision.
#
##############################################################################

TARGET_NAME=SCANDLVR
TARGET_TYPE=DLL
USE_MFC_DLL=TRUE
#USE_CRT_DLL=TRUE
PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

XOPTS = /GX                             # Enable C++ exception handling

CLOPTS += /Zp8                          # Get around bug in windows header

%if "$(CORE_BMODE)"=="R"
XOPTS += /O2                            # Turn on speed optimizations
%endif

LIBS += comctl32.lib
LIBS += ole32.lib uuid.lib
LIBS += sdsok32$(CORE_PLAT).lib
LIBS += sdstp32$(CORE_PLAT).lib
LIBS += sdsnd32$(CORE_PLAT).lib
LIBS += sdpck32$(CORE_PLAT).lib
LIBS += sdflt32$(CORE_PLAT).lib
LIBS += quaradd.lib
LIBS += n32utils.lib
LIBS += n32cores.lib
LIBS += s32navo.lib
LIBS += n32call.lib
LIBS += s32debug.lib
LIBS += socklib.lib
LIBS += wsock32.lib

$(PROGRAM_NAME): AcceptPg.obj           \
                 CorpPg.obj              \
                 DLLMain.obj             \
                 Factory.obj             \
                 FOTDPg.obj              \
                 Global.obj              \
                 ProgDlg.obj             \
                 RejectPg.obj            \
                 ReviewPg.obj            \
                 ScanCfg.obj             \
                 ScanDeliverDll.obj      \
                 ScanDlvr.obj            \
                 ScanWiz.obj             \
                 StdAfx.obj              \
                 User1Pg.obj             \
                 User2Pg.obj             \
                 User3Pg.obj             \
                 WelcomPg.obj            \
                 WizPage.obj             \
                 WrapUpPg.obj            \
                 scandlvr.res            \
                 ScanDlvr.d32


#UPDATE#
#ENDUPDATE#
