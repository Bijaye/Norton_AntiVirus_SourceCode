########################################################################
# Copyright 1995 Symantec Corporation
########################################################################
#                                                                       
# $Header:   S:/EMULATOR/VCS/pam.mav   1.19   01 May 1997 11:45:34   JBRENNA  $
#                                                                       
# Description:                                                          
#   Contains PAM File Scanning And Repair Routines
#
#   This is used to build NEWVIR projects (Win32, Win16 install, VXD, NLM, DX)
#                                                                       
########################################################################
# $Log:   S:/EMULATOR/VCS/pam.mav  $
#  
#     Rev 1.19   01 May 1997 11:45:34   JBRENNA
#  Change the TARGET_NAME to PAMNTK for NTK platforms.
#  
#     Rev 1.18   29 Apr 1997 14:14:48   JBRENNA
#  Convert .WIN platform to CORE_WIN16 platform check.
#  
#     Rev 1.17   28 Apr 1997 16:38:46   JBRENNA
#  Convert to new CORE_* platform spec.
#  
#     Rev 1.16   13 Feb 1997 11:25:28   MKEATIN
#  added the NCX platform
#  
#     Rev 1.15   19 Sep 1996 13:07:32   JALLEE
#  Added DVX platform.
#  
#  
#     Rev 1.14   11 Apr 1996 07:33:44   JMILLAR
#  add NTK build of PAMLIB
#  
#     Rev 1.13   04 Mar 1996 16:22:26   CNACHEN
#  Updated cache.h to pamcache.h
#  
#  
#     Rev 1.12   04 Mar 1996 16:02:18   CNACHEN
#  Added CACHE support.
#  
#     Rev 1.11   27 Feb 1996 18:45:34   CNACHEN
#  Added cover.h support...
#  
#     Rev 1.10   27 Feb 1996 17:38:38   CNACHEN
#  Added DX platform for PAM...
#  
#     Rev 1.9   21 Feb 1996 19:31:28   DCHI
#  Added DOS section to take care of undefined errors in splitdat
#  make under VIRSCAN.DAT.  This may need to be changed in the near future.
#  
#     Rev 1.8   16 Feb 1996 11:07:00   CNACHEN
#  Added COVER.OBJ dependencies...
#  
#     Rev 1.7   13 Feb 1996 15:15:14   CNACHEN
#  Added comments to headers...
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

%If ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")
        TARGET_NAME=PAM32
        TARGET_TYPE=DLL
%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "V")
        TARGET_NAME=PAMVXD
        TARGET_TYPE=LIB
%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "K")
        TARGET_NAME=PAMNTK
        TARGET_TYPE=LIB
%ElseIf ("$[u,$(PLATFORM)]" == ".NLM")
        TARGET_NAME=PAMNLM
        TARGET_TYPE=LIB
%ElseIf ("$(CORE_WIN16)" != "")
        MODEL=L
        TARGET_NAME=PAM16
        TARGET_TYPE=DLL
%ElseIf ("$[u,$(PLATFORM)]" == ".DOS")
        MODEL=L
        TARGET_NAME=PAMDOS
        TARGET_TYPE=LIB
%ElseIf ("$[u,$(PLATFORM)]" == ".DX" || "$[u,$(PLATFORM)]" == ".DVX" || \
         "$[u,$(PLATFORM)]" == ".NCX")
        MODEL=L
        TARGET_NAME=PAMDX
        TARGET_TYPE=LIB
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

