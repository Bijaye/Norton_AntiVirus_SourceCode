# Copyright 1992-1993 Symantec, Peter Norton Product Group
########################################################################
#
# $Header:   S:/NAVW/VCS/navw.mav   1.43   20 Oct 1998 20:31:52   CEATON  $
#
# Description:
#       This is the make file for the Norton AntiVirus for Windows
#       Clinic program.
########################################################################
# $Log:   S:/NAVW/VCS/navw.mav  $
#  
#     Rev 1.43   20 Oct 1998 20:31:52   CEATON
#  Added ABOUTSW.LIB (contains SystemWorks upsell dialog box).
#  
#  
#     Rev 1.42   10 Jun 1998 22:41:46   tcashin
#  Added quarmon.cpp
#  
#     Rev 1.41   07 Jun 1998 15:56:54   tcashin
#  Added Infodlg.cpp.
#  
#     Rev 1.40   02 Jun 1998 17:56:54   mdunn
#  Now using $(CORE_PLAT) to get the right channel/patch LIBs.
#  
#     Rev 1.39   28 Apr 1998 09:16:40   DALLEE
#  Added drvcomm.lib and status.obj.
#  
#     Rev 1.38   23 Apr 1998 21:03:46   DALLEE
#  Added icons for main UI real-time status displays.
#  Added wndsize.lib for resizing the scan folders dialog.
#  
#     Rev 1.37   20 Apr 1998 23:10:18   DALLEE
#  Added nvbrowse.obj and treectrl.obj for new scan folder dialog.
#  
#     Rev 1.36   14 Apr 1998 23:39:36   DALLEE
#  Removed RC dependencies on button bar mask bitmaps - no longer used.
#  Added dependency for new quarantine bitmap.
#  
#     Rev 1.35   01 Apr 1998 14:57:44   DALLEE
#  Removed navwinoc.obj.
#  Added dependencies for maintool.obj and toolbar.obj.
#  
#     Rev 1.34   01 Apr 1998 14:48:28   DALLEE
#  Added maintool.obj and toolbar.obj, and comctl32.lib.
#  Nuked some ancient log comments.
#  
#     Rev 1.33   17 Mar 1998 10:14:54   tcashin
#  Added libs needed for microdefs: patch32i.lib and chan32i.lib
#  
#     Rev 1.32   24 Nov 1997 19:33:22   jtaylor
#  Added a n32behav.lib to navw.obj.  Added a dependency on n32behav.h to navw.obj.
#
#     Rev 1.31   13 Aug 1997 21:50:16   DSACKIN
#  Added $(FAMILYVERS) for AVCORE branch support.
#
#     Rev 1.30   11 Aug 1997 00:11:28   KSACKIN
#  Added -GX clopt for structured exception handling.
########################################################################

TARGET_TYPE=EXE
USE_MFC_DLL=TRUE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OLE32_LIB=OLE32.LIB

DEF += /D_WIN32

# ---------------------------------------------------------------------
# Compile NAVW (or NAVWPRV, the Preview version of NAVW)

#If TARGET is NAVW or NAVWPRV ...
%If %member($(TARGET),NAVW NAVWPRV)
CLOPTS += -GX

%If ("$(CORE_WIN32)" != "")
    DEF += -D_MBCS
%If ("$[u,$(TARGET)]" == "NAVW")
    PROGRAM_NAME=NAVW32.EXE
    DEF     += /DNAV95_CODE
%Else
    #Build Preview version...
    PROGRAM_NAME=NAVW32P.EXE
    DEF      += /DNAV_PREVIEW
    OLE32_LIB = OLE32.LIB
%Endif

    ABOUTSW=ABOUTSW
    NAVABOUT=NAVABOUT
    SCANNING=N32WORK
    ACTIVITYLOG=N32ALOG
    SCARYCODE=$(Q_SYMALOG)
    NAVUTILITY=N32XUTIL
    VIRUSLIST=N32VLIST
    EXCLUDE=N32EXCLU
    CERTLIB=CERTLIBN
    SECURITY=N32SECUR
    INOCULATE=N32INOC
    NETNOTIFY=N32SERVE
    VIRUSSCAN=V32SCAN
    NAVUTILS=N32UTILS
    NavLuUtils_LIB=NavLuUtils.LIB
    DefUtils_LIB=DefUtils.LIB
    Patch_LIB=Patch32$(CORE_PLAT).LIB
    Channel_LIB=Chan32$(CORE_PLAT).LIB
