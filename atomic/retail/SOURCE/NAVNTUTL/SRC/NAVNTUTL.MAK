########################################################################
#
# $Header:   S:/NAVNTUTL/VCS/navntutl.mav   1.0   10 Jun 1998 16:11:20   DBuches  $
#
# Description:
#
########################################################################
# $Log:   S:/NAVNTUTL/VCS/navntutl.mav  $
#  
#     Rev 1.0   10 Jun 1998 16:11:20   DBuches
#  Initial revision.
########################################################################

TARGET_NAME=NAVNTUTL
PROGRAM_NAME=$(TARGET_NAME).DLL
TARGET_TYPE=DLL
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OBJS=copystream.OBJ \
     DLLMAIN.OBJ \
     STREAMS.OBJ

LIBS+= ntdll.lib     

DEF+= /DBUILDING_NAVNUTIL

$(PROGRAM_NAME): $(OBJS) \
                 NAVNTUTL.RES

#UPDATE#
#ENDUPDATE#


