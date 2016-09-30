# Microsoft Developer Studio Project File - Name="AVISServlets" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AVISServlets - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AVISServlets.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVISServlets.mak" CFG="AVISServlets - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVISServlets - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVISServlets - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AVISServlets - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISSERVLETS_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(SRCD)" /I "$(SRCD)/avisunzip" /I "$(SRCD)/avisunpack" /I "$(SRCD)/avisfile" /I "$(SRCD)/aviscommon/system" /I "$(SRCD)/avisfilter" /I "$(SRCD)/aviscommon" /I "$(SRCD)/avisdb" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISSERVLETS_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"$(OBJD)/ea/AvisServlets/AVISServlets.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 vdbunpacker.lib aviscommon.lib avisfile.lib avisfilter.lib avisdb.lib httpdapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"$(OBJD)/AVISServlets.pdb" /machine:I386 /out:"$(OUTD)/AVISServlets.dll" /libpath:"d:\additionalLibraryPath" /libpath:"$(OUTD)" /libpath:"$(OBJD)" /libpath:"$(LIBD)" /libpath:"$(LIBD)\nt"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "AVISServlets - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISSERVLETS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SRCD)" /I "$(SRCD)/avisunzip" /I "$(SRCD)/avisunpack" /I "$(SRCD)/avisfile" /I "$(SRCD)/aviscommon/system" /I "$(SRCD)/avisfilter" /I "$(SRCD)/aviscommon" /I "$(SRCD)/avisdb" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISSERVLETS_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"$(OBJD)/ea/AvisServlets/AVISServlets.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vdbunpacker.lib aviscommon.lib avisfile.lib avisfilter.lib avisdb.lib httpdapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"$(OBJD)/AVISServlets.pdb" /debug /debugtype:both /machine:I386 /out:"$(OUTD)/AVISServlets.dll" /pdbtype:sept /libpath:"$(LIBD)\ea" /libpath:"$(OUTD)" /libpath:"$(OBJD)" /libpath:"$(LIBD)" /libpath:"$(LIBD)\nt"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "AVISServlets - Win32 Release"
# Name "AVISServlets - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\agetsamp.cpp
# End Source File
# Begin Source File

SOURCE=.\agetsig.cpp
# End Source File
# Begin Source File

SOURCE=.\agetstat.cpp
# End Source File
# Begin Source File

SOURCE=.\apstsig.cpp
# End Source File
# Begin Source File

SOURCE=.\apststat.cpp
# End Source File
# Begin Source File

SOURCE=.\asubsamp.cpp
# End Source File
# Begin Source File

SOURCE=.\avisredirector.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISServlets.cpp
# End Source File
# Begin Source File

SOURCE=.\avisservlets.def
# End Source File
# Begin Source File

SOURCE=.\dbconn.cpp
# End Source File
# Begin Source File

SOURCE=.\dbconnhnd.cpp
# End Source File
# Begin Source File

SOURCE=.\dbconpool.cpp
# End Source File
# Begin Source File

SOURCE=.\dllentry.cpp
# End Source File
# Begin Source File

SOURCE=.\goservletclient.cpp
# End Source File
# Begin Source File

SOURCE=.\goservlethost.cpp
# End Source File
# Begin Source File

SOURCE=.\gosession.cpp
# End Source File
# Begin Source File

SOURCE=.\gwapiprx.cpp
# End Source File
# Begin Source File

SOURCE=.\processclient.cpp
# End Source File
# Begin Source File

SOURCE=.\redirectorstate.cpp
# End Source File
# Begin Source File

SOURCE=.\sessionclient.cpp
# End Source File
# Begin Source File

SOURCE=.\sessionhost.cpp
# End Source File
# Begin Source File

SOURCE=.\sigcache.cpp
# End Source File
# Begin Source File

SOURCE=.\sigdlselect.cpp
# End Source File
# Begin Source File

SOURCE=.\sigfile.cpp
# End Source File
# Begin Source File

SOURCE=.\sigfilehnd.cpp
# End Source File
# Begin Source File

SOURCE=.\stringmap.cpp
# End Source File
# Begin Source File

SOURCE=.\strutil.cpp
# End Source File
# Begin Source File

SOURCE=.\utilexception.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\agetsamp.h
# End Source File
# Begin Source File

SOURCE=.\agetsig.h
# End Source File
# Begin Source File

SOURCE=.\agetstat.h
# End Source File
# Begin Source File

SOURCE=.\apstsig.h
# End Source File
# Begin Source File

SOURCE=.\apststat.h
# End Source File
# Begin Source File

SOURCE=.\asubsamp.h
# End Source File
# Begin Source File

SOURCE=.\avisredirector.h
# End Source File
# Begin Source File

SOURCE=.\AVISServlets.h
# End Source File
# Begin Source File

SOURCE=.\critsec.h
# End Source File
# Begin Source File

SOURCE=.\dbconn.h
# End Source File
# Begin Source File

SOURCE=.\dbconnhnd.h
# End Source File
# Begin Source File

SOURCE=.\dbconpool.h
# End Source File
# Begin Source File

SOURCE=.\dllentry.h
# End Source File
# Begin Source File

SOURCE=.\goservletclient.h
# End Source File
# Begin Source File

SOURCE=.\goservlethost.h
# End Source File
# Begin Source File

SOURCE=.\gosession.h
# End Source File
# Begin Source File

SOURCE=.\goxactn.h
# End Source File
# Begin Source File

SOURCE=.\gwapiprx.h
# End Source File
# Begin Source File

SOURCE=.\htapi.h
# End Source File
# Begin Source File

SOURCE=.\lckobj.h
# End Source File
# Begin Source File

SOURCE=.\lock.h
# End Source File
# Begin Source File

SOURCE=.\maputil.h
# End Source File
# Begin Source File

SOURCE=.\mutex.h
# End Source File
# Begin Source File

SOURCE=.\namedmutex.h
# End Source File
# Begin Source File

SOURCE=.\processclient.h
# End Source File
# Begin Source File

SOURCE=.\redirectorstate.h
# End Source File
# Begin Source File

SOURCE=.\sessionclient.h
# End Source File
# Begin Source File

SOURCE=.\sessionhost.h
# End Source File
# Begin Source File

SOURCE=.\sigcache.h
# End Source File
# Begin Source File

SOURCE=.\sigdlselect.h
# End Source File
# Begin Source File

SOURCE=.\sigfile.h
# End Source File
# Begin Source File

SOURCE=.\sigfilehnd.h
# End Source File
# Begin Source File

SOURCE=.\stringmap.h
# End Source File
# Begin Source File

SOURCE=.\strutil.h
# End Source File
# Begin Source File

SOURCE=.\traceclient.h
# End Source File
# Begin Source File

SOURCE=.\types.h
# End Source File
# Begin Source File

SOURCE=.\utilexception.h
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
