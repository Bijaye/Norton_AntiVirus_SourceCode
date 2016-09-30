# Microsoft Developer Studio Project File - Name="snavapi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=snavapi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "snavapi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "snavapi.mak" CFG="snavapi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "snavapi - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "snavapi - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "snavapi"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "snavapi - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "SYM_WIN" /D "SYM_WIN32" /D "QCUST" /D Q_VER=\"R\" /D "_X86" /D "_M_X86" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib navapi32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "snavapi - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "SYM_WIN" /D "SYM_WIN32" /D "QCUST" /D Q_VER=\"R\" /D "_X86" /D "_M_X86" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib navapi32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "snavapi - Win32 Release"
# Name "snavapi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\apiver.cpp
# End Source File
# Begin Source File

SOURCE=.\avshared.cpp
# End Source File
# Begin Source File

SOURCE=.\fileops.cpp
# End Source File
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\scanboot.cpp
# End Source File
# Begin Source File

SOURCE=.\scanfile.cpp
# End Source File
# Begin Source File

SOURCE=.\scanmbr.cpp
# End Source File
# Begin Source File

SOURCE=.\scanmem.cpp
# End Source File
# Begin Source File

SOURCE=.\shared.cpp
# End Source File
# Begin Source File

SOURCE=.\snavapi.cpp
# End Source File
# Begin Source File

SOURCE=.\spthread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\apiver.h
# End Source File
# Begin Source File

SOURCE=.\avshared.h
# End Source File
# Begin Source File

SOURCE=.\fileops.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\scanboot.h
# End Source File
# Begin Source File

SOURCE=.\scanfile.h
# End Source File
# Begin Source File

SOURCE=.\scanmbr.h
# End Source File
# Begin Source File

SOURCE=.\scanmem.h
# End Source File
# Begin Source File

SOURCE=.\shared.h
# End Source File
# Begin Source File

SOURCE=.\snavapi.h
# End Source File
# Begin Source File

SOURCE=.\spthread.h
# End Source File
# End Group
# Begin Group "//depot/core/trunk/Source/INCLUDE/SRC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DBCS_STR.H
# End Source File
# Begin Source File

SOURCE=.\platform.h
# End Source File
# Begin Source File

SOURCE=.\PLATWIN.H
# End Source File
# Begin Source File

SOURCE=.\SYMVXD.H
# End Source File
# End Group
# Begin Group "../../navapi/SRC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NAVAPI.H
# End Source File
# End Group
# Begin Group "//depot/Release/navapi/version/LibIntelAnsiWin32Debug"

# PROP Default_Filter ""
# End Group
# End Target
# End Project
