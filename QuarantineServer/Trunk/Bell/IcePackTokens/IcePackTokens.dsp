# Microsoft Developer Studio Project File - Name="IcePackTokens" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=IcePackTokens - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IcePackTokens.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IcePackTokens.mak" CFG="IcePackTokens - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IcePackTokens - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IcePackTokens - Win32 Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "IcePackTokens"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IcePackTokens - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "IcePackTokens___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "IcePackTokens___Win32_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\shared\include" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\shared\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /debug /debugtype:both /machine:I386 /nodefaultlib:"LIBCMT.LIB" /nodefaultlib:"LIBCMTD.LIB" /out:"..\..\shared\BinIntelAnsiWin32Debug//IcePackTokens.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /debug /debugtype:both /machine:I386 /nodefaultlib:"LIBCMT.LIB" /nodefaultlib:"LIBCMTD.LIB" /out:"..\..\shared\BinIntelAnsiWin32Debug//IcePackTokens.dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=call ..\..\cm\mdir.bat ..\..\shared\BinIntelAnsiWin32Debug	call ..\..\cm\mdir.bat ..\..\shared\BinIntelUnicodeWin32Debug	call ..\..\cm\mdir.bat ..\..\shared\LibIntelUnicodeWin32Debug	call ..\..\cm\mdir.bat ..\..\shared\LibIntelAnsiWin32Debug	copy  ..\..\Shared\BinIntelAnsiWin32Debug\IcePackTokens.dll   ..\..\Shared\BinIntelUnicodeWin32Debug	copy  Debug\IcePackTokens.lib   ..\..\Shared\LibIntelAnsiWin32Debug	copy  Debug\IcePackTokens.lib   ..\..\Shared\LibIntelUnicodeWin32Debug
# End Special Build Tool

!ELSEIF  "$(CFG)" == "IcePackTokens - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "IcePackTokens___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "IcePackTokens___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\shared\include" /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\shared\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /nodefaultlib:"LIBCMT.LIB" /nodefaultlib:"LIBCMTD.LIB" /out:"..\..\shared\BinIntelAnsiWin32Release/IcePackTokens.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /nodefaultlib:"LIBCMT.LIB" /nodefaultlib:"LIBCMTD.LIB" /out:"..\..\shared\BinIntelAnsiWin32Release/IcePackTokens.dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=call ..\..\cm\mdir.bat ..\..\shared\BinIntelAnsiWin32Release	call ..\..\cm\mdir.bat ..\..\shared\BinIntelUnicodeWin32Release	call ..\..\cm\mdir.bat ..\..\shared\LibIntelAnsiWin32Release	call ..\..\cm\mdir.bat ..\..\shared\LibIntelUnicodeWin32Release	copy  release\IcePackTokens.lib   ..\..\Shared\LibIntelAnsiWin32Release	copy  release\IcePackTokens.lib   ..\..\Shared\LibIntelUnicodeWin32Release	copy   ..\..\shared\BinIntelAnsiWin32Release\IcePackTokens.dll    ..\..\Shared\LibIntelUnicodeWin32Release
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "IcePackTokens - Win32 Unicode Debug"
# Name "IcePackTokens - Win32 Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\IcePackTokens.cpp
# End Source File
# Begin Source File

SOURCE=.\IcePackTokens.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\IcePackTokens.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
