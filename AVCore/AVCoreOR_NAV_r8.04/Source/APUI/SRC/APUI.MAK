#-------------------------------------------------------------------------
#
# $Header:   S:/APUI/VCS/APUI.MAv   1.4   28 May 1997 21:29:48   RStanev  $
#
# Description:
#       APUI makefile
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

#UPDATE#
#ENDUPDATE#
