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
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /I "inc.core" /I "navapi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "SYM_WIN" /D "SYM_WIN32" /D "QCUST" /D Q_VER=\"R\" /D "_X86" /D "_M_X86" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib navapi32.lib /nologo /subsystem:console /machine:I386 /libpath:"Release"

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
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /I "inc.core" /I "navapi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "SYM_WIN" /D "SYM_WIN32" /D "QCUST" /D Q_VER=\"R\" /D "_X86" /D "_M_X86" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib navapi32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"Debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "snavapi - Win32 Release"
# Name "snavapi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\snavapi\apiver.cpp
# End Source File
# Begin Source File

SOURCE=.\snavapi\avshared.cpp
# End Source File
# Begin Source File

SOURCE=.\snavapi\fileops.cpp
# End Source File
# Begin Source File

SOURCE=.\snavapi\log.cpp
# End Source File
# Begin Source File

SOURCE=.\snavapi\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\snavapi\scanboot.cpp
# End Source File
# Begin Source File

SOURCE=.\snavapi\scanfile.cpp
# End Source File
# Begin Source File

SOURCE=.\snavapi\scanmbr.cpp
# End Source File
# Begin Source File

SOURCE=.\snavapi\scanmem.cpp
# End Source File
# Begin Source File

SOURCE=.\snavapi\shared.cpp
# End Source File
# Begin Source File

SOURCE=.\snavapi\snavapi.cpp
# End Source File
# Begin Source File

SOURCE=.\snavapi\spthread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\snavapi\apiver.h
# End Source File
# Begin Source File

SOURCE=.\snavapi\avshared.h
# End Source File
# Begin Source File

SOURCE=.\inc.core\dbcs_str.H
# End Source File
# Begin Source File

SOURCE=.\snavapi\fileops.h
# End Source File
# Begin Source File

SOURCE=.\snavapi\log.h
# End Source File
# Begin Source File

SOURCE=.\snavapi\md5.h
# End Source File
# Begin Source File

SOURCE=.\navapi\navapi.h
# End Source File
# Begin Source File

SOURCE=.\inc.core\platform.h
# End Source File
# Begin Source File

SOURCE=.\inc.core\platwin.h
# End Source File
# Begin Source File

SOURCE=.\snavapi\scanboot.h
# End Source File
# Begin Source File

SOURCE=.\snavapi\scanfile.h
# End Source File
# Begin Source File

SOURCE=.\snavapi\scanmbr.h
# End Source File
# Begin Source File

SOURCE=.\snavapi\scanmem.h
# End Source File
# Begin Source File

SOURCE=.\snavapi\shared.h
# End Source File
# Begin Source File

SOURCE=.\snavapi\snavapi.h
# End Source File
# Begin Source File

SOURCE=.\snavapi\spthread.h
# End Source File
# Begin Source File

SOURCE=.\inc.core\symvxd.h
# End Source File
# End Group
# End Target
# End Project
