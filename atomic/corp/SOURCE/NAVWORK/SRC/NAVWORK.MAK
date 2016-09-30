#-------------------------------------------------------------------------
#
# $Header:   S:/NAVWORK/VCS/navwork.mav   1.26   03 Jun 1998 18:49:04   tcashin  $
#
# Description:
#       NAVINOC makefile.
#
#-------------------------------------------------------------------------
# $Log:   S:/NAVWORK/VCS/navwork.mav  $
#  
#     Rev 1.26   03 Jun 1998 18:49:04   tcashin
#  Added n_forwrd.cpp.
#  
#     Rev 1.25   11 May 1998 10:54:36   DBuches
#  Removed CAB file stuff.
#
#     Rev 1.24   21 Apr 1998 15:31:16   MKEATIN
#  Added CAB scanning support.
#
#     Rev 1.23   07 Apr 1998 13:09:46   tcashin
#  Oops. The OLE32 .lib was in the wrong place causing Alpha builds to break.
#
#     Rev 1.22   06 Apr 1998 12:29:14   MKEATIN
#  Removed n_zip.obj
#
#     Rev 1.21   03 Apr 1998 12:17:02   tcashin
#  Changes for new quarantine stuff.
#
#     Rev 1.20   30 Mar 1998 17:32:04   MKEATIN
#  Don't link the DEC2 libs on ALPHA.
#
#     Rev 1.19   16 Mar 1998 18:56:34   MKEATIN
#  Made changed to add the decomposer
#
#     Rev 1.18   10 Mar 1998 00:35:38   DALLEE
#  Added comctl32.lib. Required when linking animation.obj for IRA.
#  Debug MFC image lists apparently use a different implementation not
#  dependent on comctl32.lib.
#
#     Rev 1.17   09 Mar 1998 23:48:02   DALLEE
#  Now using MFC's.
#  Added OBJ's for new Repair Wizard.
#
#     Rev 1.16   11 Aug 1997 01:06:30   KSACKIN
#  Enabled structured exception handleing in this DLL.  We use this to catch
#  the NavZip engine before it actually gives us an exception.
#
#     Rev 1.15   18 Jul 1997 19:47:56   GWESTER
#  Added animation BMPs to the navwork.rc dependencies.
#
#     Rev 1.14   15 Jul 1997 14:42:56   DBUCHES
#  Added Ctl3d for DEC Alpha
#
#     Rev 1.13   30 Jun 1997 19:20:26   DSACKIN
#  Added a dependency to the behavior DLL.
#
#     Rev 1.12   22 May 1997 16:17:14   BILL
#  Alpha changes (removed ctl3d)
#
#     Rev 1.11   18 May 1997 05:50:58   DSACKIN
#  Switched from CAlertNotification to CVirusAlert
#
#     Rev 1.10   02 May 1997 23:14:34   DSACKIN
#  NAVWNT should be using XNTSERVE.LIB instead of N32SERVE.LIB
#-------------------------------------------------------------------------

%If "$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == ""

  %if ("$[u,$(TARGET)]" == "NAVWNT")
    TARGET_NAME=XNTWORK
    DEF += /DNAVWNT /DNAVSCANW

  %elseif ("$[u,$(TARGET)]"== "NAVSCANW")
    TARGET_NAME=NNTWORK
    DEF += /DNAVSCANW

  %else
    TARGET_NAME=N32WORK
    DEF += /DNAV95_CODE
  %endif

  # This define provides the C++ definition of NetAlert() in NETALERT.H. The
  # actual function body is declared locally (N_ALERT.CPP).
  DEF += /DNAV_IN_NAVWORK

%Else

BAD_PLATFORM:
    beep
    echo error: $(PLATFORM) is not supported by the $(PROJNAME) project. >> make.err
    echo error: $(PLATFORM) is not supported by the $(PROJNAME) project.

%EndIf

USE_MFC_DLL=YES
TARGET_TYPE=DLL

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=$(TARGET_NAME).DLL

UNIPATH=$(PROJPRE)LIB.$(CORE_PLAT)$(CORE_BMODE)U

%If "$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == ""
    ## Turn on structured exception handling.
    CLOPTS += -GX
%Endif

%if ("$[u,$(TARGET)]" == "NAVWNT")
    ACTIVITYLOG=XNTALOG
    EXCLUDE=XNTEXCLU
    NAVUTILITIES=XNTXUTIL
    INOCULATE=XNTINOC
    NETCOMM_LIB=XNTSERVE.LIB
    LOADMANAGER_LIB=LOADMGMT.LIB
    INTEGRITY_LIB=
    BEHAVIOR_LIB=N32PDLL.LIB
    UNIUTIL_LIB=$(UNIPATH)\UNIUTIL.LIB

    CALERT_LIB=$(UNIPATH)\CALERT.LIB
    CVALERT_LIB=$(UNIPATH)\CVALERT.LIB

