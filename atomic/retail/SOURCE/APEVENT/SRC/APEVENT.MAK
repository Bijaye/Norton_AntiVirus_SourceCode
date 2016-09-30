#-------------------------------------------------------------------------
#
# $Header:   S:/APEVENT/VCS/APEVENT.MAv   1.9   08 Jul 1998 16:14:34   RStanev  $
#
# Description:
#       APEVENT makefile.
#
#-------------------------------------------------------------------------
#
# $Log:   S:/APEVENT/VCS/APEVENT.MAv  $
#  
#     Rev 1.9   08 Jul 1998 16:14:34   RStanev
#  Added asyncecb.obj.
#
#     Rev 1.8   17 Feb 1998 16:48:40   RStanev
#  Cleaned up stack swapping.
#
#     Rev 1.7   05 Jul 1997 20:08:32   RStanev
#  Forced new stack swapping architecture.
#
#     Rev 1.6   02 Jul 1997 19:03:02   RStanev
#  Updated dependencies.
#
#     Rev 1.5   03 Jun 1997 09:35:50   RStanev
#  Updated dependencies.
#
#     Rev 1.4   28 May 1997 21:15:00   RStanev
#  NAVEX15 is now defined for all platforms.
#
#     Rev 1.3   22 May 1997 11:35:38   MKEATIN
#  define NAVEX15 on the VXD platform
#
#     Rev 1.2   15 May 1997 01:42:04   RStanev
#  Cleaned up and updated dependencies.
#
#     Rev 1.1   29 Apr 1997 17:56:46   JBRENNA
#  Change from VXD platfrom spec to use CORE_SOFT check.
#
#     Rev 1.0   16 Mar 1997 13:00:34   RStanev
#  Initial revision.
#
#-------------------------------------------------------------------------

TARGET_NAME = APEVENT
TARGET_TYPE = LIB

XOPTS += /DNAVEX15

OBJS = eventcbs.obj \
       edextra.obj  \
       evpacket.obj \
       evtasks.obj  \
       pel.obj      \
       monitor.obj  \
       filter.obj   \
       device.obj   \
       evstrs.obj

%If "$(CORE_SOFT)" == "V"

OBJS += oefilter.obj \
        asyncecb.obj

%EndIf

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=$(TARGET_NAME).LIB

$(PROGRAM_NAME): $(OBJS)

COMMON_H = platform.h navap.h symevent.h avapi.h exclude.h actions.h apcfg.h

#UPDATE#
eventcbs.obj: $(COMMON_H) apevent.h pel.h monitor.h filter.h
edextra.obj:  $(COMMON_H) apevent.h evstrs.h filter.h
evpacket.obj: $(COMMON_H) apevent.h
evtasks.obj:  $(COMMON_H) apevent.h monitor.h filter.h
pel.obj:      $(COMMON_H) symsync.h pel.h
monitor.obj:  $(COMMON_H) symsync.h monitor.h
filter.obj:   $(COMMON_H) dbcs.h symsync.h monitor.h filter.h asfcache.h
device.obj:   $(COMMON_H) apevent.h xapi.h disk.h
evstrs.obj:   platform.h evstrs.h
oefilter.obj: $(COMMON_H) filter.h
asyncecb.obj: platform.inc
#ENDUPDATE#
