# Copyright 1993 Symantec Corporation
#*************************************************************************
#
# $Header:   S:/scand/VCS/scand.mav   1.0   06 Feb 1997 21:08:42   RFULLER  $
#
# Description:
#       NAV Scanner make file.
#
#*************************************************************************
# $Log:   S:/scand/VCS/scand.mav  $
#  
#     Rev 1.0   06 Feb 1997 21:08:42   RFULLER
#  Initial revision
#  
#     Rev 1.1   08 Aug 1996 13:29:58   JBRENNA
#  Ported from DOSNAV environment
#  
#     Rev 1.48   24 Jun 1996 14:27:28   JALLEE
#  Defined USE_ZIP.
#  
#     Rev 1.47   13 Jun 1996 10:37:32   MZAREMB
#  Added NETWORKALERTS and USE_PASSWORD #defines back in for Pegasus
#  
#     Rev 1.46   07 Jun 1995 12:23:20   BARRY
#  remove nethelp.h dependency
#  
#     Rev 1.45   03 Aug 1994 17:14:54   DALLEE
#  Updated dependencies.
#  
#     Rev 1.44   12 May 1994 13:46:50   DALLEE
#  Updated dependencies (after changing to CTSN.H).
#*************************************************************************


PROGRAM_NAME=SCAND.LIB
NDSTRINGS=YES
DEF += /DUSE_NETWORKALERTS /DUSE_PASSWORD /DUSE_ZIP

TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME):    \
    bootact.obj     \
    delete.obj      \
    exclude.obj     \
    inocact.obj     \
    navdini.obj     \
    repair.obj      \
    scan.obj        \
    scanact.obj     \
    scanboot.obj    \
    scandlg.obj     \
    scanfnd.obj     \
    scaninf.obj     \
    scaninoc.obj    \
    scanitem.obj    \
    scanmem.obj     \
    scanutil.obj    \
    scnstart.obj    \
    sysinoc.obj


#UPDATE#
bootact.obj:	platform.h stddos.h ibmpc.h ctsn.h repair.h virscan.h \
		cts4.h virusinf.h options.h navopts.h exclude.h netobj.h \
		symnet.h register.h navutil.h stdproto.h symcfg.h npt.h \
		quakever.h version.h drvprblm.h nonav.h syminteg.h navdprot.h \
		navhelp.h scand.h netalert.h tsr.h tsrcomm.h 
delete.obj:	platform.h stddos.h ibmpc.h stddlg.h tsr.h ctsn.h \
		virscan.h cts4.h virusinf.h options.h navopts.h exclude.h \
		netobj.h symnet.h register.h navutil.h stdproto.h symcfg.h \
		npt.h quakever.h version.h drvprblm.h nonav.h syminteg.h \
		navdprot.h navhelp.h tsrcomm.h scand.h 
exclude.obj:	platform.h stddos.h ibmpc.h ctsn.h options.h navopts.h \
		exclude.h netobj.h symnet.h register.h navutil.h stdproto.h \
		symcfg.h npt.h quakever.h version.h drvprblm.h nonav.h \
		syminteg.h navdprot.h navhelp.h scand.h 
inocact.obj:	platform.h stddos.h ibmpc.h stddlg.h ctsn.h virscan.h \
		cts4.h virusinf.h navutil.h options.h navopts.h exclude.h \
		netobj.h symnet.h register.h stdproto.h symcfg.h npt.h \
		quakever.h version.h drvprblm.h nonav.h syminteg.h navdprot.h \
		navhelp.h scand.h inoc.h netalert.h 
navdini.obj:	platform.h stddos.h ibmpc.h navutil.h ctsn.h options.h \
		navopts.h exclude.h netobj.h symnet.h register.h stdproto.h \
		symcfg.h npt.h quakever.h version.h drvprblm.h nonav.h \
		syminteg.h navdprot.h navhelp.h 
repair.obj:	platform.h stddos.h ibmpc.h stddlg.h file.h tsr.h ctsn.h \
		virscan.h cts4.h virusinf.h options.h navopts.h exclude.h \
		netobj.h symnet.h register.h navutil.h stdproto.h symcfg.h \
		npt.h quakever.h version.h drvprblm.h nonav.h syminteg.h \
		navdprot.h navhelp.h inoc.h tsrcomm.h scand.h 
scan.obj:	platform.h xapi.h file.h stddos.h ibmpc.h stddlg.h tsr.h \
		tsrcomm.h ctsn.h virscan.h cts4.h virusinf.h options.h \
		navopts.h exclude.h netobj.h symnet.h register.h navutil.h \
		stdproto.h symcfg.h npt.h quakever.h version.h drvprblm.h \
		nonav.h syminteg.h inoc.h netalert.h navalog.h navdprot.h \
		navhelp.h scand.h 
