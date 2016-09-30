# Microsoft Developer Studio Project File - Name="qserver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=qserver - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qserver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qserver.mak" CFG="qserver - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qserver - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "qserver - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "qserver"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qserver - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "qserver___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "qserver___Win32_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "qserver___Win32_Unicode_Debug"
# PROP Intermediate_Dir "qserver___Win32_Unicode_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\shared\include" /I "..\include" /I "..\..\..\LDVP_shared\src\pscan\common\src" /I "..\..\..\Core_technology\LDVP_shared\src\include" /I "..\..\..\Release\SESA\AppSupport\applibsrc\include" /I "..\..\..\Release\SESA\AppSupport\include" /I "..\..\..\Release\NavAPI\Include" /I "..\..\..\Release\avcomp\Defutils\include" /D "_DEBUG" /D "UNICODE" /D "SESDEBUG" /D "SY" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "SYM_WIN" /D "SYM_WIN32" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\shared\include" /I "..\include" /I "..\..\..\LDVP_shared\src\pscan\common\src" /I "..\..\..\Core_technology\LDVP_shared\src\include" /I "..\..\..\Release\SESA\AppSupport\applibsrc\include" /I "..\..\..\Release\SESA\AppSupport\include" /I "..\..\..\Release\NavAPI\Include" /I "..\..\..\Release\avcomp\Defutils\include" /D "_DEBUG" /D "UNICODE" /D "SESDEBUG" /D "SY" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "SYM_WIN" /D "SYM_WIN32" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /fo"qserver.res" /i "..\Shared\Include" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /fo"qserver.res" /i "..\Shared\Include" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib navapi32.lib ws2_32.lib qspak32.lib IcePackTokens.lib applib.lib wininet.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"..\shared\BinIntelUnicodeWin32Debug/qserver.exe" /pdbtype:sept /libpath:"..\..\..\Release\Navapi\LibIntelAnsiWin32Debug" /libpath:"..\..\..\release\SESA\appsupport\lib\win32\debug" /libpath:"..\..\..\release\avcomp\defutils\LibWin32AnsiDebug" /libpath:"..\shared\LibIntelAnsiWin32Debug" /libpath:"..\..\LDVP_Shared\src\nobuilds\lib\intelansi\win32\release"
# SUBTRACT BASE LINK32 /verbose /incremental:no
# ADD LINK32 version.lib navapi32.lib ws2_32.lib qspak32.lib IcePackTokens.lib applib.lib wininet.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"..\shared\BinIntelUnicodeWin32Debug/qserver.exe" /pdbtype:sept /libpath:"..\..\..\Release\Navapi\LibIntelAnsiWin32Debug" /libpath:"..\..\..\release\SESA\appsupport\lib\win32\debug" /libpath:"..\..\..\release\avcomp\defutils\LibWin32AnsiDebug" /libpath:"..\shared\LibIntelAnsiWin32Debug" /libpath:"..\..\LDVP_Shared\src\nobuilds\lib\intelansi\win32\release"
# SUBTRACT LINK32 /verbose /incremental:no
# Begin Custom Build - Performing registration
OutDir=.\qserver___Win32_Unicode_Debug
InputPath=\work\qserver.trunk\Norton_AntiVirus\QuarantineServer\shared\BinIntelUnicodeWin32Debug\qserver.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo Building proxy stub DLL 
	nmake -f qserverps.mk 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=call ..\cm\mdir.bat ..\Shared\BinIntelAnsiWin32Debug	copy qserverps.dll ..\Shared\BinIntelAnsiWin32Debug	copy qserver.tlb ..\shared\include	copy qserver.h ..\shared\include
# End Special Build Tool

