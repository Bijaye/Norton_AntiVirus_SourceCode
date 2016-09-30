########################################################################
#
# $Header:   S:/NAVAPW/VCS/navapw.mav   1.10   26 Mar 1998 15:53:52   DALLEE  $
#
# Description:
#       NAVAPW:  VxD Norton AntiVirus AutoProtect Windows Agent
#
#       This file makes the NAVAPW.EXE 32-bit windows agent for the
#       Auto-Protect VxD
#
########################################################################
# $Log:   S:/NAVAPW/VCS/navapw.mav  $
#  
#     Rev 1.10   26 Mar 1998 15:53:52   DALLEE
#  Removed NAVWBMP.OBJ and NAVAP16/256.BMP.
#  We're no longer displaying the splash screen.
#  
#     Rev 1.9   13 Aug 1997 21:36:40   DSACKIN
#  Added $(FAMILYVERS) for AVCORE branch support.
#
#     Rev 1.8   31 Jul 1997 18:18:14   JBRENNA
#  1. Don't need /DNAVWNT for the RC compiles.
#  2. Switch from NAVWNT_PREVIEW to NAV_PREVIEW ... this matches the
#     NAVW project.
#
#     Rev 1.7   15 Jul 1997 16:29:14   DBUCHES
#  Added target support for NAVWNT
#
#     Rev 1.6   15 Jul 1997 16:27:32   DBUCHES
#  Added target support for NAVWNT
#
#     Rev 1.5   11 Jul 1997 13:22:38   DBUCHES
#  Added Ctl3d32.LIB for DEC Alpha platform
#
#     Rev 1.4   11 Jul 1997 12:23:22   DBUCHES
#  Removed CERTLIBN for DEC Alpha builds.
#
#     Rev 1.3   22 May 1997 17:40:54   DALLEE
#  Removed a few unused .OBJ's.
#  Also changed to FAMILYUNIDIR to use $(FAMILYPRE) instead of %FAMILYPRE%.
#
#     Rev 1.2   20 May 1997 12:54:52   KSACKIN
#  Now pulling the svcctrl, linkcli, and apscli libraries from AVCORE instead
#  of the Roswell area.
#
#     Rev 1.1   30 Apr 1997 16:31:04   JBRENNA
#  Add OptsMask.OBJ to the list of NAVXUTIL files to compile.
#
#     Rev 1.0   24 Apr 1997 21:27:48   ksackin
#  Initial revision.
#
#     Rev 1.0   13 Mar 1997 15:12:22   RFULLER
#  Initial revision
#
#     Rev 1.3   26 Feb 1997 07:11:50   TSmith
#  EXE file now named NVPW32PR.EXE when building target NAVWNTPR (30 day trial).
#
#     Rev 1.2   21 Feb 1997 11:09:26   TSmith
#  Now defines NAVWNT_PREVIEW if target is NAVWNTPR.
#
#     Rev 1.1   03 Feb 1997 18:58:14   JBELDEN
#  added OLE32.LIB to the libraries so that the NAVWNT_PREVIEW
#  code can use the symtrial COM object.
#
#     Rev 1.0   08 Nov 1996 12:13:28   DKEEFER
#  Initial revision
#
#     Rev 1.8   23 Oct 1996 15:34:24   JBELDEN
#  commented out define for navwnt_prevew
#
#     Rev 1.7   23 Oct 1996 15:33:22   JBELDEN
#  temporary def navwnt_preview
#
#     Rev 1.6   23 Oct 1996 09:59:56   JBELDEN
#  added def for NAVWNT_PREVIEW
#
#     Rev 1.5   11 Sep 1996 09:05:48   jmillar
#  setup RCOPTS for NAVWNTPR make
#
#     Rev 1.4   19 Jul 1996 01:29:10   DSACKIN
#  Link to LinkCli.lib and APSCli.lib.
#
#     Rev 1.3   08 May 1996 20:32:12   DSACKIN
#  Removed some of the OBJ files that weren't needed, and added back
#  in the NAVAPCMD.OBJ for our new CmdXXX function.
#
#     Rev 1.2   06 May 1996 19:44:36   DSACKIN
#  Added a couple new libs and removed OBJ files that the NT platform doesn't need.
#
#     Rev 1.1   22 Feb 1996 14:15:14   YVANG
#  Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
#  NAV version.
#
#     Rev 1.0   30 Jan 1996 15:52:34   BARRY
#  Initial revision.
#
#     Rev 1.0   30 Jan 1996 10:14:50   BARRY
#  Initial revision.
#
#     Rev 1.27   14 Dec 1995 13:51:30   BARRY
#  Ported changes from LUG1 branch
########################################################################

%if "$[u,$(TARGET)]" == "NAVWNTPR"  # Check for NT - preview edition
    DEF += /DNAV_PREVIEW
    RCOPTS += /DNAV_PREVIEW
    TARGET_NAME=NVPW32PR
