# Copyright 1996- Symantec Corporation
#*************************************************************************
#
# $Header:   S:/navzip/VCS/navzip.mav   1.0   06 Feb 1997 21:05:56   RFULLER  $
#
# Description:
#       Nav zip library make file.
#
#*************************************************************************
# $Log:   S:/navzip/VCS/navzip.mav  $
#  
#     Rev 1.0   06 Feb 1997 21:05:56   RFULLER
#  Initial revision
#  
#     Rev 1.2   21 Nov 1996 16:47:54   MKEATIN
#  Added the NCX platform
#  
#     Rev 1.1   19 Sep 1996 14:29:36   JALLEE
#  Added DVX platform.
#  
#     Rev 1.0   24 Jun 1996 14:03:36   JALLEE
#  Initial revision.
#*************************************************************************
PROGRAM_NAME=NAVZIP.LIB
NDSTRINGS=YES

TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME):    \
%if "$(PLATFORM)" == ".DX" || "$(PLATFORM)" == ".DVX" || "$(PLATFORM)" == ".NCX"
    noldname.obj    \
%endif
    navzip.obj      \
    mntr.obj        \
    all.lib


#UPDATE#
test.obj: platform.h
#ENDUPDATE#
