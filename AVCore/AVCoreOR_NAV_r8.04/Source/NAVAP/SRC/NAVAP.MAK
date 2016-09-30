#-------------------------------------------------------------------------
#
# $Header:   S:/NAVAP/VCS/navap.mav   1.16   27 Apr 1998 10:39:14   DALLEE  $
#
# Description:
#       NAVAP makefile.
#
#-------------------------------------------------------------------------

TARGET_NAME=NAVAP

XOPTS += /DNAVEX15

NAVCRC=$(PROJ)\SRC$(PROJVERS)\NAVCRC.EXE

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
 N32UTILS.LIB \
 APUTIL.LIB   \
 APEVENT.LIB  \
 APKERNEL.LIB \
 APUI.LIB     \
 APCOMM.LIB   \
 CALLBACK.LIB \
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
 N32UTILS.LIB \
 APUTIL.LIB   \
 APEVENT.LIB  \
 APKERNEL.LIB \
 APUI.LIB     \
 APCOMM.LIB   \
 CALLBACK.LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

.vxd.crc:
  $(NAVCRC) $[r,$*].vxd
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

#UPDATE#
navapnt.obj:  navapnt.res
navapnt.res:  navapnt.rc
navapnt.rc:   navapnt.mc
navapvxd.res: navapvxd.rc
#ENDUPDATE#
