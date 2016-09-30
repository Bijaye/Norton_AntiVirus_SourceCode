#************************************************************************
# Copyright 1992-1993 Symantec, Peter Norton Product Group
#************************************************************************
# NAVSTUB.MAK
#
# $Header:   S:/NAVC/VCS/navstub.mav   1.1   15 Aug 1997 17:14:22   KSACKIN  $
#
# This file contains the dependencies and rules required to build the
# NAVSTUB DOS program
#************************************************************************
# $Log:   S:/NAVC/VCS/navstub.mav  $
#  
#     Rev 1.1   15 Aug 1997 17:14:22   KSACKIN
#  Updated this makefile to crc it's product, being NavDX.exe.
#
#     Rev 1.0   13 Aug 1997 21:22:48   TCASHIN
#  Initial revision.
#
#     Rev 1.4   19 Jun 1997 11:45:18   JBELDEN
#  added define for VERIFY_CHECKSUM
#
#     Rev 1.3   19 Jul 1996 10:35:42   JALLEE
#  Kludge.  Added -Zp (pack structures) to CLOPTS.  We are compiling a DOS
#  program under DX platform => no platform.mak.
#
#     Rev 1.2   12 Jul 1996 13:18:36   MZAREMB
#  Wasn't trying to create NAVBOOT because dependancies were wrong. Now fixed.
#
#     Rev 1.1   12 Jul 1996 12:57:30   MZAREMB
#  Cleaned up makefile for Nirvana Plus project (ported from Luigi Plus)
#
#     Rev 1.0   26 Mar 1996 16:13:26   MZAREMB
#  Initial revision.
#
#************************************************************************

PROGRAM_NAME=NAVDX.EXE

%if "$(PLATFORM)" == ".DVX" || "$(PLATFORM)" = ".NCX"

    TARGET_TYPE=EXE

%else

    beep
    echo "$(PLATFORM)" is not a valid platform for the NAVC project.

%endif


# Since we don't use platform.mak, need to add the NCD compile switch if we're compiling 
# for NEC (since there is an #ifdef in navstub.c) ...

%if "$(PLATFORM)" = ".NCX"

    CLOPTS+= /DNCD

%endif


MODEL=S
SYSLIBS=
CLOPTS+= -Zp /DVERIFY_CHECKSUM

navcrc.nav:       $(PROGRAM_NAME)
                   if exist $(PROGRAM_NAME) navcrc $(PROGRAM_NAME)

$(PROGRAM_NAME): navstub.obj


LINKER=OLINK403.EXE

navstub.obj: navstub.c
