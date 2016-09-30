#-------------------------------------------------------------------------
#
# $Header:   S:/NAVAP/VCS/navap.mav   1.16   27 Apr 1998 10:39:14   DALLEE  $
#
# Description:
#       NAVAP makefile.
#
#-------------------------------------------------------------------------
#
# $Log:   S:/NAVAP/VCS/navap.mav  $
#  
#     Rev 1.16   27 Apr 1998 10:39:14   DALLEE
#  Added QUARADD.LIB to VXD and NTK builds.
#  
#     Rev 1.15   17 Feb 1998 21:33:38   RStanev
#  Updated stack swapping.
#
#     Rev 1.14   10 Jul 1997 13:34:36   RStanev
#  Alpha builds export DriverEntry, not DriverEntry@8.
#
#     Rev 1.13   08 Jul 1997 17:22:46   ksackin
#  Not linking in CERTLIBN.LIB and int64.lib on non-Intel platforms.
#
#     Rev 1.12   05 Jul 1997 20:10:48   RStanev
#  Forced new stack swapping architecture.
#
#     Rev 1.11   17 Jun 1997 16:53:28   MKEATIN
#  Added CALLBACK.LIB
#
#     Rev 1.10   02 Jun 1997 15:25:36   RStanev
#  Updated dependencies.
#
#     Rev 1.9   02 Jun 1997 15:19:36   MKEATIN
#  Removed NAVEX.LIB from the NTK platform.
#
#     Rev 1.8   28 May 1997 21:37:58   RStanev
#  NAVEX15 is now defined for all platforms.
#
#     Rev 1.7   23 May 1997 23:42:22   RStanev
#  Removed navcrc for NAVAP's NT driver.  Apparently the PE loader checks
#  the CRC of the driver image that it loads, and because navcrc writes to
#  the header of the executable without correcting the checksum, NT refuses
#  to load the driver.
#
#     Rev 1.6   21 May 1997 16:59:54   MKEATIN
#  Added XOPTS += /DNAVEX15
#
#     Rev 1.5   21 May 1997 04:51:10   RStanev
#  Added startup.obj and updated dependencies.
#
#     Rev 1.4   16 May 1997 10:28:40   JBRENNA
#  Chnage PAMLIB.LIB to PAMNTK.LIB ... that is its new name.
#
#     Rev 1.3   15 May 1997 03:04:20   RStanev
#  Cleaned up and updated dependencies.
#
#     Rev 1.2   29 Apr 1997 16:20:34   JBRENNA
#  Change to use CORE_* platform spec.
#
#     Rev 1.1   16 Apr 1997 11:27:08   RStanev
#  Removed Win32 support.  Driver is now linking to APCOMM.LIB instead of
#  using API.CPP.
#
#     Rev 1.0   09 Apr 1997 12:21:40   RStanev
#  Initial revision.
#
#-------------------------------------------------------------------------

TARGET_NAME=NAVAP

XOPTS += /DNAVEX15

#-------------------------------------------------------------------------
#
# Check for NTK platform.
#
#-------------------------------------------------------------------------
%If "$(CORE_SOFT)" == "K"

TARGET_TYPE = SYS

RES_FILE = NAVAPNT.RES
RES_SRC  = NAVAPNT.RC
DVX_FILE =

PLATOBJS = navapnt.obj

LIBS = \
 ntoskrnl.lib \
 hal.lib      \
 SYMEVENT.LIB \
 SYMKRNL.LIB  \
 DRVCOMM.LIB  \
 ASFCACHE.lib \
%If "$(CORE_PLAT)" == "I"
 CERTLIBN.LIB \
%EndIf
 PAMNTK.LIB   \
 N32COREI.LIB \
 N32EXCLU.LIB \
 N32XUTIL.LIB \
 APEVENT.LIB  \
 APKERNEL.LIB \
 APUI.LIB     \
 APCOMM.LIB   \
 CALLBACK.LIB \
 QUARADD.LIB  \
%If "$(CORE_PLAT)" == "I"
 int64.lib
%EndIf

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

LINKOPTSExt1=-MERGE:_PAGE=PAGE -MERGE:_TEXT=.text
LINKOPTSExt2=-MERGE:.rdata=.text
LINKOPTSExt3=-SECTION:INIT,d -OPT:REF
LINKOPTSExt4=-RELEASE -INCREMENTAL:NO -FULLBUILD -FORCE:MULTIPLE
LINKOPTSExt5=-version:3.51 -osversion:3.51 -align:0x20 -subsystem:native,3.51
%If "$(CORE_PLAT)" == "I"
LINKOPTSExt6=-base:0x10000 -entry:DriverEntry@8
%Else
LINKOPTSExt6=-base:0x10000 -entry:DriverEntry
%EndIf

SYSLIBS=

%ElseIf "$(CORE_SOFT)" == "V"

TARGET_TYPE = VXD

RES_FILE = NAVAPVXD.RES
RES_SRC  = NAVAPVXD.RC
DVX_FILE = NAVAP.DVX

PLATOBJS = vxdentry.obj \
           navapvxd.obj

LIBS = \
 DRVCOMM.LIB  \
 CERTLIBN.LIB \
 N32COREI.LIB \
 N32EXCLU.LIB \
 N32XUTIL.LIB \
 APEVENT.LIB  \
 APKERNEL.LIB \
 APUI.LIB     \
 APCOMM.LIB   \
 CALLBACK.LIB \
 QUARADD.LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

.vxd.crc:
  navcrc $[r,$*].vxd
  copy $[r,$*].vxd $[r,$*].crc

navap.crc: navap.vxd

%EndIf

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

$(PROGRAM_NAME): $(PLATOBJS)  \
                 init.obj     \
                 startup.obj  \
                 exit.obj     \
                 apstack.obj  \
                 config.obj   \
                 vlcomm.obj   \
                 apexcl.obj   \
                 shutdown.obj \
                 apstrs.obj   \
                 $(LIBS)      \
                 $(RES_FILE)  \
                 $(DVX_FILE)

COMMON_H = platform.h navap.h symevent.h avapi.h exclude.h actions.h apcfg.h

#UPDATE#
navapnt.obj:  $(COMMON_H) drvcomm.h navapi.h apstrs.h navapnt.res
vxdentry.obj: $(COMMON_H) platform.inc shell.inc navap.inc
navapvxd.obj: $(COMMON_H) xapi.h navapi.h apstrs.h
init.obj:     $(COMMON_H) drvcomm.h navapi.h
startup.obj:  $(COMMON_H)
exit.obj:     $(COMMON_H) drvcomm.h navapi.h
apstack.obj:  $(COMMON_H)
config.obj:   $(COMMON_H) symsync.h symcfg.h xapi.h file.h navapi.h \
              apexcl.h apstrs.h navbin.h
vlcomm.obj:   $(COMMON_H) file.h disk.h
apexcl.obj:   $(COMMON_H) apexcl.h
shutdown.obj: $(COMMON_H) symsync.h drvcomm.h apcomm.h navutil.h navapi.h \
              apstrs.h
apstrs.obj:   $(COMMON_H) apstrs.h
navapnt.res:  navapnt.rc
navapnt.rc:   navapnt.mc
navapvxd.res: navapvxd.rc
#ENDUPDATE#
