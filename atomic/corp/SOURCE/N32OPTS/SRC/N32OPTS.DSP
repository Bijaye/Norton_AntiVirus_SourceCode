# Microsoft Developer Studio Project File - Name="n32opts" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=n32opts - Win32 NTDEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "n32opts.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "n32opts.mak" CFG="n32opts - Win32 NTDEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "n32opts - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "n32opts - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "n32opts - Win32 NTDEBUG" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "n32opts - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "BUILDING_DLL" /D "SYM_WIN" /D "SYM_WIN32" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 n32exclu.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "n32opts - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "BUILDING_DLL" /D "SYM_WIN" /D "SYM_WIN32" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 n32exclu.lib s32navk.lib n32serve.lib n32pdll.lib netbrext.lib n32alert.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "n32opts - Win32 NTDEBUG"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "n32opts_"
# PROP BASE Intermediate_Dir "n32opts_"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ntdebug"
# PROP Intermediate_Dir "ntdebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "BUILDING_DLL" /D "SYM_WIN" /D "SYM_WIN32" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "BUILDING_DLL" /D "SYM_WIN" /D "SYM_WIN32" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 n32exclu.lib s32navk.lib n32serve.lib n32pdll.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 xntexclu.lib s32navk.lib xntserve.lib n32pdll.lib netbrext.lib n32alert.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "n32opts - Win32 Release"
# Name "n32opts - Win32 Debug"
# Name "n32opts - Win32 NTDEBUG"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AlertPge.cpp
# End Source File
# Begin Source File

SOURCE=.\APPage.cpp
# End Source File
# Begin Source File

SOURCE=.\cinclude.cpp
# End Source File
# Begin Source File

SOURCE=.\CustResp.cpp
# End Source File
# Begin Source File

SOURCE=.\ExclPage.cpp
# End Source File
# Begin Source File

SOURCE=.\exports.cpp
# End Source File
# Begin Source File

SOURCE=.\ExtList.cpp
# End Source File
# Begin Source File

SOURCE=.\GenPage.cpp
# End Source File
# Begin Source File

SOURCE=.\HELPDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\houndDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IconBtn.cpp
# End Source File
# Begin Source File

SOURCE=.\InocPage.cpp
# End Source File
# Begin Source File

SOURCE=.\LogPage.cpp
# End Source File
# Begin Source File

SOURCE=.\n32opts.cpp
# End Source File
# Begin Source File

SOURCE=.\n32opts.def
# End Source File
# Begin Source File

SOURCE=.\n32opts.rc

!IF  "$(CFG)" == "n32opts - Win32 Release"

!ELSEIF  "$(CFG)" == "n32opts - Win32 Debug"

!ELSEIF  "$(CFG)" == "n32opts - Win32 NTDEBUG"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PassPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Sheet.cpp
# End Source File
# Begin Source File

SOURCE=.\Startpge.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AlertPge.h
# End Source File
# Begin Source File

SOURCE=.\APPage.h
# End Source File
# Begin Source File

SOURCE=.\cinclude.h
# End Source File
# Begin Source File

SOURCE=.\CustResp.h
# End Source File
# Begin Source File

SOURCE=.\ExclPage.h
# End Source File
# Begin Source File

SOURCE=.\ExtList.h
# End Source File
# Begin Source File

SOURCE=.\GenPage.h
# End Source File
# Begin Source File

SOURCE=.\HELPDLG.H
# End Source File
# Begin Source File

SOURCE=.\houndDlg.h
# End Source File
# Begin Source File

SOURCE=.\IconBtn.h
# End Source File
# Begin Source File

SOURCE=.\InocPage.h
# End Source File
# Begin Source File

SOURCE=.\LogPage.h
# End Source File
# Begin Source File

SOURCE=.\n32opts.h
# End Source File
# Begin Source File

SOURCE=.\PassPage.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScanPage.h
# End Source File
# Begin Source File

SOURCE=.\Sheet.h
# End Source File
# Begin Source File

SOURCE=.\Startpge.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\BROWSE.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Browse.ico
# End Source File
# Begin Source File

SOURCE=.\NAVEXTEN.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Navexten.ico
# End Source File
# Begin Source File

SOURCE=.\NtAlertBrowse.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
