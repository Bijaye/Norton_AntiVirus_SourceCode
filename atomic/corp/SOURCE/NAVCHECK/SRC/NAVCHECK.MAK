#************************************************************************
# Copyright 1997 Symantec, Peter Norton Product Group
#************************************************************************
# WKSTDIST.MAK
#
# $Header:   S:/NAVCHECK/VCS/navcheck.mav   1.1   18 Aug 1997 21:31:56   TCASHIN  $
#
# This file contains the dependencies and rules required to build the
# NAVCHECK program
#
#************************************************************************
# $Log:   S:/NAVCHECK/VCS/navcheck.mav  $
#  
#     Rev 1.1   18 Aug 1997 21:31:56   TCASHIN
#  Removed unused libs.
#  
#     Rev 1.0   18 Aug 1997 21:15:26   TCASHIN
#  Initial revision.
#************************************************************************

PROGRAM_NAME=NAVCHECK.EXE

MODEL=L
SYSLIBS=
NDSTRINGS=YES
LINKER=OLINK403.EXE
CLOPTS+=/Zi

.include $(QUAKEINC)\PLATFORM.MAK

TARGET_TYPE=EXE

$(PROGRAM_NAME): navcheck.obj     \
                 navckstr.obj     \
                 dosio.lib        \
                 dosui.lib        \
                 $(Q_SYMKRNL).lib

navcheck.obj:    navcheck.c navcheck.h platform.h
navckstr.obj :   navckstr.c navcheck.h platform.h