%Else

BAD_PLATFORM:
    beep
    echo the $(PLATFORM) platform is not supported for the project.

%EndIf


RES_FILE     =NAVW.RES
RES_SRC      =NAVW.RC
OBJS         =navwfile.obj navrpc.obj  navwhmem.obj NetAlertStub.obj
NAVW_ICO     =navw.ico

# Build a list of the UNICODE libs that this project uses.

UNILIBS = \
 $(FAMILYPRE)\LIB$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)U\LinkSrv.LIB

# $(SCARYCODE).LIB \

LIBS= \
 NAVWC.LIB \
 NAVCONV.LIB \
 drvcomm.lib \
 $(EXCLUDE).LIB \
 $(INOCULATE).LIB \
 $(CERTLIB).LIB \
 $(NETNOTIFY).LIB \
 $(VIRUSSCAN).LIB \
 $(NAVUTILS).LIB \
 $(SECURITY).LIB \
 NAVUICOM.LIB \
 N32PDLL.LIB \
 N32OPTS.LIB \
 N32CALL.LIB \
 progman.lib \
 uuid.lib \
 CMBSTR.LIB

MORELIBS = \
 $(ABOUTSW).LIB \
 $(NAVABOUT).LIB \
 $(SCANNING).LIB \
 $(NavLuUtils_LIB) \
 $(DefUtils_LIB) \
 $(ACTIVITYLOG).LIB \
 $(SCARYCODE).LIB \
 $(NAVUTILITY).LIB \
 $(VIRUSLIST).LIB \
 $(Patch_LIB) \
 $(Channel_LIB) \
 $(UNILIBS)

%ElseIf %member($(TARGET),NAVWNT NAVWNTPR)

# ---------------------------------------------------------------------
# Compile NAVWNT (Marconi 1.0)

%If ("$(CORE_WIN32)" != "")

    CLOPTS += -GX
    %If ("$[u,$(TARGET)]" == "NAVWNT")
	PROGRAM_NAME=NAVWNT.EXE
    %Else
	#Build Preview version...
	PROGRAM_NAME=NAVWNTPR.EXE
	DEF     += /DNAV_PREVIEW
	OLE32_LIB = OLE32.LIB
    %Endif

    ABOUTSW=ABOUTSW
    NAVABOUT=NAVABOUT
    SCANNING=XNTWORK
    ACTIVITYLOG=XNTALOG
    SCARYCODE=$(Q_SYMALOG)
    NAVUTILITY=XNTXUTIL
    VIRUSLIST=N32VLIST
    EXCLUDE=XNTEXCLU
    CERTLIB=CERTLIBN
    SECURITY=N32SECUR
    INOCULATE=XNTINOC
    NETNOTIFY=XNTSERVE
    VIRUSSCAN=V32SCAN
    NAVUTILS=N32UTILS
    NavLuUtils_LIB=NavLuUtils.LIB
    DefUtils_LIB=DefUtils.LIB
    Patch_LIB=Patch32$(CORE_PLAT).LIB
    Channel_LIB=Chan32$(CORE_PLAT).LIB
%Else

# Platform is not W32 (required for NT)
BAD_PLATFORM:
    beep
    echo The $(PLATFORM) platform is not supported for the project.

%EndIf

RES_FILE     =NAVW.RES
RES_SRC      =NAVW.RC
OBJS         =navwfile.obj navwhmem.obj navrpc.obj NetAlertStub.OBJ
DEF_FILE     =NAVW.DEF
NAVW_ICO     =navw.ico
DEF          += /DNAVSCANW /DNAVWNT

# Build a list of the UNICODE libs that this project uses.

UNILIBS = \
 $(FAMILYPRE)\LIB$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)U\LinkSrv.LIB

# NT Libraries
LIBS= \
 NAVWC.LIB \
 NAVCONV.LIB \
 drvcomm.lib \
 $(EXCLUDE).LIB \
 $(INOCULATE).LIB \
%if($(CORE_PLAT) != "A")
 $(CERTLIB).LIB \
%endif
 $(NETNOTIFY).LIB \
 $(VIRUSSCAN).LIB \
 $(NAVUTILS).LIB \
 $(SECURITY).LIB \
 NAVUICOM.LIB \
 XNTOPTS.LIB \
 N32CALL.LIB \
 CMBSTR.LIB \
 progman.lib \
 uuid.lib