%elseif ("$[u,$(TARGET)]" == "NAVSCANW")
    ACTIVITYLOG=NNTALOG
    EXCLUDE=NNTEXCLU
    NAVUTILITIES=NNTXUTIL
    INOCULATE=NNTLIB01
    NETCOMM_LIB=
    LOADMANAGER_LIB=
    INTEGRITY_LIB=
    BEHAVIOR_LIB=N32PDLL.LIB
    UNIUTIL_LIB=$(UNIPATH)\UNIUTIL.LIB
    CALERT_LIB=
    CVALERT_LIB=
%else
    ACTIVITYLOG=N32ALOG
    EXCLUDE=N32EXCLU
    NAVUTILITIES=N32XUTIL
    INOCULATE=N32INOC
    NETCOMM_LIB=N32SERVE.LIB
    LOADMANAGER_LIB=
    INTEGRITY_LIB=S32INTEG.LIB
    BEHAVIOR_LIB=N32PDLL.LIB
    UNIUTIL_LIB=

    CALERT_LIB=$(UNIPATH)\CALERT.LIB
    CVALERT_LIB=$(UNIPATH)\CVALERT.LIB
%endif

CERTLIB=CERTLIBN.LIB
NAVUTILS=N32UTILS
VIRUSLIST=N32VLIST
PASSWORDCHECK=N32SECUR
NAVFAMILYUTILITIES=N32UTILS
VIRUSSCAN=V32SCAN

LIBS= \
 cmbstr.lib \
 $(EXCLUDE).LIB \
 $(INOCULATE).LIB \
 $(ACTIVITYLOG).LIB \
 $(Q_SYMALOG).LIB \
 $(NAVUTILITIES).LIB \
 $(VIRUSLIST).LIB \
 $(NAVFAMILYUTILITIES).LIB \
 $(VIRUSSCAN).LIB \
 $(NAVUTILS).LIB \
 $(Q_SYMDEBUG).LIB \
 $(Q_SYMGUI).LIB \
 $(Q_SYMKRNL).LIB \
 $(Q_SYMUTIL).LIB \
 $(Q_NPT).LIB \
 $(NETCOMM_LIB) \
 $(LOADMANAGER_LIB) \
 $(PASSWORDCHECK).LIB \
 $(INTEGRITY_LIB) \
 $(BEHAVIOR_LIB) \
 $(UNIUTIL_LIB) \
 $(CALERT_LIB) \
 $(CVALERT_LIB) \
 rpcrt4.lib \
 rpcndr.lib \
 comctl32.lib \
%If "$(CORE_PLAT)" == "A"
 CTL3D32.LIB \
%Endif
 netapi32.lib \
 OLE32.LIB \
%If "$(CORE_PLAT)" == "I"
 DEC2.LIB \
 DEC2AMG.LIB \
 DEC2ARJ.LIB \
 DEC2CAB.LIB \
 DEC2GZIP.LIB \
 DEC2HQX.LIB \
 DEC2ID.LIB \
 DEC2LHA.LIB \
 DEC2LZ.LIB \
 DEC2MIME.LIB \
 DEC2SS.LIB \
 DEC2TAR.LIB \
 DEC2UUE.LIB \
 DEC2ZIP.LIB \
 $(CERTLIB)
%Endif



$(PROGRAM_NAME): \
 $(STUB) \
 navwork.obj \
 animation.obj \
 cdcos.obj \
 picture.obj \
 problist.obj \
 sortlist.obj \
 rwizard.obj \
 rwpage.obj \
 rwintro.obj \
 rwrepair.obj \
 rwdelete.obj \
 rwstill.obj \
 rwfininf.obj \
 rwfincln.obj \
 rwquar.obj \
 n_wizard.obj \
 n_scanvw.obj \
 n_save.obj \
 n_questi.obj \
 n_print.obj \
 n_virus.obj \
 n_forwrd.obj \
 n_ini.obj \
 n_inoc.obj \
 n_inocfi.obj \
 n_sumary.obj \
 n_scnstr.obj \
 n_scanio.obj \
 n_scan2.obj \
 n_scan.obj \
 n_result.obj \
 n_repkil.obj \
 n_report.obj \
 n_memscn.obj \
 n_info.obj \
 n_dll.obj \
 n_dowork.obj \
%if ("$[u,$(TARGET)]" != "NAVSCANW" && "$[u,$(TARGET)]" != "NAVWNT")
 n_sysint.obj \
%endif
 n_wrilog.obj \
 n_misc.obj \
 n_alert.obj \
 n_decomp.obj \
 navwork.res

