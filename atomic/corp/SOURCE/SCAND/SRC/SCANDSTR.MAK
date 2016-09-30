# Copyright 1993 Symantec Corporation
#*************************************************************************
#
# $Header:   S:/scand/VCS/scandstr.mav   1.0   06 Feb 1997 21:08:54   RFULLER  $
#
# Description:
#       NAV Scanner strings make file
#*************************************************************************
# $Log:   S:/scand/VCS/scandstr.mav  $
#  
#     Rev 1.0   06 Feb 1997 21:08:54   RFULLER
#  Initial revision
#  
#     Rev 1.1   08 Aug 1996 13:29:40   JBRENNA
#  Ported from DOSNAV environment
#  
#     Rev 1.22   24 Jun 1996 14:28:54   JALLEE
#  Defined USE_ZIP
#  
#     Rev 1.21   13 Jun 1996 10:37:08   MZAREMB
#  Added NETWORKALERTS and USE_PASSWORD #defines back in for Pegasus
#  
#     Rev 1.20   07 Jun 1995 12:23:20   BARRY
#  remove nethelp.h dependency
#  
#     Rev 1.19   03 Aug 1994 17:32:06   DALLEE
#  MKDEPS
#  
#     Rev 1.18   12 May 1994 14:22:06   DALLEE
#  Updated dependencies (after cts4.h to ctsn.h)
#*************************************************************************


NDSTRINGS=YES
PROGRAM_NAME=SCANDSTR.LIB
DEF += /DUSE_NETWORKALERTS /DUSE_PASSWORD /DUSE_ZIP

TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME):    \
    bootacts.obj    \
    inocacts.obj    \
    scanacts.obj    \
    scandlgs.obj    \
    scanfnds.obj


#UPDATE#
bootacts.obj:	platform.h stddos.h ibmpc.h 
inocacts.obj:	platform.h stddos.h ibmpc.h 
scanacts.obj:	platform.h stddos.h ibmpc.h 
scandlgs.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h undoc.h 
scanfnds.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h undoc.h \
		ctsn.h options.h navopts.h exclude.h netobj.h symnet.h \
		register.h navutil.h stdproto.h symcfg.h npt.h quakever.h \
		version.h drvprblm.h nonav.h syminteg.h scand.h navdprot.h \
		navhelp.h 
#ENDUPDATE#
