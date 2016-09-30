#-------------------------------------------------------------------------
#
# $Header:   S:/APKERNEL/VCS/APKERNEL.MAv   1.7   19 Feb 1998 16:52:22   RStanev  $
#
# Description:
#       APKERNEL makefile.
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

#UPDATE#
#ENDUPDATE#
