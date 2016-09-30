#########################################################################
#
#
# $Header:   S:/NAVLU/VCS/navlu.mav   1.20   20 May 1998 16:52:56   tcashin  $
#
#########################################################################
# $Log:   S:/NAVLU/VCS/navlu.mav  $ #
#  
#     Rev 1.20   20 May 1998 16:52:56   tcashin
#  Added files for the patching progress dialog.
#  
#     Rev 1.19   27 Apr 1998 14:35:12   RStanev
#  Added def annuity.
#
#     Rev 1.18   09 Apr 1998 17:07:24   mdunn
#  Changed CHAN/PATCH32 LIB lines to use $(CORE_PLAT).
#  Added CTaskAPI LIB.
#
#     Rev 1.17   15 Mar 1998 14:10:12   tcashin
#  Added PATCH32.LIB and CHAN32I.LIB for microdef support.
#
#     Rev 1.16   09 Mar 1998 20:48:36   tcashin
#  Back out the changes for MicroDefs until we get
#  AVCOMP build issues resolved.
#
#     Rev 1.14   06 Aug 1997 10:29:02   JBRENNA
#  Add OLE32.LIB and UUID.LIB for NPSAPI.LIB.
#
#     Rev 1.13   25 Jul 1997 20:52:44   JBRENNA
#  Added /GX to the CL options. This compiles with exception handling.
#
#     Rev 1.12   01 Jul 1997 18:08:54   JBRENNA
#  Update dependencies.
#
#     Rev 1.11   01 Jul 1997 17:22:34   JBRENNA
#  Add BogusMainWnd.obj.
#
#     Rev 1.10   26 Jun 1997 18:10:58   JBRENNA
#  Use precompiled header files (PCH).
#
#     Rev 1.9   18 Jun 1997 14:45:04   JBRENNA
#  Add N32Call.LIB to the list of linking LIBs.
#
#     Rev 1.8   04 Jun 1997 16:22:18   JBRENNA
#  Remove afxres.rc dependency. It was causing problems with the build system.
#
#     Rev 1.7   03 Jun 1997 14:30:46   JBRENNA
#  Update dependencies.
#
#     Rev 1.6   03 Jun 1997 13:25:18   JBRENNA
#  Add /D_MBCS to the compile options.
#
#     Rev 1.5   02 Jun 1997 18:17:26   JBRENNA
#  Link in NavLuUtils.OBJ.
#
#     Rev 1.4   01 Jun 1997 18:57:54   JBRENNA
#  Link in MyUtils.obj, DefUtils.LIB, NpsApi.LIB, N32CoreS.LIB (AvApi1),
#  $(Q_SYMDEBUG).LIB, and $(Q_SYMKRNL).LIB.
#
#     Rev 1.3   23 May 1997 21:32:06   JBRENNA
#  Add CmdLineInfo.obj and ScheduleDlg.obj to the list of files to build.
#
#     Rev 1.2   16 May 1997 11:26:06   JBRENNA
#  Remove SymKrnl from the build. It is no longer necessary.
#
#     Rev 1.1   16 May 1997 11:07:36   JBRENNA
#  Use MFC support.
#
#     Rev 1.0   10 May 1997 01:21:04   JBRENNA
#  Initial revision.
#
#########################################################################

TARGET_TYPE=EXE
PROGRAM_NAME = NAVLU32.EXE
USE_MFC_DLL=TRUE
#USE_MFC_OLE=TRUE

%if "$(CV)" != ""
DEBUG_AFX=YES
DEBUG_CRT=YES
%endif

PCH=StdAfx

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OPT_NOCV=-Os -Ob2 /MD /GX
XOPTS += -W3 -Zp1 /GX /D_MBCS
FIRSTLIBS += MFCS42$(_AFXDEBUG).lib

LIBS += DefUtils.LIB
LIBS += NpsApi.LIB
LIBS += N32CoreS.LIB
LIBS += $(Q_SYMDEBUG).LIB
LIBS += $(Q_SYMKRNL).LIB
LIBS += N32Call.LIB
LIBS += Ole32.lib
LIBS += uuid.lib
LIBS += CHAN32$(CORE_PLAT).LIB
LIBS += PATCH32$(CORE_PLAT).LIB
LIBS += ctaskapi.lib
LIBS += shell32.lib
LIBS += DefAnnty.lib

%if "$[upper,$(CORE_PLAT)]" == "I"
LIBS += mstask.lib
%endif

$(PROGRAM_NAME):        \
	StdAfx.obj      \
	NavLUApp.obj    \
	NavLU.obj       \
	MyUtils.obj     \
	NavLuUtils.obj  \
	CmdLineInfo.obj \
	ScheduleDlg.obj \
	BogusMainWnd.obj\
	ProgDlg.obj     \
	navluprog.obj   \
	NavLU.res


#UPDATE#
myutils.obj:    stdafx.h afxdisp.h platform.h dbcs_str.h dosvmm.h \
		nlm_str.h MyUtils.h resource.h nstdhelp.h NpsApi.h
navlu.obj:      StdAfx.h platform.h dbcs_str.h dosvmm.h nlm_str.h file.h \
		xapi.h resource.h virscan.h n30type.h avtypes.h virusinf.h \
		navcb.h avcb.h callbk15.h pamapi.h \
		datafile.h cpu.h exclude.h opcode.h profile.h page.h search.h \
		sigex.h prepair.h pamcache.h cover.h proto.h avapi.h \
		defutils.h NavLu.h NavLuUtils.h MyUtils.h strings.h
navluapp.obj:   StdAfx.h platform.h dbcs_str.h dosvmm.h nlm_str.h \
		NavLuApp.h resource.h CmdLineInfo.h ScheduleDlg.h NavLu.h \
		DefUtils.h onramp.h MyUtils.h Strings.h BogusMainWnd.h \
		DefAnnuity.h
navluutils.obj: StdAfx.h platform.h dbcs_str.h dosvmm.h nlm_str.h file.h \
		xapi.h NavLuUtils.h strings.h
cmdlineinfo.obj:        StdAfx.h CmdLineInfo.h resource.h
scheduledlg.obj:        stdafx.h afxdisp.h platform.h dbcs_str.h dosvmm.h \
		nlm_str.h navlu.h DefUtils.h MyUtils.h resource.h \
		ScheduleDlg.h strings.h navluhlp.h
stdafx.obj:     stdafx.h
bogusmainwnd.obj:       stdafx.h BogusMainWnd.h
navlu.res:      resource.h afxres.h winres.h
ProgDlg.obj:    progdlg.h       
Navluprog.obj:  navluprog.h pa.h defutils.h
#ENDUPDATE#
