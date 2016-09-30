# Microsoft Developer Studio Project File - Name="NAVNTUTL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NAVNTUTL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Navntutl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Navntutl.mak" CFG="NAVNTUTL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NAVNTUTL - Win32 IntelAnsiDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NAVNTUTL - Win32 IntelAnsiRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NAVNTUTL - Win32 AlphaAnsiDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NAVNTUTL - Win32 AlphaAnsiRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NAVNTUTL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NAVNTUTL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Atomic/NAVNTUtilityDLL", DEIAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NAVNTUTL - Win32 IntelAnsiDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "NAVNTUTL"
# PROP BASE Intermediate_Dir "NAVNTUTL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "IntelAnsiDebug"
# PROP Intermediate_Dir "IntelAnsiDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MTd /W3 /WX /ZI /Od /I "..\..\Include\src" /I "r:\ntsrcinc\public\INC" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D "_WINDOWS" /D "BUILDING_NAVNUTIL" /D "_DEBUG" /D Q_VER=\"N\" /D "WIN32" /D "_X86_" /D "_M_X86" /J /FD /Gys /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ntdll.lib libcmt.lib libcimt.lib oldnames.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib advapi32.lib winspool.lib /nologo /dll /incremental:no /map /debug /machine:I386 /nodefaultlib /pdbtype:sept /libpath:"r:\nt40addk\ddk\lib\i386\checked" /subsystem:windows,4.0
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "NAVNTUTL - Win32 IntelAnsiRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NAVNTUT0"
# PROP BASE Intermediate_Dir "NAVNTUT0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "IntelAnsiRelease"
# PROP Intermediate_Dir "IntelAnsiRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /WX /O1 /I "..\..\Include\src" /I "r:\ntsrcinc\public\INC" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D "_WINDOWS" /D "BUILDING_NAVNUTIL" /D "NDEBUG" /D Q_VER=\"N\" /D "WIN32" /D "_X86_" /D "_M_X86" /J /FD /Gys /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ntdll.lib libcmt.lib libcimt.lib oldnames.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib advapi32.lib winspool.lib /nologo /dll /map /machine:I386 /nodefaultlib /libpath:"r:\nt40addk\ddk\lib\i386\free" /subsystem:windows,4.0
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "NAVNTUTL - Win32 AlphaAnsiDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "NAVNTUT1"
# PROP BASE Intermediate_Dir "NAVNTUT1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "AlphaAnsiDebug"
# PROP Intermediate_Dir "AlphaAnsiDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /WX /ZI /Od /I "..\..\Include\src" /I "r:\ntsrcinc\public\INC" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D "_WINDOWS" /D "BUILDING_NAVNUTIL" /D "_DEBUG" /D Q_VER=\"N\" /D "WIN32" /D "_ALPHA_" /D "_M_ALPHA" /J /FD /Gys /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ntdll.lib libcmt.lib libcimt.lib oldnames.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib advapi32.lib winspool.lib /nologo /dll /incremental:no /map /debug /machine:ALPHA /nodefaultlib /pdbtype:sept /libpath:"r:\nt40addk\ddk\lib\alpha\checked" /subsystem:windows,4.0
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy AlphaAnsiDebug\NAVNTUTL.dll    ..\Shared\Bin\AlphaAnsi\WinNT\Debug	copy AlphaAnsiDebug\NAVNTUTL.lib    ..\Shared\Lib\AlphaAnsi\WinNT\Debug
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NAVNTUTL - Win32 AlphaAnsiRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NAVNTUT2"
# PROP BASE Intermediate_Dir "NAVNTUT2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "AlphaAnsiRelease"
# PROP Intermediate_Dir "AlphaAnsiRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /WX /O1 /I "..\..\Include\src" /I "r:\ntsrcinc\public\INC" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D "_WINDOWS" /D "BUILDING_NAVNUTIL" /D "NDEBUG" /D Q_VER=\"N\" /D "WIN32" /D "_ALPHA_" /D "_M_ALPHA" /J /FD /Gys /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ntdll.lib libcmt.lib libcimt.lib oldnames.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib advapi32.lib winspool.lib /nologo /dll /map /machine:ALPHA /nodefaultlib /libpath:"r:\nt40addk\ddk\lib\alpha\free" /subsystem:windows,4.0
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy AlphaAnsiRelease\NAVNTUTL.dll    ..\Shared\Bin\AlphaAnsi\WinNT\Release	copy AlphaAnsiRelease\NAVNTUTL.lib    ..\Shared\Lib\AlphaAnsi\WinNT\Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NAVNTUTL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NAVNTUTL___Win32_Release"
# PROP BASE Intermediate_Dir "NAVNTUTL___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MT /W3 /WX /O1 /I "..\Shared\Include" /I "r:\ntsrcinc\public\INC" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D "_WINDOWS" /D "BUILDING_NAVNUTIL" /D "NDEBUG" /D Q_VER=\"N\" /D "WIN32" /D "_X86_" /D "_M_X86" /J /FD /Gys /c
# ADD CPP /nologo /Zp1 /MT /W3 /WX /O1 /I "..\..\Include\src" /I "r:\ntsrcinc\public\INC" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D "_WINDOWS" /D "BUILDING_NAVNUTIL" /D "NDEBUG" /D Q_VER=\"N\" /D "WIN32" /D "_X86_" /D "_M_X86" /J /FD /Gys /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ntdll.lib libcmt.lib libcimt.lib oldnames.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib advapi32.lib winspool.lib /nologo /dll /map /machine:I386 /nodefaultlib /libpath:"r:\nt40addk\ddk\lib\i386\free" /subsystem:windows,4.0
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ntdll.lib libcmt.lib libcimt.lib oldnames.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib advapi32.lib winspool.lib /nologo /dll /map /machine:I386 /nodefaultlib /libpath:"r:\nt40addk\ddk\lib\i386\free" /subsystem:windows,4.0
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "NAVNTUTL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "NAVNTUTL___Win32_Debug"
# PROP BASE Intermediate_Dir "NAVNTUTL___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MTd /W3 /WX /ZI /Od /I "..\Shared\Include" /I "r:\ntsrcinc\public\INC" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D "_WINDOWS" /D "BUILDING_NAVNUTIL" /D "_DEBUG" /D Q_VER=\"N\" /D "WIN32" /D "_X86_" /D "_M_X86" /J /FD /Gys /c
# ADD CPP /nologo /Zp1 /MTd /W3 /WX /ZI /Od /I "..\..\Include\src" /I "r:\ntsrcinc\public\INC" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D "_WINDOWS" /D "BUILDING_NAVNUTIL" /D "_DEBUG" /D Q_VER=\"N\" /D "WIN32" /D "_X86_" /D "_M_X86" /J /FD /Gys /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ntdll.lib libcmt.lib libcimt.lib oldnames.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib advapi32.lib winspool.lib /nologo /dll /incremental:no /map /debug /machine:I386 /nodefaultlib /pdbtype:sept /libpath:"r:\nt40addk\ddk\lib\i386\checked" /subsystem:windows,4.0
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ntdll.lib libcmt.lib libcimt.lib oldnames.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib advapi32.lib winspool.lib /nologo /dll /incremental:no /map /debug /machine:I386 /nodefaultlib /pdbtype:sept /libpath:"r:\nt40addk\ddk\lib\i386\checked" /subsystem:windows,4.0
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "NAVNTUTL - Win32 IntelAnsiDebug"
# Name "NAVNTUTL - Win32 IntelAnsiRelease"
# Name "NAVNTUTL - Win32 AlphaAnsiDebug"
# Name "NAVNTUTL - Win32 AlphaAnsiRelease"
# Name "NAVNTUTL - Win32 Release"
# Name "NAVNTUTL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;rc"
# Begin Source File

SOURCE=.\copystream.cpp
# End Source File
# Begin Source File

SOURCE=.\dllmain.cpp
# End Source File
# Begin Source File

SOURCE=.\navntutl.rc
# End Source File
# Begin Source File

SOURCE=.\Policy.cpp
# End Source File
# Begin Source File

SOURCE=.\process.cpp
# End Source File
# Begin Source File

SOURCE=.\secdesc.cpp
# End Source File
# Begin Source File

SOURCE=.\streams.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\copystream.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# End Target
# End Project
