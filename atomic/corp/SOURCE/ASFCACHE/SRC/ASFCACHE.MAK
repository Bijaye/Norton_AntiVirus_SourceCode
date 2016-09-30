# Copyright 1996 Symantec Corporation
#############################################################################
#
# $Header:   S:/ASFCACHE/VCS/asfcache.mav   1.1   21 May 1997 18:46:06   BILL  $
#
# Description:
#
#############################################################################
#
# $Log:   S:/ASFCACHE/VCS/asfcache.mav  $
#  
#     Rev 1.1   21 May 1997 18:46:06   BILL
#  Don't build asm routines for alpha, use c ones
#  
#     Rev 1.0   06 Feb 1997 20:48:24   RFULLER
#  Initial revision
#  
#     Rev 1.1   13 May 1996 18:58:10   TSmith
#
#     Rev 1.0   02 May 1996 16:20:06   TSmith
#  Initial revision.
#
#############################################################################
TARGET_NAME=ASFCACHE
PROGRAM_NAME=$(TARGET_NAME).LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

%If "$(CORE_PLAT)" == "I"
OBJS =  ASFCACHE.OBJ    \
        CACHEMOV.OBJ
%Else
OBJS =  ASFCACHE.OBJ    
%Endif

$(PROGRAM_NAME):        $(OBJS)

#UPDATE#
ASFCACHE.OBJ:   PLATFORM.H FILE.H ASFCACHE.H
CACHEMOV.OBJ:   PLATFORM.INC CTSN.INC
#ENDUPDATE#