navwork.res:    navwork.rc navwork.dlg wtsr_res.h nunavdlg.h \
		ngui.h stdhelp.h navwork.stb nav0.ico nav1.ico nav2.ico \
		nav3.ico nunavstb.h \
		check.bmp checkm.bmp noway.bmp nowaym.bmp \
		scanner1.ico scanner2.ico scanner3.ico scanner4.ico \
		scanner5.ico scanner6.ico scanner7.ico scanner8.ico \
		netscp.bmp netscpm.bmp advoinfo.ico sirens.bmp \
		wz_16_1.bmp wz_16_1m.bmp wz256_1.bmp wz256_1m.bmp \
		wz_16_2.bmp wz_16_2m.bmp wz256_2.bmp wz256_2m.bmp \
		wz_16_3.bmp wz_16_3m.bmp wz256_3.bmp wz256_3m.bmp \
		wz_16_4.bmp wz_16_4m.bmp wz256_4.bmp wz256_4m.bmp \
		wz_16_5.bmp wz_16_5m.bmp wz256_5.bmp wz256_5m.bmp \
		wz_16_6.bmp wz_16_6m.bmp wz256_6.bmp wz256_6m.bmp \
		wz_16_7.bmp wz_16_7m.bmp wz256_7.bmp wz256_7m.bmp \
		inscan01.bmp inscan02.bmp inscan03.bmp \
		inscan1m.bmp inscan2m.bmp inscan3m.bmp \
		ins256_1.bmp ins256_2.bmp ins256_3.bmp ins256_m.bmp

#UPDATE#
animation.obj:  stdafx.h n_app.h Animation.h cdcos.h
cdcos.obj:      stdafx.h cdcos.h
rwdelete.obj:   stdafx.h nunavdlg.h nunavstb.h RWDelete.h picture.h \
		RwPage.h nonav.h options.h navopts.h exclude.h cts4.h ctsn.h \
		callback.h callbk15.h n30type.h avtypes.h platform.h \
		dbcs_str.h dosvmm.h nlm_str.h excludel.h netobj.h symnet.h \
		symnd2.h navstart.h quarset.h custom.h navutil.h npt.h \
		quakever.h version.h virlist.h avapi.h pamapi.h \
		file.h datafile.h cpu.h opcode.h profile.h page.h \
		search.h sigex.h prepair.h pamcache.h cover.h proto.h navcb.h \
		avcb.h xapi.h symcfg.h symalog.h drvprblm.h stdhelp.h \
		syminteg.h problist.h sortlist.h
rwintro.obj:    stdafx.h nunavdlg.h RWIntro.h picture.h RWPage.h nonav.h \
		options.h navopts.h exclude.h cts4.h ctsn.h callback.h \
		callbk15.h n30type.h avtypes.h platform.h dbcs_str.h dosvmm.h \
		nlm_str.h excludel.h netobj.h symnet.h symnd2.h navstart.h \
		quarset.h custom.h navutil.h npt.h quakever.h version.h \
		virlist.h avapi.h pamapi.h file.h \
		datafile.h cpu.h opcode.h profile.h page.h search.h sigex.h \
		prepair.h pamcache.h cover.h proto.h navcb.h avcb.h xapi.h \
		symcfg.h symalog.h drvprblm.h stdhelp.h syminteg.h rwizard.h \
		RwRepair.h problist.h sortlist.h RwDelete.h RwStill.h \
		RwFinInf.h animation.h RwFinCln.h
rwrepair.obj:   stdafx.h nunavdlg.h nunavstb.h RwRepair.h picture.h \
		RwPage.h nonav.h options.h navopts.h exclude.h cts4.h ctsn.h \
		callback.h callbk15.h n30type.h avtypes.h platform.h \
		dbcs_str.h dosvmm.h nlm_str.h excludel.h netobj.h symnet.h \
		symnd2.h navstart.h quarset.h custom.h navutil.h npt.h \
		quakever.h version.h virlist.h avapi.h pamapi.h \
		file.h datafile.h cpu.h opcode.h profile.h page.h \
		search.h sigex.h prepair.h pamcache.h cover.h proto.h navcb.h \
		avcb.h xapi.h symcfg.h symalog.h drvprblm.h stdhelp.h \
		syminteg.h problist.h sortlist.h
rwfincln.obj:   stdafx.h nunavdlg.h RWFinCln.h picture.h RWPage.h \
		nonav.h options.h navopts.h exclude.h cts4.h ctsn.h \
		callback.h callbk15.h n30type.h avtypes.h platform.h \
		dbcs_str.h dosvmm.h nlm_str.h excludel.h netobj.h symnet.h \
		symnd2.h navstart.h quarset.h custom.h navutil.h npt.h \
		quakever.h version.h virlist.h avapi.h pamapi.h \
		file.h datafile.h cpu.h opcode.h profile.h page.h \
		search.h sigex.h prepair.h pamcache.h cover.h proto.h navcb.h \
		avcb.h xapi.h symcfg.h symalog.h drvprblm.h stdhelp.h \
		syminteg.h nunavstb.h
