# Copyright 1996 Symantec, Peter Norton Product Group
########################################################################
#
# $Header:   S:/ALERTSVC/VCS/alertsvc.mav   1.14   14 Aug 1997 18:23:16   DSACKIN  $
#
# Description:
#       This is the make file for the Norton AntiVirus Alert Service
#
########################################################################
# $Log:   S:/ALERTSVC/VCS/alertsvc.mav  $
#  
#     Rev 1.14   14 Aug 1997 18:23:16   DSACKIN
#  Added AlertRPC.obj
#
#     Rev 1.13   14 Aug 1997 13:42:02   DSACKIN
#  Early checkin for the build.  This has the added $(FAMILYVERS) and $(QUAKE)
#  for the branch support.
#
#     Rev 1.12   23 Jun 1997 12:25:48   DSACKIN
#  Added the personality DLL lib.
#
#     Rev 1.11   27 May 1997 16:19:12   KSACKIN
#  Changed dependency on NavAlert.lib to XNTAlert.lib.  NavAlert.lib no longer
#  exists.  This is now built for Unicode as XNTAlert.
#
#     Rev 1.10   21 May 1997 13:42:28   DSACKIN
#  Added Marconi.obj
#
#     Rev 1.9   18 May 1997 05:07:28   DSACKIN
#  Changed over from CAlertNotification to CVirusAlert
#
#     Rev 1.8   12 May 1997 17:25:18   ksackin
#  Changed the N32Serve.Lib dependency to XNTServe.lib since this file is only
#  used on the NT side.  This projects has no targets as of yet.  When we do
#  build this for Windows 95 we are going to have to have targets and include
#  N32Serve for the Windows 95 side.
#
#     Rev 1.7   07 May 1997 15:42:12   GDZIECI
#  Added RCVSTART to start and stop listening for AppleTalk, IPX and IP alerts.
#
#     Rev 1.6   06 May 1997 16:01:16   GDZIECI
#  Added modules to receive AppleTalk, IPX and IP based alerts.
#
#     Rev 1.5   02 May 1997 13:04:24   JBRENNA
#  Ops ... missed a couple of Ansi libraries.
#
#     Rev 1.4   02 May 1997 12:50:56   JBRENNA
#  Use full path when specifying a ANSI library to link to.
#
#     Rev 1.3   01 May 1997 15:36:06   JBRENNA
#  Opts ... this time reallly change NAVSERVE to N32SERVE.
#
#     Rev 1.2   01 May 1997 15:27:40   JBRENNA
#  Change NAVSERVE to N32SERVE.
#
#     Rev 1.1   18 Apr 1997 13:40:46   DSACKIN
#  Ported from Tesla
#
#     Rev 1.1   24 Mar 1997 20:26:20   DSACKIN
#  Moved the queue mechanism into QueueMgr and AlertQ.
#
#     Rev 1.0   13 Mar 1997 15:06:06   RFULLER
#  Initial revision
#
#     Rev 1.10   18 Jan 1997 07:07:54   DSACKIN
#  Made numerous changes to accomodate the new CAlertQueueManager object usage.
#
#     Rev 1.9   02 Jan 1997 12:59:46   TSmith
#  Fixed a typo...
#
#     Rev 1.8   30 Dec 1996 12:09:18   TSmith
#  Brought file into synch with NAVAPSVC.MAK so both services build with same
#  settings.
#
#     Rev 1.7   27 Nov 1996 05:30:46   DSACKIN
#  Added AlertDef.Obj
#
#     Rev 1.6   21 Nov 1996 01:29:08   DSACKIN
#  Added UTIL.OBJ as a new file.
#
#     Rev 1.5   18 Nov 1996 12:34:08   DSACKIN
#  Added NTMAIL.OBJ, NTSNMP.OBJ and NTFWD.OBJ.
#
#     Rev 1.4   15 Nov 1996 03:36:42   KSACKIN
#  Changed from statically linking to MFC to dynamic.  We have to do this for
#  NavAlert.dll anyway, so we may as well do it here as well.
#
#     Rev 1.3   14 Nov 1996 01:55:36   KSACKIN
#  Restored the NavAlert.Lib library in our library list.  This was done to
#  implement the CAlertOptions class for loading alert events.
#
#     Rev 1.2   13 Nov 1996 22:14:20   DSACKIN
#  Split up alertsvc into many separate source files.
#
#     Rev 1.1   08 Nov 1996 17:29:36   TSMITH
#  Setup for proper MFC builds, added ALRTNTFY.LIB.
#
#     Rev 1.9   23 Jul 1996 13:49:50   DSACKIN
#  Added SvcCtrl.LIB and LinkSrv.LIB.
#
#     Rev 1.8   21 Jun 1996 16:19:02   PVENKAT
#  Removed NETAPI32.LIB
#
#     Rev 1.7   20 Jun 1996 02:30:38   DSACKIN
#  Removed DEBUG_PROFILE defines.  This is now done in Profiler.h
#
#     Rev 1.6   13 Jun 1996 17:33:10   DSACKIN
#  Added the DEBUG_PROFILE flag.
#
#     Rev 1.5   16 Apr 1996 21:00:42   DSACKIN
#  Fixed the dependencies so that the MC file is taken care of.
#  Changed the makefile to support new BUILTINS rules.
#
########################################################################
TARGET_TYPE=EXE
CHAR_SET=U                              # Set Unicode
USE_MFC_DLL=TRUE                        # Link MFC dynamically
USE_CRT_DLL=TRUE                        # Link C run time library dynamically
CONSOLE_MODE=YES                        # Run in console mode?
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

