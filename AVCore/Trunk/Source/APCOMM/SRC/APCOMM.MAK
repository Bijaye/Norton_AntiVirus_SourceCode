#-------------------------------------------------------------------------
#
# $Header:   S:/APCOMM/VCS/APCOMM.MAv   1.7   17 Feb 1998 14:24:24   RStanev  $
#
# Description:
#       APCOMM makefile.
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
#ENDUPDATE#
