#-------------------------------------------------------------------------
#
# $Header:   S:/APKERNEL/VCS/APKERNEL.MAv   1.7   19 Feb 1998 16:52:22   RStanev  $
#
# Description:
#       APKERNEL makefile.
#
#-------------------------------------------------------------------------
#
# $Log:   S:/APKERNEL/VCS/APKERNEL.MAv  $
#  
#     Rev 1.7   19 Feb 1998 16:52:22   RStanev
#  Included fquarant.obj in OBJS.
#
#     Rev 1.6   19 Feb 1998 16:10:34   RStanev
#  Added quarantine support.
#
#     Rev 1.5   18 Feb 1998 21:46:54   RStanev
#  Removed frepair.obj dependency on krnlstrs.h.
#
#     Rev 1.4   18 Feb 1998 21:15:28   RStanev
#  Removed unknown virus detection.
#
#     Rev 1.3   28 May 1997 20:43:20   RStanev
#  NAVEX15 is now defined for all platforms.
#
#     Rev 1.2   22 May 1997 11:39:12   MKEATIN
#  Defined NAVEX15 for the VXD platform
#
#     Rev 1.1   15 May 1997 02:09:58   RStanev
#  Cleaned up and updated dependencies.
#
#     Rev 1.0   26 Feb 1997 16:39:40   RSTANEV
#  Initial revision.
#
#-------------------------------------------------------------------------

TARGET_NAME = APKERNEL
TARGET_TYPE = LIB

XOPTS += /DNAVEX15

OBJS = krnlapi.obj  \
       taskscan.obj \
       actions.obj  \
       frepair.obj  \
       fquarant.obj \
       fmoveren.obj \
       krnlstrs.obj

%If "$(CORE_SOFT)" == "V"

OBJS += taskvl.obj

%EndIf

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=$(TARGET_NAME).LIB

$(PROGRAM_NAME): $(OBJS)

COMMON_H = platform.h navap.h symevent.h avapi.h exclude.h actions.h apcfg.h

#UPDATE#
krnlapi.obj:  $(COMMON_H) apkernel.h
taskscan.obj: $(COMMON_H) apkernel.h drvcomm.h apcomm.h
actions.obj:  $(COMMON_H)
frepair.obj:  $(COMMON_H) apkernel.h xapi.h
fquarant.obj: $(COMMON_H) apkernel.h
fmoveren.obj: $(COMMON_H) apkernel.h xapi.h
krnlstrs.obj: platform.h krnlstrs.h
taskvl.obj:   $(COMMON_H) apkernel.h disk.h
#ENDUPDATE#
