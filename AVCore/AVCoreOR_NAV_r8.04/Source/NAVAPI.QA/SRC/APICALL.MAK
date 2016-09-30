# Copyright 1998 Symantec, Peter Norton Product Group
#************************************************************************
#
# $Header:   S:/NAVAPI.QA/VCS/apicall.mav   1.2   11 Aug 1998 16:31:00   mmendon  $
#
# Description: Makefile for APICall library file
#
#************************************************************************
# $Log:   S:/NAVAPI.QA/VCS/apicall.mav  $
#  
#     Rev 1.2   11 Aug 1998 16:31:00   mmendon
#  Backed out header file change
#
#     Rev 1.1   11 Aug 1998 13:40:26   mmendon
#  Added path for navapi.h
#
#     Rev 1.0   06 Aug 1998 12:16:38   NMILLER
#  Initial revision.
#
#************************************************************************

TARGET_NAME=APICALL
TARGET_TYPE=LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE)

# WIN16
# -----

%If ("$(CORE_WIN16)" != "")
    MODEL=L

# DX
# --

%ElseIf ("$[u,$(PLATFORM)]" == ".DX")  || \
        ("$[u,$(PLATFORM)]" == ".DVX") || \
        ("$[u,$(PLATFORM)]" == ".NCX")
    LIBS+= $(Q_SYMKRNL).LIB

%EndIf

OBJS  = APICall.obj

$(PROGRAM_NAME): $(OBJS) $(LIBS)

#UPDATE#
APICall.obj:      APICall.cpp APICall.h Structs.h
#ENDUPDATE#