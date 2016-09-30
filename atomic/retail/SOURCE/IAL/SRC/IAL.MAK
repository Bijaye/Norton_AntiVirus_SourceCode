#-------------------------------------------------------------------------
#
# $Header:   S:/IAL/VCS/ial.mav   1.5   27 Jun 1997 18:53:08   DSACKIN  $
#
# Description:
#       IAL makefile.
#
#-------------------------------------------------------------------------
# $Log:   S:/IAL/VCS/ial.mav  $
#  
#     Rev 1.5   27 Jun 1997 18:53:08   DSACKIN
#  Added the -DIMPERSONATE to the DEF line.
#
#     Rev 1.4   27 Jun 1997 18:14:42   DSACKIN
#  Added SpawnUsr.obj
#
#     Rev 1.2   02 May 1997 23:41:10   KSACKIN
#  Cut out 99% of this make file because it was linking with all of the libs
#  that NAVOPTS used.  This was cut from a template .mak file and left with
#  all of the old dependancies.
#
#     Rev 1.1   30 Apr 1997 17:27:52   JBRENNA
#  Convert to new CORE_* platform specification.
#
#     Rev 1.0   18 Apr 1997 13:53:20   DSACKIN
#  Ported from Tesla
#
#     Rev 1.0   13 Mar 1997 15:09:56   RFULLER
#  Initial revision
#
#     Rev 1.2   24 Jan 1997 14:08:38   pvenkat
#  Added IAL.RC
#
#
#     Rev 1.1   15 Jan 1997 18:05:10   pvenkat
#  Forcing TARGET=NAVWNT to avoid compilation problems.
#
#
#     Rev 1.0   15 Jan 1997 17:55:14   pvenkat
#  Initial revision.
#

%If "$(CORE_WIN32)" != ""
   TARGET_NAME=IAL

   DEF += -DIMPERSONATE

%Else

BAD_PLATFORM:
    beep
    echo the '$(PLATFORM)' platform is not supported by IAL.
%Endif

TARGET_TYPE=EXE
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

PROGRAM_NAME=$(TARGET_NAME).EXE

$(PROGRAM_NAME): \
    ial.obj \
    objinst.obj \
    objlist.obj \
    perfcntr.obj \
    perfobj.obj \
    perfsnap.obj \
    titledb.obj \
    workset.obj \
    spawnusr.obj \
    IAL.RES
