# Microsoft Developer Studio Project File - Name="CPLD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=CPLD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CPLD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CPLD.mak" CFG="CPLD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CPLD - Win32 Release" (based on "Win32 (x86) Generic Project")
!MESSAGE "CPLD - Win32 Debug" (based on "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "CPLD"
# PROP Scc_LocalPath ".."
MTL=midl.exe

!IF  "$(CFG)" == "CPLD - Win32 Release"

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

!ELSEIF  "$(CFG)" == "CPLD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "CPLD - Win32 Release"
# Name "CPLD - Win32 Debug"
# Begin Source File

SOURCE=..\cm\Buildit.btm

!IF  "$(CFG)" == "CPLD - Win32 Release"

# Begin Custom Build
OutDir=.\Release
ProjDir=.
InputPath=..\cm\Buildit.btm

"$(OutDir)\Results.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\Nobuilds\551\4dos.com /e:5000 /c $(ProjDir)\..\cm\BUILDIT.BTM /RELEASE $(OutDir) CPLD

# End Custom Build

!ELSEIF  "$(CFG)" == "CPLD - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
ProjDir=.
InputPath=..\cm\Buildit.btm

"$(OutDir)\Results.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\..\Nobuilds\551\4dos.com /e:5000 /c $(ProjDir)\..\cm\BUILDIT.BTM /DEBUG $(OutDir) CPLD

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
