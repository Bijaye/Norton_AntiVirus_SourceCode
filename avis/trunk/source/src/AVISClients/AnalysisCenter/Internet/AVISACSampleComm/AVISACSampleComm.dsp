# Microsoft Developer Studio Project File - Name="AVISACSampleComm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AVISACSampleComm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AVISACSampleComm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVISACSampleComm.mak" CFG="AVISACSampleComm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVISACSampleComm - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AVISACSampleComm - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AVISACSampleComm - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(SRCD)\AVISClients\AVISTransactions" /I "$(SRCD)\AVISCommon" /I "$(SRCD)\AVISCommon\system" /I "$(SRCD)\AVISCommon\log" /I "$(SRCD)\AVISdb" /I "$(SRCD)\AVISFile" /I "$(SRCD)\AVISFilter" /I "$(SRCD)\AVISSendMail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "$(SRCD)\inc" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 AVISFilter.lib AVISTransactions.lib AVISDB.lib AVISCommon.lib AVISSendMail.lib /nologo /subsystem:windows /machine:I386 /libpath:"$(LIBD)"

!ELSEIF  "$(CFG)" == "AVISACSampleComm - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SRCD)\AVISSendMail" /I "$(SRCD)\AVISClients\AVISTransactions" /I "$(SRCD)\AVISCommon" /I "$(SRCD)\AVISCommon\system" /I "$(SRCD)\AVISCommon\log" /I "$(SRCD)\AVISdb" /I "$(SRCD)\AVISFile" /I "$(SRCD)\AVISFilter" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "$(SRCD)\inc" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AVISSendMail.lib AVISFilter.lib AVISTransactions.lib AVISDB.lib AVISCommon.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"$(LIBD)"

!ENDIF 

# Begin Target

# Name "AVISACSampleComm - Win32 Release"
# Name "AVISACSampleComm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AVISACSampleComm.cpp

!IF  "$(CFG)" == "AVISACSampleComm - Win32 Release"

!ELSEIF  "$(CFG)" == "AVISACSampleComm - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AVISACSampleComm.rc
# End Source File
# Begin Source File

SOURCE=.\AVISACSampleCommDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISClientSampleImporter.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISClientStatusReporter.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISGatewayManagerSampleComm.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISGatewaySampleComm.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISTransactionImportSample.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISTransactionImportSampleException.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISTransactionReportStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AVISACSampleComm.h
# End Source File
# Begin Source File

SOURCE=.\AVISACSampleCommDlg.h
# End Source File
# Begin Source File

SOURCE=.\AVISClientSampleImporter.h
# End Source File
# Begin Source File

SOURCE=.\AVISClientStatusReporter.h
# End Source File
# Begin Source File

SOURCE=.\AVISGatewayManagerSampleComm.h
# End Source File
# Begin Source File

SOURCE=.\AVISGatewaySampleComm.h
# End Source File
# Begin Source File

SOURCE=.\AVISTransactionImportSample.h
# End Source File
# Begin Source File

SOURCE=.\AVISTransactionImportSampleException.h
# End Source File
# Begin Source File

SOURCE=.\AVISTransactionReportStatus.h
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

SOURCE=.\res\AVISACSampleComm.ico
# End Source File
# End Group
# End Target
# End Project
