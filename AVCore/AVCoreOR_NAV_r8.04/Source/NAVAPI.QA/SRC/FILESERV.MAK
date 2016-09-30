# Copyright 1998 Symantec, Peter Norton Product Group
#************************************************************************
#
# $Header:   S:/NAVAPI.QA/VCS/fileserv.mav   1.0   05 Aug 1998 15:19:52   mmendon  $
#
# Description: Makefile for CFileServ class.
#
#************************************************************************
# $Log:   S:/NAVAPI.QA/VCS/fileserv.mav  $
#  
#     Rev 1.0   05 Aug 1998 15:19:52   mmendon
#  Initial revision.
#
#     Rev 1.0   04 Aug 1998 11:22:18   NMILLER
#  Initial revision.
#
#************************************************************************

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

TARGET_NAME=FILESERV
TARGET_TYPE=LIB

PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE)

CLOPTS += /GX

# WIN16
# -----

%If ("$(CORE_WIN16)" != "")
    MODEL=L
    LIBS+= $(Q_SYMKRNL).LIB

# DX
# --

%ElseIf ("$[u,$(PLATFORM)]" == ".DX")  || \
        ("$[u,$(PLATFORM)]" == ".DVX") || \
        ("$[u,$(PLATFORM)]" == ".NCX")
    LIBS+= $(Q_SYMKRNL).LIB

%EndIf

OBJS  = FileServ.obj

$(PROGRAM_NAME): $(OBJS)

#UPDATE#
FileServ.obj:      FileServ.cpp FileServ.h
#ENDUPDATE#