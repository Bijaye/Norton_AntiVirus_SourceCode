#####################################################################
## FSLINK.MAK - Created - 12/01/97
##
## Copyright 1992-1997 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/FILELINK/VCS/FSLINK.MAv   1.2   23 Jun 1998 15:10:10   mdunn  $
##
## Description:
##    This is the makefile for the Server Link
##
#####################################################################
##
## $Log:   S:/FILELINK/VCS/FSLINK.MAv  $
#  
#     Rev 1.2   23 Jun 1998 15:10:10   mdunn
#  Added filelink.rc.
#  
#     Rev 1.1   20 Apr 1998 22:06:20   SEDWARD
#  Added QuarLink stuff.
#
#     Rev 1.0   17 Mar 1998 19:45:20   SEDWARD
#  Initial revision.
#
#     Rev 1.0   28 Jan 1998 15:20:26   KSACKIN
#  Initial revision.
#
#     Rev 1.0   27 Jan 1998 12:52:24   KSACKIN
#  Initial revision.
#
#     Rev 1.0   02 Dec 1997 10:42:34   DSACKIN
#  Initial revision.
##
#####################################################################

TARGET_TYPE=DLL
TARGET_NAME=FSLINK
PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

MIDLOPTS=

XOPTS += /DBUILDING_FILELINK_DLL

UNILIBS = $(FAMILYPRE)\LIB$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)U\LinkCli.lib

OBJS  = FSLink.obj
OBJS += FileLink.obj
OBJS += QuarLink.obj
OBJS += Serial.obj
OBJS += Protocol.obj
OBJS += ISLink_c.obj

$(PROGRAM_NAME): $(OBJS) $(LIBS) $(UNILIBS) filelink.res

#UPDATE#
FSLink.obj:          FSLink.cpp FSLink.h LinkCli.h ISLink.h ISLink_c.c
FileLink.obj:        FileLink.cpp FileLink.h
QuarLink.obj:        QuarLink.cpp QuarLink.h
Serial.obj:          Serial.cpp Serial.h
Protocol.obj:        Protocol.cpp Protocol.h
ISLink_c.obj:        ISLink.h ISLink_c.c
ISLink.h ISLink_c.c: ISLink.idl ISLink.acf
#ENDUPDATE#
