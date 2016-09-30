# Microsoft Developer Studio Project File - Name="AVISDFRL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AVISDFRL - Win32 IDE Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AVISDFRL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVISDFRL.mak" CFG="AVISDFRL - Win32 IDE Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVISDFRL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVISDFRL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVISDFRL - Win32 IDE Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVISDFRL - Win32 IDE Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AVISDFRL - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "///OUTD///\nt"
# PROP Intermediate_Dir "///OBJD///\DataFlow\AVISDFRL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /MD /W3 /GX /O2 /Ob2 /I "..\inc" /I "..\com" /I "..\..\inc" /I "..\..\avissendmail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /nologo
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 avissendmail.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /libpath:"///LIBD///\\" /map:"///OBJD///\DataFlow\AVISDFRL/AVISDFRL.map"

!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "///OUTD///\nt"
# PROP Intermediate_Dir "///OBJD///\DataFlow\AVISDFRL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /MDd /W3 /GX /ZI /Od /I "..\inc" /I "..\com" /I "..\..\inc" /I "..\..\avissendmail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /nologo
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /i ///SRCD///\DataFlow\AVISDFRL" "
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 avissendmail.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /libpath:"///LIBD///\\" /map:"///OBJD///\DataFlow\AVISDFRL/AVISDFRL.map"

!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 IDE Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DFLaunch"
# PROP BASE Intermediate_Dir "DFLaunch"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /MDd /W3 /Gm /Gi /GX /ZI /Od /I "..\inc" /I "..\com" /I "..\..\inc" /I "..\..\avissendmail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /nologo
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /pdbtype:sept
# ADD LINK32 avissendmail.lib /nologo /subsystem:windows /dll /profile /map /debug /machine:I386 /libpath:"..\..\..\dlib\ea"
# SUBTRACT LINK32 /verbose

!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 IDE Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AVISDFRL___Win32_IDE_Release"
# PROP BASE Intermediate_Dir "AVISDFRL___Win32_IDE_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /MD /W3 /GX /O2 /I "..\inc" /I "..\com" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /nologo
# ADD CPP /MD /W3 /GX /Zi /O2 /I "..\inc" /I "..\com" /I "..\..\inc" /I "..\..\avissendmail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /nologo
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /map /machine:I386 ///OBJD///\DataFlow\AVISDFRL\AVISDFRL.res
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 avissendmail.lib /nologo /subsystem:windows /dll /profile /map /debug /machine:I386 /libpath:"..\..\..\rlib\ea"

!ENDIF 

# Begin Target

# Name "AVISDFRL - Win32 Release"
# Name "AVISDFRL - Win32 Debug"
# Name "AVISDFRL - Win32 IDE Debug"
# Name "AVISDFRL - Win32 IDE Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Common Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\com\listctex.cpp
# End Source File
# Begin Source File

SOURCE=..\com\listvwex.cpp
# End Source File
# Begin Source File

SOURCE=..\com\ParamValue.cpp
# End Source File
# Begin Source File

SOURCE=..\com\PerformanceMonitor.cpp
# End Source File
# Begin Source File

SOURCE=..\com\titletip.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ActivityLogSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoDisableErrorCount.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISDFRL.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISDFRLView.cpp
# End Source File
# Begin Source File

SOURCE=.\CookieStatistics.cpp
# End Source File
# Begin Source File

SOURCE=.\DFResource.cpp
# End Source File
# Begin Source File

SOURCE=.\MachineStatistics.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceSummary.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Common Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\inc\dfdefaults.h
# End Source File
# Begin Source File

SOURCE=..\inc\dfjob.h
# End Source File
# Begin Source File

SOURCE=..\inc\DFMsg.h
# End Source File
# Begin Source File

SOURCE=..\com\listctex.h
# End Source File
# Begin Source File

SOURCE=..\com\listvwex.h
# End Source File
# Begin Source File

SOURCE=..\com\ParamValue.h
# End Source File
# Begin Source File

SOURCE=..\com\PerformanceMonitor.h
# End Source File
# Begin Source File

SOURCE=..\com\titletip.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ActivityLogSetting.h
# End Source File
# Begin Source File

SOURCE=.\AutoDisableErrorCount.h
# End Source File
# Begin Source File

SOURCE=.\AVISDFRL.h
# End Source File
# Begin Source File

SOURCE=.\AVISDFRLView.h
# End Source File
# Begin Source File

SOURCE=.\CookieStatistics.h
# End Source File
# Begin Source File

SOURCE=.\DFResource.h
# End Source File
# Begin Source File

SOURCE=.\MachineStatistics.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\ResourceDialog.h
# End Source File
# Begin Source File

SOURCE=.\ResourceSummary.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AVISDFRL.ico
# End Source File
# Begin Source File

SOURCE=.\AVISDFRL.rc

!IF  "$(CFG)" == "AVISDFRL - Win32 Release"

!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 Debug"

!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 IDE Debug"

# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409

!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 IDE Release"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\version.rc
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\AVISDFRL.def
# End Source File
# Begin Source File

SOURCE=..\inc\AVISDFRLimp.h
# End Source File
# Begin Source File

SOURCE=.\Debug\DFResource.lst
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
