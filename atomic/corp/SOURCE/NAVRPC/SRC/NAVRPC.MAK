#####################################################################
## NAVRPC.MAK - Created - 06-20-97
##
## Copyright 1992-1997 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/NAVRPC/VCS/NAVRPC.MAv   1.2   13 Aug 1997 21:46:06   DSACKIN  $
##
## Description:
##    This DLL will contain only three functions that will be used
##    in our RPC communications.
##
#####################################################################
##
## $Log:   S:/NAVRPC/VCS/NAVRPC.MAv  $
#  
#     Rev 1.2   13 Aug 1997 21:46:06   DSACKIN
#  Added $(FAMILYVERS) for AVCORE branch support.
#
#     Rev 1.1   25 Jul 1997 18:08:00   MKEATIN
#  Added NAVRPC.RES for version info.
#
#     Rev 1.0   23 Jun 1997 01:18:22   DSACKIN
#  Initial revision.
##
#####################################################################

TARGET_NAME=NAVRPC
TARGET_TYPE=DLL

#####################################################################
## Platform Quake Junk...

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OBJS  = navrpc.obj
OBJS += dllmain.obj

DEFS  = navrpc.def

UNILIBS = \
 $(FAMILYPRE)\LIB$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)U\LinkCli.LIB

LIBS  = $(UNILIBS)

RES   = navrpc.res


#####################################################################
## Target Specification


PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

$(PROGRAM_NAME): $(OBJS) $(LIBS) $(RES) $(DEFS)


#####################################################################
## Specific Updates.


#UPDATE#

navrpc.obj:     navrpc.cpp navrpc.h
dllmain.obj:    dllmain.cpp
navrpc.res:     navrpc.rc

#ENDUPDATE#

