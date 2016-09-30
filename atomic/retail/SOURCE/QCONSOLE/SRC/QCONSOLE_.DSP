# Microsoft Developer Studio Project File - Name="qconsole" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=qconsole - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qconsole.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qconsole.mak" CFG="qconsole - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qconsole - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "qconsole - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Quarantine/qconsole", TAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qconsole - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "SYM_WIN" /D "SYM_WIN32" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 defutils.lib n32cores.lib s32navn.lib n32call.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "qconsole - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\quar32" /I "..\qconres" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "SYM_WIN" /D "SYM_WIN32" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\qconres" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 defutils.lib n32cores.lib s32navn.lib n32call.lib s32debug.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "qconsole - Win32 Release"
# Name "qconsole - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Cdcos.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\GenProp.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageButton.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemPropSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\proppage.cpp
# End Source File
# Begin Source File

SOURCE=.\qconsole.cpp
# End Source File
# Begin Source File

SOURCE=.\qconsole.rc

!IF  "$(CFG)" == "qconsole - Win32 Release"

!ELSEIF  "$(CFG)" == "qconsole - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qconsoleDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\qconsoleView.cpp
# End Source File
# Begin Source File

SOURCE=.\QTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\RANGESTR.CPP
# End Source File
# Begin Source File

SOURCE=.\ResultsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerProp.cpp
# End Source File
# Begin Source File

SOURCE=.\SortList.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Subclass.cpp
# End Source File
# Begin Source File

SOURCE=.\VirusPage.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Cdcos.h
# End Source File
# Begin Source File

SOURCE=.\const.h
# End Source File
# Begin Source File

SOURCE=.\CoolBar.h
# End Source File
# Begin Source File

SOURCE=.\CoolMenu.h
# End Source File
# Begin Source File

SOURCE=.\GenProp.h
# End Source File
# Begin Source File

SOURCE=.\ImageButton.h
# End Source File
# Begin Source File

SOURCE=.\ItemPropSheet.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MoveFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProgDlg.h
# End Source File
# Begin Source File

SOURCE=.\proppage.h
# End Source File
# Begin Source File

SOURCE=.\qconsole.h
# End Source File
# Begin Source File

SOURCE=.\qconsoleDoc.h
# End Source File
# Begin Source File

SOURCE=.\qconsoleView.h
# End Source File
# Begin Source File

SOURCE=.\QTreeView.h
# End Source File
# Begin Source File

SOURCE=.\RANGESTR.H
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ResultsDlg.h
# End Source File
# Begin Source File

SOURCE=.\ServerProp.h
# End Source File
# Begin Source File

SOURCE=.\SortList.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Subclass.h
# End Source File
# Begin Source File

SOURCE=.\VirusPage.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\menutool.bmp
# End Source File
# Begin Source File

SOURCE=.\QCONSOLE.ICO
# End Source File
# Begin Source File

SOURCE=.\res\qconsole.ico
# End Source File
# Begin Source File

SOURCE=.\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\tree_bit.bmp
# End Source File
# Begin Source File

SOURCE=.\viruschars.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section qconsole : {6BDE3868-AA31-11D1-9210-00C04F98023E}
# 	2:10:SortList.h:SortList.h
# 	2:19:Application Include:qconsole.h
# 	2:12:SortList.cpp:SortList.cpp
# 	2:22:CLASS: CSortedListCtrl:CSortedListCtrl
# End Section
