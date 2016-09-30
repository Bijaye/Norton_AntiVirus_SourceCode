#************************************************************************
# Copyright 1992-1996 Symantec, Peter Norton Product Group
#************************************************************************
# NAVC.MAK
#
# $Header:   S:/NAVC/VCS/navc.mav   1.9   01 Jul 1998 20:56:30   VGLOSTE  $
#
# This file contains the dependencies and rules required to build the
# NAVCMDL program
#************************************************************************
# $Log:   S:/NAVC/VCS/navc.mav  $
#  
#     Rev 1.9   01 Jul 1998 20:56:30   VGLOSTE
#  Migrated changes made by CM to Roswell.  This allows there to be separate
#  targets for the trialware version and the normal version.
#  
#     Rev 1.8   13 Aug 1997 21:24:12   TCASHIN
#  This now builds NAVDX.OVL.
#  
#     Rev 1.7   10 Jul 1997 14:02:44   MKEATIN
#  A trial version will be make if the environment variable MAKE_PREVIEW is
#  set to ON.
#  
#     Rev 1.6   20 Jun 1997 14:29:24   MKEATIN
#  splice now looks for NAVDX.EXP
#  
#     Rev 1.5   20 Jun 1997 13:48:50   MKEATIN
#  Made the program name NAVDX.
#  
#     Rev 1.4   17 Jun 1997 17:19:58   MKEATIN
#  Added callback.lib.
#  
#     Rev 1.3   01 Jun 1997 20:35:06   BGERHAR
#  Add cdefwrap.cpp for Hawking
#  
#     Rev 1.2   09 May 1997 12:06:48   JBRENNA
#  Remove linking to NAVSERVE.LIB. It is not needed.
#  
#     Rev 1.1   29 Apr 1997 12:47:44   JTAYLOR
#  removed scn_init.obj and scn_dta.obj
#  
#     Rev 1.0   06 Feb 1997 20:56:34   RFULLER
#  Initial revision
#  
#     Rev 1.2   06 Jan 1997 21:19:30   MKEATIN
#  Added NCX platform
#  
#     Rev 1.1   31 Dec 1996 15:42:32   MKEATIN
#  Changed navd.obj to navc.obj
#  
#     Rev 1.0   31 Dec 1996 15:23:20   MKEATIN
#  Initial revision.
#  
#     Rev 1.19   18 Dec 1996 16:04:26   JBELDEN
#  removed insert.lib
#  
#     Rev 1.18   11 Dec 1996 17:19:58   JBELDEN
#  added inistuff.cpp from navxutil project
#  
#     Rev 1.17   10 Dec 1996 16:50:08   JBELDEN
#  added breaker.obj
#  
#     Rev 1.16   06 Dec 1996 11:51:54   JBELDEN
#  added files copied from other projects
#
#     Rev 1.15   14 Nov 1996 15:09:40   JBELDEN
#  removed parse.c.   not longer needed since there is no 
#  navboot.
#  
#     Rev 1.14   11 Nov 1996 13:50:56   JBELDEN
#  added code to splice so that mouse33.exp and int10.exp aren't
#  spliced in and we can save 20K
#  
#     Rev 1.13   08 Nov 1996 11:08:18   JBELDEN
#  removed locate.c and locstr.str.  No longer used
#  
#     Rev 1.12   08 Nov 1996 10:11:42   JBELDEN
#  removed navdir.obj.  No longer needed.
#  
#     Rev 1.11   07 Nov 1996 17:44:02   JBELDEN
#  removed scaninf.obj  no longer userd
#  
#     Rev 1.10   07 Nov 1996 12:19:18   JBELDEN
#  removed file seluser.str  this file isn't needed for navc
#
#     Rev 1.9   31 Oct 1996 14:20:52   JBELDEN
#  removed source files related to inoculations
#  
#     Rev 1.8   28 Oct 1996 10:17:20   JBELDEN
#  romved toolinoc.obj since not needed and coomented out def for
#  USE_NETWORKALERTS
#  
#     Rev 1.7   16 Oct 1996 18:44:38   JBELDEN
#  removed tools.obj.  Not used.
#  
#     Rev 1.6   16 Oct 1996 11:56:32   JBELDEN
#  removed sfsstr.str
#  
#     Rev 1.5   16 Oct 1996 11:07:08   JBELDEN
#  removed browstr.str
#  
#     Rev 1.4   10 Oct 1996 13:46:02   JBELDEN
#  removed reports.obj from project
#
#     Rev 1.3   03 Oct 1996 14:48:04   JBELDEN
#  removed vl.obj and events.obj  Not used for this project.
#  
#     Rev 1.2   02 Oct 1996 15:22:50   JBELDEN
#  Cleaned up the makefile some more.
#
#     Rev 1.1   02 Oct 1996 14:55:16   JBELDEN
#  removed dependency on nethelp.h removed scand.lib and scandstr.lib
#  since they have been integrated into the project and fixed a line
#  that wasn't terminated correctly.
#
#     Rev 1.0   02 Oct 1996 14:32:44   JBELDEN
#  Initial revision.
#************************************************************************

