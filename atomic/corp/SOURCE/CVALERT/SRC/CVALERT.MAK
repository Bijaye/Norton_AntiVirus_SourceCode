#####################################################################
## CVALERT.MAK - Created - 05-14-97
##
## Copyright 1992-1997 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/CVALERT/VCS/cvalert.mav   1.0   16 May 1997 13:29:24   DSACKIN  $
##
## Description:
##    This is the makefile for the CVirusAlert class.
##
#####################################################################
##
## $Log:   S:/CVALERT/VCS/cvalert.mav  $
#  
#     Rev 1.0   16 May 1997 13:29:24   DSACKIN
#  Initial revision.
##
#####################################################################

TARGET_TYPE=LIB
TARGET_NAME=CVALERT

PROGRAM_NAME=$(TARGET_NAME).LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OBJS  = CVAlert.obj
OBJS += Strings.obj
OBJS += Data.obj

$(PROGRAM_NAME): $(OBJS)

#UPDATE#
CVAlert.obj:    CValert.cpp CVAlert.h CAlert.h
Strings.obj:    Strings.cpp CVAlert.h CAlert.h
Data.obj:       Data.cpp CVAlert.h CAlert.h
#ENDUPDATE#
