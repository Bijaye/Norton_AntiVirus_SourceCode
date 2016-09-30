# Copyright 1996-1997, Symantec Corporation                             #
#########################################################################
#                                                                       #
# $Header:   S:/SYMKRNL/VCS/drvcomm.mav   1.5   12 Apr 1997 14:49:48   DALLEE  $
#                                                                       #
# Description:                                                          #
#                                                                       #
#########################################################################
# $Log:   S:/SYMKRNL/VCS/drvcomm.mav  $
#  
#     Rev 1.5   12 Apr 1997 14:49:48   DALLEE
#  Modified to support new VxD and NT Kernel platforms.
#  Uses CORE_SOFT for conditionals.
#  
#     Rev 1.4   06 Jan 1997 16:57:48   RSTANEV
#  Added SYM_VXD support.
#
#     Rev 1.3   02 Dec 1996 16:45:26   BMCCORK
#  Added test platform support
#
#     Rev 1.2   09 Apr 1996 10:58:16   JMILLAR
#  add NTK support, building drvcommk instead of drvcommu
#  update dependencies
#
#     Rev 1.1   22 Mar 1996 15:01:54   RSTANEV
#  Added /DSYM_PARANOID switch to C compiles.
#
#     Rev 1.0   18 Mar 1996 11:17:46   RSTANEV
#  Initial revision.
#
#########################################################################

TARGET_NAME=DRVCOMM
TARGET_TYPE=LIB
.INCLUDE <PLATFORM.MAK>

PROGRAM_NAME=$(TARGET_NAME).LIB

%If "$(PLATFORM)" == ".NTK" || "$(PLATFORM)" == ".VXD" || "$(CORE_SOFT)" == "V" || "$(CORE_SOFT)" == "K"

$(PROGRAM_NAME): drvcommk.obj

%Else

$(PROGRAM_NAME): drvcommu.obj

%Endif

#UPDATE#
drvcommk.obj:	platform.h dbcs_str.h dosvmm.h symsync.h drvcomm.h
drvcommu.obj:	platform.h dbcs_str.h dosvmm.h process.h drvcomm.h
#ENDUPDATE#