!ELSEIF  "$(CFG)" == "qserver - Win32 Unicode Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "qserver___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "qserver___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "qserver___Win32_Unicode_Release"
# PROP Intermediate_Dir "qserver___Win32_Unicode_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /I "..\shared\include" /I "..\include" /I "..\..\..\LDVP_shared\src\pscan\common\src" /I "..\..\..\Core_technology\LDVP_shared\src\include" /I "..\..\..\Release\SESA\AppSupport\applibsrc\include" /I "..\..\..\Release\SESA\AppSupport\include" /I "..\..\..\Release\NavAPI\Include" /I "..\..\..\Release\avcomp\Defutils\include" /D "NDEBUG" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "SYM_WIN" /D "SYM_WIN32" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "..\shared\include" /I "..\include" /I "..\..\..\LDVP_shared\src\pscan\common\src" /I "..\..\..\Core_technology\LDVP_shared\src\include" /I "..\..\..\Release\SESA\AppSupport\applibsrc\include" /I "..\..\..\Release\SESA\AppSupport\include" /I "..\..\..\Release\NavAPI\Include" /I "..\..\..\Release\avcomp\Defutils\include" /D "NDEBUG" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "SYM_WIN" /D "SYM_WIN32" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /i "..\Shared\Include" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Shared\Include" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 navapi32.lib ws2_32.lib qspak32.lib IcePackTokens.lib applib.lib wininet.lib version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"..\shared\BinIntelUnicodeWin32Release/qserver.exe" /libpath:"..\..\..\Release\Navapi\LibIntelAnsiWin32Release" /libpath:"..\..\..\Release\SESA\AppSupport\lib\win32\release" /libpath:"..\..\..\release\avcomp\defutils\LibWin32AnsiRelease" /libpath:"..\shared\LibIntelAnsiWin32Release" /libpath:"..\..\LDVP_Shared\src\nobuilds\lib\intelansi\win32\release"
# SUBTRACT BASE LINK32 /verbose
# ADD LINK32 navapi32.lib ws2_32.lib qspak32.lib IcePackTokens.lib applib.lib wininet.lib version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"..\shared\BinIntelUnicodeWin32Release/qserver.exe" /libpath:"..\..\..\Release\Navapi\LibIntelAnsiWin32Release" /libpath:"..\..\..\Release\SESA\AppSupport\lib\win32\release" /libpath:"..\..\..\release\avcomp\defutils\LibWin32AnsiRelease" /libpath:"..\shared\LibIntelAnsiWin32Release" /libpath:"..\..\LDVP_Shared\src\nobuilds\lib\intelansi\win32\release"
# SUBTRACT LINK32 /verbose
# Begin Custom Build - Performing registration
OutDir=.\qserver___Win32_Unicode_Release
InputPath=\work\qserver.trunk\Norton_AntiVirus\QuarantineServer\shared\BinIntelUnicodeWin32Release\qserver.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo Building proxy stub DLL 
	nmake -f qserverps.mk 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=call ..\cm\mdir.bat ..\Shared\BinIntelAnsiWin32Release	copy qserverps.dll ..\Shared\BinIntelAnsiWin32Release	copy qserver.tlb ..\shared\include	copy qserver.h ..\shared\include
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "qserver - Win32 Unicode Debug"
# Name "qserver - Win32 Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ams.cpp
# End Source File
# Begin Source File

SOURCE=.\AvisConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\AVScanner.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyItemData.cpp
# End Source File
# Begin Source File

SOURCE=.\Enumerator.cpp
# End Source File
# Begin Source File

SOURCE=.\EnumQserverItemsVariant.cpp
# End Source File
# Begin Source File

SOURCE=.\eresource.cpp
# End Source File
# Begin Source File

SOURCE=.\FileNameParse.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralEventDetection.cpp
# End Source File
# Begin Source File

SOURCE=.\ModulVer.cpp
# End Source File
# Begin Source File

SOURCE=.\NavAPICallbacks.cpp
# End Source File
# Begin Source File

SOURCE=.\ProccessSamples.cpp
# End Source File
# Begin Source File

SOURCE=.\qserver.cpp
# End Source File
# Begin Source File

