# Microsoft Developer Studio Project File - Name="PAMemulate" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=PAMemulate - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pamemulate.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pamemulate.mak" CFG="PAMemulate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PAMemulate - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "PAMemulate - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PAMemulate - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Zp1 /W3 /GX /O2 /I "inc" /I "inc\w32" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "REPAIR_DEBUG" /D "BUILD_SET" /D "SYM_WIN" /D "SYM_WIN32" /D "SYM_CPU_X86" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
TargetPath=.\Release\pamemulate.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) D:\is\bin	xcopy $(TargetPath)   D:\is\progs\stage\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /Zp1 /W3 /Gm /ZI /Od /I "inc" /I "inc\w32" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "REPAIR_DEBUG" /D "BUILD_SET" /D "SYM_WIN" /D "SYM_WIN32" /D "SYM_CPU_X86" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "PAMemulate - Win32 Release"
# Name "PAMemulate - Win32 Debug"
# Begin Source File

SOURCE=.\analysis.c
# End Source File
# Begin Source File

SOURCE=.\CACHE.C
# End Source File
# Begin Source File

SOURCE=.\CALLFAKE.C
# End Source File
# Begin Source File

SOURCE=.\CPU.C
# End Source File
# Begin Source File

SOURCE=.\DATAFILE.C
# End Source File
# Begin Source File

SOURCE=.\EXCLUDE.C
# End Source File
# Begin Source File

SOURCE=.\EXEC.C
# End Source File
# Begin Source File

SOURCE=.\FLAGS.C
# End Source File
# Begin Source File

SOURCE=.\INSTR.C
# End Source File
# Begin Source File

SOURCE=.\INSTR2.C
# End Source File
# Begin Source File

SOURCE=.\MAIN.C
# End Source File
# Begin Source File

SOURCE=.\OPCODELE.C
# End Source File
# Begin Source File

SOURCE=.\PAGE.C
# End Source File
# Begin Source File

SOURCE=.\PAMAPI.C
# End Source File
# Begin Source File

SOURCE=.\REGION.C
# End Source File
# Begin Source File

SOURCE=.\REPAIR.C
# End Source File
# Begin Source File

SOURCE=.\SEARCH.C
# End Source File
# Begin Source File

SOURCE=.\SIGEX.C
# End Source File
# End Target
# End Project