# More NT Libraries
MORELIBS = \
 $(ABOUTSW).LIB \
 $(NAVABOUT).LIB \
 $(SCANNING).LIB \
 $(NavLuUtils_LIB) \
 $(DefUtils_LIB) \
 $(ACTIVITYLOG).LIB \
 $(SCARYCODE).LIB \
 $(NAVUTILITY).LIB \
 $(VIRUSLIST).LIB \
 $(Patch_LIB) \
 $(Channel_LIB) \
 $(UNILIBS)

%if($(CORE_PLAT) == "A")
LIBS += CTL3D32.LIB
%endif


# End NAVWNT
# ---------------------------------------------------------------------


%ElseIf "$[u,$(TARGET)]" == "NAVSCANW"  # Check for NT

# ---------------------------------------------------------------------
# Compile NAVSCANW (Gargoyle 1.0)

%If ("$(CORE_WIN32)" != "")
    CLOPTS += -GX
    PROGRAM_NAME=N32SCANW.EXE
    SCANNING=NNTWORK
    ACTIVITYLOG=NNTALOG
    SCARYCODE=$(Q_SYMALOG)
    NAVUTILITY=NNTXUTIL
    VIRUSLIST=N32VLIST
    EXCLUDE=NNTEXCLU
    CERTLIB=CERTLIBN
    SECURITY=N32SECUR
    INOCULATE=NNTLIB01
    NETNOTIFY=XNTSERVE
    VIRUSSCAN=V32SCAN
    NAVUTILS=N32UTILS
%Else

# Platform is not W32 (required for NT)
BAD_PLATFORM:
    beep
    echo The $(PLATFORM) platform is not supported for the project.

%EndIf

RES_FILE     =NAVW.RES
RES_SRC      =NAVW.RC
OBJS         =navwfile.obj navwhmem.obj navrpc.obj
DEF_FILE     =NAVW.DEF
NAVW_ICO     =navw.ico
DEF          += /DNAVSCANW  # Define NAVSCANW for target-specific code

# NT Libraries
LIBS= \
 NAVWC.LIB \
 NAVCONV.LIB \
 drvcomm.lib \
 $(EXCLUDE).LIB \
 $(INOCULATE).LIB \
%if($(CORE_PLAT) != "A")
 $(CERTLIB).LIB \
%endif
 $(VIRUSSCAN).LIB \
 $(NAVUTILS).LIB \
 $(SECURITY).LIB \
 NAVUICOM.LIB \
 XNTOPTS.LIB \
 progman.lib \
 uuid.lib \
 CMBSTR.LIB

# More NT Libraries
MORELIBS = \
 $(SCANNING).LIB \
 $(ACTIVITYLOG).LIB \
 $(SCARYCODE).LIB \
 $(NAVUTILITY).LIB \
 $(VIRUSLIST).LIB

%if($(CORE_PLAT) == "A")
LIBS += CTL3D32.LIB
%endif


# End NAVSCANW
# ---------------------------------------------------------------------

%Else

BAD_TARGET:
    beep
    echo the $(TARGET) target is not supported for NAVW.

%Endif # NAVW

navcrc.nav:       $(PROGRAM_NAME)
		   if exist $(PROGRAM_NAME) navcrc $(PROGRAM_NAME)

$(PROGRAM_NAME): \
 navw.obj \
 navwbmp.obj \
 navwmcmd.obj \
 navwdrop.obj \
 msgreg.obj \
 NavVDefs.obj \
 toolbar.obj \
 maintool.obj \
 treectrl.obj \
 infodlg.obj \
 quarmon.obj \
 nvbrowse.obj \
 status.obj \
 $(OBJS) \
 $(MORELIBS) \
 $(Q_SYMGUI).LIB \
 $(Q_SYMKRNL).LIB \
 $(Q_SYMUTIL).LIB \
%If "$(TARGET)" == "NAVSCANW"  # Check for NT
 NAVNTSCH.LIB \
%EndIf
 $(Q_SYMDEBUG).LIB \
 $(OLE32_LIB) \
 comctl32.lib \
 wndsize.lib \
 $(RES_FILE)

$(RES_FILE): \
 $(RES_SRC) \
 activity.bmp \
 options.bmp \
 onramp.bmp \
 quarent.bmp \
 schedule.bmp \
 viruslst.bmp \
 brwschks.bmp \
