#####################################################################
## CALERT.MAK - Created - 05-07-97
##
## Copyright 1992-1997 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/CALERT/VCS/calert.mav   1.1   14 May 1997 17:25:02   DSACKIN  $
##
## Description:
##    This is the base CAlert class makefile.  This was taken from
##    the Alrtntfy project.
##
#####################################################################
##
## $Log:   S:/CALERT/VCS/calert.mav  $
#  
#     Rev 1.1   14 May 1997 17:25:02   DSACKIN
#  Added Packet.obj
#
#     Rev 1.0   14 May 1997 13:40:00   DSACKIN
#  Initial revision.
##
#####################################################################

TARGET_TYPE=LIB
TARGET_NAME=CALERT

PROGRAM_NAME=$(TARGET_NAME).LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OBJS  = CAlert.obj
OBJS += Packet.obj
OBJS += Buffers.obj
OBJS += Custom.obj
OBJS += DataAccs.obj
OBJS += String.obj

$(PROGRAM_NAME): $(OBJS)

#UPDATE#
CAlert.obj:     CAlert.cpp CAlert.h
Packet.obj:     Packet.cpp CAlert.h
Buffers.obj:    Buffers.cpp CAlert.h
Custom.obj:     Custom.cpp CAlert.h
DataAccs.obj:   DataAccs.cpp CAlert.h
String.obj:     String.cpp CAlert.h
#ENDUPDATE#