scanact.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h ctsn.h \
		virscan.h cts4.h virusinf.h navutil.h options.h navopts.h \
		exclude.h netobj.h symnet.h register.h stdproto.h symcfg.h \
		npt.h quakever.h version.h drvprblm.h nonav.h syminteg.h \
		navdprot.h navhelp.h scand.h netalert.h 
scanboot.obj:	platform.h xapi.h stddos.h ibmpc.h disk.h tsr.h ctsn.h \
		virscan.h cts4.h virusinf.h options.h navopts.h exclude.h \
		netobj.h symnet.h register.h navutil.h stdproto.h symcfg.h \
		npt.h quakever.h version.h drvprblm.h nonav.h syminteg.h \
		navdprot.h navhelp.h tsrcomm.h scand.h 
scandlg.obj:	platform.h stddos.h ibmpc.h stddlg.h ctsn.h virscan.h \
		cts4.h virusinf.h navutil.h options.h navopts.h exclude.h \
		netobj.h symnet.h register.h stdproto.h symcfg.h npt.h \
		quakever.h version.h drvprblm.h nonav.h syminteg.h scand.h \
		navdprot.h navhelp.h 
scanfnd.obj:	platform.h xapi.h stddos.h ibmpc.h symintl.h stddlg.h \
		file.h navutil.h ctsn.h options.h navopts.h exclude.h \
		netobj.h symnet.h register.h stdproto.h symcfg.h npt.h \
		quakever.h version.h drvprblm.h nonav.h syminteg.h navdprot.h \
		navhelp.h scand.h 
scaninf.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h navutil.h \
		ctsn.h options.h navopts.h exclude.h netobj.h symnet.h \
		register.h stdproto.h symcfg.h npt.h quakever.h version.h \
		drvprblm.h nonav.h syminteg.h navdprot.h navhelp.h \
		scand.h 
scaninoc.obj:	platform.h stddos.h ibmpc.h stddlg.h disk.h ctsn.h \
		virscan.h cts4.h virusinf.h options.h navopts.h exclude.h \
		netobj.h symnet.h register.h navutil.h stdproto.h symcfg.h \
		npt.h quakever.h version.h drvprblm.h nonav.h syminteg.h \
		inoc.h navdprot.h navhelp.h scand.h 
scanitem.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h file.h \
		disk.h tsr.h symzip.h ctsn.h virscan.h cts4.h virusinf.h \
		navutil.h options.h navopts.h exclude.h netobj.h symnet.h \
		register.h stdproto.h symcfg.h npt.h quakever.h version.h \
		drvprblm.h nonav.h syminteg.h inoc.h navdprot.h \
		navhelp.h tsrcomm.h scand.h dosnet.h 
scanmem.obj:	platform.h stddos.h ibmpc.h stddlg.h ctsn.h virscan.h \
		cts4.h virusinf.h options.h navopts.h exclude.h netobj.h \
		symnet.h register.h navutil.h stdproto.h symcfg.h npt.h \
		quakever.h version.h drvprblm.h nonav.h syminteg.h navdprot.h \
		navhelp.h scand.h netalert.h 
scanutil.obj:	platform.h xapi.h stddlg.h stddos.h ibmpc.h file.h \
		disk.h dosnet.h symzip.h ctsn.h options.h navopts.h exclude.h \
		netobj.h symnet.h register.h navutil.h stdproto.h symcfg.h \
		npt.h quakever.h version.h drvprblm.h navalog.h nonav.h \
		syminteg.h navdprot.h navhelp.h scand.h tsr.h \
		tsrcomm.h filestat.h 
scnstart.obj:	platform.h stddos.h ibmpc.h stddlg.h file.h ctsn.h \
		virscan.h cts4.h virusinf.h navutil.h options.h navopts.h \
		exclude.h netobj.h symnet.h register.h stdproto.h symcfg.h \
		npt.h quakever.h version.h drvprblm.h nonav.h syminteg.h \
		navdprot.h navhelp.h scand.h inoc.h netalert.h \
		navalog.h 
sysinoc.obj:	platform.h file.h stddos.h ibmpc.h stddlg.h syminteg.h \
		virscan.h cts4.h virusinf.h nonav.h options.h navopts.h \
		exclude.h ctsn.h netobj.h symnet.h register.h navutil.h \
		stdproto.h symcfg.h npt.h quakever.h version.h drvprblm.h \
		scand.h navdprot.h navhelp.h netalert.h tsr.h \
		tsrcomm.h 
#ENDUPDATE#
