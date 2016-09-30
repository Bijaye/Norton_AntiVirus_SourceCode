# Microsoft Developer Studio Project File - Name="IcePack" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=IcePack - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IcePack.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IcePack.mak" CFG="IcePack - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IcePack - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "IcePack - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IcePack - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I ".\common" /I "$(SRCD)\AVIScommon" /I "$(SRCD)\AVIScommon\system" /I "$(SRCD)\AVIScommon\log" /I "$(SRCD)\AVISdb" /I "$(SRCD)\AVISfile" /I "$(SRCD)\AVISscan\interface" /I "$(SRCD)\AVISClients\IcePackAgentComm" /I "$(SRCD)\AVISClients\AVISTransactions" /I "$(SRCD)\QuarantineAPI" /I "$(SRCD)\inc" /I "$(SRCD)\TopologyAPI\include" /I "$(SRCD)\StripperAPI" /I "$(SRCD)\VDBUnpacker" /I "$(SRCD)\DefCastAPI" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_WIN32_DCOM" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib AVIScommon.lib AVISFile.lib scanInterface.lib IcePackAgentComm.lib stpdc32i.lib VDBUnpacker.lib DefCast.lib /nologo /subsystem:console /machine:I386 /libpath:"$(SRCD)\AVIScommon\Debug" /libpath:"$(SRCD)\AVISFile\Debug" /libpath:"$(SRCD)\AVISScan\interface\debug" /libpath:"$(SRCD)\QuarantineAPI" /libpath:"$(SRCD)\VDBUnpacker" /libpath:"$(SRCD)\DefCastAPI\Debug" /libpath:"$(LIBD)" /libpath:"$(LIBD)\nt"

!ELSEIF  "$(CFG)" == "IcePack - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I ".\common" /I "$(SRCD)\AVIScommon" /I "$(SRCD)\AVIScommon\system" /I "$(SRCD)\AVIScommon\log" /I "$(SRCD)\AVISdb" /I "$(SRCD)\AVISfile" /I "$(SRCD)\AVISscan\interface" /I "$(SRCD)\AVISClients\IcePackAgentComm" /I "$(SRCD)\AVISClients\AVISTransactions" /I "$(SRCD)\QuarantineAPI" /I "$(SRCD)\inc" /I "$(SRCD)\TopologyAPI\include" /I "$(SRCD)\StripperAPI" /I "$(SRCD)\VDBUnpacker" /I "$(SRCD)\DefCastAPI" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_WIN32_DCOM" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib AVIScommon.lib AVISFile.lib scanInterface.lib IcePackAgentComm.lib stpdc32i.lib VDBUnpacker.lib DefCast.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"$(SRCD)\AVIScommon\Debug" /libpath:"$(SRCD)\AVISFile\Debug" /libpath:"$(SRCD)\AVISScan\interface\debug" /libpath:"$(SRCD)\QuarantineAPI" /libpath:"$(SRCD)\VDBUnpacker" /libpath:"$(SRCD)\DefCastAPI\Debug" /libpath:"$(LIBD)" /libpath:"$(LIBD)\nt"

!ENDIF 

# Begin Target

# Name "IcePack - Win32 Release"
# Name "IcePack - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\common\Attention.cpp
# End Source File
# Begin Source File

SOURCE=.\common\BlessedMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\common\CommErrorHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\common\DefTarget.cpp

!IF  "$(CFG)" == "IcePack - Win32 Release"

!ELSEIF  "$(CFG)" == "IcePack - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\GlobalData.cpp
# End Source File
# Begin Source File

SOURCE=.\IcePack.cpp
# End Source File
# Begin Source File

SOURCE=.\common\IcePack.rc
# End Source File
# Begin Source File

SOURCE=.\common\IcePackException.cpp
# End Source File
# Begin Source File

SOURCE=.\IcePackServiceInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\common\JobPackager.cpp
# End Source File
# Begin Source File

SOURCE=.\common\MappedMemCallback.cpp
# End Source File
# Begin Source File

SOURCE=.\common\QuarantineMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\common\RegistryMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\common\Sample.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleBase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\SampleStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\common\SampleStream.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ScannerSubmittor.cpp
# End Source File
# Begin Source File

SOURCE=.\common\StatusMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ThreadControler.cpp
# End Source File
# Begin Source File

SOURCE=.\VQSEnum.cpp
# End Source File
# Begin Source File

SOURCE=.\VQuarantineServer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\common\Attention.h
# End Source File
# Begin Source File

SOURCE=.\common\BlessedMonitor.h
# End Source File
# Begin Source File

SOURCE=.\common\CommErrorHandler.h
# End Source File
# Begin Source File

SOURCE=.\common\GlobalData.h
# End Source File
# Begin Source File

SOURCE=.\common\IcePackException.h
# End Source File
# Begin Source File

SOURCE=.\IcePackServiceInterface.h
# End Source File
# Begin Source File

SOURCE=.\common\JobPackager.h
# End Source File
# Begin Source File

SOURCE=.\common\MappedMemCallback.h
# End Source File
# Begin Source File

SOURCE=.\common\MsgData.h
# End Source File
# Begin Source File

SOURCE=.\common\QuarantineMonitor.h
# End Source File
# Begin Source File

SOURCE=.\common\RegistryMonitor.h
# End Source File
# Begin Source File

SOURCE=.\common\Sample.h
# End Source File
# Begin Source File

SOURCE=.\SampleBase.h
# End Source File
# Begin Source File

SOURCE=.\common\SampleStatus.h
# End Source File
# Begin Source File

SOURCE=.\common\SampleStream.h
# End Source File
# Begin Source File

SOURCE=.\common\ScannerSubmittor.h
# End Source File
# Begin Source File

SOURCE=.\common\StatusMonitor.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ThreadControler.h
# End Source File
# Begin Source File

SOURCE=.\VQSEnum.h
# End Source File
# Begin Source File

SOURCE=.\VQuarantineServer.h
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
