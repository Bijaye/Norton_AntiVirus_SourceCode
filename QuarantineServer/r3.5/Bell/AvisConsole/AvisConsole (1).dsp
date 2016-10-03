# Microsoft Developer Studio Project File - Name="AvisConsole" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AvisConsole - Win32 Unicode Release MinDependency
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AvisConsole.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AvisConsole.mak" CFG="AvisConsole - Win32 Unicode Release MinDependency"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AvisConsole - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AvisConsole - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "AvisConsole"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AvisConsole - Win32 Unicode Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\..\core_technology\LDVP_shared\src\include" /I "..\..\mmc12" /I "..\HelpFiles" /I "..\htmlhelp" /I "..\shared" /I "..\..\include" /I "..\..\shared\include" /I "..\..\..\..\Core\Source\include\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /D "UNICODE" /D "SYM_WIN" /D "SYM_WIN32" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\shared\include" /i "..\..\..\shared\include" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib htmlhelp.lib mmc.lib IcePackTokens.Lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCMT.LIB" /nodefaultlib:"LIBCMTD.LIB" /out:"..\..\shared\BinIntelUnicodeWin32Debug/AvisConsole.dll" /pdbtype:sept /libpath:"..\..\mmc12" /libpath:"..\htmlhelp" /libpath:"..\..\shared\LibIntelAnsiWin32Debug"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy executables
PostBuild_Cmds=copy  ..\HelpFiles\AvisConsole.chm   ..\..\Shared\BinIntelUniCodeWin32Debug
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AvisConsole - Win32 Unicode Release MinDependency"

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
# ADD CPP /nologo /MD /W3 /GX /O1 /I "..\..\..\..\core_technology\LDVP_shared\src\include" /I "..\..\mmc12" /I "..\HelpFiles" /I "..\htmlhelp" /I "..\shared" /I "..\..\include" /I "..\..\shared\include" /I "..\..\..\..\Core\Source\include\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /D "UNICODE" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\shared\include" /i "..\..\..\shared\include" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 wsock32.lib htmlhelp.lib mmc.lib IcePackTokens.Lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"LIBCMT.LIB" /nodefaultlib:"LIBCMTD.LIB" /out:"..\..\shared\BinIntelUnicodeWin32Release/AvisConsole.dll" /libpath:"..\..\mmc12" /libpath:"..\htmlhelp" /libpath:"..\..\shared\LibIntelAnsiWin32Release"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  ..\HelpFiles\AvisConsole.chm   ..\..\Shared\BinIntelUnicodeWin32Release
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AvisConsole - Win32 Unicode Debug"
# Name "AvisConsole - Win32 Unicode Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ACStateListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\AlertingSetConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\ams.cpp
# End Source File
# Begin Source File

SOURCE=.\AServerDefPolicyConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\AServerFirewallConfig2.cpp
# End Source File
# Begin Source File

SOURCE=.\AttributeAccess.cpp
# End Source File
# Begin Source File

SOURCE=.\AvisCon.cpp
# End Source File
# Begin Source File

SOURCE=.\AvisConExtData.cpp
# End Source File
# Begin Source File

SOURCE=.\AvisConsole.cpp

!IF  "$(CFG)" == "AvisConsole - Win32 Unicode Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "AvisConsole - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AvisConsole.def
# End Source File
# Begin Source File

SOURCE=.\AvisConsole.idl
# ADD MTL /tlb ".\AvisConsole.tlb" /h "AvisConsole.h" /iid "AvisConsole_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\AvisConsole.rc
# End Source File
# Begin Source File

SOURCE=.\AvisHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\AvisHelpTopics.cpp
# End Source File
# Begin Source File

SOURCE=.\AvisServerConfig1.cpp
# End Source File
# Begin Source File

SOURCE=.\AvisServerConfig2.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfirmationDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomerInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\FileNameParse.cpp
# End Source File
# Begin Source File

SOURCE=.\GetAllAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\HiddenWndMmcThread.cpp
# End Source File
# Begin Source File

SOURCE=.\htmlcall.cpp

!IF  "$(CFG)" == "AvisConsole - Win32 Unicode Debug"

# ADD CPP /GR- /Z7
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "AvisConsole - Win32 Unicode Release MinDependency"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IconButton.cpp
# End Source File
# Begin Source File

SOURCE=.\InstallDefinitions.cpp
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\ListNavServers.cpp
# End Source File
# Begin Source File

SOURCE=.\myListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\NscTop.idl
# End Source File
# Begin Source File

SOURCE=.\ParseEventData.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleActionsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleAttributesPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleErrorPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ScopeExtData.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectNavServersDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectTargetDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerConfigData.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerGeneralError.cpp
# End Source File
# Begin Source File

SOURCE=.\SmartEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SubmitOrDeliver.cpp
# End Source File
# Begin Source File

SOURCE=.\util.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\UtilPlus.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ACStateListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\AttributeAccess.h
# End Source File
# Begin Source File

SOURCE=.\AvisCon.h
# End Source File
# Begin Source File

SOURCE=.\AvisConExtData.h
# End Source File
# Begin Source File

SOURCE=.\AvisSampleFields.h
# End Source File
# Begin Source File

SOURCE=.\AvisServerConfig1.h
# End Source File
# Begin Source File

SOURCE=.\AvisServerConfig2.h
# End Source File
# Begin Source File

SOURCE=.\ConfirmationDialog.h
# End Source File
# Begin Source File

SOURCE=.\CustomerInfo.h
# End Source File
# Begin Source File

SOURCE=.\GetAllAttributes.h
# End Source File
# Begin Source File

SOURCE=.\htmlcall.h
# End Source File
# Begin Source File

SOURCE=.\InputRange.h
# End Source File
# Begin Source File

SOURCE=.\InstallDefinitions.h
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\Localize.h
# End Source File
# Begin Source File

SOURCE=.\MyListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SampleActionsPage.h
# End Source File
# Begin Source File

SOURCE=.\SampleAttributesPage.h
# End Source File
# Begin Source File

SOURCE=.\ScopeExtData.h
# End Source File
# Begin Source File

SOURCE=.\SelectTargetDialog.h
# End Source File
# Begin Source File

SOURCE=.\ServerConfigData.h
# End Source File
# Begin Source File

SOURCE=.\SmartEdit.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UtilPlus.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ascend.bmp
# End Source File
# Begin Source File

SOURCE=.\AvisCon.rgs
# End Source File
# Begin Source File

SOURCE=.\aviscon_.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\descend.bmp
# End Source File
# Begin Source File

SOURCE=.\Itemtool.bmp
# End Source File
# Begin Source File

SOURCE=.\none.bmp
# End Source File
# Begin Source File

SOURCE=.\State.bmp
# End Source File
# Begin Source File

SOURCE=.\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\toolbar2.bmp
# End Source File
# End Group
# End Target
# End Project
