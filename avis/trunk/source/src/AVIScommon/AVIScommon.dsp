# Microsoft Developer Studio Project File - Name="AVIScommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AVIScommon - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AVIScommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVIScommon.mak" CFG="AVIScommon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVIScommon - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVIScommon - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AVIScommon - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISCOMMON_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I ".\handle" /I "..\log" /I "..\system" /I "..\profile" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISCOMMON_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "AVIScommon - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISCOMMON_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I ".\handle" /I "..\log" /I "..\system" /I "..\profile" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISCOMMON_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "AVIScommon - Win32 Release"
# Name "AVIScommon - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AppInstance.cpp
# End Source File
# Begin Source File

SOURCE=.\AttributeKeys.cpp
# End Source File
# Begin Source File

SOURCE=.\AttributesParser.cpp
# End Source File
# Begin Source File

SOURCE=.\AVIScommon.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISException.cpp
# End Source File
# Begin Source File

SOURCE=.\log\cfgprof.cpp
# End Source File
# Begin Source File

SOURCE=.\system\CGrowableThreadPool.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclAutoLock.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclAutoPtr.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclCritSec.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclEvent.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclGlobal.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclKernel.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclMailbox.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclMonitor.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclMutex.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclSemaphore.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclSharedMemory.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclThread.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\CMclWaitableCollection.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\system\cservice.cpp
# End Source File
# Begin Source File

SOURCE=.\system\CServiceInstall.cpp
# End Source File
# Begin Source File

SOURCE=.\DateTime.cpp
# End Source File
# Begin Source File

SOURCE=.\DateTimeException.cpp
# End Source File
# Begin Source File

SOURCE=.\system\EventLog.cpp
# End Source File
# Begin Source File

SOURCE=.\ExceptionTyper.cpp
# End Source File
# Begin Source File

SOURCE=.\log\Logger.cpp
# End Source File
# Begin Source File

SOURCE=.\system\MappedMemory.cpp
# End Source File
# Begin Source File

SOURCE=.\system\MapView.cpp
# End Source File
# Begin Source File

SOURCE=.\log\mtrace.cpp
# End Source File
# Begin Source File

SOURCE=.\log\stacktrace.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\log\stringmap.cpp
# End Source File
# Begin Source File

SOURCE=.\log\strutil.cpp
# End Source File
# Begin Source File

SOURCE=.\system\SystemException.cpp
# End Source File
# Begin Source File

SOURCE=.\log\traceclient.cpp
# End Source File
# Begin Source File

SOURCE=.\log\tracesinkfile.cpp
# End Source File
# Begin Source File

SOURCE=.\XAnalysisState.cpp
# End Source File
# Begin Source File

SOURCE=.\XError.cpp
# End Source File
# Begin Source File

SOURCE=.\XScanResults.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AppInstance.h
# End Source File
# Begin Source File

SOURCE=.\AttributeKeys.h
# End Source File
# Begin Source File

SOURCE=.\AttributesParser.h
# End Source File
# Begin Source File

SOURCE=.\AVIS.h
# End Source File
# Begin Source File

SOURCE=.\AVIScommon.h
# End Source File
# Begin Source File

SOURCE=.\AVISException.h
# End Source File
# Begin Source File

SOURCE=.\log\cfgprof.h
# End Source File
# Begin Source File

SOURCE=.\system\CGrowableThreadPool.h
# End Source File
# Begin Source File

SOURCE=.\system\CMcl.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclAutoLock.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclAutoPtr.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclCritSec.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclEvent.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclGlobal.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclKernel.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclLinkedLists.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclMailbox.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclMonitor.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclMutex.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclSemaphore.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclSharedMemory.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclThread.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclWaitableCollection.h
# End Source File
# Begin Source File

SOURCE=.\system\CMclWaitableObject.h
# End Source File
# Begin Source File

SOURCE=.\log\critsec.h
# End Source File
# Begin Source File

SOURCE=.\system\cservice.h
# End Source File
# Begin Source File

SOURCE=.\system\CServiceInstall.h
# End Source File
# Begin Source File

SOURCE=.\system\CThreadPool.h
# End Source File
# Begin Source File

SOURCE=.\DateTime.h
# End Source File
# Begin Source File

SOURCE=.\DateTimeException.h
# End Source File
# Begin Source File

SOURCE=.\system\EventLog.h
# End Source File
# Begin Source File

SOURCE=.\ExceptionTyper.h
# End Source File
# Begin Source File

SOURCE=.\log\FileReporter.h
# End Source File
# Begin Source File

SOURCE=.\system\Handle.h
# End Source File
# Begin Source File

SOURCE=.\log\lckobj.h
# End Source File
# Begin Source File

SOURCE=.\log\lock.h
# End Source File
# Begin Source File

SOURCE=.\log\Logger.h
# End Source File
# Begin Source File

SOURCE=.\system\MappedMemory.h
# End Source File
# Begin Source File

SOURCE=.\system\MapView.h
# End Source File
# Begin Source File

SOURCE=.\log\mtrace.h
# End Source File
# Begin Source File

SOURCE=.\log\namedmutex.h
# End Source File
# Begin Source File

SOURCE=.\log\Reporter.h
# End Source File
# Begin Source File

SOURCE=.\log\stacktrace.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\log\stringmap.h
# End Source File
# Begin Source File

SOURCE=.\log\strutil.h
# End Source File
# Begin Source File

SOURCE=.\system\SystemException.h
# End Source File
# Begin Source File

SOURCE=.\log\traceclient.h
# End Source File
# Begin Source File

SOURCE=.\log\tracesink.h
# End Source File
# Begin Source File

SOURCE=.\log\tracesinkfile.h
# End Source File
# Begin Source File

SOURCE=.\log\types.h
# End Source File
# Begin Source File

SOURCE=.\log\utilexception.h
# End Source File
# Begin Source File

SOURCE=.\XAnalysisState.h
# End Source File
# Begin Source File

SOURCE=.\XError.h
# End Source File
# Begin Source File

SOURCE=.\XScanResults.h
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
