# Microsoft Developer Studio Project File - Name="navapi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=navapi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NAVAPI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NAVAPI.mak" CFG="navapi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "navapi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "navapi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "NAVAPI"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "navapi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NAVAPI_EXPORTS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /Zd /O2 /I "symkrnl" /I "english\symkrnl" /I "inc.avcore" /I "inc.core" /I "inc.shared" /I "inc.win95ddk\inc32" /I "inc.win95ddk\block" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SYM_WIN" /D "SYM_WIN32" /D "_NAVAPIDLL_" /D "QCUSTNOX" /D "_WINDLL" /D "QCUST" /D Q_VER=\"P\" /D "_X86" /D "_M_X86" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "SYM_WIN" /d "SYM_WIN32" /d "SBCS_DLG" /d "_NAVAPIDLL_" /d "QCUSTNOX" /d "_WINDLL" /d "QCUST" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib drvcomm.lib avenge.lib /nologo /dll /map /machine:I386 /out:"Release\navapi32.dll" /libpath:"Release"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "navapi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NAVAPI_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /Zi /Od /I "symkrnl" /I "english\symkrnl" /I "inc.avcore" /I "inc.core" /I "inc.shared" /I "inc.win95ddk\inc32" /I "inc.win95ddk\block" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SYM_WIN" /D "SYM_WIN32" /D "_NAVAPIDLL_" /D "QCUSTNOX" /D "SYM_DEBUG" /D "_WINDLL" /D "QCUST" /D Q_VER=\"P\" /D "_X86" /D "_M_X86" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "SYM_WIN" /d "SYM_WIN32" /d "SBCS_DLG" /d "_NAVAPIDLL_" /d "QCUSTNOX" /d "SYM_DEBUG" /d "_WINDLL" /d "QCUST" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib drvcomm.lib avenge.lib /nologo /dll /debug /machine:I386 /out:"Debug/navapi32.dll" /pdbtype:sept /libpath:"Debug"
# SUBTRACT LINK32 /pdb:none /incremental:no /nodefaultlib

!ENDIF 

# Begin Target

# Name "navapi - Win32 Release"
# Name "navapi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\navapi\APENABLE.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\AVAPI.CPP
# End Source File
# Begin Source File

SOURCE=.\symkrnl\BADCHAR.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\BOOT.CPP
# End Source File
# Begin Source File

SOURCE=.\symkrnl\BUFFERIO.C
# End Source File
# Begin Source File

SOURCE=.\navapi\CACHE.CPP
# End Source File
# Begin Source File

SOURCE=.\symkrnl\CFG_BLOC.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\CFG_INI.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\CFG_REG.CPP
# End Source File
# Begin Source File

SOURCE=.\symkrnl\CFG_SWIT.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\CFG_TEXT.C
# End Source File
# Begin Source File

SOURCE=.\navapi\COVER.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\CPU.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\DATAFILE.CPP
# End Source File
# Begin Source File

SOURCE=.\symkrnl\DBCS.C
# End Source File
# Begin Source File

SOURCE=.\navapi\DSK_ABS.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\DSK_ACCS.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\DSK_BLOC.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\DSK_BPB.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\DSK_CACH.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\DSK_DRVR.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\DSK_GEN.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\DSK_INT.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\DSK_MISC.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\DSK_STAC.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\DSK_TYPE.C
# End Source File
# Begin Source File

SOURCE=.\navapi\EXCLUDE.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\EXEC.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\FILE.CPP
# End Source File
# Begin Source File

SOURCE=.\symkrnl\FILETIME.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\FINDFIRS.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\FLAGS.CPP
# End Source File
# Begin Source File

SOURCE=.\symkrnl\FOP_DIR.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\FOP_DIR2.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\FOP_DISK.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\FOP_DOS.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\FOP_FILE.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\FOP_WIN.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\HSTRING.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\HUGEOEM.C
# End Source File
# Begin Source File

SOURCE=.\navapi\INFO.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\INIT.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\INSTR.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\INSTR2.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\MEMORY.CPP
# End Source File
# Begin Source File

SOURCE=.\symkrnl\NAME.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\NAME_WIN.C
# End Source File
# Begin Source File

SOURCE=.\navapi\NAVCB.CPP
# End Source File
# Begin Source File

SOURCE=.\symkrnl\NEC.C
# End Source File
# Begin Source File

SOURCE=.\navapi\PAGE.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\PAMAPI.CPP
# End Source File
# Begin Source File

SOURCE=.\symkrnl\PRM_CRPT.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\PRM_DATE.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\PRM_DOS.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\PRM_HARD.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\PRM_MEM.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\PRM_MISC.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\PRM_STR.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\PRM_STRD.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\PRM_SZZ.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\PRM_WIN.C
# End Source File
# Begin Source File

SOURCE=.\navapi\REPAIR.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\SEARCH.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\SIGEX.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\STATIC.CPP
# End Source File
# Begin Source File

SOURCE=.\symkrnl\SYMASSER.CPP
# End Source File
# Begin Source File