%if "$(PLATFORM)" != ".DVX" && "$(PLATFORM)" != ".NCX"
    beep
    echo Platform must be DVX or NCX for this project.
%endif

%if "$(MAKE_PREVIEW)" == "ON" || "$[u,$(TARGET)]" == "NAVPRV"
  # Creates a trial version of NAVDX
  DEF += /DNAV_PREVIEW
  PROGRAM_NAME=NAVDXP.OVL
%else 
  PROGRAM_NAME=NAVDX.OVL
%endif

#  DEF += /DUSE_NETWORKALERTS /DUSE_FULLOPTIONS /DUSE_ZIP /DTTYSCAN
DEF += /DUSE_ZIP /DTTYSCAN


NDSTRINGS=YES
TARGET_TYPE=EXE
.include $(QUAKEINC)\PLATFORM.MAK

# This kludge excludes the linking of oldnames, used only by Greenleaf lib
# in the NAVZIP project.  Oldnames _kbhit is incompatible with DOSUI.LIB.
# Navzip provides the aliases required by Greenleaf.

LINKER=GLUV.EXE
LINKOPTS+= -stack 14000 -exclude(oldnames oldnames)


NAV.EXE: "$(PROGRAM_NAME)"
%if "$(MAKE_PREVIEW)" == "ON" || "$[u,$(TARGET)]" == "NAVPRV"
         splice "$(PROGRAM_NAME)" R:\DOS16M.507\BIN\DOS25.EXP NAVDXP.EXP >> link.err
%else
         splice "$(PROGRAM_NAME)" R:\DOS16M.507\BIN\DOS25.EXP NAVDX.EXP >> link.err
%endif
         optedit -nobanner "$(PROGRAM_NAME)"  >> link.err
         navcrc "$(PROGRAM_NAME)"  >> link.err

$(PROGRAM_NAME):        \
    lnipatch.obj    \
    libhookl.obj    \
    cfginit.obj     \
    navc.obj        \
    cdefwrap.obj     \
    navdutil.obj    \
    navstr.obj      \
    optstr.obj      \
    optnavd.obj     \
    navdlist.obj    \
    selusers.obj    \
    inscan.obj    \
    winmisc.obj   \
    inistuff.obj \
    breaker.obj   \
    navinoc.lib     \
    cpldstr.lib     \
    navexclu.lib    \
    navxutil.lib    \
    navconv.lib     \
    navzip.lib      \
    $(Q_SYMALOG).LIB\
    cpld.lib        \
    scandrv.obj     \
    search.obj      \
    navdirs.obj     \
    bootact.obj     \
    delete.obj      \
    exclude.obj     \
    navdini.obj     \
    repair.obj      \
    scan.obj        \
    scanact.obj     \
    scanboot.obj    \
    scandlg.obj     \
    scanfnd.obj     \
    scanitem.obj    \
    scanmem.obj     \
    scanutil.obj    \
    scnstart.obj    \
    bootacts.obj    \
    scanacts.obj    \
    scandlgs.obj    \
    scanfnds.obj   \
    navdc.lib        \
    $(Q_VIRSCAN).LIB \
    syminteg.lib    \
    navcores.lib    \
    certlibn.lib    \
    strings.lib     \
    dosui.lib       \
    dosdlg.lib      \
    dosio.lib       \
    dosnet.lib      \
    dostool.lib     \
    pamdx.lib       \
    defutils.lib    \
    callback.lib    \
    $(DOSVMM)       \
    $(Q_SYMKRNL).LIB \
    $(Q_SYMDEBUG).LIB \
    $(Q_SYMUTIL).LIB \
