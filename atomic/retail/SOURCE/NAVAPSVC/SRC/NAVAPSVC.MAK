# Copyright 1996 Symantec Coproation, Peter Norton Computing Group
# ***************************************************************************
#
# $Header:   S:/NAVAPSVC/VCS/navapsvc.mav   1.14   28 May 1998 19:04:26   mdunn  $
#
# Description:
#	This is the make file for NAV Auto-Protect  Service
#
# ***************************************************************************
# $Log:   S:/NAVAPSVC/VCS/navapsvc.mav  $
#  
#     Rev 1.14   28 May 1998 19:04:26   mdunn
#  Fixed DEFUTILS.LIB line - no longer hard-coded to get the file from CORE,
#  since CORE ain't the right place any more (it's AVCOMP).
#  
#     Rev 1.13   13 Aug 1997 09:35:18   JBRENNA
#  Enchange the makefile such that it supports branched projects. Hopefully,
#  this works.
#  
#     Rev 1.12   12 Aug 1997 13:39:34   JBRENNA
#  Remove linking to N32VList.lib. We no longer need it.
#  
#     Rev 1.11   16 Jul 1997 12:17:08   KSACKIN
#  Added an RCOPT define of _ALPHA_ if we are compiling for DEC Alpha.  This
#  is so that we can ifdef the name of the services in our resource file.
#
#     Rev 1.10   19 Jun 1997 12:07:20   JBRENNA
#  Changed N32CALLBK.LIB to N32Call.LIB.
#
#     Rev 1.9   17 Jun 1997 18:39:24   MKEATIN
#  Added N32CALLBK.LIB
#
#     Rev 1.8   06 Jun 1997 11:29:04   JBRENNA
#  include DefUtils.LIB in the linking.
#
#     Rev 1.7   27 May 1997 16:39:02   KSACKIN
#  Changed the NavAlert.lib dependency to XNTAlert.lib.  This is the dll for
#  UNICODE.
#
#     Rev 1.6   20 May 1997 11:52:28   DSACKIN
#  Removed the dependency on ALRTNTFY.LIB.
#
#     Rev 1.5   16 May 1997 16:27:22   DSACKIN
#  Added CAlert.lib and CVAlert.lib
#
#     Rev 1.4   02 May 1997 22:21:26   DSACKIN
#  We no longer do anything with n32serve.lib
#
#     Rev 1.3   02 May 1997 12:40:00   JBRENNA
#  Provide Full path for ANSI libraries this project uses.
#
#     Rev 1.2   30 Apr 1997 13:45:32   JBRENNA
#  Use N32SERVE.LIB instead of NAVSERVE.LIB.
#
#     Rev 1.1   18 Apr 1997 13:59:10   DSACKIN
#  Ported from Tesla
#
#     Rev 1.0   13 Mar 1997 15:12:16   RFULLER
#  Initial revision
#
#     Rev 1.7   07 Feb 1997 07:24:16   TSmith
#  Added UNIUTIL.LIB.
#
#     Rev 1.6   03 Feb 1997 19:48:26   JBELDEN
#  added OLE32.LIB to libraries so that symtrial can be used
#  for preview version
#
#     Rev 1.5   27 Jan 1997 18:19:42   DSACKIN
#  Added a dependancy to NV32List
#
#     Rev 1.4   30 Dec 1996 09:44:34   TSmith
#  Brought file into synch with ALERTSVC.MAK so both services
#  will build the same way.
#
#     Rev 1.3   24 Dec 1996 18:55:08   DSACKIN
#  Switched it back to linking with ALRTNFYD.LIB when we are in debug mode.
#
#     Rev 1.2   05 Dec 1996 03:58:54   DSACKIN
#  Include enough MFC stuff to get the alert options loaded.
#
#     Rev 1.1   08 Nov 1996 17:47:56   TSMITH
#  Added ALRTNTFY.LIB, removed NAVAPLOG.OBJ.
#
#     Rev 1.17   09 Sep 1996 11:51:40   jmillar
#  change the name of executable for the preview version to NAVSVCPR
#
#     Rev 1.16   08 Sep 1996 15:41:12   DSACKIN
#  Added entries for the NAVWNTPR version.
#
#     Rev 1.15   22 Aug 1996 00:42:44   DSACKIN
#  Added NAVAPDEF.OBJ to the list of files for this project.
#
#     Rev 1.14   19 Jul 1996 01:01:24   DSACKIN
#  Link to our CLinkServer library.
#
#     Rev 1.13   20 Jun 1996 02:34:24   DSACKIN
#  Removed DEBUG_PROFILE define.  This is now done in Profiler.h
#
#     Rev 1.12   13 Jun 1996 17:24:24   DSACKIN
#  Added DEBUG_PROFILE strings.
#
#     Rev 1.11   22 May 1996 14:10:48   jmillar
#  change nntxutil to xntxutil
#
#     Rev 1.10   17 May 1996 16:57:38   PVENKAT
#  Added NAVSERVE.DLL      for NotifyServersOfVirus.
#
#     Rev 1.9   16 May 1996 20:36:22   DSACKIN
#  Added Rad's new appacket file.
#
#     Rev 1.8   16 May 1996 11:10:14   DSACKIN
#  Split NAVAPSVC.OBJ into 5 new files.
#
#     Rev 1.7   08 May 1996 20:02:20   DSACKIN
#  Changed the makefile to set the subsystem before including platform.mak.
#
#     Rev 1.6   30 Apr 1996 16:30:32   PVENKAT
#
#     Rev 1.5   30 Apr 1996 15:04:00   PVENKAT
#
#     Rev 1.4   25 Apr 1996 14:52:08   PVENKAT
#  Added NNTXUTIL.LIB to use NavApDoCommandActivityLog
#
#     Rev 1.3   24 Apr 1996 20:52:10   DSACKIN
#  Added SVCCTRL.LIB for controlling the other services.
#
#     Rev 1.2   18 Apr 1996 12:57:22   PVENKAT
#  Included Service.lib (CService Version.).
# ***************************************************************************
TARGET_TYPE=EXE
CHAR_SET=U                              # Set Unicode
USE_MFC_DLL=TRUE                        # Link MFC dynamically
USE_CRT_DLL=TRUE                        # Link C run time library dynamically
CONSOLE_MODE=YES
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

                                        ## Define _ALPHA_ for our RC compiler.
