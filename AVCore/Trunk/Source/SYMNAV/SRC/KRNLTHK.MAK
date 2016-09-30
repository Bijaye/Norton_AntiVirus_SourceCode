# Copyright 1994 Symantec Corporation                                   #
#########################################################################
#                                                                       #
# $Header:   S:/SYMNAV/VCS/krnlthk.mav   1.4   03 Sep 1996 15:05:18   BMCCORK  $ #
#                                                                       #
# Description:                                                          #
#                                                                       #
#########################################################################
# $Log:   S:/SYMNAV/VCS/krnlthk.mav  $ #
#  
#     Rev 1.4   03 Sep 1996 15:05:18   BMCCORK
#  Fixed xopts to use +=== and to use $(QUAKEinstead of 
#  
#     Rev 1.3   11 Jan 1996 06:49:26   JWORDEN
#  Add explicit rule for building .res file
#  
#     Rev 1.2   30 Jun 1995 12:14:32   JJOHNSON
#  More fixes after the Branch
#
#     Rev 1.1   27 Jun 1995 23:17:26   JREARDON
#  Updated for Quake 8 branch
#
#     Rev 1.0   02 Mar 1995 13:29:06   BRIANF
#  Initial revision.
#
#     Rev 1.6   14 Jan 1995 00:37:02   SCOTTP
#  Bumped rev number to 7
#
#     Rev 1.5   09 Dec 1994 15:12:56   HENRI
#  Added SystemHeapInfo and GetSystemResources thunks
#
#     Rev 1.4   15 Aug 1994 14:10:54   SCOTTP
#  trunk is now quake 6
#
#     Rev 1.3   04 Aug 1994 19:17:08   PATRICKM
#  Removed \TOOLS\ reference from RC line
#
#
#     Rev 1.2   30 Jun 1994 19:21:40   HENRI
#  Used Win40 16bit RC for building 16bit thunker dll and mark it a Win4.0 dll
#
#     Rev 1.1   24 Jun 1994 10:42:24   HENRI
#  Fixed for thunk compiler include
#
#     Rev 1.0   23 Jun 1994 17:57:54   HENRI
#  Initial revision.
#
#########################################################################
XOPTS+= /Il:\source\symkrnl\src$(QUAKE)
KRNL=$(QUAKELANGPRE)\SYMKRNL\SRC$(QUAKE)
.include $(QUAKEPRE)\symkrnl\src$(QUAKE)\krnlthk.mak

krnlthk.res:  $(KRNL)\krnlthk.rc
