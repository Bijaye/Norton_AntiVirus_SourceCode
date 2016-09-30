# Microsoft Developer Studio Project File - Name="AVISFile" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AVISFile - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AVISFile.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVISFile.mak" CFG="AVISFile - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVISFile - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVISFile - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AVISFile - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISFILE_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(SRCD)\AVIScommon" /I "." /I ".\base64" /I ".\deflate" /I ".\md5" /I ".\xor" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISFILE_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib AVISCommon.lib /nologo /dll /machine:I386 /libpath:"$(SRCD)\AVISCommon\Debug" /libpath:"$(LIBD)"

!ELSEIF  "$(CFG)" == "AVISFile - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISFILE_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\base64" /I ".\deflate" /I ".\md5" /I ".\xor" /I "$(SRCD)\AVIScommon" /I "." /I ".\defalte" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISFILE_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib AVISCommon.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(SRCD)\AVISCommon\Debug" /libpath:"$(LIBD)"

!ENDIF 

# Begin Target

# Name "AVISFile - Win32 Release"
# Name "AVISFile - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\deflate\Adler32.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\AVISFile.cpp
# End Source File
# Begin Source File

SOURCE=.\AVISFileException.cpp
# End Source File
# Begin Source File

SOURCE=.\Base64Decoder.cpp
# End Source File
# Begin Source File

SOURCE=.\Base64Encoder.cpp
# End Source File
# Begin Source File

SOURCE=.\CompoundDataManipulator.cpp
# End Source File
# Begin Source File

SOURCE=.\Compressor.cpp
# End Source File
# Begin Source File

SOURCE=.\CRCCalculator.cpp
# End Source File
# Begin Source File

SOURCE=.\DataManipulator.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder.cpp
# End Source File
# Begin Source File

SOURCE=.\DeCompressor.cpp
# End Source File
# Begin Source File

SOURCE=.\deflate\DEFLATE.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\deflate\Deflator.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder.cpp
# End Source File
# Begin Source File

SOURCE=.\deflate\INFBLOCK.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\deflate\INFCODES.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\deflate\INFFAST.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\deflate\INFLATE.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\deflate\Inflator.cpp
# End Source File
# Begin Source File

SOURCE=.\deflate\INFTREES.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\deflate\INFUTIL.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\md5\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\md5\MD5Calculator.cpp
# End Source File
# Begin Source File

SOURCE=.\NullDataManipulator.cpp
# End Source File
# Begin Source File

SOURCE=.\Scrambler.cpp
# End Source File
# Begin Source File

SOURCE=.\xor\SimpleScrambler.cpp
# End Source File
# Begin Source File

SOURCE=.\xor\SimpleUnScrambler.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ToolFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\deflate\TREES.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\UnScrambler.cpp
# End Source File
# Begin Source File

SOURCE=.\deflate\ZExcept.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\deflate\ZlibTool.cpp
# End Source File
# Begin Source File

SOURCE=.\deflate\ZUTIL.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AVISFile.h
# End Source File
# Begin Source File

SOURCE=.\AVISFileException.h
# End Source File
# Begin Source File

SOURCE=.\Base64Decoder.h
# End Source File
# Begin Source File

SOURCE=.\Base64Encoder.h
# End Source File
# Begin Source File

SOURCE=.\CompoundDataManipulator.h
# End Source File
# Begin Source File

SOURCE=.\Compressor.h
# End Source File
# Begin Source File

SOURCE=.\CRCCalculator.h
# End Source File
# Begin Source File

SOURCE=.\DataManipulator.h
# End Source File
# Begin Source File

SOURCE=.\Decoder.h
# End Source File
# Begin Source File

SOURCE=.\DeCompressor.h
# End Source File
# Begin Source File

SOURCE=.\deflate\DEFLATE.H
# End Source File
# Begin Source File

SOURCE=.\deflate\Deflator.h
# End Source File
# Begin Source File

SOURCE=.\deflate\INFBLOCK.H
# End Source File
# Begin Source File

SOURCE=.\deflate\INFCODES.H
# End Source File
# Begin Source File

SOURCE=.\deflate\INFFAST.H
# End Source File
# Begin Source File

SOURCE=.\deflate\Inflator.h
# End Source File
# Begin Source File

SOURCE=.\deflate\INFTREES.H
# End Source File
# Begin Source File

SOURCE=.\deflate\INFUTIL.H
# End Source File
# Begin Source File

SOURCE=.\md5\md5.h
# End Source File
# Begin Source File

SOURCE=.\md5\MD5Calculator.h
# End Source File
# Begin Source File

SOURCE=.\NullDataManipulator.h
# End Source File
# Begin Source File

SOURCE=.\Scrambler.h
# End Source File
# Begin Source File

SOURCE=.\xor\SimpleScrambler.h
# End Source File
# Begin Source File

SOURCE=.\xor\SimpleUnScrambler.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ToolFactory.h
# End Source File
# Begin Source File

SOURCE=.\UnScrambler.h
# End Source File
# Begin Source File

SOURCE=.\deflate\ZCONF.H
# End Source File
# Begin Source File

SOURCE=.\deflate\ZExcept.h
# End Source File
# Begin Source File

SOURCE=.\deflate\ZLIB.H
# End Source File
# Begin Source File

SOURCE=.\deflate\ZlibTool.h
# End Source File
# Begin Source File

SOURCE=.\deflate\ZUTIL.H
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