%If "$(CORE_PLAT)" == "A"
   RCOPTS += /D_ALPHA_
%Endif

%If "$[u,$(TARGET)]" == "NAVWNTPR"      # Check for NT - preview edition
    TARGET_NAME = NAVSVCPR
    DEF += -DNAVWNT_PREVIEW
%Else
    TARGET_NAME = NAVAPSVC
%Endif

PROGRAM_NAME    = $(TARGET_NAME).EXE
RES_FILE        = NAVAPSVC.RES
RES_SRC         = NAVAPSVC.RC
XOPTS           = -GX                   # Enable C++ exception handling
UNICODE         = -DUNICODE -D_UNICODE
DEF             += $(UNICODE)           # More Unicode

SYSLIBS += COMCTL32.LIB                 # Default SysLibs settings forgot this

OBJS  = NAVAPSVC.OBJ
OBJS += NAVAPSCN.OBJ
OBJS += NAVAPCMD.OBJ
OBJS += NAVAPCBK.OBJ
OBJS += APPACKET.OBJ
OBJS += NAVAPDEF.OBJ

AnsiLibDir       = $(PROJPRE)LIB$(PROJVERS).$(CORE_PLAT)$(CORE_BMODE)A
AnsiFamilyLibDir = $(FAMILYPRE)\LIB$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)A
AnsiCoreLibDir   = $(QUAKEPRE)\LIB$(QUAKE).$(CORE_PLAT)$(CORE_BMODE)A

LIBS  = NETAPI32.LIB
LIBS += $(AnsiCoreLibDir)\DRVCOMM.LIB
LIBS += $(AnsiCoreLibDir)\$(Q_SYMDEBUG).LIB
LIBS += DefUtils.LIB
LIBS += $(AnsiFamilyLibDir)\$(Q_SYMUTIL).LIB
LIBS += $(AnsiFamilyLibDir)\$(Q_SYMKRNL).LIB
LIBS += $(AnsiFamilyLibDir)\$(Q_SYMGUI).LIB
LIBS += $(AnsiFamilyLibDir)\V32SCAN.LIB
LIBS += $(AnsiFamilyLibDir)\N32CALL.LIB
LIBS += $(AnsiLibDir)\XNTXUTIL.LIB
LIBS += SERVICE.LIB
LIBS += SVCCTRL.LIB
LIBS += LINKSRV.LIB
LIBS += XNTALERT.LIB
LIBS += OLE32.LIB
LIBS += UNIUTIL.LIB
LIBS += CALERT.LIB
LIBS += CVALERT.LIB

#  %If "$(CV)" == ""
#      LIBS += ALRTNTFY.LIB
#  %Else
#      LIBS += ALRTNFYD.LIB
#  %Endif

$(PROGRAM_NAME): $(OBJS) $(RES_FILE) $(LIBS)

#UPDATE#
NAVAPSVC.OBJ:   NAVAPSVC.H APSVCSTR.H
NAVAPSCN.OBJ:   NAVAPSVC.H
NAVAPSVC.RES:   NAVAPSVC.RC APSVCMSG.RC APSVCSTR.H
APSVCMSG.RC:    APSVCMSG.MC
#ENDUPDATE#