%if "$(MAKE_PREVIEW)" == "ON"
    dostrial.lib    \
%endif
    dosdrvwn.lib    


#UPDATE#
scandrv.obj: platform.h dbcs_str.h xapi.h stddos.h ibmpc.h stddlg.h \
		file.h navutil.h ctsn.h callback.h options.h navopts.h \
		exclude.h cts4.h netobj.h symnet.h register.h symcfg.h \
		symalog.h drvprblm.h navdprot.h  navhelp.h
navd.obj:	platform.h dbcs_str.h xapi.h stddos.h ibmpc.h nlibcfg.h \
		stddlg.h symnet.h symcfg.h hardware.h file.h tsr.h logio.h \
		defs.h navutil.h ctsn.h callback.h options.h navopts.h \
		exclude.h cts4.h netobj.h register.h symalog.h drvprblm.h \
		navdprot.h  navhelp.h syminteg.h nonav.h inoc.h \
		common.h scand.h virscan.h virusinf.h
selusers.obj:	platform.h dbcs_str.h xapi.h stddlg.h dosnet.h stddos.h \
		ibmpc.h navdprot.h ctsn.h callback.h  navhelp.h \
		netobj.h symnet.h
navdutil.obj:	platform.h dbcs_str.h xapi.h symcfg.h stddos.h ibmpc.h \
		stddlg.h file.h ctsn.h callback.h navutil.h options.h \
		navopts.h exclude.h cts4.h netobj.h symnet.h register.h \
		symalog.h drvprblm.h syminteg.h nonav.h defs.h navdprot.h \
		 navhelp.h
optnavd.obj:	platform.h dbcs_str.h xapi.h stddos.h ibmpc.h stddlg.h \
		navdprot.h ctsn.h callback.h  navhelp.h options.h \
		navopts.h exclude.h cts4.h netobj.h symnet.h register.h
navdlist.obj:	Platform.h dbcs_str.h xapi.h stddos.h ibmpc.h stddlg.h \
		undoc.h navdprot.h ctsn.h callback.h  navhelp.h
net.obj:	platform.h dbcs_str.h xapi.h stddos.h ibmpc.h stddlg.h \
		navutil.h ctsn.h callback.h options.h navopts.h exclude.h \
		cts4.h netobj.h symnet.h register.h symcfg.h symalog.h \
		drvprblm.h navdprot.h  navhelp.h
tunevml.asm:    tunestr.inc
netstr.obj:	platform.h dbcs_str.h xapi.h stddos.h ibmpc.h
optstr.obj:	platform.h dbcs_str.h xapi.h stddos.h ibmpc.h stddlg.h \
		navdprot.h ctsn.h callback.h  navhelp.h
navstr.obj:	platform.h dbcs_str.h xapi.h stddos.h ibmpc.h symcfg.h \
		ctsn.h callback.h symalog.h navdprot.h  navhelp.h \
		navutil.h options.h navopts.h exclude.h cts4.h netobj.h \
		symnet.h register.h drvprblm.h menuitem.h defs.h
search.obj:	platform.h dbcs_str.h dosvmm.h stddos.h ibmpc.h navdprot.h \
		ctsn.h callback.h  navhelp.h file.h
navdirs.obj:	Platform.h dbcs_str.h dosvmm.h STDDos.h ibmpc.h
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
bootacts.obj:	platform.h stddos.h ibmpc.h
scanacts.obj:	platform.h stddos.h ibmpc.h
scandlgs.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h undoc.h
scanfnds.obj:	platform.h xapi.h stddos.h ibmpc.h stddlg.h undoc.h \
		ctsn.h options.h navopts.h exclude.h netobj.h symnet.h \
		register.h navutil.h stdproto.h symcfg.h npt.h quakever.h \
		version.h drvprblm.h nonav.h syminteg.h scand.h navdprot.h \
		navhelp.h
#ENDUPDATE#