%If "$(TARGET)" == "NAVSCANW"
 NSC95_16.bmp \
 NSC95256.bmp \
%ElseIf %member($(TARGET),NAVWPRV NAVWNTPR)
 Navo16T.bmp \
 Navo256T.bmp \
%else
 Navo16.bmp \
 Navo256.bmp \
%endif
 navw.stb \
 navw.dlg \
 $(NAVW_ICO) \
 virus.ico \
 navw.mnu \
 navwres.h \
 navwstb.h \
 navteam.bmp \
 apon.ico \
 apoff.ico \
 quarent.ico \
 vdefs.ico

#UPDATE#
navwmcmd.obj:   platform.h dbcs_str.h windowsx.h xapi.h disk.h \
		undoc.h stddir.h stdhelp.h ngui.h version.h quakever.h \
		wndclass.h passwd.h stockbmp.h ctl3d.h commdlg.h ndrvlist.h \
		msgdisp.h virscan.h ctsn.h virusinf.h netalert.h \
		common.h options.h navopts.h exclude.h netobj.h symnet.h \
		register.h cts4.h navwres.h navwstb.h security.h \
		navw_hlp.h cmbstr.h nonav.h navutil.h symcfg.h navalog.h \
		virlist.h npt.h avapi.h callback.h navw.h navvdefs.h
navwhmem.obj:   platform.h dbcs_str.h windowsx.h xapi.h disk.h \
		undoc.h stddir.h stdhelp.h ngui.h version.h quakever.h \
		wndclass.h passwd.h stockbmp.h ctl3d.h commdlg.h ndrvlist.h \
		msgdisp.h virscan.h ctsn.h virusinf.h netalert.h \
		common.h options.h navopts.h exclude.h netobj.h symnet.h \
		register.h cts4.h navwres.h navwstb.h security.h \
		navw_hlp.h cmbstr.h nonav.h navutil.h symcfg.h navalog.h \
		virlist.h npt.h avapi.h callback.h navw.h \
		navwhmem.cpp
navw.obj:       platform.h dbcs_str.h xapi.h ngui.h version.h quakever.h \
		wndclass.h passwd.h stockbmp.h disk.h ndrvlist.h \
		msgdisp.h stdhelp.h msgreg.h navw_hlp.h symnet.h \
		virscan.h ctsn.h virusinf.h inoc.h symcfg.h nonav.h options.h \
		navopts.h exclude.h netobj.h register.h cts4.h navutil.h \
		navwres.h navwstb.h security.h cmbstr.h n32behav.h \
		logio.h navw.h navalog.h navbin.h navbnhdr.h navvdefs.h
navwbmp.obj:    platform.h dbcs_str.h navutil.h ctsn.h options.h \
		navopts.h exclude.h netobj.h symnet.h register.h cts4.h \
		symcfg.h navw.h nonav.h navwres.h
navwdrop.obj:   platform.h dbcs_str.h nonav.h options.h navopts.h \
		exclude.h ctsn.h netobj.h symnet.h register.h cts4.h \
		navutil.h symcfg.h navw.h
navwfile.obj:   platform.h dbcs_str.h dlgs.h ngui.h version.h quakever.h \
		wndclass.h passwd.h stockbmp.h ctl3d.h commdlg.h disk.h \
		ndrvlist.h msgdisp.h nonav.h options.h \
		navopts.h exclude.h ctsn.h netobj.h symnet.h register.h \
		cts4.h navutil.h symcfg.h stdhelp.h navw.h \
		navw_hlp.h navwres.h navwstb.h
msgreg.obj:     platform.h dbcs_str.h msgreg.h
navvdefs.obj: platform.h dbcs_str.h msgreg.h xapi.h DefUtils.h \
	VirScan.h NavVDefs.h NavAppIDs.h
maintool.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h MainTool.h \
		ToolBar.h navwres.h 
infodlg.obj:    platform.h dbcs_str.h nonav.h infodlg.h
quarmon.obj:    platform.h navw.h navwstb.h navwres.h navcmdid.h
toolbar.obj:    platform.h dbcs_str.h dosvmm.h nlm_str.h toolbar.h 
treectrl.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h treectrl.h 
nvbrowse.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h NVBrowse.h \
		treectrl.h navwres.h 
#ENDUPDATE#
