# Microsoft Developer Studio Project File - Name="AVISdb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AVISdb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AVISdb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVISdb.mak" CFG="AVISdb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVISdb - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVISdb - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AVISdb - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "g:\ImmuneSystem\bogus\rout"
# PROP BASE Intermediate_Dir "g:\ImmuneSystem\bogus\robj"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISDB_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(SRCD)\AVIScommon" /I "$(SRCD)\AVIScommon\log" /I "$(SRCD)\AVIScommon\system" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISDB_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"$(OBJD)/AVISdb.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib AVIScommon.lib /nologo /dll /machine:I386 /libpath:"$(SRCD)\AVIScommon\Debug" /libpath:"$(LIBD)" /libpath:"$(LIBD)\nt"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "AVISdb - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "g:\ImmuneSystem\bogus\dout"
# PROP BASE Intermediate_Dir "g:\ImmuneSystem\bogus\dobj"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISDB_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SRCD)\AVIScommon" /I "$(SRCD)\AVIScommon\log" /I "$(SRCD)\AVIScommon\system" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISDB_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"$(OBJD)/AVISdb.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib AVIScommon.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(SRCD)\AVIScommon\Debug" /libpath:"$(LIBD)" /libpath:"$(LIBD)\nt"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "AVISdb - Win32 Release"
# Name "AVISdb - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AnalysisRequest.cpp
# End Source File
# Begin Source File

SOURCE=.\AnalysisResults.cpp
# End Source File
# Begin Source File

SOURCE=.\AnalysisStateInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Attributes.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISdbDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISDBException.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckSum.cpp
# End Source File
# Begin Source File

SOURCE=.\DatabaseClassesTest.cpp
# End Source File
# Begin Source File

SOURCE=.\DatabaseTests.cpp
# End Source File
# Begin Source File

SOURCE=.\DBService.cpp
# End Source File
# Begin Source File

SOURCE=.\DBSession.cpp
# End Source File
# Begin Source File

SOURCE=.\DBTester.cpp
# End Source File
# Begin Source File

SOURCE=.\Globals.cpp
# End Source File
# Begin Source File

SOURCE=.\Internal.cpp
# End Source File
# Begin Source File

SOURCE=.\Server.cpp
# End Source File
# Begin Source File

SOURCE=.\Signature.cpp
# End Source File
# Begin Source File

SOURCE=.\SigsToBeExported.cpp
# End Source File
# Begin Source File

SOURCE=.\SQLStmt.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusToBeReported.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Submittor.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AnalysisRequest.h
# End Source File
# Begin Source File

SOURCE=.\AnalysisResults.h
# End Source File
# Begin Source File

SOURCE=.\AnalysisStateInfo.h
# End Source File
# Begin Source File

SOURCE=.\Attributes.h
# End Source File
# Begin Source File

SOURCE=..\AVISInclude\AVIS.h
# End Source File
# Begin Source File

SOURCE=.\AVISdbDLL.h
# End Source File
# Begin Source File

SOURCE=.\AVISDBException.h
# End Source File
# Begin Source File

SOURCE=.\CheckSum.h
# End Source File
# Begin Source File

SOURCE=.\DatabaseClassesTest.h
# End Source File
# Begin Source File

SOURCE=.\DatabaseTests.h
# End Source File
# Begin Source File

SOURCE=.\DBService.h
# End Source File
# Begin Source File

SOURCE=.\DBSession.h
# End Source File
# Begin Source File

SOURCE=.\DBTester.h
# End Source File
# Begin Source File

SOURCE=.\Globals.h
# End Source File
# Begin Source File

SOURCE=.\Internal.h
# End Source File
# Begin Source File

SOURCE=..\AVISInclude\Reporter.h
# End Source File
# Begin Source File

SOURCE=.\Server.h
# End Source File
# Begin Source File

SOURCE=.\Signature.h
# End Source File
# Begin Source File

SOURCE=.\SigsToBeExported.h
# End Source File
# Begin Source File

SOURCE=.\SQLStmt.h
# End Source File
# Begin Source File

SOURCE=.\StatusToBeReported.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StringHash.h
# End Source File
# Begin Source File

SOURCE=.\Submittor.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
