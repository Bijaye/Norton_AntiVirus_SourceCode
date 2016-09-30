#####################################################################
## MSDUUTIL.MAK - Created - 10/28/96
##
## Copyright 1992-1996 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/WNDSIZE/VCS/WNDSIZE.MAv   1.2   20 Feb 1998 16:52:46   KSACKIN  $
##
## Description:
##    NSDUUTIL makefile.
##
#####################################################################
##
## $Log:   S:/WNDSIZE/VCS/WNDSIZE.MAv  $
#  
#     Rev 1.2   20 Feb 1998 16:52:46   KSACKIN
#  Added the flag USE_CRT_DLL=TRUE to the switches section.  This will link in
#  the DLL version of the CRT functions, as opposed to the static ones.
#
#     Rev 1.1   10 Dec 1997 20:34:44   KSACKIN
#  Removed all MFC stuff from this Makefile.  This is no longer needed.  All of
#  our funcitonality will be C now.
#
#     Rev 1.0   10 Dec 1997 20:21:10   KSACKIN
#  Initial revision.
#
#     Rev 1.1   09 Oct 1997 21:35:54   ksackin
#  Removed charset specification as this is now taken care of by the new
#  platform specification options.
#
#     Rev 1.0   07 Oct 1997 17:26:34   ksackin
#  Initial revision.
#
#     Rev 1.0   02 Oct 1997 20:27:54   ksackin
#  Initial revision.
#
#     Rev 1.0   02 Oct 1997 20:26:54   ksackin
#  Initial revision.
##
#####################################################################

TARGET_NAME=WNDSIZE
TARGET_TYPE=LIB

#####################################################################
## General Switches

PACKING_ALIGNMENT=8
USE_CRT_DLL=TRUE

#####################################################################
## Platform Quake Junk...

.INCLUDE $(QUAKEINC)\PLATFORM.MAK


#####################################################################
## Object Modules.


OBJS  = wndsize.obj



#####################################################################
## Target Specification


PROGRAM_NAME=$(TARGET_NAME).LIB


$(PROGRAM_NAME): $(OBJS) \
                 $(LIBS)


#####################################################################
## Specific Updates.


#UPDATE#

wndsize.obj:       wndsize.cpp wndsize.h

#ENDUPDATE#
