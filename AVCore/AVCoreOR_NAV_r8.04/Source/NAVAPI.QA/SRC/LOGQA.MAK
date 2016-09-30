# Copyright 1996 Symantec, Peter Norton Product Group
###########################################################################
#
# $Header:   S:/NAVAPI.QA/VCS/logqa.mav   1.0   05 Aug 1998 07:59:56   mmendon  $
#
###########################################################################
#
# $Log:   S:/NAVAPI.QA/VCS/logqa.mav  $
#  
#     Rev 1.0   05 Aug 1998 07:59:56   mmendon
#  Initial revision.
#  
#     Rev 1.1   24 Nov 1997 20:12:12   ERAHHAL
#  Renamed library to logqa
#  
#     Rev 1.0   24 Nov 1997 11:09:44   ERAHHAL
#  Initial revision.
#  
###########################################################################

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

TARGET_NAME=logqa
PROGRAM_NAME=$(TARGET_NAME).lib
TARGET_TYPE=LIB


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

$(PROGRAM_NAME):    eventobj.obj    \
                    logqa.obj


#UPDATE#

eventobj.cpp: eventobj.h

logqa.cpp: logqa.h eventobj.h ccritsec.h

#ENDUPDATE#
