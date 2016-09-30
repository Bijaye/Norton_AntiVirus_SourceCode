# Microsoft Developer Studio Project File - Name="NavLu32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=NAVLU32 - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NavLu32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NavLu32.mak" CFG="NAVLU32 - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NavLu32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE F90 /Zi /I "Debug/" /c /nologo
# ADD F90 /Zi /I "Debug/" /c /nologo
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "i:" /I "l:\source\include\src" /I "s:\LiveUpdate\Src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "SYM_WIN" /D "SYM_WIN32" /D "SYM_DEBUG" /FR /Yu"StdAfx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "i:" /i "l:\source\include\src" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 s:\lib.ida\n32cores.lib l:\source\lib.ira\S32DEBUG.lib s:\lib.ira\S32NAVK.lib l:\source\lib.ida\defutils.lib s:\lib.ida\npsapi.lib s:\lib.ida\n32call.lib r:\defannty\defannty.release\release /nologo /subsystem:windows /debug /machine:I386
# Begin Target

# Name "NavLu32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\BogusMainWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdLineInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\MyUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\NavLU.cpp
# End Source File
# Begin Source File

SOURCE=.\NavLU.rc
# End Source File
# Begin Source File

SOURCE=.\NavLuApp.cpp
# End Source File
# Begin Source File

SOURCE=.\NavLuUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\ScheduleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"StdAfx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\BogusMainWnd.h
# End Source File
# Begin Source File

SOURCE=.\CmdLineInfo.h
# End Source File
# Begin Source File

SOURCE=.\MyUtils.h
# End Source File
# Begin Source File

SOURCE=.\NavLu.h
# End Source File
# Begin Source File

SOURCE=.\NavLuApp.h
# End Source File
# Begin Source File

SOURCE=.\ScheduleDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\strings.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\NavLU.ico
# End Source File
# Begin Source File

SOURCE=.\navlu.rc2
# End Source File
# Begin Source File

SOURCE=.\schedule.ico
# End Source File
# End Group
# End Target
# End Project
# Section NavLu32 : {8E27C92C-1264-101C-8A2F-040224009C02}
# 	2:5:Class:CMsacal70
# 	2:10:HeaderFile:msacal70.h
# 	2:8:ImplFile:msacal70.cpp
# End Section
# Section NavLu32 : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:COleFont
# 	2:10:HeaderFile:font.h
# 	2:8:ImplFile:font.cpp
# End Section
# Section NavLu32 : {8E27C92B-1264-101C-8A2F-040224009C02}
# 	0:8:Font.cpp:C:\work\NavLU\Font.cpp
# 	0:6:Font.h:C:\work\NavLU\Font.h
# 	0:12:Msacal70.cpp:C:\work\NavLU\Msacal70.cpp
# 	0:10:Msacal70.h:C:\work\NavLU\Msacal70.h
# 	2:21:DefaultSinkHeaderFile:msacal70.h
# 	2:16:DefaultSinkClass:CMsacal70
# End Section
# Section OLE Controls
# 	{8E27C92B-1264-101C-8A2F-040224009C02}
# End Section