SOURCE=.\symkrnl\SYMCFG.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\SYMINTL.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\SYMKRNL.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\VERSION.C
# End Source File
# Begin Source File

SOURCE=.\navapi\VIRTCB.CPP
# End Source File
# Begin Source File

SOURCE=.\navapi\VXDLOAD.C
# End Source File
# Begin Source File

SOURCE=.\symkrnl\WILDCARD.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\inc.core\_TSR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\ACTIONS.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\ADVISE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\AFXEXENW.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\AFXHACK.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\APCFG.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\APCOMM.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\APNTKDrv.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\APQUAR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\APSCLI.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\APSCOMM.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\apw2kddk.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\ASFCACHE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVAPI.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVAPI_R1.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVCB.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVDATFIL.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVENDIAN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVENGE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVEX.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\avexcl.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\avexcll.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVOBJECT.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\AVTYPES.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\BASEC.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\BROWSDLG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\BROWSE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CALCTABS.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CALLBACK.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CALLBK15.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CAPTURE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CDISK.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CFG_IDS.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CHECKSUM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CINDEXWK.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CNDX.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CNDXDLL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CNDXMAC.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CNDXMSC.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CNDXOS2.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CODEPAGE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\COPYFILE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\COUNTRY.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\COVER.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CPL_RES.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CPU.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CTASK.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CTIFIL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CTL3D.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CTRLPANL.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CTS4.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CTS4.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\CTSN.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\CTSN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\DATAFILE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DATAOBJ.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DBASE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DBCS.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DBCS_STR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DBF.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DBFGLOB.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\DBPROTO.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\DefAnnuity.h
# End Source File
# Begin Source File

SOURCE=.\inc.core\DELITEM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DESCRIPT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DESKHELP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DEVNODE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DIRTREE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DISK.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DLL.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\DLLRCSTA.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DOMAIN.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DOMDEL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DOMDOS.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DOMKERNL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DOMMEM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DOSITEM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DOSLINK.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DOSNET.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DOSSRV.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DOSVMM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DRAGDROP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DRAWMARK.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DRIVEWND.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DRVCOMM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DRVMAPI.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DRVSEG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\DTLB.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\EDITOR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\EDITPAGE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\EDTPGAPI.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\EMSG.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\ENUMFILE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\EXCLUDE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\EXE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\EXEVIEW.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\FILE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\FILEENUM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\FILELIST.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\FILEMON.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\FILEOP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\FILETYPE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\FILLBAR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\FLB.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\FNAME.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\FORMAT.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\FPROT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\FSBMP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\FSCTLDRG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\FSCUR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\GALPOLY.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\GETSRVR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\GROUPT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\HARDWARE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\HASHDB.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\HCTXMENU.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\HDHLP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\HELP_DLG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\HELP_ED.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\HELP_NV.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\HELP_RD.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\HELP_SCH.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\HELP_SM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\HISTORY.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\HSTRING.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\HUGEOEM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\IBMPC.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\ICOLPAGE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\IDEID.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\IDENT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\IMGSAVE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\INDEX.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\INF.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\INIFILE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\INOCAPI.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\INSTRING.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\IPAGEINF.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\IPFILTER.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\IQUARAN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\iscandeliver.h
# End Source File
# Begin Source File

SOURCE=.\inc.core\ITEM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\KANJI.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\LARGEIO.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\LASTRUN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LASTSCAN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LINKCBK.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LINKCLI.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\LINKCLI2.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LINKSRV.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\LINKSRV2.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LINKVAL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\LOADER.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LOG.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LOGIO.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\LOGIO.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LUISHTML.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LUISSERV.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LUISSTR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\LUISTEXT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\MEMITEM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\MENUITEM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\MMCBASE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\MODVER.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\MSGDISP.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\N30TYPE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NAME.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\namespace.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\napiver.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVAP.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVAPEL.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVBIN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVBNHDR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVBTMAP.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVCB.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVDEFS.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVDOABT.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVEX.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVEX15.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NavLuUtils.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVNLMEX.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\navntutl.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVOPTS.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVOR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVRPC.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVSDK.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVSDKR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVWHLP.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NAVWHLP5.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NBLOB.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NBLOBQ.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NCDTREE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NCFRAME.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NCSERIES.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NDBTEST.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NDD.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NDRVLIST.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NDWNET.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NETBRWSD.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NETDIR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NETERR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NETINTR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NETSPEC.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NETTREE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NGUI.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NLIBCFG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NLL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NLM_STR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NLOADER.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NLOOK.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NMAIL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NMEMORY.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NNOVELL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NODE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NODEVIEW.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NPS.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NPS_HLP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NPS_HLP.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NPSAPI.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NPSSVC.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NPT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NREGINFO.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NRESCONT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NSDU_I.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NSPART.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\NSTDHELP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NSTRING.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NTRACE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\NWAITCUR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\OBQUERY.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\OPCODE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\OPTIONS.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\OS2DISK.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\OS2WIN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\OSSOCKET.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\OTHUNK.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\OVLMAN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PA.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PACKET.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PAGE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PAGE_MEM.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PAMAPI.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PAMCACHE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PANE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PANEDLG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PANEDSP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PANEMGR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PANEMGR.INL
# End Source File
# Begin Source File

