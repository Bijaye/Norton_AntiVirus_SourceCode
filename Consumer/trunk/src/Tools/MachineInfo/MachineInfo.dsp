# Microsoft Developer Studio Project File - Name="MachineInfo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MachineInfo - Win32 Debug Static
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MachineInfo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MachineInfo.mak" CFG="MachineInfo - Win32 Debug Static"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MachineInfo - Win32 Debug Static" (based on "Win32 (x86) Application")
!MESSAGE "MachineInfo - Win32 Release Static" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "MachineInfo"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MachineInfo - Win32 Debug Static"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MachineInfo___Win32_Debug_Static"
# PROP BASE Intermediate_Dir "MachineInfo___Win32_Debug_Static"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MachineInfo___Win32_Debug_Static"
# PROP Intermediate_Dir "MachineInfo___Win32_Debug_Static"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "MachineInfo - Win32 Release Static"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MachineInfo___Win32_Release_Static"
# PROP BASE Intermediate_Dir "MachineInfo___Win32_Release_Static"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MachineInfo___Win32_Release_Static"
# PROP Intermediate_Dir "MachineInfo___Win32_Release_Static"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib /nologo /subsystem:windows /machine:I386

!ENDIF 

# Begin Target

# Name "MachineInfo - Win32 Debug Static"
# Name "MachineInfo - Win32 Release Static"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ActivityLogReport.cpp
# End Source File
# Begin Source File

SOURCE=.\AppLauncher.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugLogs.cpp
# End Source File
# Begin Source File

SOURCE=.\FileEnum.cpp
# End Source File
# Begin Source File

SOURCE=.\FileIO.cpp
# End Source File
# Begin Source File

SOURCE=.\LiveUpdateLogs.cpp
# End Source File
# Begin Source File

SOURCE=.\MachineInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\MachineInfo.rc
# End Source File
# Begin Source File

SOURCE=.\MachineInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MSSystemInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\NAVFileVersions.cpp
# End Source File
# Begin Source File

SOURCE=.\NAVOptionsReport.cpp
# End Source File
# Begin Source File

SOURCE=.\Report.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SymRegKeys.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ActivityLogReport.h
# End Source File
# Begin Source File

SOURCE=.\AppLauncher.h
# End Source File
# Begin Source File

SOURCE=.\DebugLogs.h
# End Source File
# Begin Source File

SOURCE=.\FileEnum.h
# End Source File
# Begin Source File

SOURCE=.\FileIO.h
# End Source File
# Begin Source File

SOURCE=.\LiveUpdateLogs.h
# End Source File
# Begin Source File

SOURCE=.\MachineInfo.h
# End Source File
# Begin Source File

SOURCE=.\MachineInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\MSSystemInfo.h
# End Source File
# Begin Source File

SOURCE=.\NAVFileVersions.h
# End Source File
# Begin Source File

SOURCE=.\NAVOptionsReport.h
# End Source File
# Begin Source File

SOURCE=.\Report.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SymRegKeys.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\MachineInfo.ico
# End Source File
# Begin Source File

SOURCE=.\res\MachineInfo.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
