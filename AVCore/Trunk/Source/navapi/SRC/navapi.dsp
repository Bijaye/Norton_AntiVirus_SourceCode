# Microsoft Developer Studio Project File - Name="navapi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=navapi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "navapi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "navapi.mak" CFG="navapi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "navapi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "navapi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "SHRIKE_NewAP"
# PROP Scc_LocalPath "..\..\..\TRUNK\SOURCE\NAVAPI\SRC"
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
# ADD CPP /nologo /MT /W3 /Zd /O2 /I "..\..\include\src" /I "..\..\..\..\core\source\include\src" /I "..\..\..\..\symevent\inc" /I "..\..\..\english\navapi\src" /I "..\..\..\..\core\source\symkrnl\src" /I "\\ussm-tools\tools\tools\win95ddk\inc32" /I "\\ussm-tools\tools\tools\win95ddk\block\inc" /D "SYM_WIN" /D "SYM_WIN32" /D "_NAVAPIDLL_" /D "QCUSTNOX" /D "_WINDLL" /D "QCUST" /D "_WINDOWS" /D "NDEBUG" /D Q_VER=\"P\" /D "WIN32" /D "_X86" /D "_M_X86" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "SYM_WIN" /d "SYM_WIN32" /d "SBCS_DLG" /d "_NAVAPIDLL_" /d "QCUSTNOX" /d "_WINDLL" /d "QCUST" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\..\bin.ira/navapi32.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib avenge.lib drvcomm.lib /nologo /dll /pdb:"..\..\bin.ira/navapi32.pdb" /map:"..\..\bin.ira/navapi32.map" /debug /machine:I386 /out:"..\..\bin.ira/navapi32.dll" /libpath:"..\..\lib.ira" /libpath:"..\..\..\..\core\source\lib.ira"
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
# ADD CPP /nologo /MTd /W3 /Gm /Zi /Od /I "source\symkrnl\src" /I "l:english\symkrnl\src" /I "..\..\include\src" /I "..\..\..\..\..\..\core\trunk\source\include\src" /I "..\..\..\..\symevent\inc" /I "..\..\..\english\navapi\src" /I "..\..\..\..\core\source\symkrnl\src" /I "r:\win95ddk\inc32" /I "r:\win95ddk\block\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SYM_WIN" /D "SYM_WIN32" /D "_NAVAPIDLL_" /D "QCUSTNOX" /D "SYM_DEBUG" /D "_WINDLL" /D "QCUST" /D Q_VER=\"P\" /D "_X86" /D "_M_X86" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "SYM_WIN" /d "SYM_WIN32" /d "SBCS_DLG" /d "_NAVAPIDLL_" /d "QCUSTNOX" /d "SYM_DEBUG" /d "_WINDLL" /d "QCUST" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\..\bin.ida/navapi32.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib drvcomm.lib avenge.lib drvcomm.lib /nologo /dll /pdb:"..\..\bin.ida/navapi.pdb" /debug /machine:I386 /out:"..\..\bin.ida/navapi32.dll" /pdbtype:sept /libpath:"..\..\lib.ida" /libpath:"..\..\..\..\core\source\lib.ida"
# SUBTRACT LINK32 /pdb:none /incremental:no /nodefaultlib

!ENDIF 

# Begin Target

# Name "navapi - Win32 Release"
# Name "navapi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\APENABLE.CPP
# End Source File
# Begin Source File

SOURCE=.\AVAPI.CPP
# End Source File
# Begin Source File

SOURCE=.\BADCHAR.CPP
# End Source File
# Begin Source File

SOURCE=.\BOOT.CPP
# End Source File
# Begin Source File

SOURCE=.\BUFFERIO.C
# End Source File
# Begin Source File

SOURCE=.\CACHE.CPP
# End Source File
# Begin Source File

SOURCE=.\CFG_BLOC.C
# End Source File
# Begin Source File

SOURCE=.\CFG_INI.C
# End Source File
# Begin Source File

SOURCE=.\CFG_REG.CPP
# End Source File
# Begin Source File

SOURCE=.\CFG_SWIT.C
# End Source File
# Begin Source File

SOURCE=.\CFG_TEXT.C
# End Source File
# Begin Source File

SOURCE=.\COVER.CPP
# End Source File
# Begin Source File

SOURCE=.\CPU.CPP
# End Source File
# Begin Source File

SOURCE=.\DATAFILE.CPP
# End Source File
# Begin Source File

SOURCE=.\DBCS.C
# End Source File
# Begin Source File

SOURCE=.\DSK_ABS.C
# End Source File
# Begin Source File

