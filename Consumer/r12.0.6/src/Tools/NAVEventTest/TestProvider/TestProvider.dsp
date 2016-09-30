# Microsoft Developer Studio Project File - Name="TestProvider" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TestProvider - Win32 Release MFC DLL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TestProvider.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TestProvider.mak" CFG="TestProvider - Win32 Release MFC DLL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TestProvider - Win32 Release MFC DLL" (based on "Win32 (x86) Application")
!MESSAGE "TestProvider - Win32 Debug MFC DLL" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "TestProvider"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TestProvider - Win32 Release MFC DLL"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "..\..\..\nobuilds\PlatSDK\Include" /I "..\..\..\nobuilds\msvc6sp5\VC98\ATL\Include" /I "..\..\..\nobuilds\msvc6sp5\VC98\Include" /I "..\..\..\SDKs\EventManager\Include" /I "..\..\..\SDKs\commonclient\Include" /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "..\..\..\include" /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 navevent.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\bin.ira\NAVProvider.exe" /libpath:"..\..\..\lib.ira" /libpath:"..\..\..\Nobuilds\msvc6sp5\VC98\Lib" /libpath:"..\..\..\Nobuilds\msvc6sp5\VC98\mfc\Lib" /libpath:"E:\NAV\Norton_AntiVirus\Consumer\SDKs\CommonClient\LIB\LIB.IRA"

!ELSEIF  "$(CFG)" == "TestProvider - Win32 Debug MFC DLL"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\..\..\nobuilds\PlatSDK\Include" /I "..\..\..\nobuilds\msvc6sp5\VC98\ATL\Include" /I "..\..\..\nobuilds\msvc6sp5\VC98\Include" /I "..\..\..\SDKs\EventManager\Include" /I "..\..\..\SDKs\commonclient\Include" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_ATL_DEBUG_INTERFACES" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "..\..\..\include" /D "_DEBUG" /Oicf /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 navevent.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\..\bin.ida\NAVProvider.exe" /pdbtype:sept /libpath:"..\..\..\lib.ida" /libpath:"..\..\..\Nobuilds\msvc6sp5\VC98\Lib" /libpath:"..\..\..\Nobuilds\msvc6sp5\VC98\mfc\Lib" /libpath:"E:\NAV\Norton_AntiVirus\Consumer\SDKs\CommonClient\LIB\LIB.IdA"

!ENDIF 

# Begin Target

# Name "TestProvider - Win32 Release MFC DLL"
# Name "TestProvider - Win32 Debug MFC DLL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /W1 /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TestProvider.cpp
# End Source File
# Begin Source File

SOURCE=.\TestProvider.rc
# End Source File
# Begin Source File

SOURCE=.\TestProviderDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TestProviderPro.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TestProvider.h
# End Source File
# Begin Source File

SOURCE=.\TestProviderDlg.h
# End Source File
# Begin Source File

SOURCE=.\TestProviderPro.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\TestProvider.ico
# End Source File
# Begin Source File

SOURCE=.\res\TestProvider.rc2
# End Source File
# End Group
# Begin Group "NAVEvents"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\include\AllNAVEvents.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\NAVEventCommonInt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\NAVEvents.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\NAVEvents.tlb
# End Source File
# Begin Source File

SOURCE=..\..\..\include\NAVEventUtils.h
# End Source File
# End Group
# Begin Group "EventManager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\SDKs\EventManager\Include\ccEventManagerEx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDKs\EventManager\Include\ccExceptionInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDKs\EventManager\Include\ccProviderEx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SDKs\EventManager\Include\ccSerializableEventImpl.h
# End Source File
# End Group
# End Target
# End Project
