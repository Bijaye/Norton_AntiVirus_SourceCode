# Microsoft Developer Studio Project File - Name="CodeRunner" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=CodeRunner - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CodeRunner.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CodeRunner.mak" CFG="CodeRunner - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CodeRunner - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "CodeRunner - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avis100/CodeRunner", CVPAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CodeRunner - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\bin\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /Ox /Ot /Oa /Ow /Og /Oi /Op /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "W95_32" /D "W_32" /D "CR_MOREFEATURES" /D "CR_FULLDECRYPTION" /D "CR_MOREFEATURES2" /D "CR_MYDEBUG" /D "CR_TSR" /D "NEW1" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
TargetPath=.\bin\Release\CodeRunner.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) D:\is\progs\stage\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CodeRunner - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\bin\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "W95_32" /D "W_32" /D "CR_MOREFEATURES" /D "CR_FULLDECRYPTION" /D "CR_MOREFEATURES2" /D "CR_MYDEBUG" /D "CR_TSR" /D "NEW1" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386

!ENDIF 

# Begin Target

# Name "CodeRunner - Win32 Release"
# Name "CodeRunner - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=src\ALGSCAN.C
# End Source File
# Begin Source File

SOURCE=src\CR_FSCAN.C
# End Source File
# Begin Source File

SOURCE=src\CR_GLOB.C
# End Source File
# Begin Source File

SOURCE=src\CR_MISC.C
# End Source File
# Begin Source File

SOURCE=src\CR_RUNR1.C
# End Source File
# Begin Source File

SOURCE=src\CR_RUNR2.C
# End Source File
# Begin Source File

SOURCE=src\CR_RUNR3.C
# End Source File
# Begin Source File

SOURCE=src\MYSTUFF.C
# End Source File
# Begin Source File

SOURCE=src\MYUTILS.C
# End Source File
# Begin Source File

SOURCE=src\SIGCMP.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\src\ALGSCAN.PRO
# End Source File
# Begin Source File

SOURCE=.\src\CODEDATA.H
# End Source File
# Begin Source File

SOURCE=.\src\COMMON.H
# End Source File
# Begin Source File

SOURCE=.\src\CR_ALL.H
# End Source File
# Begin Source File

SOURCE=.\src\CR_DEFS.H
# End Source File
# Begin Source File

SOURCE=.\src\CR_EXTS.H
# End Source File
# Begin Source File

SOURCE=.\src\CR_FSCAN.PRO
# End Source File
# Begin Source File

SOURCE=.\src\EPTTYPES.H
# End Source File
# Begin Source File

SOURCE=.\src\SIGCMP.PRO
# End Source File
# Begin Source File

SOURCE=.\src\VSDEFS.H
# End Source File
# Begin Source File

SOURCE=.\src\VSPDDEFS.H
# End Source File
# Begin Source File

SOURCE=.\src\VSPDTYPE.H
# End Source File
# Begin Source File

SOURCE=.\src\VSTYPES.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