rwfininf.obj:   stdafx.h nunavdlg.h RWFinInf.h picture.h RWPage.h \
		nonav.h options.h navopts.h exclude.h cts4.h ctsn.h \
		callback.h callbk15.h n30type.h avtypes.h platform.h \
		dbcs_str.h dosvmm.h nlm_str.h excludel.h netobj.h symnet.h \
		symnd2.h navstart.h quarset.h custom.h navutil.h npt.h \
		quakever.h version.h virlist.h avapi.h pamapi.h \
		file.h datafile.h cpu.h opcode.h profile.h page.h \
		search.h sigex.h prepair.h pamcache.h cover.h proto.h navcb.h \
		avcb.h xapi.h symcfg.h symalog.h drvprblm.h stdhelp.h \
		syminteg.h animation.h nunavstb.h
rwstill.obj:    stdafx.h nunavdlg.h RwStill.h picture.h RwPage.h nonav.h \
		options.h navopts.h exclude.h cts4.h ctsn.h callback.h \
		callbk15.h n30type.h avtypes.h platform.h dbcs_str.h dosvmm.h \
		nlm_str.h excludel.h netobj.h symnet.h symnd2.h navstart.h \
		quarset.h custom.h navutil.h npt.h quakever.h version.h \
		virlist.h avapi.h pamapi.h file.h \
		datafile.h cpu.h opcode.h profile.h page.h search.h sigex.h \
		prepair.h pamcache.h cover.h proto.h navcb.h avcb.h xapi.h \
		symcfg.h symalog.h drvprblm.h stdhelp.h syminteg.h problist.h \
		sortlist.h nunavstb.h cmbstr.h
navwork.obj:    stdafx.h n_app.h
picture.obj:    stdafx.h n_app.h Picture.h cdcos.h
problist.obj:   stdafx.h navwork.h nonav.h options.h navopts.h exclude.h \
		cts4.h ctsn.h callback.h callbk15.h n30type.h avtypes.h \
		platform.h dbcs_str.h dosvmm.h nlm_str.h excludel.h netobj.h \
		symnet.h symnd2.h navstart.h quarset.h custom.h navutil.h \
		npt.h quakever.h version.h virlist.h avapi.h pamapi.h \
		file.h datafile.h cpu.h opcode.h profile.h page.h \
		search.h sigex.h prepair.h pamcache.h cover.h proto.h navcb.h \
		avcb.h xapi.h symcfg.h symalog.h drvprblm.h stdhelp.h \
		syminteg.h ProbList.h sortlist.h nunavdlg.h nunavstb.h
rwizard.obj:    stdafx.h nunavdlg.h nunavstb.h RWizard.h nonav.h \
		options.h navopts.h exclude.h cts4.h ctsn.h callback.h \
		callbk15.h n30type.h avtypes.h platform.h dbcs_str.h dosvmm.h \
		nlm_str.h excludel.h netobj.h symnet.h symnd2.h navstart.h \
		quarset.h custom.h navutil.h npt.h quakever.h version.h \
		virlist.h avapi.h pamapi.h file.h \
		datafile.h cpu.h opcode.h profile.h page.h search.h sigex.h \
		prepair.h pamcache.h cover.h proto.h navcb.h avcb.h xapi.h \
		symcfg.h symalog.h drvprblm.h stdhelp.h syminteg.h RwIntro.h \
		picture.h RWPage.h RwRepair.h problist.h sortlist.h \
		RwDelete.h RwStill.h RwFinInf.h animation.h RwFinCln.h
sortlist.obj:   stdafx.h navwork.h nonav.h options.h navopts.h exclude.h \
		cts4.h ctsn.h callback.h callbk15.h n30type.h avtypes.h \
		platform.h dbcs_str.h dosvmm.h nlm_str.h excludel.h netobj.h \
		symnet.h symnd2.h navstart.h quarset.h custom.h navutil.h \
		npt.h quakever.h version.h virlist.h avapi.h pamapi.h \
		file.h datafile.h cpu.h opcode.h profile.h page.h \
		search.h sigex.h prepair.h pamcache.h cover.h proto.h navcb.h \
		avcb.h xapi.h symcfg.h symalog.h drvprblm.h stdhelp.h \
		syminteg.h SortList.h cdcos.h
rwpage.obj:     stdafx.h n_app.h RWPage.h nonav.h options.h navopts.h \
		exclude.h cts4.h ctsn.h callback.h callbk15.h n30type.h \
		avtypes.h platform.h dbcs_str.h dosvmm.h nlm_str.h excludel.h \
		netobj.h symnet.h symnd2.h navstart.h quarset.h custom.h \
		navutil.h npt.h quakever.h version.h virlist.h avapi.h \
		pamapi.h file.h datafile.h cpu.h opcode.h \
		profile.h page.h search.h sigex.h prepair.h pamcache.h \
		cover.h proto.h navcb.h avcb.h xapi.h symcfg.h symalog.h \
		drvprblm.h stdhelp.h syminteg.h nunavstb.h
