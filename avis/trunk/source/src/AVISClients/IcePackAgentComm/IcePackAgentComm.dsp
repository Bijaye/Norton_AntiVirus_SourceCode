# Microsoft Developer Studio Project File - Name="IcePackAgentComm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=IcePackAgentComm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IcePackAgentComm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IcePackAgentComm.mak" CFG="IcePackAgentComm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IcePackAgentComm - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IcePackAgentComm - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IcePackAgentComm - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ICEPACKAGENTCOMM_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(SRCD)\AVISCommon\log" /I "$(SRCD)\AVISCommon" /I "$(SRCD)\VDBUnpacker" /I "$(SRCD)\AVISunpack\FatAlbertUnpack" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ICEPACKAGENTCOMM_EXPORTS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 AVISCommon.lib VDBUnpacker.lib wininet.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /machine:I386 /libpath:"$(LIBD)"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "IcePackAgentComm - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ICEPACKAGENTCOMM_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SRCD)\AVISCommon\log" /I "$(SRCD)\AVISCommon" /I "$(SRCD)\VDBUnpacker" /I "$(SRCD)\AVISunpack\FatAlbertUnpack" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ICEPACKAGENTCOMM_EXPORTS" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AVISCommon.lib VDBUnpacker.lib wininet.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(LIBD)"
# SUBTRACT LINK32 /pdb:none /map /nodefaultlib

!ENDIF 

# Begin Target

# Name "IcePackAgentComm - Win32 Release"
# Name "IcePackAgentComm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AutoEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoHandle.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoMutex.cpp
# End Source File
# Begin Source File

SOURCE=.\CommClient.cpp
# End Source File
# Begin Source File

SOURCE=.\HttpConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\HttpFile.cpp
# End Source File
# Begin Source File

SOURCE=.\IcePackAgentComm.rc
# End Source File
# Begin Source File

SOURCE=.\InternetException.cpp
# End Source File
# Begin Source File

SOURCE=.\InternetSession.cpp
# End Source File
# Begin Source File

SOURCE=.\InternetUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\MyTraceClient.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleComm.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleUploader.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleUploaderTransaction.cpp
# End Source File
# Begin Source File

SOURCE=.\SignatureComm.cpp
# End Source File
# Begin Source File

SOURCE=.\SignatureImporter.cpp
# End Source File
# Begin Source File

SOURCE=.\SignatureImporterTransaction.cpp
# End Source File
# Begin Source File

SOURCE=.\SignatureInquirer.cpp
# End Source File
# Begin Source File

SOURCE=.\SignatureInquirerTransaction.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusReceiver.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusReceiverTransaction.cpp
# End Source File
# Begin Source File

SOURCE=.\StrStrMap.cpp
# End Source File
# Begin Source File

SOURCE=.\StrUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\StrVector.cpp
# End Source File
# Begin Source File

SOURCE=.\TagValuePair.cpp
# End Source File
# Begin Source File

SOURCE=.\Thread.cpp
# End Source File
# Begin Source File

SOURCE=.\TraceThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Transaction.cpp
# End Source File
# Begin Source File

SOURCE=.\TransactionExecutor.cpp
# End Source File
# Begin Source File

SOURCE=.\TransactionSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Util.cpp
# End Source File
# Begin Source File

SOURCE=.\ZDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\ZExcept.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AutoEvent.h
# End Source File
# Begin Source File

SOURCE=.\AutoHandle.h
# End Source File
# Begin Source File

SOURCE=.\AutoMutex.h
# End Source File
# Begin Source File

SOURCE=.\base.h
# End Source File
# Begin Source File

SOURCE=.\CommClient.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\HttpConnection.h
# End Source File
# Begin Source File

SOURCE=.\HttpFile.h
# End Source File
# Begin Source File

SOURCE=.\IcePackAgentComm.h
# End Source File
# Begin Source File

SOURCE=.\InternetException.h
# End Source File
# Begin Source File

SOURCE=.\InternetSession.h
# End Source File
# Begin Source File

SOURCE=.\InternetUtil.h
# End Source File
# Begin Source File

SOURCE=.\MyTraceClient.h
# End Source File
# Begin Source File

SOURCE=.\SampleUploader.h
# End Source File
# Begin Source File

SOURCE=.\SampleUploaderTransaction.h
# End Source File
# Begin Source File

SOURCE=.\SignatureImporter.h
# End Source File
# Begin Source File

SOURCE=.\SignatureImporterTransaction.h
# End Source File
# Begin Source File

SOURCE=.\SignatureInquirer.h
# End Source File
# Begin Source File

SOURCE=.\SignatureInquirerTransaction.h
# End Source File
# Begin Source File

SOURCE=.\StatusReceiver.h
# End Source File
# Begin Source File

SOURCE=.\StatusReceiverTransaction.h
# End Source File
# Begin Source File

SOURCE=.\StrStrMap.h
# End Source File
# Begin Source File

SOURCE=.\StrUtil.h
# End Source File
# Begin Source File

SOURCE=.\StrVector.h
# End Source File
# Begin Source File

SOURCE=.\TagValuePair.h
# End Source File
# Begin Source File

SOURCE=.\Thread.h
# End Source File
# Begin Source File

SOURCE=.\TraceThread.h
# End Source File
# Begin Source File

SOURCE=.\Transaction.h
# End Source File
# Begin Source File

SOURCE=.\TransactionExecutor.h
# End Source File
# Begin Source File

SOURCE=.\TransactionSettings.h
# End Source File
# Begin Source File

SOURCE=.\Util.h
# End Source File
# Begin Source File

SOURCE=.\ZDirectory.h
# End Source File
# Begin Source File

SOURCE=.\ZExcept.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
