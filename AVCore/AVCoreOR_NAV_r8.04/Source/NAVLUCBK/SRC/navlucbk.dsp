# Microsoft Developer Studio Project File - Name="NAVLUCBK" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NAVLUCBK - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NAVLUCBK.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NAVLUCBK.mak" CFG="NAVLUCBK - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NAVLUCBK - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NAVLUCBK - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "NAVLUCBK"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
/Dstrcmpi=_strcmpi /Dstricmp=_stricmp /Dwcsicmp=_wcsicmp /Dwcsnicmp=_wcsnicmp /DDBG=0 /DDEVL=1 /DFPO=1 /DIS_32 /D_M_X86 /D_X86_
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Od /I "..\..\..\..\nobuilds\LiveSubscribe\include" /I "..\..\..\..\nobuilds\LiveUpdate\include" /I "..\..\LIVEUPDATEI\SRC" /I "R:\AVCOMP\DEFUTILS\RELEASE.004\INCLUDE" /I "..\..\include\src" /I "l:\source\include\src" /I "..\..\..\include" /I "..\..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "SYM_WIN32" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "Release" /i "..\..\include\src" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 n32cores.lib S32DEBUG.lib s32navo.lib n32call.lib defutils.lib defannty.lib chan32i.lib patch32i.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"r:\avcomp\defutils\release.004\lib.ira" /libpath:"..\..\lib.ira" /libpath:"l:\source\lib.ira" /libpath:"R:\AVCOMP\CHANNEL\RELEASE.001\LIB.IRA" /libpath:"R:\AVCOMP\PATCHAPP\RELEASE.001\LIB.IRA" /libpath:"..\..\..\..\nobuilds\defannty\release" /libpath:"t:\source\lib.ira"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\Release\*.dll ..\..\bin.ira
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
/Dstrcmpi=_strcmpi /Dstricmp=_stricmp /Dwcsicmp=_wcsicmp /Dwcsnicmp=_wcsnicmp /DDBG=0 /DDEVL=1 /DFPO=1 /DIS_32 /D_M_X86 /D_X86_
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\LiveSubscribe\include" /I "..\..\..\..\nobuilds\LiveSubscribe\include" /I "..\..\..\..\nobuilds\LiveUpdate\include" /I "..\..\LIVEUPDATEI\SRC" /I "R:\AVCOMP\DEFUTILS\RELEASE.004\INCLUDE" /I "..\..\include\src" /I "l:\source\include\src" /I "..\..\..\..\include" /D "_DEBUG" /D "SYM_WIN" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "SYM_WIN32" /D "SYM_DEBUG" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "Debug" /i "..\..\include\src" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 n32cores.lib S32DEBUG.lib s32navo.lib n32call.lib defutils.lib defannty.lib chan32i.lib patch32i.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"r:\avcomp\defutils\release.004\lib.ida" /libpath:"..\..\lib.ida" /libpath:"l:\source\lib.ida" /libpath:"R:\AVCOMP\DEFUTILS\RELEASE.004\LIB.IRA" /libpath:"N:\LIB.IRA" /libpath:"S:\LIB.IRA" /libpath:"S:\NAVLUCBK\LIB.IRA" /libpath:"N:\NAVLUCBK\LIB.IRA" /libpath:"L:\ENGLISH\LIB.IRA" /libpath:"L:\SOURCE\LIB.IRA" /libpath:"T:\source\lib.ida" /libpath:"R:\AVCOMP\CHANNEL\RELEASE.001\LIB.IRA" /libpath:"R:\AVCOMP\PATCHAPP\RELEASE.001\LIB.IRA" /libpath:"..\..\..\..\nobuilds\defannty\release"
# SUBTRACT LINK32 /pdb:none /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\Debug\*.dll ..\..\bin.ida
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "NAVLUCBK - Win32 Release"
# Name "NAVLUCBK - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\LSUIHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\luNavCallBack.cpp
# End Source File
# Begin Source File

SOURCE=.\Myutils.cpp
# End Source File
# Begin Source File

SOURCE=.\NavLu.cpp
# End Source File
# Begin Source File

SOURCE=.\navlucallback.idl
# ADD MTL /tlb "NavLuCallback.tlb" /h "NavLuCallback.h" /iid "NavLuCallback_i.c"
# End Source File
# Begin Source File

SOURCE=.\NAVLUCBK.cpp
# End Source File
# Begin Source File

SOURCE=.\NAVLUCBK.def
# End Source File
# Begin Source File

SOURCE=.\NAVLUCBK.rc

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"

# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
# SUBTRACT RSC /i "Release"

!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"

# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
# SUBTRACT RSC /i "Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\navluprog.cpp
# End Source File
# Begin Source File

SOURCE=.\navluutils.cpp

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"

!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"

# ADD CPP /I "\\USSM-DEV1\VOL1\NAV\AVCORE\AVCOREORNAV7\SOURCE\include\src" /I "\\USSM-DEV1\VOL1\NAV\AVCORE\AVCOREORNAV7\SOURCE\LIVEUPDATEI\SRC"
# SUBTRACT CPP /X

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\progdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LSUIHelper.h
# End Source File
# Begin Source File

SOURCE=.\luNavCallBack.h
# End Source File
# Begin Source File

SOURCE=.\MyUtils.h
# End Source File
# Begin Source File

SOURCE=.\NavLu.h
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

SOURCE=.\luNavCallBack.rgs
# End Source File
# Begin Source File

SOURCE=.\res\NAVLUCBK.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
