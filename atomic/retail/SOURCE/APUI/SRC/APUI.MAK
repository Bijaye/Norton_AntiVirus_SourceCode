#-------------------------------------------------------------------------
#
# $Header:   S:/APUI/VCS/APUI.MAv   1.4   28 May 1997 21:29:48   RStanev  $
#
# Description:
#       APUI makefile
#
#-------------------------------------------------------------------------
#
# $Log:   S:/APUI/VCS/APUI.MAv  $
#  
#     Rev 1.4   28 May 1997 21:29:48   RStanev
#  NAVEX15 is now defined for all platforms.
#
#     Rev 1.3   22 May 1997 11:42:22   MKEATIN
#  Defined NAVEX15 for the VXD platform
#
#     Rev 1.2   15 May 1997 02:38:02   RStanev
#  Cleaned up and updated dependencies.
#
#     Rev 1.1   29 Apr 1997 18:40:52   JBRENNA
#  Change VXD platform check to use CORE_SOFT instead.
#
#     Rev 1.0   09 Apr 1997 12:15:52   RStanev
#  Initial revision.
#
#-------------------------------------------------------------------------

TARGET_NAME = APUI
TARGET_TYPE = LIB

XOPTS += /DNAVEX15

OBJS = buildtxt.obj \
       known.obj    \
       unknown.obj  \
       uimisc.obj   \
       asyncpck.obj \
       asyncmsg.obj \
       uistrs.obj

%If "$(CORE_SOFT)" == "V"

OBJS += vlact.obj \
        vxdui.obj

%EndIf

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=$(TARGET_NAME).LIB

$(PROGRAM_NAME): $(OBJS)

COMMON_H = platform.h navap.h symevent.h avapi.h exclude.h actions.h apcfg.h

#UPDATE#
buildtxt.obj: $(COMMON_H) apui.h uistrs.h xapi.h
known.obj:    $(COMMON_H) apui.h uistrs.h
unknown.obj:  $(COMMON_H) apui.h uistrs.h
uimisc.obj:
asyncpck.obj: $(COMMON_H) apui.h uistrs.h drvcomm.h apcomm.h
asyncmsg.obj: $(COMMON_H) apui.h uistrs.h
uistrs.obj:   $(COMMON_H) uistrs.h
vlact.obj:    $(COMMON_H) apui.h uistrs.h
vxdui.obj:    platform.inc minivdd.inc vkd.inc vmd.inc
#ENDUPDATE#
