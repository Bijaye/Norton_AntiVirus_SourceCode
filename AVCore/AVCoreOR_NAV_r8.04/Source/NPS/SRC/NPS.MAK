#########################################################################
# $Header:   S:/NPS/VCS/nps.mav   1.31   15 Apr 1998 17:19:12   mdunn  $ */
#
#########################################################################
# $Log:   S:/NPS/VCS/nps.mav  $ */
#  
#     Rev 1.31   15 Apr 1998 17:19:12   mdunn
#  Added switches to have the build system include MFC stuff.
#  
#     Rev 1.30   10 Sep 1997 15:35:14   KSACKIN
#  Ported changes from QAKG.
#  
#     Rev 1.29.1.1   21 Aug 1997 12:56:32   KSACKIN
#  Removed the NavAbout.lib file, as we are no longer using the NAV About box.
#  We are now using the old StdAbout box again.
#
#     Rev 1.29.1.0   12 Aug 1997 22:49:48   DALVIDR
#  Branch base for version QAKG
#
#     Rev 1.29   15 Jul 1997 15:13:26   DBUCHES
#  Added Ctl3d Support for DEC Alpha
#
#     Rev 1.28   02 Jul 1997 10:34:40   TIVANOV
#  include the new lib to the list
#
#     Rev 1.27   01 Jul 1997 16:13:22   TIVANOV
#  No change.
#
#     Rev 1.26   22 Feb 1996 13:28:50   YVANG
#  Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
#  NAV version.
#
#     Rev 1.25   11 Jan 1996 09:50:26   JWORDEN
#  Port Quake 8 changes
#
#     Rev 1.24.1.1   30 Nov 1995 21:11:24   BARRY
#  Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
#
#     Rev 1.24   26 Jun 1995 15:16:56   MARKL
#  Fixed defect #38667
#
#     Rev 1.23   02 May 1995 13:39:22   MFALLEN
#  Using quake shell api.
#
#     Rev 1.22   23 Apr 1995 22:20:58   MFALLEN
#  npsshell.cpp
#
#     Rev 1.21   18 Apr 1995 14:13:00   MFALLEN
#  New name
#
#     Rev 1.20   14 Mar 1995 13:11:10   MFALLEN
#  Removed cntxhelp.cpp
#
#     Rev 1.19   06 Mar 1995 10:31:28   MFALLEN
#  Made it work for Win95
#
#     Rev 1.18   17 Oct 1994 19:15:32   BRAD
#  Removed references to VMM.H
#
#     Rev 1.17   31 Mar 1994 17:45:28   BRAD
#  Removed CTL3D for WIN32
#
#     Rev 1.16   30 Mar 1994 17:41:00   BRAD
#  Replace stubenv and remove Windows system libs
#
#     Rev 1.15   28 Mar 1994 14:20:00   BRAD
#  Static link to CTL3d for WIN32 for now
#
#     Rev 1.14   23 Mar 1994 20:32:36   BRAD
#  For now, don't include DEF file
#
#     Rev 1.13   09 Mar 1994 14:54:08   BRAD
#  Moved standard libs to global MAK file.
#
#     Rev 1.12   08 Mar 1994 19:11:44   BRAD
#  Use PLATFORM.MAK
#
#     Rev 1.11   15 Oct 1993 02:19:34   BRUCEH
#  Added dependency
#
#     Rev 1.10   29 Sep 1993 14:58:16   MARKL
#  No change.
#
#     Rev 1.9   23 Sep 1993 20:25:26   LMULCAHY
#  removed ctl3d.lib
#
#     Rev 1.8   16 Aug 1993 14:14:50   DLEVITON
#  Replace MKTOOLS with QUAKEINC
#
#     Rev 1.7   11 Aug 1993 16:46:12   MARKL
#  It helps to spell the option correctly.
#
#     Rev 1.6   11 Aug 1993 16:39:58   MARKL
#  Now builds for windows 3.1
#
#     Rev 1.5   26 Jul 1993 09:52:26   MARKL
#  Updated make file for version 0 quake.
#
#     Rev 1.4   22 Jul 1993 12:01:00   MARKL
#  Updated dependencies.
#
#     Rev 1.3   21 Jul 1993 11:16:08   DLEVITON
#  Quake versioning support.
#
#########################################################################
PROGRAM_NAME=NSCHED32.EXE
TARGET_TYPE=EXE
USE_MFC_DLL=TRUE
USE_CRT_DLL=TRUE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

nps.exe: \
    nps.obj      \
    npsadedt.obj \
    npscfg.obj   \
    npsclock.obj \
    npsctls.obj  \
    npsdates.obj \
    npsdo.obj    \
    npsevnts.obj \
    npsgener.obj \
    npsglobl.obj \
    npsinit.obj  \
    npslook.obj  \
    npsnotfy.obj \
    npsopts.obj  \
    nps_conf.obj \
    nps_tray.obj \
    uuid.lib \
    ole32.lib \
    $(Q_SYMGUI).LIB \
    $(Q_SYMKRNL).LIB \
    $(Q_SYMUTIL).LIB \
    $(Q_SYMDEBUG).LIB \
    nps.def          \
    nps.res

