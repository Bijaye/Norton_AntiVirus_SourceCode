# Copyright 1996 Symantec Corporation
#############################################################################
#
# $Header:   S:/LOADMGR/VCS/loadmgr.mav   1.1   18 Apr 1997 13:54:22   DSACKIN  $
#
# Description:
#
#############################################################################
#
# $Log:   S:/LOADMGR/VCS/loadmgr.mav  $
#  
#     Rev 1.1   18 Apr 1997 13:54:22   DSACKIN
#  Ported from Tesla
#  
#     Rev 1.0   13 Mar 1997 15:11:40   RFULLER
#  Initial revision
#  
#     Rev 1.0   08 Nov 1996 11:52:42   DKEEFER
#  Initial revision
#  
#     Rev 1.1   20 Apr 1996 13:47:00   TSmith
#  Changed target name from LOADMGR to LOADMGMT.
#
#     Rev 1.0   16 Apr 1996 19:20:54   TSmith
#  Initial revision.
#
#############################################################################
TARGET_NAME=LOADMGMT

%if "$(CV)" != ""
	XOPTS+=/D"_DEBUG" /DDEBUG
	LINKOPTS+=/DEBUG /DEBUGTYPE:BOTH
%endif

PROGRAM_NAME=$(TARGET_NAME).LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK


OBJS =	fuzzy.obj	\
	fuzyload.obj	\
	ntsrvutl.obj	\
	loadmgmt.obj

$(PROGRAM_NAME):        $(OBJS)

#UPDATE#
fuzzy.obj:	platform.h fuzzy.h errcode.h symassrt.h
fuzyload.obj:	platform.h fuzzy.h fuzyload.h errcode.h
ntsrvutl.obj:	platform.h ntsrvutl.h
loadmgmt.obj:	platform.h fuzzy.h fuzyload.h loadmgmt.h errcode.h ntsrvutl.h
#ENDUPDATE#
