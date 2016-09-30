# Microsoft Developer Studio Project File - Name="TestSubscriber" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TestSubscriber - Win32 Release MFC DLL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TestSubscriber.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TestSubscriber.mak" CFG="TestSubscriber - Win32 Release MFC DLL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TestSubscriber - Win32 Release MFC DLL" (based on "Win32 (x86) Application")
!MESSAGE "TestSubscriber - Win32 Debug MFC DLL" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "TestSubscriber"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TestSubscriber - Win32 Release MFC DLL"

# PROP BASE Use_MFC 6
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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\\" /I "..\TestEvent45Factory" /I "..\..\..\nobuilds\PlatSDK\Include" /I "..\..\..\nobuilds\msvc6sp5\VC98\ATL\Include" /I "..\..\..\include" /I "..\..\..\nobuilds\msvc6sp5\VC98\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "..\..\nobuilds\eventmanager" /I "..\..\engine\navevents" /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 navevent.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\..\bin.ira\NAVSubscriber.exe" /libpath:"..\..\..\lib.ira" /libpath:"..\..\..\Nobuilds\msvc6sp5\VC98\Lib" /libpath:"..\..\..\Nobuilds\msvc6sp5\VC98\mfc\Lib"

!ELSEIF  "$(CFG)" == "TestSubscriber - Win32 Debug MFC DLL"

# PROP BASE Use_MFC 6
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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "..\..\..\nobuilds\PlatSDK\Include" /I "..\..\..\nobuilds\msvc6sp5\VC98\ATL\Include" /I "..\..\..\include" /I "..\..\..\nobuilds\msvc6sp5\VC98\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "..\..\nobuilds\eventmanager" /I "..\..\engine\navevents" /D "_DEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 navevent.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\..\bin.ida\NAVSubscriber.exe" /pdbtype:sept /libpath:"..\..\..\lib.ida" /libpath:"..\..\..\Nobuilds\msvc6sp5\VC98\Lib" /libpath:"..\..\..\Nobuilds\msvc6sp5\VC98\mfc\Lib"

!ENDIF 

# Begin Target

# Name "TestSubscriber - Win32 Release MFC DLL"
# Name "TestSubscriber - Win32 Debug MFC DLL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "TestSubscriber - Win32 Release MFC DLL"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "TestSubscriber - Win32 Debug MFC DLL"

# ADD CPP /W1 /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TestSubscriber.cpp
# End Source File
# Begin Source File

SOURCE=.\TestSubscriber.rc
# End Source File
# Begin Source File

SOURCE=.\TestSubscriberDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TestSubscriberSub.cpp
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

SOURCE=.\TestSubscriber.h
# End Source File
# Begin Source File

SOURCE=.\TestSubscriberDlg.h
# End Source File
# Begin Source File

SOURCE=.\TestSubscriberSub.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\TestSubscriber.ico
# End Source File
# Begin Source File

SOURCE=.\res\TestSubscriber.rc2
# End Source File
# End Group
# Begin Group "EventManager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\include\ccSubscriberEx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\SymInterface.h
# End Source File
# End Group
# End Target
# End Project