n_questi.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h ngui.h \
		version.h quakever.h wndclass.h passwd.h stockbmp.h stdhelp.h \
		ctl3d.h syminteg.h symcfg.h navutil.h ctsn.h callback.h \
		options.h navopts.h exclude.h cts4.h excludel.h netobj.h \
		symnet.h symnd2.h navstart.h register.h npt.h virlist.h \
		avapi.h symalog.h drvprblm.h navw_hlp.h navwhlp.h cmbstr.h \
		nunavdlg.h nunavstb.h stub.h virscan.h virusinf.h pamapi.h \
		file.h datafile.h cpu.h opcode.h profile.h \
		page.h search.h sigex.h prepair.h pamcache.h cover.h proto.h \
		inoc.h nonav.h dbcs.h
n_scanvw.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h commdlg.h \
		xapi.h ngui.h version.h quakever.h wndclass.h passwd.h \
		stockbmp.h ctl3d.h file.h cmbstr.h symcfg.h stdhelp.h \
		navwork.h navutil.h ctsn.h callback.h options.h navopts.h \
		exclude.h cts4.h excludel.h netobj.h symnet.h symnd2.h \
		navstart.h register.h npt.h virlist.h avapi.h symalog.h \
		drvprblm.h nunavdlg.h nunavstb.h navw_hlp.h navwhlp.h \
		navalog.h stub.h virscan.h virusinf.h pamapi.h \
		datafile.h cpu.h opcode.h profile.h page.h \
		search.h sigex.h prepair.h pamcache.h cover.h proto.h nonav.h \
		syminteg.h
n_result.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h disk.h \
		ngui.h version.h quakever.h wndclass.h passwd.h stockbmp.h \
		ctl3d.h syminteg.h stdhelp.h symcfg.h navutil.h ctsn.h \
		callback.h options.h navopts.h exclude.h cts4.h excludel.h \
		netobj.h symnet.h symnd2.h navstart.h register.h npt.h \
		virlist.h avapi.h symalog.h drvprblm.h navw_hlp.h navwhlp.h \
		cmbstr.h nunavdlg.h nunavstb.h security.h stub.h virscan.h \
		virusinf.h pamapi.h file.h datafile.h \
		cpu.h opcode.h profile.h page.h search.h sigex.h prepair.h \
		pamcache.h cover.h proto.h inoc.h nonav.h filestat.h
n_report.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h file.h symcfg.h \
		navutil.h ctsn.h callback.h options.h navopts.h exclude.h \
		cts4.h excludel.h netobj.h symnet.h symnd2.h navstart.h \
		register.h npt.h quakever.h version.h virlist.h avapi.h \
		xapi.h symalog.h drvprblm.h stdhelp.h nunavdlg.h nunavstb.h \
		stub.h nonav.h syminteg.h virscan.h virusinf.h pamapi.h \
		datafile.h cpu.h opcode.h profile.h page.h \
		search.h sigex.h prepair.h pamcache.h cover.h proto.h \
		netalert.h
n_dowork.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h symcfg.h \
		file.h navutil.h ctsn.h callback.h options.h navopts.h \
		exclude.h cts4.h excludel.h netobj.h symnet.h symnd2.h \
		navstart.h register.h npt.h quakever.h version.h virlist.h \
		avapi.h symalog.h drvprblm.h stdhelp.h navwork.h nunavstb.h \
		syminteg.h stub.h inoc.h virscan.h virusinf.h pamapi.h \
		datafile.h cpu.h opcode.h profile.h page.h \
		search.h sigex.h prepair.h pamcache.h cover.h proto.h nonav.h \
		navstat.h symstat.h navver.h disk.h msgdisp.h
n_wrilog.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h ngui.h \
		version.h quakever.h wndclass.h passwd.h stockbmp.h stdhelp.h \
		ctl3d.h syminteg.h navgraph.hpp stack.hpp stub.h virscan.h \
		ctsn.h callback.h virusinf.h pamapi.h \
		file.h datafile.h cpu.h exclude.h opcode.h profile.h page.h \
		search.h sigex.h prepair.h pamcache.h cover.h proto.h \
		symcfg.h navutil.h options.h navopts.h netobj.h symnet.h \
		symnd2.h navstart.h register.h cts4.h npt.h virlist.h avapi.h \
		symalog.h drvprblm.h inoc.h nonav.h navw_hlp.h navwhlp.h \
		cmbstr.h nunavdlg.h nunavstb.h navwork.h
