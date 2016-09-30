#########################################################################
# $Header:   S:/CSERVICE/VCS/service.mav   1.1   12 Dec 1997 14:45:02   TSmith  $
#
#########################################################################
# $Log:   S:/CSERVICE/VCS/service.mav  $
#  
#     Rev 1.1   12 Dec 1997 14:45:02   TSmith
#  Removed symkrnl linkage.
#  
#     Rev 1.0   09 May 1997 17:50:28   ksackin
#  Initial revision.
#  
#     Rev 1.1   18 Apr 1997 13:48:50   DSACKIN
#  Ported from Tesla
#  
#     Rev 1.0   13 Mar 1997 15:09:26   RFULLER
#  Initial revision
#  
#     Rev 1.0   08 Nov 1996 11:46:48   DKEEFER
#  Initial revision
#  
#     Rev 1.12   24 Oct 1996 01:09:30   DSACKIN
#  Removed a commented line.
#
#     Rev 1.11   20 Jun 1996 02:21:32   DSACKIN
#  Removed the DEBUG_PROFILE information from here.
#
#     Rev 1.10   13 Jun 1996 17:14:20   DSACKIN
#  Added support for DEBUG_PROFILE.
#
#     Rev 1.9   12 Apr 1996 17:19:16   DSACKING
#  Removed many of the options.  They are taken care of by builtins.
#
#     Rev 1.8   11 Apr 1996 16:40:38   DSACKING
#  Changed the name back to SERVICE.LIB
#
#     Rev 1.7   11 Apr 1996 15:49:08   DSACKING
#  Changed it back to creating a LIB.
#
#     Rev 1.6   11 Apr 1996 13:28:32   DSACKING
#  Added the symkrnl lib.
#
#     Rev 1.5   11 Apr 1996 13:22:04   DSACKING
#  Don't tell MK how to build the RES file because then it won't look on N drive.
#
#     Rev 1.4   11 Apr 1996 12:54:20   PVENKAT
#  Changed dependancy order to compile MC file first, RC next and CPP.
#
#
#     Rev 1.3   10 Apr 1996 18:21:00   PVENKAT
#  Added service.rc srvcmsg.mc with header files for constants.
#
#
#     Rev 1.2   10 Apr 1996 16:29:04   DSACKING
#  CService now comes in the DLL form.
#
#     Rev 1.1   09 Apr 1996 12:06:32   DSACKING
#  Got rid of a couple of libs that were not needed.
#
#     Rev 1.0   09 Apr 1996 11:23:34   DSACKING
#  Initial revision.
#
#########################################################################
PROGRAM_NAME=SERVICE.LIB
TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

DEF=-DUNICODE -D_UNICODE

$(PROGRAM_NAME): \
        service.obj \
        profile.obj \
        timercli.lib

#UPDATE#
npsserve.obj:   platform.h

#ENDUPDATE#
