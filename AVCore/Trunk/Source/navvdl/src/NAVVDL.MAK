# Copyright 1997 Symantec, Peter Norton Product Group
#************************************************************************
#
# $Header:   S:/NAVSDK/VCS/navdefs.mav   1.2   31 Aug 1998 18:42:10   jsulton  $
#
# Description:
#    Make file for NAVDEF DLL
#
#************************************************************************
# $Log:   S:/NAVSDK/VCS/navdefs.mav  $
#
#     Rev 1.2   31 Aug 1998 18:42:10   jsulton
#  We no longer run EXECRC for the ARA and ADA platforms.
#
#     Rev 1.1   05 Dec 1997 18:20:04   DALLEE
#  Removed SYMKRNL - this is no longer required.
#
#     Rev 1.0   01 Dec 1997 13:36:24   tcashin
#  Initial revision.
#************************************************************************

#########################################################################
# 6/10/1999: Jim Brennan: As per Tim Cashin's investigation, there was a
# problem with optimizations and installing definitions. Rather than
# truly figuring out the problem (too time consuming), we are just
# turning optimizations off.
#########################################################################
STOP_OPT=1
OPT_NOCV=-Od

%If "$(CORE_WIN16)" != ""
    PROGRAM_NAME=NAVOEM.DLL
    NAVOEMDEF=NAVOEM.DEF
%ElseIf "$(CORE_WIN32)" != ""
    PROGRAM_NAME=NAVVDL32.DLL
    NAVOEMDEF=NAVVDL.D32
%Endif

TARGET_TYPE=DLL
QUAKE_CUSTOM=
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

KRNL=$(QUAKELANGPRE)\SYMKRNL\SRC$(QUAKE)
KRNL_PATH=$(QUAKEPRE)\SYMKRNL\SRC$(QUAKE)

RCOPTS=/i$(KRNL) /DQCUST

%If ("$(CORE_WIN16)" != "")
    RCBINDOPTS=-k
    XOPTS=/I$(KRNL_PATH) /DNAVOEM
%ElseIf ("$(CORE_WIN32)" != "")
    XOPTS=/DQCUSTNOX /DNAVOEM /DQCUST_NOVXD_EMSG
%Endif


%if ("$(PLATFORM)" != ".ARA") && ("$(PLATFORM)" != ".ADA")

NAVOEM.CRC:      $(PROGRAM_NAME)
		 if exist $(PROGRAM_NAME) execrc $(PROGRAM_NAME)

%endif

$(PROGRAM_NAME): navvdl.obj   \
                 navvdl.res   \
                 defutils.lib \
                 navvdl.d32

NAVDEFS.RES:     navvdl.rc \
                 vdlver.h

#UPDATE#
navvdl.obj:   platform.h xapi.h defutils.h navvdl.h
#ENDUPDATE#

