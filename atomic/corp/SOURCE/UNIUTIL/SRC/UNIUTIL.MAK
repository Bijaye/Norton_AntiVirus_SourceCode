#####################################################################
## UniUtil.mak
##
## Copyright 1992-1996 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/UNIUTIL/VCS/UNIUTIL.MAv   1.0
##
## This project contains utility functions needed for Unicode
## Applications.  These may be either quake function, or simply misc.
## utility functions that need to be shared accross out unicode
## modules.
##
#####################################################################
##
## $Log:   S:/UNIUTIL/VCS/uniutil.mav  $
#  
#     Rev 1.1   19 May 1997 15:30:20   DSACKIN
#  Removed the UNICODE defines.  These are now part of the I?U platform
#
#     Rev 1.0   18 Apr 1997 14:09:48   DSACKIN
#  Ported from Tesla
#
#     Rev 1.0   13 Mar 1997 15:19:32   RFULLER
#  Initial revision
#
#     Rev 1.2   06 Feb 1997 22:46:44   TSmith
#  Added module NAVLMUTL.OBJ.
#
#     Rev 1.1   03 Feb 1997 17:03:28   ksackin
#  Changed header dependency for the nqfonts.cpp file.  Now we are using a
#  global header file for the Unicode Utility Dll, instead of a local nqfonts.h
#  file.
#
#     Rev 1.0   03 Feb 1997 16:46:56   ksackin
#  Initial revision.
##
#####################################################################

UTILITY=UniUtil

PROGRAM_NAME=$(UTILITY).lib
TARGET_TYPE=LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(UTILITY).lib: NQFonts.obj \
                NAVLMUtl.obj \

# Header file dependencies

NQFonts.obj: NQFonts.cpp UniUtil.h dbcs.h codepage.h
NAVLMUtl.obj: NAVLMUtl.Cpp UniUtil.h

