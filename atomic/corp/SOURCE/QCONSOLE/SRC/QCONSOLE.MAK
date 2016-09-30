# Copyright (C) 1997 Symantec Corporation
##############################################################################
# $Header:   S:/QCONSOLE/VCS/qconsole.mav   1.16   30 Jun 1998 18:37:50   VGLOSTE  $
#
#
##############################################################################
# $Log:   S:/QCONSOLE/VCS/qconsole.mav  $
#  
#     Rev 1.16   30 Jun 1998 18:37:50   VGLOSTE
#  Finished up build system changes.  There are now two targets for QCONSOLE,
#  NAVW and NAVWPRV, and they are set up to build correctly.
#  
#     Rev 1.15   11 Jun 1998 15:53:56   DBuches
#  Added switches for trial ware stuff.
#  
#     Rev 1.14   20 May 1998 17:18:58   DBuches
#  Added heelpdlg.OBJ
#
#     Rev 1.13   11 May 1998 15:46:40   DBuches
#  Added CorpRepairDlg.OBJ.
#
#     Rev 1.12   08 May 1998 17:23:58   DBuches
#  Added NetBrowseDlg.OBJ.
#
#     Rev 1.11   07 May 1999 17:35:20   jtaylor
#  Updated .libs to have the correct platform extension.
#
#     Rev 1.10   28 Apr 1998 15:19:06   DBuches
#  Added Navabout.lib.
#
#     Rev 1.9   14 Apr 1998 00:42:54   DBuches
#  Added objs.
#
#     Rev 1.8   03 Apr 1998 13:32:20   DBuches
#  Added objs.
#
#     Rev 1.7   16 Mar 1998 19:32:58   SEDWARD
#  Added 'chan32i.lib' and 'patch32i.lib' to library list (support for
#  microdefs).
#
#     Rev 1.6   13 Mar 1998 15:24:50   DBuches
#  Added progdlg.obj.
#
#     Rev 1.5   11 Mar 1998 15:19:40   DBuches
#  Added objs.
#
#     Rev 1.4   06 Mar 1998 21:11:26   DBuches
#  Added MoveFileDlg.obj.
#
#     Rev 1.3   06 Mar 1998 11:25:52   DBuches
#  Added objs.
#
#     Rev 1.2   06 Mar 1998 11:23:38   DBuches
#  Added objs.
#
#     Rev 1.1   02 Mar 1998 15:38:52   DBuches
#  Added stuff needed for AVAPI.
#
#     Rev 1.0   27 Feb 1998 15:59:34   DBuches
#  Initial revision.
##############################################################################

TARGET_NAME=QCONSOLE

                                        # Handle trialware target.
%If ("$[u,$(TARGET)]" == "NAVWPRV")
TARGET_NAME=QCONSOLP
%Endif

TARGET_TYPE=EXE
USE_MFC_DLL=TRUE
#USE_CRT_DLL=TRUE
PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

XOPTS = /GX                             # Enable C++ exception handling

                                        # Get around bug in windows header
CLOPTS += /Zp8

                                        # Handle trialware targets.
%If ("$[u,$(TARGET)]" == "NAVWPRV")
DEF += /DNAV_PREVIEW
%Endif



%if "$(CORE_BMODE)"=="R"
XOPTS += /O2                            # Turn on speed optimizations
%endif

LIBS  = qconres.lib
LIBS += OLE32.LIB
LIBS += UUID.LIB
LIBS += comctl32.lib
LIBS += version.lib
LIBS += $(Q_SYMKRNL).lib
LIBS += n32cores.lib
LIBS += defutils.lib
LIBS += n32call.lib
LIBS += chan32$(CORE_PLAT).lib
LIBS += patch32$(CORE_PLAT).lib
LIBS += NAVABOUT.lib
LIBS += NETBREXT.lib

%if "$(CORE_BMODE)"=="D"
LIBS += $(Q_SYMDEBUG).lib
%endif


$(PROGRAM_NAME): CDCOS.OBJ \
                COOLMENU.OBJ \
                GENPROP.OBJ \
                ImageButton.OBJ \
                ItemPropSheet.OBJ \
                MAINFRM.OBJ \
                QCONSOLE.OBJ \
                qconsoleDoc.OBJ \
                qconsoleView.OBJ \
                QTreeView.OBJ \
                ServerProp.OBJ \
                STDAFX.OBJ \
                SUBCLASS.OBJ \
                VirusPage.OBJ \
                coolbar.obj \
                rangestr.obj \
                proppage.obj \
                movefiledlg.obj \
                sortlist.obj \
                resultsdlg.obj \
                progdlg.obj \
                GenOptsPage.OBJ \
                OptionsSheet.OBJ \
                password.OBJ \
                FileTypePage.OBJ \
                ExportDlg.OBJ \
                NetBrowseDlg.OBJ \
                CorpRepairDlg.OBJ \
                helpdlg.OBJ \
                QCONSOLE.RES


#UPDATE#
#ENDUPDATE#