n_save.obj:     platform.h dbcs_str.h dosvmm.h nlm_str.h commdlg.h xapi.h \
		ngui.h version.h quakever.h wndclass.h passwd.h stockbmp.h \
		ctl3d.h file.h cmbstr.h symcfg.h stdhelp.h navwork.h \
		navutil.h ctsn.h callback.h options.h navopts.h exclude.h \
		cts4.h excludel.h netobj.h symnet.h symnd2.h navstart.h \
		register.h npt.h virlist.h avapi.h symalog.h drvprblm.h \
		navw_hlp.h navwhlp.h navalog.h stub.h virscan.h virusinf.h \
		pamapi.h datafile.h cpu.h opcode.h \
		profile.h page.h search.h sigex.h prepair.h pamcache.h \
		cover.h proto.h nonav.h syminteg.h nunavdlg.h nunavstb.h \
		navw.h navver.h
n_print.obj:    platform.h dbcs_str.h dosvmm.h nlm_str.h hstring.h xapi.h \
		windowsx.h symcfg.h cmbstr.h nunavdlg.h nunavstb.h navutil.h \
		ctsn.h callback.h options.h navopts.h exclude.h cts4.h \
		excludel.h netobj.h symnet.h symnd2.h navstart.h register.h \
		npt.h quakever.h version.h virlist.h avapi.h symalog.h \
		drvprblm.h stdhelp.h syminteg.h stub.h nonav.h dbcs.h
n_dll.obj:      platform.h dbcs_str.h dosvmm.h nlm_str.h ngui.h version.h \
		quakever.h wndclass.h passwd.h stockbmp.h file.h disk.h \
		ctl3d.h syminteg.h stub.h cmbstr.h symcfg.h navutil.h ctsn.h \
		callback.h options.h navopts.h exclude.h cts4.h excludel.h \
		netobj.h symnet.h symnd2.h navstart.h register.h npt.h \
		virlist.h avapi.h xapi.h symalog.h drvprblm.h stdhelp.h \
		nonav.h nunavdlg.h nunavstb.h
n_forwrd.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h ngui.h version.h \
		quakever.h wndclass.h passwd.h stockbmp.h file.h disk.h \
		ctl3d.h syminteg.h stub.h cmbstr.h symcfg.h navutil.h ctsn.h \
		callback.h options.h navopts.h exclude.h cts4.h excludel.h \
		netobj.h symnet.h symnd2.h navstart.h register.h npt.h \
		virlist.h avapi.h xapi.h symalog.h drvprblm.h stdhelp.h \
		nonav.h nunavdlg.h nunavstb.h
n_repkil.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h ngui.h \
		version.h quakever.h wndclass.h passwd.h stockbmp.h ctl3d.h \
		syminteg.h file.h cmbstr.h symcfg.h stdhelp.h navwork.h \
		navutil.h ctsn.h callback.h options.h navopts.h exclude.h \
		cts4.h excludel.h netobj.h symnet.h symnd2.h navstart.h \
		register.h npt.h virlist.h avapi.h symalog.h drvprblm.h \
		nunavdlg.h nunavstb.h navw_hlp.h navwhlp.h navalog.h stub.h \
		virscan.h virusinf.h pamapi.h datafile.h \
		cpu.h opcode.h profile.h page.h search.h sigex.h prepair.h \
		pamcache.h cover.h proto.h nonav.h navapcom.h navwcomm.h
n_scanio.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h ngui.h \
		version.h quakever.h wndclass.h passwd.h stockbmp.h symcfg.h \
		navutil.h ctsn.h callback.h options.h navopts.h exclude.h \
		cts4.h excludel.h netobj.h symnet.h symnd2.h navstart.h \
		register.h npt.h virlist.h avapi.h symalog.h drvprblm.h \
		stdhelp.h cmbstr.h syminteg.h stub.h nonav.h nunavdlg.h \
		nunavstb.h navwork.h
n_misc.obj:     platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h file.h \
		ctsn.h callback.h syminteg.h stub.h virscan.h virusinf.h \
		pamapi.h datafile.h cpu.h exclude.h \
		opcode.h profile.h page.h search.h sigex.h prepair.h \
		pamcache.h cover.h proto.h nonav.h options.h navopts.h \
		netobj.h symnet.h symnd2.h navstart.h register.h cts4.h \
		navutil.h npt.h quakever.h version.h virlist.h avapi.h \
		symcfg.h symalog.h drvprblm.h stdhelp.h inoc.h
n_sumary.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h ngui.h \
		version.h quakever.h wndclass.h passwd.h stockbmp.h xapi.h \
		disk.h file.h hstring.h npt.h ctl3d.h syminteg.h stdhelp.h \
		symcfg.h symintl.h navutil.h ctsn.h callback.h options.h \
		navopts.h exclude.h cts4.h excludel.h netobj.h symnet.h \
		symnd2.h navstart.h register.h virlist.h avapi.h symalog.h \
		drvprblm.h stub.h nonav.h navw_hlp.h navwhlp.h cmbstr.h \
		nunavdlg.h nunavstb.h msgdisp.h AlignWnd.h
