# Microsoft Developer Studio Project File - Name="ScanDlvr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ScanDlvr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ScanDlvr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ScanDlvr.mak" CFG="ScanDlvr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ScanDlvr - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ScanDlvr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ScanDlvr - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "SYM_WIN" /D "SYM_WIN32" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 defannty.lib sdflt32i.lib sdsok32i.lib sdstp32i.lib sdsnd32i.lib sdpck32i.lib quaradd.lib n32utils.lib n32cores.lib s32navn.lib n32call.lib s32debug.lib socklib.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "ScanDlvr - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "SYM_WIN" /D "SYM_WIN32" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 defannty.lib sdflt32i.lib sdsok32i.lib sdstp32i.lib sdsnd32i.lib sdpck32i.lib quaradd.lib n32utils.lib n32cores.lib s32navn.lib n32call.lib s32debug.lib socklib.lib wsock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "ScanDlvr - Win32 Release"
# Name "ScanDlvr - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AcceptPg.cpp
# End Source File
# Begin Source File

SOURCE=.\CLEANPG.CPP
# End Source File
# Begin Source File

SOURCE=.\CorpPg.cpp
# End Source File
# Begin Source File

SOURCE=.\dllmain.cpp
# End Source File
# Begin Source File

SOURCE=.\factory.cpp
# End Source File
# Begin Source File

SOURCE=.\FOTDPG.CPP
# End Source File
# Begin Source File

SOURCE=.\GLOBAL.CPP
# End Source File
# Begin Source File

SOURCE=.\ProgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RejectPg.cpp
# End Source File
# Begin Source File

SOURCE=.\ReviewPg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanDeliverDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanDlvr.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanDlvr.def
# End Source File
# Begin Source File

SOURCE=.\ScanWiz.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\USER1PG.CPP
# End Source File
# Begin Source File

SOURCE=.\USER2PG.CPP
# End Source File
# Begin Source File

SOURCE=.\USER3PG.CPP
# End Source File
# Begin Source File

SOURCE=.\WELCOMPG.CPP
# End Source File
# Begin Source File

SOURCE=.\WizPage.cpp
# End Source File
# Begin Source File

SOURCE=.\WrapUpPg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AcceptPg.h
# End Source File
# Begin Source File

SOURCE=.\CorpPg.h
# End Source File
# Begin Source File

SOURCE=.\factory.h
# End Source File
# Begin Source File

SOURCE=.\FOTDPg.h
# End Source File
# Begin Source File

SOURCE=.\GLOBAL.H
# End Source File
# Begin Source File

SOURCE=.\ProgDlg.h
# End Source File
# Begin Source File

SOURCE=.\RejectPg.h
# End Source File
# Begin Source File

SOURCE=.\ReviewPg.h
# End Source File
# Begin Source File

SOURCE=.\SCANCFG.H
# End Source File
# Begin Source File

SOURCE=.\ScanDeliverDLL.h
# End Source File
# Begin Source File

SOURCE=.\ScanDlvr.h
# End Source File
# Begin Source File

SOURCE=.\ScanWiz.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SystemPg.h
# End Source File
# Begin Source File

SOURCE=.\UserPg.h
# End Source File
# Begin Source File

SOURCE=.\VirusPg.h
# End Source File
# Begin Source File

SOURCE=.\Wizard1.h
# End Source File
# Begin Source File

SOURCE=.\Wizard2.h
# End Source File
# Begin Source File

SOURCE=.\Wizard3.h
# End Source File
# Begin Source File

SOURCE=.\Wizard4.h
# End Source File
# Begin Source File

SOURCE=.\Wizard5.h
# End Source File
# Begin Source File

SOURCE=.\Wizard6.h
# End Source File
# Begin Source File

SOURCE=.\WizPage.h
# End Source File
# Begin Source File

SOURCE=.\WrapUpPg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
