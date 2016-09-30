#  Copyright 1994 Symantec Corporation
#############################################################################
#  $Header:   S:/NAVFAMLY/VCS/vnavcore.mav   1.6   29 Apr 1997 15:04:06   JBRENNA  $
#
#  Description:
#       This is a make file for NAVCORE.VXD.
#
#  See Also:
#
#############################################################################
#  $Log:   S:/NAVFAMLY/VCS/vnavcore.mav  $
#  
#     Rev 1.6   29 Apr 1997 15:04:06   JBRENNA
#  Ensure that this project does not build on the Trunk. Use AVAPI1 instead
#  of this project.
#  
#     Rev 1.5   03 Jan 1995 15:20:24   RSTANEV
#  Oops, XOPTS does not work for ASM files.
#
#     Rev 1.4   03 Jan 1995 14:18:02   RSTANEV
#  NAVCORE is now a static library.
#
#     Rev 1.3   09 Dec 1994 17:51:50   RSTANEV
#  No more Carey's inoculation library :-(.
#
#     Rev 1.2   25 Oct 1994 17:35:30   RSTANEV
#  Now using CERTLIBN.LIB instead of CERTLIB4.LIB.
#
#     Rev 1.1   15 Sep 1994 19:48:48   RSTANEV
#  Added inoculation library.
#
#     Rev 1.0   23 Aug 1994 22:37:56   RSTANEV
#  Initial revision.
#
#############################################################################

all:
    @echo NAVFAMLY is obsolete. Build the AVAPI1 project instead.


TARGET_NAME=NAVCORE
TARGET_TYPE=VXD
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

PROGRAM_NAME=$(TARGET_NAME).LIB
ASMOPTS+= /DSTATIC_NAVCORE

$(PROGRAM_NAME): vnavcore.obj

#UPDATE#
vnavcore.obj: platform.inc vnavcore.inc
#ENDUPDATE#