%elseif "$[u,$(TARGET)]" == "NAVWNT"  # Check for NT - preview edition
    TARGET_NAME=NAVAPW32
%else
    TARGET_NAME=NAVAPW32
%endif

TARGET_TYPE=EXE

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

NAVXUTILPATH=$(PROJPRE)navxutil\src$(PROJVERS)
XOPTS=/I$(NAVXUTILPATH)
RCOPTS += /i$(LANGPRE)navxutil\src$(PROJVERS)


OBJS= \
    NAVAPW.OBJ \
    NAVAPCMD.OBJ

# These objs such that we don't have to bring in all of NAVXUTIL, which
# brings in S32STAT, which brings in, ... you get the point.  This allows
# Tomahawk to install correctly.
NAVXUTILOBJS= \
    INISTUFF.OBJ \
    WINMISC.OBJ \
    NOTIFY.OBJ \
    POINTER.OBJ \
    NAVHALT.OBJ \
    OptsMask.OBJ

FAMILYUNIDIR=$(FAMILYPRE)\lib$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)U

UNILIBS= \
    $(FAMILYUNIDIR)\svcctrl.lib \
    $(FAMILYUNIDIR)\linkcli.lib \
    $(FAMILYUNIDIR)\apscli.lib

$(PROGRAM_NAME): \
    $(OBJS) \
    $(NAVXUTILOBJS) \
    NAVAPW.RES \
    $(Q_SYMUTIL).LIB \
    $(Q_SYMKRNL).LIB \
    $(Q_SYMDEBUG).LIB \
    $(Q_SYMGUI).LIB \
%if ($(CORE_PLAT) != "A")
    CERTLIBN.LIB \
%else
    ctl3d32.lib \
%endif
    drvcomm.lib \
    OLE32.LIB \
    $(UNILIBS)



#UPDATE#
navapbmp.obj:	platform.h dbcs_str.h dosvmm.h navapw.h navbin.h \
		navbnhdr.h options.h navopts.h exclude.h cts4.h ctsn.h \
		callback.h excludel.h netobj.h symnet.h navstart.h register.h \
		navconv.h oldnavop.h vsymkrnl.h vxdload.h navapres.h
navapw.obj:	platform.h dbcs_str.h dosvmm.h windowsx.h xapi.h file.h \
		msgdisp.h ngui.h version.h quakever.h wndclass.h passwd.h \
		stockbmp.h navutil.h ctsn.h callback.h options.h navopts.h \
		exclude.h cts4.h excludel.h netobj.h symnet.h navstart.h \
		register.h npt.h virlist.h avapi.h symcfg.h symalog.h \
		drvprblm.h stdhelp.h navaplog.h navapw.h navbin.h navbnhdr.h \
		navconv.h oldnavop.h vsymkrnl.h vxdload.h navapcom.h \
		navapstb.h navapres.h
navapemu.obj:	platform.h dbcs_str.h dosvmm.h windowsx.h xapi.h \
		symalog.h msgdisp.h navapw.h navbin.h navbnhdr.h options.h \
		navopts.h exclude.h cts4.h ctsn.h callback.h excludel.h \
		netobj.h symnet.h navstart.h register.h navconv.h oldnavop.h \
		vsymkrnl.h vxdload.h navapcom.h navaplog.h navapstb.h
navapcom.obj:	platform.h dbcs_str.h dosvmm.h windowsx.h xapi.h \
		msgdisp.h navapw.h navbin.h navbnhdr.h options.h navopts.h \
		exclude.h cts4.h ctsn.h callback.h excludel.h netobj.h \
		symnet.h navstart.h register.h navconv.h oldnavop.h \
		vsymkrnl.h vxdload.h navapcom.h navapstb.h
navapcmd.obj:	platform.h dbcs_str.h dosvmm.h windowsx.h msgdisp.h \
		navutil.h ctsn.h callback.h options.h navopts.h exclude.h \
		cts4.h excludel.h netobj.h symnet.h navstart.h register.h \
		npt.h quakever.h version.h virlist.h avapi.h symcfg.h \
		symalog.h drvprblm.h stdhelp.h navapw.h navbin.h navbnhdr.h \
		navconv.h oldnavop.h vsymkrnl.h vxdload.h navapcom.h \
                navaplog.h navapstb.h navapres.h nonav.h netalert.h
navapw.res:	navapstb.h navapres.h navapw.rc navapstb.h navapres.h \
		navapw.stb navapt.ico navaptd.ico navapw.ico navapwd.ico \
                $(LANGPRE)navxutil\src$(PROJVERS)\navaplog.stb \
                $(LANGPRE)navxutil\src$(PROJVERS)\navxutil.stb
#ENDUPDATE#
