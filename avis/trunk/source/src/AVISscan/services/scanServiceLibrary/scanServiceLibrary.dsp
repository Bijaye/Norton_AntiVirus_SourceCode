# Microsoft Developer Studio Project File - Name="scanServiceLibrary" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=scanServiceLibrary - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "scanServiceLibrary.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "scanServiceLibrary.mak" CFG="scanServiceLibrary - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "scanServiceLibrary - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "scanServiceLibrary - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "scanServiceLibrary - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(SRCD)\AVIScommon" /I "$(SRCD)\AVIScommon\system" /I "$(SRCD)\AVIScommon\log" /I "$(SRCD)\AVISscan\Interface" /I "$(SRCD)\ScannerAPI" /I "$(SRCD)\inc" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"$(OUTD)\scanServiceLibrary.lib"

!ELSEIF  "$(CFG)" == "scanServiceLibrary - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SRCD)\AVIScommon" /I "$(SRCD)\AVIScommon\system" /I "$(SRCD)\AVIScommon\log" /I "$(SRCD)\AVISscan\Interface" /I "$(SRCD)\ScannerAPI" /I "$(SRCD)\inc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"$(LIBD)\scanServiceLibrary.lib"

!ENDIF 

# Begin Target

# Name "scanServiceLibrary - Win32 Release"
# Name "scanServiceLibrary - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\NAVEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Scan.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanException.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanService.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanServiceGlobals.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WaitToScan.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MonitorDBForSigChanges.h
# End Source File
# Begin Source File

SOURCE=.\NAVEngine.h
# End Source File
# Begin Source File

SOURCE=.\Scan.h
# End Source File
# Begin Source File

SOURCE=.\ScanException.h
# End Source File
# Begin Source File

SOURCE=.\ScanService.h
# End Source File
# Begin Source File

SOURCE=.\ScanServiceGlobals.h
# End Source File
# Begin Source File

SOURCE=.\ServiceInterface.h
# End Source File
# Begin Source File

SOURCE=.\serviceTemplates.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WaitToScan.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