TARGET_NAME     = ALERTSVC
PROGRAM_NAME    = $(TARGET_NAME).EXE
RES_FILE        = ALERTSVC.RES
RES_SRC         = ALERTSVC.RC
XOPTS           = -GX                   # Enable C++ exception handling
UNICODE         = -DUNICODE -D_UNICODE
DEF             += $(UNICODE)           # More Unicode

SYSLIBS += COMCTL32.LIB                 # Default SysLibs settings forgot this

OBJS  = ALERTSVC.OBJ                    # Main Alert Service module
OBJS += ALERTRPC.OBJ                    # RPC Server Manager
OBJS += UTIL.OBJ                        # Utility functions for Alert Service
OBJS += NTPOPUP.OBJ                     # Local Popup Module
OBJS += NTMSNGR.OBJ                     # NetMessenger Module
OBJS += NTEVLOG.OBJ                     # NT Event Log Module
OBJS += NTALOG.OBJ                      # NAV Activity Log Module
OBJS += NTNLM.OBJ                       # NAV NLM Notification Module
OBJS += NTFWD.OBJ                       # Forwarding of Alerts Module
OBJS += NTMAIL.OBJ                      # e-mail module
OBJS += NTSNMP.OBJ                      # SNMP module
OBJS += ALERTDEF.OBJ                    # For def watching mode
OBJS += MAINMGR.OBJ                     # For the main queue manager
OBJS += MANAGERS.OBJ                    # For the rest of the managers
OBJS += RCVSTART.OBJ                    # Start/Stop alert receivers
OBJS += ALRTRECV.OBJ                    # AlertReceiver module
OBJS += ATPRCEIV.OBJ                    # Listen for AppleTalk alerts
OBJS += IPXRCEIV.OBJ                    # Listen for IPX alerts
OBJS += IPRECEIV.OBJ                    # Listen for IP alerts
OBJS += MARCONI.OBJ                     # Translate from MARCONI Alerts.

AnsiLibDir       = $(PROJPRE)LIB$(PROJVERS).$(CORE_PLAT)$(CORE_BMODE)A
AnsiFamilyLibDir = $(FAMILYPRE)\LIB$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)A
AnsiCoreLibDir   = $(QUAKEPRE)\LIB$(QUAKE).$(CORE_PLAT)$(CORE_BMODE)A

LIBS  = $(AnsiCoreLibDir)\DRVCOMM.LIB
LIBS += SERVICE.LIB
LIBS += SVCCTRL.LIB
LIBS += LINKSRV.LIB
LIBS += $(AnsiCoreLibDir)\$(Q_SYMDEBUG).LIB
LIBS += $(AnsiFamilyLibDir)\$(Q_SYMUTIL).LIB
LIBS += $(AnsiFamilyLibDir)\$(Q_SYMKRNL).LIB
LIBS += $(AnsiLibDir)\XNTXUTIL.LIB
LIBS += $(AnsiLibDir)\XNTSERVE.LIB
LIBS += XNTALERT.LIB
                                        ## For use with CMail
LIBS += CMAIL.LIB
LIBS += $(AnsiFamilyLibDir)\SOCKLIB.LIB
LIBS += WSOCK32.LIB
                                        ## For queue management functions
LIBS += QUEUEMGR.LIB
LIBS += ALERTQ.LIB

                                        ## New Virus Alert Stuff
LIBS += CALERT.LIB
LIBS += CVALERT.LIB

                                        ## Behavior DLL
LIBS += $(AnsiLibDir)\N32PDLL.LIB

$(PROGRAM_NAME): $(OBJS) $(RES_FILE) $(LIBS)

#UPDATE#
ALERTSVC.OBJ:   ALERTSVC.H ALERTSTR.H ALRTNTFY.H MANAGERS.H QUEUEMGR.H
UTIL.OBJ:       ALERTSVC.H
NTPOPUP.OBJ:    NTPOPUP.CPP ALERTSVC.H MANAGERS.H QUEUEMGR.H
NTMSNGR.OBJ:    NTMSNGR.CPP ALERTSVC.H MANAGERS.H QUEUEMGR.H
NTEVLOG.OBJ:    NTEVLOG.CPP ALERTSVC.H MANAGERS.H QUEUEMGR.H
NTALOG.OBJ:     NTALOG.CPP ALERTSVC.H MANAGERS.H QUEUEMGR.H
NTNLM.OBJ:      NTNLM.CPP ALERTSVC.H MANAGERS.H QUEUEMGR.H
NTFWD.OBJ:      NTFWD.CPP ALERTSVC.H MANAGERS.H QUEUEMGR.H
NTMAIL.OBJ:     NTMAIL.CPP ALERTSVC.H MANAGERS.H QUEUEMGR.H
NTSNMP.OBJ:     NTSNMP.CPP ALERTSVC.H MANAGERS.H QUEUEMGR.H
ALERTDEF.OBJ:   ALERTDEF.CPP ALERTSVC.H
MAINMGR.OBJ:    MAINMGR.CPP QUEUEMGR.H MAINMGR.H
MANAGERS.OBJ:   MANAGERS.CPP QUEUEMGR.H MANAGERS.H
QMGRPROF.OBJ:   QMGRPROF.CPP QUEUEMGR.H
RCVSTART.OBJ:   RCVSTART.CPP ALERTSVC.H ALRTRECV.H
ALRTRECV.OBJ:   ALRTRECV.CPP ALRTRECV.H
ATPRCEIV.OBJ:   ATPRCEIV.CPP ALRTRECV.H
IPXRCEIV.OBJ:   IPXRCEIV.CPP ALRTRECV.H
IPRECEIV.OBJ:   IPRECEIV.CPP ALRTRECV.H
ALERTSVC.RES:   ALERTSVC.RC ALERTMSG.RC ALERTSTR.H
ALERTMSG.RC:    ALERTMSG.MC
#ENDUPDATE#
