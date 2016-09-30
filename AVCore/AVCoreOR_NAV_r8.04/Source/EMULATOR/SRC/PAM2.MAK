########################################################################
# Copyright 1995 Symantec Corporation
########################################################################
#                                                                       
# $Header:   S:/EMULATOR/VCS/pam2.mav   1.6   29 Apr 1997 14:14:26   JBRENNA  $
#                                                                       
# Description:                                                          
#   Contains PAM File Scanning And Repair Routines
#
#   This is used to build QUAKE2 EMULATOR libraries for Nirvana WIN16.
#                                                                       
########################################################################
# $Log:   S:/EMULATOR/VCS/pam2.mav  $
#  
#     Rev 1.6   29 Apr 1997 14:14:26   JBRENNA
#  Convert .WIN platform to CORE_WIN16 platform check.
#  
#     Rev 1.5   04 Mar 1996 16:22:18   CNACHEN
#  Updated cache.h to pamcache.h
#  
#  
#     Rev 1.4   04 Mar 1996 16:02:26   CNACHEN
#  Added CACHE support.
#  
#     Rev 1.3   27 Feb 1996 18:45:38   CNACHEN
#  Added cover.h support...
#  
#     Rev 1.2   16 Feb 1996 11:06:52   CNACHEN
#  Added COVER.OBJ dependencies...
#  
#     Rev 1.1   13 Feb 1996 15:15:20   CNACHEN
#  Added comments to headers...
#  
#     Rev 1.0   12 Feb 1996 17:47:14   CNACHEN
#  Initial revision.
#  
#     Rev 1.6   08 Feb 1996 17:10:00   CNACHEN
#  Added WIN16 support.
#  
#     Rev 1.4   12 Jan 1996 12:26:48   CNACHEN
#  Removed references to opcodele.c
#  
#     Rev 1.3   13 Dec 1995 10:07:48   CNACHEN
#  Added NLM support to makefile.
#  
#     Rev 1.2   30 Oct 1995 13:13:18   CNACHEN
#  Added VXD platform support.
#  
#     Rev 1.1   29 Sep 1995 15:49:48   CNACHEN
#  
#     Rev 1.0   13 Sep 1995 18:02:54   CNACHEN
#  Initial revision.
########################################################################

%If ("$(CORE_WIN16)" != "")
        TARGET_NAME=PAM162
        TARGET_TYPE=DLL
        XOPTS += /DQUAKE2
        MODEL=L
%Endif


OBJS = cpu.obj          \
       datafile.obj     \
       exclude.obj      \
       exec.obj         \
       flags.obj        \
       instr.obj        \
       instr2.obj       \
       page.obj         \
       pamapi.obj       \
       repair.obj       \
       search.obj       \
       sigex.obj        \
       cover.obj        \
       cache.obj


.INCLUDE $(QUAKEINC)\PLATFORM.MAK

PROGRAM_NAME=$(TARGET_NAME).LIB

$(PROGRAM_NAME): $(OBJS)

#UPDATE#
cover.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h

cpu.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h

datafile.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h

exclude.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h

exec.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h


flags.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h

instr.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h

instr2.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h

page.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h

pamapi.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h

repair.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h

search.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h

sigex.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h

cache.obj: cpu.h datafile.h exclude.h ident.h opcode.h page.h pamapi.h \
         profile.h proto.h prepair.h search.h sigex.h cover.h pamcache.h
#ENDUPDATE#

