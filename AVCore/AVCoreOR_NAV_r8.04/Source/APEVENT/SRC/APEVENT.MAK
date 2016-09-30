#-------------------------------------------------------------------------
#
# $Header:   S:/APEVENT/VCS/APEVENT.MAv   1.9   08 Jul 1998 16:14:34   RStanev  $
#
# Description:
#       APEVENT makefile.
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

#UPDATE#
#ENDUPDATE#
