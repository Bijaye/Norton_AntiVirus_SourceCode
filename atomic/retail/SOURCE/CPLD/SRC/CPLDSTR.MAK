# Copyright 1994 Symantec Corporation
#*************************************************************************
#
# $Header:   S:/cpld/VCS/cpldstr.mav   1.0   06 Feb 1997 20:50:16   RFULLER  $
#
# Description:
#       DOS Control Panel Strings Make File.
#
#*************************************************************************
# $Log:   S:/cpld/VCS/cpldstr.mav  $
#  
#     Rev 1.0   06 Feb 1997 20:50:16   RFULLER
#  Initial revision
#  
#     Rev 1.2   22 Aug 1996 12:33:20   JBRENNA
#  No change.
#  
#     Rev 1.1   07 Aug 1996 16:45:50   JBRENNA
#  DOSNAV Port: add switchs: USE_PASSWORD, USE_NETWORKALERTS, and USE_ZIP
#  
#  
#     Rev 1.12   25 Jun 1996 15:38:10   MZAREMB
#  Added USE_ZIP define to reintroduce zip functionality for Pegasus.
#  
#     Rev 1.11   13 Jun 1996 10:33:14   MZAREMB
#  Added NETWORKALERTS and USE_PASSWORDS #defines for Pegasus project
#  
#     Rev 1.10   24 Oct 1995 19:08:22   MKEATIN
#  Undefined USE_NETWORKALERTS
#  
#     Rev 1.9   23 Oct 1995 15:23:42   MKEATIN
#  Defined USE_NETWORKALERTS for NAV
#  
#     Rev 1.8   03 Aug 1994 16:26:44   DALLEE
#  Updated dependencies.
#  
#     Rev 1.7   12 May 1994 14:52:32   DALLEE
#  Updated dependencies (after cts4.h to ctsn.h).
#  
#     Rev 1.6   28 Apr 1994 17:05:06   DALLEE
#  Updated dependencies.
#  
#     Rev 1.5   27 Apr 1994 14:10:22   DALLEE
#  Trunk conversion.
#*************************************************************************


PROGRAM_NAME=CPLDSTR.LIB
NDSTRINGS=YES
DEF=/DUSE_PASSWORD /DUSE_NETWORKALERTS /DUSE_ZIP

TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME):    \
    actstr.obj      \
    adminstr.obj    \
    alerstr.obj     \
    exclstr.obj     \
    genstr.obj      \
    inocstr.obj     \
    passstr.obj     \
    realstr.obj     \
    scanstr.obj


#UPDATE#
actstr.obj:	platform.h stddos.h ibmpc.h 
adminstr.obj:	platform.h stddos.h ibmpc.h stddlg.h 
alerstr.obj:	platform.h stddos.h ibmpc.h stddlg.h options.h navopts.h \
		exclude.h ctsn.h netobj.h symnet.h register.h 
exclstr.obj:	platform.h stddos.h ibmpc.h stddlg.h ctsn.h navdprot.h \
		nethelp.h navhelp.h 
genstr.obj:	platform.h stddos.h ibmpc.h stddlg.h navdprot.h ctsn.h \
		nethelp.h navhelp.h 
inocstr.obj:	platform.h stddos.h ibmpc.h stddlg.h 
passstr.obj:	platform.h stddos.h ibmpc.h stddlg.h nlibcfg.h passwd.h \
		navdprot.h ctsn.h nethelp.h navhelp.h navopts.h 
realstr.obj:	platform.h stddos.h ibmpc.h stddlg.h 
scanstr.obj:	platform.h stddos.h ibmpc.h stddlg.h 
#ENDUPDATE#
