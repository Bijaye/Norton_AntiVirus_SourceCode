# Copyright 1992 Symantec Corporation                                   #
#########################################################################
#                                                                       #
# $Header:   S:/SYMKRNL/VCS/cfginit.mav   1.1   02 Dec 1996 16:45:26   BMCCORK  $ #
#                                                                       #
# Description:                                                          #
#                                                                       #
#########################################################################
# $Log:   S:/SYMKRNL/VCS/cfginit.mav  $ #
#  
#     Rev 1.1   02 Dec 1996 16:45:26   BMCCORK
#  Added test platform support
#  
#     Rev 1.0   26 Jan 1996 20:22:40   JREARDON
#  Initial revision.
#  
#     Rev 1.12   27 Apr 1994 15:28:50   BRAD
#  Removed DOS_VERSION define.  Use SYM_DOS
#  
#     Rev 1.11   21 Apr 1994 20:12:02   BRAD
#  MASM 6.11 is generating different OBJ, such that when it links, the offset
#  for __psp is different in this module, then in the MAP.  Took the easy way
#  out.
#  
#     Rev 1.10   08 Mar 1994 20:46:30   BRAD
#  Use PLATFORM.MAK
#  
#     Rev 1.9   10 Dec 1993 11:46:26   DWHITE
#  No changes
#  
#     Rev 1.8   04 Oct 1993 12:19:12   ED
#  .X86 now compiles like .DOS
#  
#     Rev 1.7   16 Aug 1993 13:24:38   SCOTTP
#  CHANGED $(MKTOOLS) TO QUAKEINC
#  
#     Rev 1.6   02 Jul 1993 09:03:08   DOUGN
#  No change.
#  
#     Rev 1.5   19 May 1993 08:59:32   KEITH
#  Use same sections for OS/2 Mirrors as Windows uses
#  
#     Rev 1.4   11 May 1993 15:44:16   KEITH
#  
#  
#     Rev 1.3   12 Feb 1993 15:27:10   ENRIQUE
#  No change.
#  
#     Rev 1.2   09 Feb 1993 16:20:42   CRAIG
#  Added DOS_VERSION symbol.
#
#     Rev 1.1   10 Dec 1992 00:14:42   JOHN
#  Updated for C7 on DOS
#
#     Rev 1.0   23 Sep 1992 14:06:38   ED
#  Initial revision.
#########################################################################

TARGET_TYPE=DLL
.INCLUDE <PLATFORM.MAK>

# Unable to get MASM 6.11 to generate correct code for __psp variable offset
MA=MASM5

cfginit.obj: cfginit.asm
