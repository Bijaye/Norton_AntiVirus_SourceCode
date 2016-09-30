# Microsoft Developer Studio Project File - Name="Startup" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Startup - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Startup.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Startup.mak" CFG="Startup - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Startup - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Startup - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/EDisk", QAAAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "Startup - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Startup_"
# PROP BASE Intermediate_Dir "Startup_"
# PROP BASE Cmd_Line "NMAKE /f Startup.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Startup.exe"
# PROP BASE Bsc_Name "Startup.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Startup_"
# PROP Intermediate_Dir "Startup_"
# PROP Cmd_Line ".\bin16\NMAKE /f Startup.mak OUTDIR=Release"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Startup.exe"
# PROP Bsc_Name "Startup.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Startup - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Startup0"
# PROP BASE Intermediate_Dir "Startup0"
# PROP BASE Cmd_Line "NMAKE /f Startup.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Startup.exe"
# PROP BASE Bsc_Name "Startup.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Startup0"
# PROP Intermediate_Dir "Startup0"
# PROP Cmd_Line ".\bin16\NMAKE /f Startup.mak OUTDIR=Debug"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Startup.exe"
# PROP Bsc_Name "Startup.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Startup - Win32 Release"
# Name "Startup - Win32 Debug"

!IF  "$(CFG)" == "Startup - Win32 Release"

!ELSEIF  "$(CFG)" == "Startup - Win32 Debug"

!ENDIF 

# Begin Group "Build Automation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Publish.bat
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\advise.c
# End Source File
# Begin Source File

SOURCE=.\src\advstr.c
# End Source File
# Begin Source File

SOURCE=.\src\cmdalloc.c
# End Source File
# Begin Source File

SOURCE=.\src\cmdio.c
# End Source File
# Begin Source File

SOURCE=.\src\config.c
# End Source File
# Begin Source File

SOURCE=.\src\cvars.c
# End Source File
# Begin Source File

SOURCE=.\src\editlist.c
# End Source File
# Begin Source File

SOURCE=.\src\events.c
# End Source File
# Begin Source File

SOURCE=.\src\launch.c
# End Source File
# Begin Source File

SOURCE=.\src\nistr.c
# End Source File
# Begin Source File

SOURCE=.\src\norton.c
# End Source File
# Begin Source File

SOURCE=.\src\ops.c
# End Source File
# Begin Source File

SOURCE=.\src\sort.c
# End Source File
# Begin Source File

SOURCE=.\src\startup.c
# End Source File
# Begin Source File

SOURCE=.\src\stub.c
# End Source File
# Begin Source File

SOURCE=.\src\util.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\defs.h
# End Source File
# Begin Source File

SOURCE=.\src\menu.h
# End Source File
# Begin Source File

SOURCE=".\src\NORT-HLP.H"
# End Source File
# Begin Source File

SOURCE=.\src\proto.h
# End Source File
# End Group
# Begin Group "Make Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\STARTUP.MAK
# End Source File
# End Group
# End Target
# End Project