n_sysint.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h ngui.h \
		version.h quakever.h wndclass.h passwd.h stockbmp.h stdhelp.h \
		navutil.h ctsn.h callback.h options.h navopts.h exclude.h \
		cts4.h excludel.h netobj.h symnet.h symnd2.h navstart.h \
		register.h npt.h virlist.h avapi.h xapi.h symcfg.h symalog.h \
		drvprblm.h security.h file.h nunavdlg.h nunavstb.h cmbstr.h \
		navw_hlp.h navwhlp.h navalog.h ctl3d.h syminteg.h nonav.h \
		virscan.h virusinf.h pamapi.h datafile.h \
		cpu.h opcode.h profile.h page.h search.h sigex.h prepair.h \
		pamcache.h cover.h proto.h netalert.h navstat.h symstat.h \
		navver.h disk.h
n_inoc.obj:     platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h file.h \
		symcfg.h navutil.h ctsn.h callback.h options.h navopts.h \
		exclude.h cts4.h excludel.h netobj.h symnet.h symnd2.h \
		navstart.h register.h npt.h quakever.h version.h virlist.h \
		avapi.h symalog.h drvprblm.h stdhelp.h security.h nunavdlg.h \
		nunavstb.h cmbstr.h nonav.h syminteg.h virscan.h virusinf.h \
		pamapi.h datafile.h cpu.h opcode.h \
		profile.h page.h search.h sigex.h prepair.h pamcache.h \
		cover.h proto.h inoc.h netalert.h
n_virus.obj:    platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h \
		windowsx.h cmbstr.h symcfg.h navutil.h ctsn.h callback.h \
		options.h navopts.h exclude.h cts4.h excludel.h netobj.h \
		symnet.h symnd2.h navstart.h register.h npt.h quakever.h \
		version.h virlist.h avapi.h symalog.h drvprblm.h stdhelp.h \
		syminteg.h stub.h nonav.h nunavdlg.h nunavstb.h dbcs.h \
		CalcTabs.h
n_info.obj:     platform.h dbcs_str.h dosvmm.h nlm_str.h ngui.h version.h \
		quakever.h wndclass.h passwd.h stockbmp.h xapi.h file.h npt.h \
		hstring.h ctl3d.h syminteg.h navgraph.hpp stdhelp.h ctsn.h \
		callback.h symcfg.h navutil.h options.h navopts.h exclude.h \
		cts4.h excludel.h netobj.h symnet.h symnd2.h navstart.h \
		register.h virlist.h avapi.h symalog.h drvprblm.h navw_hlp.h \
		navwhlp.h cmbstr.h nunavdlg.h nunavstb.h stub.h virscan.h \
		virusinf.h pamapi.h datafile.h cpu.h \
		opcode.h profile.h page.h search.h sigex.h prepair.h \
		pamcache.h cover.h proto.h nonav.h
n_regfi.obj:    platform.h dbcs_str.h dosvmm.h nlm_str.h file.h ngui.h \
		version.h quakever.h wndclass.h passwd.h stockbmp.h ctl3d.h \
		syminteg.h cmbstr.h symcfg.h stdhelp.h navutil.h ctsn.h \
		callback.h options.h navopts.h exclude.h cts4.h excludel.h \
		netobj.h symnet.h symnd2.h navstart.h register.h npt.h \
		virlist.h avapi.h xapi.h symalog.h drvprblm.h virscan.h \
		virusinf.h pamapi.h datafile.h cpu.h \
		opcode.h profile.h page.h search.h sigex.h prepair.h \
		pamcache.h cover.h proto.h inoc.h security.h nonav.h \
		nunavdlg.h nunavstb.h navw_hlp.h navwhlp.h
n_ini.obj:      platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h file.h \
		symcfg.h navutil.h ctsn.h callback.h options.h navopts.h \
		exclude.h cts4.h excludel.h netobj.h symnet.h symnd2.h \
		navstart.h register.h npt.h quakever.h version.h virlist.h \
		avapi.h symalog.h drvprblm.h stdhelp.h syminteg.h stub.h \
		nonav.h nunavstb.h cmbstr.h
n_memscn.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h \
		windowsx.h ctl3d.h syminteg.h ngui.h version.h quakever.h \
		wndclass.h passwd.h stockbmp.h disk.h symcfg.h navutil.h \
		ctsn.h callback.h options.h navopts.h exclude.h cts4.h \
		excludel.h netobj.h symnet.h symnd2.h navstart.h register.h \
		npt.h virlist.h avapi.h symalog.h drvprblm.h stdhelp.h \
		navwork.h nunavdlg.h nunavstb.h stub.h virscan.h virusinf.h \
		pamapi.h file.h datafile.h cpu.h opcode.h \
		profile.h page.h search.h sigex.h prepair.h pamcache.h \
		cover.h proto.h nonav.h navstat.h symstat.h navver.h