SOURCE=.\qserver.idl
# ADD BASE MTL /tlb ".\qserver.tlb" /h "qserver.h" /iid "qserver_i.c" /Oicf
# ADD MTL /tlb ".\qserver.tlb" /h "qserver.h" /iid "qserver_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\qserver.rc
# End Source File
# Begin Source File

SOURCE=.\QserverConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\qservermsg.mc

!IF  "$(CFG)" == "qserver - Win32 Unicode Debug"

# Begin Custom Build - Compiling message file $(InputPath)
InputPath=.\qservermsg.mc
InputName=qservermsg

BuildCmds= \
	mc $(InputName)

"qservermsg.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"qservermsg.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "qserver - Win32 Unicode Release"

# Begin Custom Build - Compiling message file $(InputPath)
InputPath=.\qservermsg.mc
InputName=qservermsg

BuildCmds= \
	mc $(InputName)

"qservermsg.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"qservermsg.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\QServerSavInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\QsIsPlatinumCustomer.cpp
# End Source File
# Begin Source File

SOURCE=.\QSSesEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\QuarantineServer.cpp
# End Source File
# Begin Source File

SOURCE=.\QuarantineServerII.cpp
# End Source File
# Begin Source File

SOURCE=.\QuarantineServerItem.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleEventDetection.cpp
# End Source File
# Begin Source File

SOURCE=.\serverthreads.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\threadpool.cpp
# End Source File
# Begin Source File

SOURCE=.\util.c

!IF  "$(CFG)" == "qserver - Win32 Unicode Debug"

# SUBTRACT BASE CPP /D "UNICODE" /YX /Yc /Yu
# SUBTRACT CPP /D "UNICODE" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "qserver - Win32 Unicode Release"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AvisConfig.h
# End Source File
# Begin Source File

SOURCE=.\AVScanner.h
# End Source File
# Begin Source File

SOURCE=.\const.h
# End Source File
# Begin Source File

SOURCE=.\CopyItemData.h
# End Source File
# Begin Source File

SOURCE=.\Enumerator.h
# End Source File
# Begin Source File

SOURCE=.\EnumQserverItemsVariant.h
# End Source File
# Begin Source File

SOURCE=.\eresource.h
# End Source File
# Begin Source File

SOURCE=.\EventData.h
# End Source File
# Begin Source File

SOURCE=.\EventDetection.h
# End Source File
# Begin Source File

SOURCE=.\FileNameParse.h
# End Source File
# Begin Source File

SOURCE=.\NavAPICallbacks.h
# End Source File
# Begin Source File

SOURCE=.\ProccessSamples.h
# End Source File
# Begin Source File

SOURCE=.\qdefs.h
# End Source File
# Begin Source File

SOURCE=.\QserverConfig.h
# End Source File
# Begin Source File

SOURCE=.\QServerSavInfo.h
# End Source File
# Begin Source File

SOURCE=.\QSSesEvent.h
# End Source File
# Begin Source File

SOURCE=.\QuarantineServer.h
# End Source File
# Begin Source File

SOURCE=.\QuarantineServerII.h
# End Source File
# Begin Source File

SOURCE=.\QuarantineServerItem.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\serverthreads.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\threadpool.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\AvisConfig.rgs
# End Source File
# Begin Source File

SOURCE=.\CopyItemData.rgs
# End Source File
# Begin Source File

SOURCE=.\EnumQserverItemsVariant.rgs
# End Source File
# Begin Source File

SOURCE=.\EnumSavVirusInfo.rgs
# End Source File
# Begin Source File

SOURCE=.\qserver.rgs
# End Source File
# Begin Source File

SOURCE=.\QserverConfig.rgs
# End Source File
# Begin Source File

SOURCE=.\QServerSavInfo.rgs
# End Source File
# Begin Source File

SOURCE=.\QuarantineServer.rgs
# End Source File
# Begin Source File

SOURCE=.\QuarantineServerII.rgs
# End Source File
# Begin Source File

SOURCE=.\QuarantineServerItem.rgs
# End Source File
# End Group
# End Target
# End Project
