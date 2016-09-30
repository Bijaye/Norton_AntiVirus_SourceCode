# Microsoft Developer Studio Project File - Name="qscon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=QSCON - WIN32 UNICODE DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qscon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qscon.mak" CFG="QSCON - WIN32 UNICODE DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qscon - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "qscon - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "qscon"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qscon - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU"
# PROP BASE Intermediate_Dir "DebugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\nobuilds\NavAPI\Include" /I "..\..\..\Release\Defutils\include" /I "..\..\AVCore\source\include\src" /I "..\..\..\Core\Source\include\src" /I "..\shared\include" /I "..\Nobuilds\NavAPI\Include" /I "..\mmc11" /I "..\include" /I "..\..\..\AVCore\source\include\src" /I "..\..\..\..\Core\Source\include\src" /I "..\helpfiles" /I "..\htmlhelp" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /D "SYM_WIN" /D "SYM_WIN32" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /I "..\mmc11"
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Shared\Include" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 navapi32.lib defutils.lib ws2_32.lib mmc.lib htmlhelp.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBCMTD.LIB" /out:"..\shared\BinIntelUnicodeWin32Debug/qscon.dll" /pdbtype:sept /libpath:"..\Nobuilds\NAVAPI\LibIntelAnsiWin32Debug" /libpath:"..\..\..\release\defutils\lib.ida" /libpath:"..\mmc11" /libpath:"..\htmlhelp"

!ELSEIF  "$(CFG)" == "qscon - Win32 Unicode Release MinDependency"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinDependency"
# PROP BASE Intermediate_Dir "ReleaseUMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinDependency"
# PROP Intermediate_Dir "ReleaseUMinDependency"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "..\nobuilds\NavAPI\Include" /I "..\..\..\Release\Defutils\include" /I "..\..\AVCore\source\include\src" /I "..\..\..\Core\Source\include\src" /I "..\shared\include" /I "..\Nobuilds\NavAPI\Include" /I "..\mmc11" /I "..\include" /I "..\..\..\AVCore\source\include\src" /I "..\..\..\..\Core\Source\include\src" /I "..\helpfiles" /I "..\htmlhelp" /D "NDEBUG" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /D "SYM_WIN" /D "SYM_WIN32" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /I "..\mmc11"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Shared\Include" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 navapi32.lib defutils.lib ws2_32.lib mmc.lib htmlhelp.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"LIBCMT.LIB" /out:"..\shared\BinIntelUnicodeWin32Release/qscon.dll" /libpath:"..\Nobuilds\NAVAPI\LibIntelAnsiWin32Release" /libpath:"..\..\..\release\defutils\lib.ira" /libpath:"..\mmc11" /libpath:"..\htmlhelp"

!ENDIF 

# Begin Target

# Name "qscon - Win32 Unicode Debug"
# Name "qscon - Win32 Unicode Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AVScanner.cpp
# End Source File
# Begin Source File

SOURCE=.\ColumnInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\dataobj.cpp
# End Source File
# Begin Source File

SOURCE=.\extensions.cpp
# End Source File
# Begin Source File

SOURCE=.\FetchFileData.cpp
# End Source File
# Begin Source File

SOURCE=.\FetchFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FileNameParse.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralItemPage.cpp
# End Source File
# Begin Source File

SOURCE=.\IconButton.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageButton.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemData.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiSelDataObject.cpp
# End Source File
# Begin Source File

SOURCE=.\NavAPICallbacks.cpp
# End Source File
# Begin Source File

SOURCE=.\NumberString.cpp
# End Source File
# Begin Source File

SOURCE=.\qcresult.cpp
# End Source File
# Begin Source File

SOURCE=.\QPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\qscon.cpp
# End Source File
# Begin Source File

SOURCE=.\qscon.def
# End Source File
# Begin Source File

SOURCE=.\qscon.idl
# ADD MTL /tlb ".\qscon.tlb" /h "qscon.h" /iid "qscon_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\qscon.rc
# End Source File
# Begin Source File

SOURCE=.\QSConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\QSConsoleComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\QSConsoleData.cpp
# End Source File
# Begin Source File

SOURCE=.\RANGESTR.CPP
# End Source File
# Begin Source File

SOURCE=.\RDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\SndItem.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\UserItemPage.cpp
# End Source File
# Begin Source File

SOURCE=.\VirusPage.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AVScanner.h
# End Source File
# Begin Source File

SOURCE=.\ColumnInfo.h
# End Source File
# Begin Source File

SOURCE=.\dataobj.h
# End Source File
# Begin Source File

SOURCE=.\extend.h
# End Source File
# Begin Source File

SOURCE=.\FetchFileData.h
# End Source File
# Begin Source File

SOURCE=.\FetchFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\FileNameParse.h
# End Source File
# Begin Source File

SOURCE=.\GeneralItemPage.h
# End Source File
# Begin Source File

SOURCE=.\IconButton.h
# End Source File
# Begin Source File

SOURCE=.\ImageButton.h
# End Source File
# Begin Source File

SOURCE=.\ItemData.h
# End Source File
# Begin Source File

SOURCE=.\macros.h
# End Source File
# Begin Source File

SOURCE=.\MultiSelDataObject.h
# End Source File
# Begin Source File

SOURCE=.\NavAPICallbacks.h
# End Source File
# Begin Source File

SOURCE=.\NumberString.h
# End Source File
# Begin Source File

SOURCE=.\qcresult.h
# End Source File
# Begin Source File

SOURCE=.\QPropPage.h
# End Source File
# Begin Source File

SOURCE=.\QSConsole.h
# End Source File
# Begin Source File

SOURCE=.\QSConsoleComponent.h
# End Source File
# Begin Source File

SOURCE=.\QSConsoleData.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Include\qsfields.h
# End Source File
# Begin Source File

SOURCE=.\RANGESTR.H
# End Source File
# Begin Source File

SOURCE=.\RDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ServerDlg.h
# End Source File
# Begin Source File

SOURCE=.\ServerPage.h
# End Source File
# Begin Source File

SOURCE=.\ServerWizard.h
# End Source File
# Begin Source File

SOURCE=.\SndItem.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UserItemPage.h
# End Source File
# Begin Source File

SOURCE=.\VirusPage.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\itemtool.bmp
# End Source File
# Begin Source File

SOURCE=.\main_ico.ico
# End Source File
# Begin Source File

SOURCE=.\qsconsol.bmp
# End Source File
# Begin Source File

SOURCE=.\QSConsole.rgs
# End Source File
# Begin Source File

SOURCE=.\repair.ico
# End Source File
# Begin Source File

SOURCE=.\restore.ico
# End Source File
# Begin Source File

SOURCE=.\scope_to.bmp
# End Source File
# Begin Source File

SOURCE=.\snapinl.bmp
# End Source File
# Begin Source File

SOURCE=.\snapins.bmp
# End Source File
# Begin Source File

SOURCE=.\viruschars.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\AVCore\Source\Include\Src\AVTYPES.H
# End Source File
# End Target
# End Project