n_inocfi.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h file.h \
		ngui.h version.h quakever.h wndclass.h passwd.h stockbmp.h \
		ctl3d.h syminteg.h cmbstr.h symcfg.h stdhelp.h navutil.h \
		ctsn.h callback.h options.h navopts.h exclude.h cts4.h \
		excludel.h netobj.h symnet.h symnd2.h navstart.h register.h \
		npt.h virlist.h avapi.h symalog.h drvprblm.h security.h \
		stub.h virscan.h virusinf.h pamapi.h \
		datafile.h cpu.h opcode.h profile.h page.h search.h sigex.h \
		prepair.h pamcache.h cover.h proto.h nonav.h nunavdlg.h \
		nunavstb.h navw_hlp.h navwhlp.h
n_scan.obj:     platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h file.h \
		disk.h symcfg.h symnet.h symnd2.h netobj.h navutil.h ctsn.h \
		callback.h options.h navopts.h exclude.h cts4.h excludel.h \
		navstart.h register.h npt.h quakever.h version.h virlist.h \
		avapi.h symalog.h drvprblm.h stdhelp.h nunavdlg.h nunavstb.h \
		cmbstr.h security.h navwork.h findall.hpp \
		syminteg.h stub.h nonav.h inoc.h virscan.h virusinf.h \
		pamapi.h datafile.h cpu.h opcode.h \
		profile.h page.h search.h sigex.h prepair.h pamcache.h \
		cover.h proto.h dbcs.h netalert.h navstat.h symstat.h \
		navver.h loadmgmt.h
n_wizard.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h disk.h \
		ngui.h version.h quakever.h wndclass.h passwd.h stockbmp.h \
		ctl3d.h syminteg.h stdhelp.h symcfg.h navutil.h ctsn.h \
		callback.h options.h navopts.h exclude.h cts4.h excludel.h \
		netobj.h symnet.h symnd2.h navstart.h register.h npt.h \
		virlist.h avapi.h symalog.h drvprblm.h navw_hlp.h navwhlp.h \
		cmbstr.h nunavdlg.h nunavstb.h security.h stub.h virscan.h \
		virusinf.h pamapi.h file.h datafile.h \
		cpu.h opcode.h profile.h page.h search.h sigex.h prepair.h \
		pamcache.h cover.h proto.h inoc.h nonav.h navver.h AlignWnd.h
n_scan2.obj:    platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h cmbstr.h \
		symcfg.h navwork.h navutil.h ctsn.h callback.h options.h \
		navopts.h exclude.h cts4.h excludel.h netobj.h symnet.h \
		symnd2.h navstart.h register.h npt.h quakever.h version.h \
		virlist.h avapi.h symalog.h drvprblm.h stdhelp.h nunavdlg.h \
		nunavstb.h syminteg.h stub.h virscan.h virusinf.h pamapi.h \
		file.h datafile.h cpu.h opcode.h profile.h \
		page.h search.h sigex.h prepair.h pamcache.h cover.h proto.h \
		nonav.h
n_scnstr.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h xapi.h disk.h \
		file.h ngui.h version.h quakever.h wndclass.h passwd.h \
		stockbmp.h symcfg.h symnet.h symnd2.h netobj.h navutil.h \
		ctsn.h callback.h options.h navopts.h exclude.h cts4.h \
		excludel.h navstart.h register.h npt.h virlist.h avapi.h \
		symalog.h drvprblm.h stdhelp.h cmbstr.h nunavdlg.h nunavstb.h \
		navwork.h navalog.h syminteg.h ctl3d.h stub.h \
		virscan.h virusinf.h pamapi.h datafile.h \
		cpu.h opcode.h profile.h page.h search.h sigex.h prepair.h \
		pamcache.h cover.h proto.h inoc.h nonav.h AlignWnd.h dbcs.h \
		navstat.h symstat.h navver.h
rwquar.obj:     stdafx.h nunavdlg.h nunavstb.h RWQuar.h picture.h \
		RwPage.h nonav.h options.h navopts.h exclude.h cts4.h ctsn.h \
		callback.h callbk15.h n30type.h avtypes.h platform.h \
		dbcs_str.h dosvmm.h nlm_str.h excludel.h netobj.h symnet.h \
		symnd2.h navstart.h quarset.h custom.h navutil.h npt.h \
		quakever.h version.h virlist.h avapi.h pamapi.h \
		file.h datafile.h cpu.h opcode.h profile.h page.h \
		search.h sigex.h prepair.h pamcache.h cover.h proto.h navcb.h \
		avcb.h xapi.h symcfg.h symalog.h drvprblm.h stdhelp.h \
		syminteg.h problist.h sortlist.h
#ENDUPDATE#

