# Microsoft Developer Studio Project File - Name="chknavfiles" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=chknavfiles - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "chknavfiles.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "chknavfiles.mak" CFG="chknavfiles - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "chknavfiles - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "chknavfiles - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "chknavfiles"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "chknavfiles - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
/Dstrcmpi=_strcmpi /Dstricmp=_stricmp /Dwcsicmp=_wcsicmp /Dwcsnicmp=_wcsnicmp /DDBG=0 /DDEVL=1 /DFPO=1 /DIS_32 /D_M_X86 /D_X86_
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CHKNAVFILES_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\Nobuilds\msvc6sp5\VC98\Include" /I "..\..\Nobuilds\msvc6sp5\VC98\ATL\Include" /I "..\..\Nobuilds\msvc6sp5\VC98\MFC\Include" /I "..\..\SDKs\CommonClient\include" /I "..\..\include" /I "..\..\Nobuilds\PlatSDK\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CHKNAVFILES_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\SDKs\CommonClient\include" /i "..\..\include" /i "..\..\Nobuilds\msvc6sp5\VC98\Include" /i "..\..\Nobuilds\msvc6sp5\VC98\ATL\Include" /i "..\..\Nobuilds\msvc6sp5\VC98\MFC\Include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 toolbox.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"Release/checknav.dll" /libpath:"..\..\SDKs\CommonClient\LIB\lib.ira" /libpath:"..\..\NoBuilds\PlatSDK\lib" /libpath:"..\..\Nobuilds\msvc6sp5\VC98\Lib" /libpath:"..\..\Nobuilds\msvc6sp5\VC98\MFC\Lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\*.dll ..\..\bin.ira
# End Special Build Tool

!ELSEIF  "$(CFG)" == "chknavfiles - Win32 Debug"

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
/Dstrcmpi=_strcmpi /Dstricmp=_stricmp /Dwcsicmp=_wcsicmp /Dwcsnicmp=_wcsnicmp /DDBG=0 /DDEVL=1 /DFPO=1 /DIS_32 /D_M_X86 /D_X86_
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CHKNAVFILES_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\SDKs\CommonClient\include" /I "..\..\include" /I "..\..\Nobuilds\PlatSDK\Include" /I "..\..\Nobuilds\msvc6sp5\VC98\Include" /I "..\..\Nobuilds\msvc6sp5\VC98\ATL\Include" /I "..\..\Nobuilds\msvc6sp5\VC98\MFC\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CHKNAVFILES_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\SDKs\CommonClient\include" /i "..\..\include" /i "..\..\Nobuilds\msvc6sp5\VC98\Include" /i "..\..\Nobuilds\msvc6sp5\VC98\ATL\Include" /i "..\..\Nobuilds\msvc6sp5\VC98\MFC\Include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 toolbox.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug/checknav.dll" /pdbtype:sept /libpath:"..\..\SDKs\CommonClient\LIB\lib.ida" /libpath:"..\..\NoBuilds\PlatSDK\lib" /libpath:"..\..\Nobuilds\msvc6sp5\VC98\Lib" /libpath:"..\..\Nobuilds\msvc6sp5\VC98\MFC\Lib"
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\*.dll ..\..\bin.ida	copy $(OutDir)\*.pdb ..\..\bin.ida
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "chknavfiles - Win32 Release"
# Name "chknavfiles - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\chknavfiles.cpp
# End Source File
# Begin Source File

SOURCE=.\chknavfiles.def
# End Source File
# Begin Source File

SOURCE=.\chknavfiles.rc
# End Source File
# Begin Source File

SOURCE=.\global.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\chknavfiles.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
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
