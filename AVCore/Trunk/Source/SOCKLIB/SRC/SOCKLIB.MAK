# Copyright 1996 Symantec Corporation
########################################################################
#
# $Header:   S:/SOCKLIB/VCS/SOCKLIB.MAv   1.1   28 Apr 1997 19:59:42   JBRENNA  $
#
# Description:
#       Portable Socket Library for AvIS
#
########################################################################
# $Log:   S:/SOCKLIB/VCS/SOCKLIB.MAv  $
#  
#     Rev 1.1   28 Apr 1997 19:59:42   JBRENNA
#  Change W32 check to use new CORE_WIN32 check.
#  
#     Rev 1.0   25 Apr 1997 11:51:48   ksackin
#  Initial revision.
#  
#     Rev 1.1   04 Nov 1996 18:26:32   RAY
#  Added IPFilter
#  
#     Rev 1.0   24 Sep 1996 17:34:06   DJesch
#  Initial revision.
#
########################################################################

TARGET_TYPE=LIB
TARGET_NAME=SOCKLIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

DEF = /DVC20

%If "$(CORE_WIN32)" != ""
    PROGRAM_NAME    = $(TARGET_NAME).LIB
    DEF_NAME        = SOCKLIB
%Else
BAD_PLATFORM:
    beep
    echo The $(PLATFORM) platform is not supported for this project.
%Endif

OBJS =  ossocket.obj smtpmail.obj ipfilter.obj

$(PROGRAM_NAME): \
 $(OBJS)

#UPDATE#
ossocket.obj:   ossocket.cpp ossocket.h
smtpmail.obj:   smtpmail.cpp smtpmail.h
ipfilter.obj:   ipfilter.cpp ipfilter.h
#ENDUPDATE#
