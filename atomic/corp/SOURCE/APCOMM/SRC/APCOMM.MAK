#-------------------------------------------------------------------------
#
# $Header:   S:/APCOMM/VCS/APCOMM.MAv   1.7   17 Feb 1998 14:24:24   RStanev  $
#
# Description:
#       APCOMM makefile.
#
#-------------------------------------------------------------------------
#
# $Log:   S:/APCOMM/VCS/APCOMM.MAv  $
#  
#     Rev 1.7   17 Feb 1998 14:24:24   RStanev
#  Cleaned up stack swapping.
#
#     Rev 1.6   05 Jul 1997 20:06:04   RStanev
#  Forced new stack swapping functions.
#
#     Rev 1.5   29 May 1997 11:01:44   RStanev
#  Added commstrs.obj.
#
#     Rev 1.4   28 May 1997 21:23:34   RStanev
#  NAVEX15 is now defined for all platforms.
#
#     Rev 1.3   22 May 1997 11:32:34   MKEATIN
#  defined NAVEX15 for the VXD platform
#
#     Rev 1.2   15 May 1997 00:26:36   RStanev
#  Cleaned up and updated dependencies.
#
#     Rev 1.1   29 Apr 1997 16:28:12   JBRENNA
#  Convert to new CORE_ platform spec.
#
#     Rev 1.0   16 Apr 1997 11:00:44   RStanev
#  Initial revision.
#
#-------------------------------------------------------------------------

TARGET_NAME = APCOMM
TARGET_TYPE = LIB

XOPTS += /DNAVEX15

PROGRAM_NAME=$(TARGET_NAME).LIB

%If "$(CORE_SOFT)" != ""

$(PROGRAM_NAME): apcommk.obj  \
                 commstrs.obj

%Else

$(PROGRAM_NAME): apcommu.obj

%Endif

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

#UPDATE#
apcommk.obj:  platform.h symsync.h apcomm.h apcommi.h navap.h symevent.h \
              avapi.h exclude.h actions.h apcfg.h commstrs.h
apcommu.obj:  platform.h drvcomm.h apcomm.h
commstrs.obj: platform.h commstrs.h
#ENDUPDATE#
