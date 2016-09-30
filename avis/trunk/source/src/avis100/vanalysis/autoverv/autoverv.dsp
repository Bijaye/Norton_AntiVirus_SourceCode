# Microsoft Developer Studio Project File - Name="autoverv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=autoverv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "autoverv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "autoverv.mak" CFG="autoverv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "autoverv - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "autoverv - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "autoverv - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 buildtime.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /machine:I386
# Begin Special Build Tool
IntDir=.\Release
TargetPath=.\Release\autoverv.exe
SOURCE="$(InputPath)"
PreLink_Cmds=cl -nologo -c -Zl -Fo$(IntDir)\buildtime.obj buildtime.cpp
PostBuild_Cmds=xcopy $(TargetPath) D:\is\bin	xcopy $(TargetPath)  D:\is\progs\stage\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "MYASSERT" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /verbose /debug /machine:I386 /nodefaultlib:"D:\bchecker\bcinterf.lib" /pdbtype:sept /libpath:"J:\bchecker"
# SUBTRACT LINK32 /incremental:no /map
# Begin Special Build Tool
IntDir=.\Debug
SOURCE="$(InputPath)"
PreLink_Cmds=cl -nologo -c -Zl -Fo$(IntDir)\buildtime.obj buildtime.cpp
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "autoverv - Win32 Release"
# Name "autoverv - Win32 Debug"
# Begin Source File

SOURCE=".\!todo.txt"
# End Source File
# Begin Source File

SOURCE=.\assert.h
# End Source File
# Begin Source File

SOURCE=.\autoverv.cpp
# End Source File
# Begin Source File

SOURCE=.\autoverv.h
# End Source File
# Begin Source File

SOURCE=.\autoverv.readme
# End Source File
# Begin Source File

SOURCE=.\bigrams.cpp
# End Source File
# Begin Source File

SOURCE=.\bigrams.h
# End Source File
# Begin Source File

SOURCE=.\break.h
# End Source File
# Begin Source File

SOURCE=.\buildtime.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=.\buildtime.cpp

"dummy" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl -c buildtime.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\buildtime.h
# End Source File
# Begin Source File

SOURCE=.\dotProduct.cpp
# End Source File
# Begin Source File

SOURCE=.\dotProduct.h
# End Source File
# Begin Source File

SOURCE=.\encryption.cpp
# End Source File
# Begin Source File

SOURCE=.\encryption.h
# End Source File
# Begin Source File

SOURCE=.\File.cpp
# End Source File
# Begin Source File

SOURCE=.\File.h
# End Source File
# Begin Source File

SOURCE=.\FileClass.cpp
# End Source File
# Begin Source File

SOURCE=.\FileClass.h
# End Source File
# Begin Source File

SOURCE=.\key.cpp
# End Source File
# Begin Source File

SOURCE=.\key.h
# End Source File
# Begin Source File

SOURCE=.\lcs.cpp
# End Source File
# Begin Source File

SOURCE=.\lcs.h
# End Source File
# Begin Source File

SOURCE=.\loadsamples.cpp
# End Source File
# Begin Source File

SOURCE=.\loadsamples.h
# End Source File
# Begin Source File

SOURCE=.\location.cpp
# End Source File
# Begin Source File

SOURCE=.\location.h
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\map.cpp
# End Source File
# Begin Source File

SOURCE=.\map.h
# End Source File
# Begin Source File

SOURCE=.\match.cpp
# End Source File
# Begin Source File

SOURCE=.\match.h
# End Source File
# Begin Source File

SOURCE=.\newhandler.cpp
# End Source File
# Begin Source File

SOURCE=.\newhandler.h
# End Source File
# Begin Source File

SOURCE=.\polymath.cpp
# End Source File
# Begin Source File

SOURCE=.\polymath.h
# End Source File
# Begin Source File

SOURCE=.\recon.cpp
# End Source File
# Begin Source File

SOURCE=.\recon.h
# End Source File
# Begin Source File

SOURCE=.\sample.cpp
# End Source File
# Begin Source File

SOURCE=.\Sample.h
# End Source File
# Begin Source File

SOURCE=.\section.cpp
# End Source File
# Begin Source File

SOURCE=.\section.h
# End Source File
# Begin Source File

SOURCE=.\segment.cpp
# End Source File
# Begin Source File

SOURCE=.\segment.h
# End Source File
# Begin Source File

SOURCE=.\test.cpp
# End Source File
# Begin Source File

SOURCE=.\test.h
# End Source File
# Begin Source File

SOURCE=.\verification.cpp
# End Source File
# Begin Source File

SOURCE=.\verification.h
# End Source File
# End Target
# End Project
