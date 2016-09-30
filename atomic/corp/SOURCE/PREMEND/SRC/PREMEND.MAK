########################################################################
#
# $Header:   S:/PREMEND/VCS/premend.mav   1.0   22 Sep 1998 21:45:22   VGLOSTE  $
#
# Description:
#       PREMEND:  Utility for preinstalling NAV 4.08, 4.09, 5.X
#
#       This file makes the PREMEND.EXE 32-bit windows preinstall
#       utility.
#
########################################################################
# $Log:   S:/PREMEND/VCS/premend.mav  $
#  
#     Rev 1.0   22 Sep 1998 21:45:22   VGLOSTE
#  Initial revision.
#  
########################################################################

TARGET_NAME=PREMEND
TARGET_TYPE=EXE

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OBJS= PREMEND.OBJ 

$(PROGRAM_NAME): \
    $(OBJS) \
    PREMEND.RES \
	DEFANNTY.LIB

