# Microsoft Developer Studio Project File - Name="AVISMonitor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AVISMonitor - Win32 IDE Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AVISMonitor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVISMonitor.mak" CFG="AVISMonitor - Win32 IDE Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVISMonitor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AVISMonitor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "AVISMonitor - Win32 IDE Debug" (based on "Win32 (x86) Application")
!MESSAGE "AVISMonitor - Win32 IDE Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AVISMonitor - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "///OUTD///\nt"
# PROP Intermediate_Dir "///OBJD///\AVISMonitor"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /MD /W3 /GX /O2 /I "..\dataflow\com" /I "..\inc" /I "..\AVISMonitorLib" /I "..\AVISSendMail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /nologo
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 AVISMonitorLib.lib psapi.lib AVISSendMail.lib /subsystem:windows /map /machine:I386 /libpath:"///LIBD///"
# SUBTRACT LINK32 /nologo

!ELSEIF  "$(CFG)" == "AVISMonitor - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "///OUTD///\nt"
# PROP Intermediate_Dir "///OBJD///\AVISMonitor"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /MDd /W3 /Gm /GX /Zi /Od /I "..\dataflow\com" /I "..\inc" /I "..\AVISMonitorLib" /I "..\AVISSendMail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /nologo
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AVISMonitorLib.lib psapi.lib AVISSendMail.lib /subsystem:windows /incremental:no /map /debug /machine:I386 /pdbtype:sept /libpath:"///LIBD///"
# SUBTRACT LINK32 /nologo

!ELSEIF  "$(CFG)" == "AVISMonitor - Win32 IDE Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "IDE Debug"
# PROP BASE Intermediate_Dir "IDE Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\dataflow\com" /I "..\inc" /I "..\AVISMonitorLib" /I "..\AVISSendMail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AVISMonitorLib.lib psapi.lib AVISSendMail.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\dlib\ea"

!ELSEIF  "$(CFG)" == "AVISMonitor - Win32 IDE Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "IDE Release"
# PROP BASE Intermediate_Dir "IDE Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\dataflow\com" /I "..\inc" /I "..\AVISMonitorLib" /I "..\AVISSendMail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 AVISMonitorLib.lib psapi.lib AVISSendMail.lib /nologo /subsystem:windows /machine:I386 /libpath:"..\..\rlib\ea"

!ENDIF 

# Begin Target

# Name "AVISMonitor - Win32 Release"
# Name "AVISMonitor - Win32 Debug"
# Name "AVISMonitor - Win32 IDE Debug"
# Name "AVISMonitor - Win32 IDE Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Common Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dataflow\com\listvwex.cpp
# End Source File
# Begin Source File

SOURCE=..\dataflow\com\paramvalue.cpp
# End Source File
# Begin Source File

SOURCE=..\dataflow\com\singleinstance.cpp
# End Source File
# Begin Source File

SOURCE=..\dataflow\com\titletip.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\AVISModule.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISMonitor.rc
# End Source File
# Begin Source File

SOURCE=.\AVISMonitorView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ModuleDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MonitorSetupDialog.cpp
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

SOURCE=..\dataflow\com\listvwex.h
# End Source File
# Begin Source File

SOURCE=..\dataflow\com\paramvalue.h
# End Source File
# Begin Source File

SOURCE=..\dataflow\com\singleinstance.h
# End Source File
# Begin Source File

SOURCE=..\dataflow\com\titletip.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AVISModule.h
# End Source File
# Begin Source File

SOURCE=.\AVISMonitor.h
# End Source File
# Begin Source File

SOURCE=.\AVISMonitorView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\ModuleDialog.h
# End Source File
# Begin Source File

SOURCE=.\MonitorSetupDialog.h
# End Source File
# Begin Source File

SOURCE=.\psapi.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AVISMonitor.ico
# End Source File
# Begin Source File

SOURCE=.\res\AVISMonitor.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\AVISMonitor.doc
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
