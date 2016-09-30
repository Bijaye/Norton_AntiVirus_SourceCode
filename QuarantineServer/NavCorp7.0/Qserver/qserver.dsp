# Microsoft Developer Studio Project File - Name="qserver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=qserver - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qserver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qserver.mak" CFG="qserver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qserver - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "qserver - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "qserver"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qserver - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "qserver___Win32_Debug"
# PROP BASE Intermediate_Dir "qserver___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\shared\include" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\shared\include" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo"qserver.res" /i "..\Shared\Include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ws2_32.lib qspak32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"..\shared\BinIntelUnicodeWin32Debug/qserver.exe" /pdbtype:sept /libpath:"..\Shared\LibIntelAnsiWin32Debug"
# ADD LINK32 ws2_32.lib qspak32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"..\shared\BinIntelUnicodeWin32Debug/qserver.exe" /pdbtype:sept /libpath:"..\Shared\LibIntelAnsiWin32Debug"
# Begin Custom Build - Performing registration
OutDir=.\Debug
InputPath=\Perforce\Norton_AntiVirus\QuarantineServer\shared\BinIntelUnicodeWin32Debug\qserver.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo Building proxy stub DLL 
	nmake -f qserverps.mk 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=call ..\cm\mdir.bat ..\Shared\BinIntelAnsiWin32Debug	copy qserverps.dll ..\Shared\BinIntelAnsiWin32Debug
# End Special Build Tool

!ELSEIF  "$(CFG)" == "qserver - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "qserver___Win32_Release"
# PROP BASE Intermediate_Dir "qserver___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O1 /I "..\shared\include" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "..\shared\include" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\Shared\Include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ws2_32.lib qspak32.lib /nologo /subsystem:windows /machine:I386 /out:"..\shared\BinIntelUnicodeWin32Release/qserver.exe" /libpath:"..\shared\LibIntelAnsiWin32Release"
# ADD LINK32 ws2_32.lib qspak32.lib /nologo /subsystem:windows /machine:I386 /out:"..\shared\BinIntelUnicodeWin32Release/qserver.exe" /libpath:"..\shared\LibIntelAnsiWin32Release"
# Begin Custom Build - Performing registration
OutDir=.\Release
InputPath=\Perforce\Norton_AntiVirus\QuarantineServer\shared\BinIntelUnicodeWin32Release\qserver.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo Building proxy stub DLL 
	nmake -f qserverps.mk 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=call ..\cm\mdir.bat ..\Shared\BinIntelAnsiWin32Release	copy qserverps.dll ..\Shared\BinIntelAnsiWin32Release
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "qserver - Win32 Debug"
# Name "qserver - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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

!IF  "$(CFG)" == "qserver - Win32 Debug"

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

!ELSEIF  "$(CFG)" == "qserver - Win32 Release"

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

SOURCE=.\QuarantineServer.cpp
# End Source File
# Begin Source File

SOURCE=.\QuarantineServerItem.cpp
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
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
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

SOURCE=.\qdefs.h
# End Source File
# Begin Source File

SOURCE=.\QserverConfig.h
# End Source File
# Begin Source File

SOURCE=.\QuarantineServer.h
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
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\CopyItemData.rgs
# End Source File
# Begin Source File

SOURCE=.\EnumQserverItemsVariant.rgs
# End Source File
# Begin Source File

SOURCE=.\qserver.rgs
# End Source File
# Begin Source File

SOURCE=.\QserverConfig.rgs
# End Source File
# Begin Source File

SOURCE=.\QuarantineServer.rgs
# End Source File
# Begin Source File

SOURCE=.\QuarantineServerItem.rgs
# End Source File
# End Group
# End Target
# End Project