%If ($(CORE_PLAT) == "A")
LIBS += CTL3D32.LIB
%Endif

nps.res: nps.rc nps.h npsres.h nps.ico npsclock.ico npsstb.h npsres.h \
         add.bmp copy.bmp edit.bmp delete.bmp \
         close.bmp help.bmp options.bmp \
         addm.bmp copym.bmp editm.bmp deletem.bmp \
         closem.bmp helpm.bmp optionsm.bmp


#UPDATE#
nps.obj:        platform.h dbcs_str.h dosvmm.h undoc.h ngui.h version.h \
                quakever.h wndclass.h passwd.h stockbmp.h symintl.h msgdisp.h \
                xapi.h file.h winini.h stdhelp.h nps.h npsres.h npsstb.h \
                nps_hlp.h navbnhdr.h navopts.h siunivsl.h sirginfo.h
npsadedt.obj:   platform.h dbcs_str.h dosvmm.h undoc.h file.h xapi.h \
                msgdisp.h ngui.h version.h quakever.h wndclass.h passwd.h \
                stockbmp.h browsdlg.h stddir.h symintl.h stdhelp.h nps.h \
                npsres.h npsstb.h nps_hlp.h
npscfg.obj:     platform.h dbcs_str.h dosvmm.h ngui.h version.h quakever.h \
                wndclass.h passwd.h stockbmp.h ctrlpanl.h stdhelp.h nps.h \
                npsres.h npsstb.h nps_hlp.h
npsclock.obj:   platform.h dbcs_str.h dosvmm.h undoc.h msgdisp.h ngui.h \
                version.h quakever.h wndclass.h passwd.h stockbmp.h stdhelp.h \
                nps.h npsres.h npsstb.h
npsctls.obj:    platform.h dbcs_str.h dosvmm.h ngui.h version.h \
                quakever.h wndclass.h passwd.h stockbmp.h dbcs.h msgdisp.h \
                stdhelp.h nps.h npsres.h npsstb.h nps_hlp.h
npsdates.obj:   platform.h dbcs_str.h dosvmm.h undoc.h msgdisp.h ngui.h \
                version.h quakever.h wndclass.h passwd.h stockbmp.h symintl.h \
                stdhelp.h nps.h npsres.h npsstb.h
npsdo.obj:      platform.h dbcs_str.h dosvmm.h xapi.h undoc.h msgdisp.h \
                symintl.h ngui.h version.h quakever.h wndclass.h passwd.h \
                stockbmp.h stdhelp.h nps.h npsres.h npsstb.h nps_hlp.h
npsevnts.obj:   platform.h dbcs_str.h dosvmm.h undoc.h msgdisp.h xapi.h \
                file.h ngui.h version.h quakever.h wndclass.h passwd.h \
                stockbmp.h symintl.h stdhelp.h nps.h npsres.h npsstb.h
npsinit.obj:    platform.h dbcs_str.h dosvmm.h msgdisp.h ngui.h version.h \
                quakever.h wndclass.h passwd.h stockbmp.h stdhelp.h nps.h \
                npsres.h npsstb.h dbcs.h
npsnotfy.obj:   platform.h dbcs_str.h dosvmm.h undoc.h msgdisp.h ngui.h \
                version.h quakever.h wndclass.h passwd.h stockbmp.h stdhelp.h \
                nps.h npsres.h npsstb.h nps_hlp.h
nps_conf.obj:   platform.h dbcs_str.h dosvmm.h ngui.h version.h \
                quakever.h wndclass.h passwd.h stockbmp.h stdhelp.h nps.h \
                npsres.h npsstb.h nps_hlp.h
npslook.obj:    platform.h dbcs_str.h dosvmm.h ngui.h version.h \
                quakever.h wndclass.h passwd.h stockbmp.h ctrlpanl.h \
                stdhelp.h nps.h npsres.h npsstb.h nps_hlp.h
nps_tray.obj:   platform.h dbcs_str.h dosvmm.h ngui.h version.h \
                quakever.h wndclass.h passwd.h stockbmp.h stdhelp.h nps.h \
                npsres.h npsstb.h nps_hlp.h
npsopts.obj:    platform.h dbcs_str.h dosvmm.h ngui.h version.h \
                quakever.h wndclass.h passwd.h stockbmp.h ctrlpanl.h \
                stdhelp.h nps.h npsres.h npsstb.h nps_hlp.h
npsgener.obj:   platform.h dbcs_str.h dosvmm.h ngui.h version.h \
                quakever.h wndclass.h passwd.h stockbmp.h ctrlpanl.h \
                stdhelp.h nps.h npsres.h npsstb.h nps_hlp.h
npsglobl.obj:   platform.h dbcs_str.h dosvmm.h ngui.h version.h \
                quakever.h wndclass.h passwd.h stockbmp.h ctrlpanl.h \
                stdhelp.h nps.h npsres.h npsstb.h nps_hlp.h
#ENDUPDATE#