SOURCE=.\DSK_ACCS.C
# End Source File
# Begin Source File

SOURCE=.\DSK_BLOC.C
# End Source File
# Begin Source File

SOURCE=.\DSK_BPB.C
# End Source File
# Begin Source File

SOURCE=.\DSK_CACH.C
# End Source File
# Begin Source File

SOURCE=.\DSK_DRVR.C
# End Source File
# Begin Source File

SOURCE=.\DSK_GEN.C
# End Source File
# Begin Source File

SOURCE=.\DSK_INT.C
# End Source File
# Begin Source File

SOURCE=.\DSK_MISC.C
# End Source File
# Begin Source File

SOURCE=.\DSK_STAC.C
# End Source File
# Begin Source File

SOURCE=.\DSK_TYPE.C
# End Source File
# Begin Source File

SOURCE=.\EXCLUDE.CPP
# End Source File
# Begin Source File

SOURCE=.\EXEC.CPP
# End Source File
# Begin Source File

SOURCE=.\FILE.CPP
# End Source File
# Begin Source File

SOURCE=.\FILETIME.C
# End Source File
# Begin Source File

SOURCE=.\FINDFIRS.CPP
# End Source File
# Begin Source File

SOURCE=.\FLAGS.CPP
# End Source File
# Begin Source File

SOURCE=.\FOP_DIR.C
# End Source File
# Begin Source File

SOURCE=.\FOP_DIR2.C
# End Source File
# Begin Source File

SOURCE=.\FOP_DISK.C
# End Source File
# Begin Source File

SOURCE=.\FOP_DOS.C
# End Source File
# Begin Source File

SOURCE=.\FOP_FILE.C
# End Source File
# Begin Source File

SOURCE=.\FOP_WIN.C
# End Source File
# Begin Source File

SOURCE=.\HSTRING.C
# End Source File
# Begin Source File

SOURCE=.\HUGEOEM.C
# End Source File
# Begin Source File

SOURCE=.\INFO.CPP
# End Source File
# Begin Source File

SOURCE=.\INIT.CPP
# End Source File
# Begin Source File

SOURCE=.\INSTR.CPP
# End Source File
# Begin Source File

SOURCE=.\INSTR2.CPP
# End Source File
# Begin Source File

SOURCE=.\MEMORY.CPP
# End Source File
# Begin Source File

SOURCE=.\NAME.C
# End Source File
# Begin Source File

SOURCE=.\NAME_WIN.C
# End Source File
# Begin Source File

SOURCE=.\NAVCB.CPP
# End Source File
# Begin Source File

SOURCE=.\NEC.C
# End Source File
# Begin Source File

SOURCE=.\PAGE.CPP
# End Source File
# Begin Source File

SOURCE=.\PAMAPI.CPP
# End Source File
# Begin Source File

SOURCE=.\PRM_CRPT.C
# End Source File
# Begin Source File

SOURCE=.\PRM_DATE.C
# End Source File
# Begin Source File

SOURCE=.\PRM_DOS.C
# End Source File
# Begin Source File

SOURCE=.\PRM_HARD.C
# End Source File
# Begin Source File

SOURCE=.\PRM_MEM.C
# End Source File
# Begin Source File

SOURCE=.\PRM_MISC.C
# End Source File
# Begin Source File

SOURCE=.\PRM_STR.C
# End Source File
# Begin Source File

SOURCE=.\PRM_STRD.C
# End Source File
# Begin Source File

SOURCE=.\PRM_SZZ.C
# End Source File
# Begin Source File

SOURCE=.\PRM_WIN.C
# End Source File
# Begin Source File

SOURCE=.\REPAIR.CPP
# End Source File
# Begin Source File

SOURCE=.\SEARCH.CPP
# End Source File
# Begin Source File

SOURCE=.\SIGEX.CPP
# End Source File
# Begin Source File

SOURCE=.\STATIC.CPP
# End Source File
# Begin Source File

SOURCE=.\SYMASSER.CPP
# End Source File
# Begin Source File

SOURCE=.\SYMCFG.C
# End Source File
# Begin Source File

SOURCE=.\SYMINTL.C
# End Source File
# Begin Source File

SOURCE=.\SYMKRNL.C
# End Source File
# Begin Source File

SOURCE=.\VERSION.C
# End Source File
# Begin Source File

SOURCE=.\VIRTCB.CPP
# End Source File
# Begin Source File

SOURCE=.\VXDLOAD.C
# End Source File
# Begin Source File

SOURCE=.\WILDCARD.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\..\ENGLISH\NAVAPI\SRC\NAVAPI.RC
# End Source File
# End Group
# End Target
# End Project