SOURCE=.\inc.core\PANETRAY.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PANEUWM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PANEWCLS.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PASSWD.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\platform.h
# End Source File
# Begin Source File

SOURCE=.\inc.core\PLATMAC.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PLATNLM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PLATNTK.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PLATVXD.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PLATWIN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PREPAIR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PROFILE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PROFILER.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PROGMAN.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\PROGSCAN.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\PROTO.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\QATOOL.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\QCONRES.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\QUAKECVT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\QUAKEVER.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\QUAR32.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\QUARADD.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\QUARSET.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\READSIG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\recorditem.h
# End Source File
# Begin Source File

SOURCE=.\inc.core\recorditemgroup.h
# End Source File
# Begin Source File

SOURCE=.\inc.core\REFRESH.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\REGISTER.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\REMOTE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\REPAIR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\REPAIR.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\REPAIRN.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\RESCUEW.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\ResqLoc.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\RPCPROT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\RPCPROT2.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\RWHLP.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\s32rasu.h
# End Source File
# Begin Source File

SOURCE=.\inc.core\SALVAGEI.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\scandres.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SCANINST.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SCANINST.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SCANMEM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SCHEDULE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SCOPRES.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\sdfilter.h
# End Source File
# Begin Source File

SOURCE=.\inc.core\SDL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SDLFIELD.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\sdpack.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\sdrcinit.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\sdsendme.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\sdsocket.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\sdstrip.h
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SEARCH.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\secdesc.h
# End Source File
# Begin Source File

SOURCE=.\inc.core\SERVER.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SERVICE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SHEXCTRL.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SIGEX.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SIHLP_IN.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SIRGINFO.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SIUNIV33.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SIUNIV34.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SIUNIV40.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SIUNIVSL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SIWLFN.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SIWZIP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SIZEINFO.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SMART.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SMTPMAIL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SMTWATCH.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SOUND.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STACKER.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STACKERW.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STDDIR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STDDLG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STDDOS.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STDHELP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STDLIST.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STDOS2.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STDPRINT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STDWIN.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STOCKBM2.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STOCKBMP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\STRING32.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SUBCEDIT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SUBMEM.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SUPPORT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SVCCTRL.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SYMALOG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMALOG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMARRAY.HPP
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMASSRT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMCFG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMCHART.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMCHT32.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMCRC.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMDB.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMDBG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMEVAL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMFMT.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SYMINTEG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMINTEG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMINTL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMMDB.HPP
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMMENU.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMND.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMND2.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMNDS.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMNET.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMNEW.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMNSO.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMNW.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMNW4.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMNWS.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMPIPE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMQAW.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMSCHM.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SYMSERVE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SYMSYNC.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMSYNC.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMSYS.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMSYSID.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMTIME.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\SYMTLA.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMVCR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMVID.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMVXD.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYMZIP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYSCALL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\SYSFILE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TESTHELP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TESTTOOL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\THUNK.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TIMELEN.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TIMER.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TIMERCBK.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TIMERCLI.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TIMERSRV.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TLHELP16.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TREASURE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TREEABS.HPP
# End Source File
# Begin Source File

SOURCE=.\inc.core\TREECTRL.HPP
# End Source File
# Begin Source File

SOURCE=.\inc.core\TREEHAND.HPP
# End Source File
# Begin Source File

SOURCE=.\inc.core\TREENODE.HPP
# End Source File
# Begin Source File

SOURCE=.\inc.core\TREEOBJ.HPP
# End Source File
# Begin Source File

SOURCE=.\inc.core\TREERSID.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TROUBLE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TSKSWTCH.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TSR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\TSRINST.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\UNASM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\UNDOC.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\UNIUTIL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\UNIVERSL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\UTIL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\UTILDB.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VBIO.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VDISKREC.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VERINFO.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VERSION.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VHEAP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VIDLIB.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VIEWER.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\VIRINFDE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VIRINFDE.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\VIRLIST.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\VIRSCAN.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VIRSCAN.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VIRUSDBF.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\VIRUSINF.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VIRUSINF.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\VLIBVER.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VLIST.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VMMINT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VSYMKRNL.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VWR.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VWRRES.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VWRSDB.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\VXDLOAD.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\W32SUT.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\WERR32.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\WILDCARD.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\WINDVER.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\WININI.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\WINNET.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\WLCOMMS.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\WNDCLASS.H
# End Source File
# Begin Source File

SOURCE=.\inc.avcore\WNDSIZE.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\WRSCDIG.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\WRSCHELP.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\WX32.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\XAPI.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\XTRADOM.H
# End Source File
# Begin Source File

SOURCE=.\inc.core\ZIP.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\inc.avcore\NAVAPI.RC
# End Source File
# End Group
# End Target
# End Project
