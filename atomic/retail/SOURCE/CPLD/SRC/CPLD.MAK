# Copyright 1993-1994 Symantec Corporation
#*************************************************************************
#
# $Header:   S:/cpld/VCS/cpld.mav   1.0   06 Feb 1997 20:50:24   RFULLER  $
#
# Description:
#       DOS Control Panel Make File.
#
#*************************************************************************
# $Log:   S:/cpld/VCS/cpld.mav  $
#  
#     Rev 1.0   06 Feb 1997 20:50:24   RFULLER
#  Initial revision
#  
#     Rev 1.1   07 Aug 1996 16:43:46   JBRENNA
#  DOSNAV Port: Added switches for USE_PASSWORD, USE_NETWORKALERTS, and USE_ZIP
#  
#  
#     Rev 1.28   25 Jun 1996 15:37:42   MZAREMB
#  Added USE_ZIP define to reintroduce zip functionality for Pegasus.
#  
#     Rev 1.27   13 Jun 1996 10:32:44   MZAREMB
#  Added NETWORKALERTS and USE_PASSWORDS #defines for Pegasus project
#  
#     Rev 1.26   26 Oct 1995 16:52:54   MKEATIN
#  Undefined USE_PASSWORD
#  
#     Rev 1.25   24 Oct 1995 19:09:28   MKEATIN
#  Undefined USE_NETWORKALERTS
#  
#     Rev 1.24   23 Oct 1995 15:19:42   MKEATIN
#  
#     Rev 1.23   18 Oct 1995 18:51:40   MKEATIN
#  
#  defined USE_PASSWORD for Navboot
#  
#     Rev 1.22   03 Aug 1994 16:22:50   DALLEE
#  Updated dependencies.
#  
#     Rev 1.21   12 May 1994 14:48:08   DALLEE
#  CTS4.H to CTSN.H
#  
#     Rev 1.20   28 Apr 1994 18:43:24   DALLEE
#  Updated dependencies.
#  
#     Rev 1.19   27 Apr 1994 14:10:04   DALLEE
#  Trunk conversion.
#  
#     Rev 1.18   24 Mar 1994 08:53:22   KEITH
#  Bankers - remove lib rename now that we have distince directories.
#  
#     Rev 1.17   17 Mar 1994 23:55:58   DALLEE
#  Added targets NAVB and NAVNETB for Bankers Trust.
#*************************************************************************

%If "$(TARGET)" == "NAV"
    TARGET_NAME=CPLD
    DEF=/DNAV /DUSE_PASSWORD /DUSE_NETWORKALERTS /DUSE_ZIP

%ElseIf "$(TARGET)" == "NAVNET"
    TARGET_NAME=CPLDNET
    DEF=/DNAVNET /DNAVNETW

%ElseIf "$(TARGET)" == "NAVB"
    TARGET_NAME=CPLD
    DEF=/DNAV /DBANKERS

%ElseIf "$(TARGET)" == "NAVNETB"
    TARGET_NAME=CPLDNET
    DEF=/DNAVNET /DNAVNETW /DBANKERS

%Else
DUMMY_TARGET:
    beep
    Echo "$(TARGET)" is not a valid target for the CPLD project.
%Endif


PROGRAM_NAME=$(TARGET_NAME).LIB
NDSTRINGS=YES

TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK


$(PROGRAM_NAME):    \
    navdact.obj     \
    navdaler.obj    \
    navdexcl.obj    \
    navdgen.obj     \
    navdinoc.obj    \
    navdpass.obj    \
    navdreal.obj    \
    navdscan.obj    \
    navdadmi.obj


#UPDATE#
navdact.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h file.h \
		navutil.h ctsn.h options.h navopts.h exclude.h netobj.h \
		symnet.h register.h stdproto.h symcfg.h npt.h quakever.h \
		version.h drvprblm.h navdprot.h nethelp.h navhelp.h 
navdadmi.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h symnet.h \
		file.h navutil.h ctsn.h options.h navopts.h exclude.h \
		netobj.h register.h stdproto.h symcfg.h npt.h quakever.h \
		version.h drvprblm.h navdprot.h nethelp.h navhelp.h 
navdaler.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h dosnet.h \
		navutil.h ctsn.h options.h navopts.h exclude.h netobj.h \
		symnet.h register.h stdproto.h symcfg.h npt.h quakever.h \
		version.h drvprblm.h navdprot.h nethelp.h navhelp.h common.h 
navdexcl.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h navdprot.h \
		ctsn.h nethelp.h navhelp.h navutil.h options.h navopts.h \
		exclude.h netobj.h symnet.h register.h stdproto.h symcfg.h \
		npt.h quakever.h version.h drvprblm.h 
navdgen.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h navutil.h \
		ctsn.h options.h navopts.h exclude.h netobj.h symnet.h \
		register.h stdproto.h symcfg.h npt.h quakever.h version.h \
		drvprblm.h navdprot.h nethelp.h navhelp.h 
navdinoc.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h navutil.h \
		ctsn.h options.h navopts.h exclude.h netobj.h symnet.h \
		register.h stdproto.h symcfg.h npt.h quakever.h version.h \
		drvprblm.h navdprot.h nethelp.h navhelp.h 
navdpass.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h nlibcfg.h \
		passwd.h navutil.h ctsn.h options.h navopts.h exclude.h \
		netobj.h symnet.h register.h stdproto.h symcfg.h npt.h \
		quakever.h version.h drvprblm.h navdprot.h nethelp.h \
		navhelp.h 
navdreal.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h navutil.h \
		ctsn.h options.h navopts.h exclude.h netobj.h symnet.h \
		register.h stdproto.h symcfg.h npt.h quakever.h version.h \
		drvprblm.h navdprot.h nethelp.h navhelp.h 
navdscan.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h navutil.h \
		ctsn.h options.h navopts.h exclude.h netobj.h symnet.h \
		register.h stdproto.h symcfg.h npt.h quakever.h version.h \
		drvprblm.h navdprot.h nethelp.h navhelp.h 
#ENDUPDATE#
