# Microsoft Developer Studio Project File - Name="QSPak32ForNAVMSE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (ALPHA) Dynamic-Link Library" 0x0602

CFG=QSPak32ForNAVMSE - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "QSPak32ForNAVMSE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "QSPak32ForNAVMSE.mak" CFG="QSPak32ForNAVMSE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "QSPak32ForNAVMSE - Win32 (ALPHA) Debug" (based on\
 "Win32 (ALPHA) Dynamic-Link Library")
!MESSAGE "QSPak32ForNAVMSE - Win32 (ALPHA) Release" (based on\
 "Win32 (ALPHA) Dynamic-Link Library")
!MESSAGE "QSPak32ForNAVMSE - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "QSPak32ForNAVMSE - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "QSPak32ForNAVMSE - Win32 SymTrial Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "QSPak32ForNAVMSE - Win32 ALPHA SymTrial Release" (based on\
 "Win32 (ALPHA) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName "QSPak32ForNAVMSE"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 (ALPHA) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "QSPak32ForNAVMSE___Win32__ALPHA__Debug"
# PROP BASE Intermediate_Dir "QSPak32ForNAVMSE___Win32__ALPHA__Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Build\ADA"
# PROP Intermediate_Dir "ADA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /Gt0 /W3 /GX /Zi /Od /I "..\shared\include" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QSPAK32_EXPORTS" /D "BUILDING_QSPAK" /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /Gt0 /W3 /GX /Zi /Od /I "..\shared\include" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QSPAK32_EXPORTS" /D "BUILDING_QSPAK" /YX"stdafx.h" /Ge /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /debug /machine:ALPHA /out:"AlphaDbg/QSPak32.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /debug /machine:ALPHA /out:"..\..\..\Build\ADA/QSPak32.dll"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 (ALPHA) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "QSPak32ForNAVMSE___Win32__ALPHA__Release"
# PROP BASE Intermediate_Dir "QSPak32ForNAVMSE___Win32__ALPHA__Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Build\ARA"
# PROP Intermediate_Dir "ARA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /W3 /GX /O2 /I "..\shared\include" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QSPAK32_EXPORTS" /D "BUILDING_QSPAK" /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MD /Gt0 /W3 /GX /Zi /O2 /I "..\shared\include" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QSPAK32_EXPORTS" /D "BUILDING_QSPAK" /YX"stdafx.h" /Ge /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /machine:ALPHA /out:"AlphaRel/QSPak32.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /machine:ALPHA /out:"..\..\..\Build\ARA/QSPak32.dll"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "QSPak32ForNAVMSE___Win32_Debug"
# PROP BASE Intermediate_Dir "QSPak32ForNAVMSE___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Build\IDA"
# PROP Intermediate_Dir "IDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\shared\include" /I "..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QSPAK32_EXPORTS" /D "BUILDING_QSPAK" /YX"stdafx.h" /FD /GZ /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /out:"AlphaDbg/QSPak32.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /incremental:no
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /out:"..\..\..\Build\IDA/QSPak32.dll" /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "QSPak32ForNAVMSE___Win32_Release"
# PROP BASE Intermediate_Dir "QSPak32ForNAVMSE___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Build\IRA"
# PROP Intermediate_Dir "IRA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\shared\include" /I "..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QSPAK32_EXPORTS" /D "BUILDING_QSPAK" /YX"stdafx.h" /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:IX86 /out:"AlphaRel/QSPak32.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:IX86 /out:"..\..\..\Build\IRA/QSPak32.dll"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 SymTrial Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "QSPak32F"
# PROP BASE Intermediate_Dir "QSPak32F"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Build\SymTrial.IRA"
# PROP Intermediate_Dir "SymTrial.IRA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\shared\include" /I "..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QSPAK32_EXPORTS" /D "BUILDING_QSPAK" /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\shared\include" /I "..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QSPAK32_EXPORTS" /D "BUILDING_QSPAK" /YX"stdafx.h" /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:IX86 /out:"..\..\..\Build\IRA/QSPak32.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:IX86 /out:"..\..\..\Build\IRA/QSPak32.dll"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 ALPHA SymTrial Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "QSPak32F"
# PROP BASE Intermediate_Dir "QSPak32F"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Build\SymTrial.ARA"
# PROP Intermediate_Dir "SymTrial.ARA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /W3 /GX /Zi /O2 /I "..\shared\include" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QSPAK32_EXPORTS" /D "BUILDING_QSPAK" /YX"stdafx.h" /Ge /FD /c
# ADD CPP /nologo /MD /Gt0 /W3 /GX /Zi /O2 /I "..\shared\include" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QSPAK32_EXPORTS" /D "BUILDING_QSPAK" /YX"stdafx.h" /Ge /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /machine:ALPHA /out:"..\..\..\Build\ARA/QSPak32.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /machine:ALPHA /out:"..\..\..\Build\SymTrial.ARA\QSPak32.dll"

!ENDIF 

# Begin Target

# Name "QSPak32ForNAVMSE - Win32 (ALPHA) Debug"
# Name "QSPak32ForNAVMSE - Win32 (ALPHA) Release"
# Name "QSPak32ForNAVMSE - Win32 Debug"
# Name "QSPak32ForNAVMSE - Win32 Release"
# Name "QSPak32ForNAVMSE - Win32 SymTrial Release"
# Name "QSPak32ForNAVMSE - Win32 ALPHA SymTrial Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\package.cpp

!IF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 (ALPHA) Debug"

DEP_CPP_PACKA=\
	"..\shared\include\qspak.h"\
	".\qstructs.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 (ALPHA) Release"

DEP_CPP_PACKA=\
	"..\shared\include\qspak.h"\
	".\qstructs.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 Debug"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 Release"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 SymTrial Release"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 ALPHA SymTrial Release"

DEP_CPP_PACKA=\
	"..\shared\include\qspak.h"\
	".\qstructs.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qspak32.cpp

!IF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 (ALPHA) Debug"

DEP_CPP_QSPAK=\
	".\StdAfx.h"\
	

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 (ALPHA) Release"

DEP_CPP_QSPAK=\
	".\StdAfx.h"\
	

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 Debug"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 Release"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 SymTrial Release"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 ALPHA SymTrial Release"

DEP_CPP_QSPAK=\
	".\StdAfx.h"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 (ALPHA) Debug"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 (ALPHA) Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 Debug"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 Release"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 SymTrial Release"

!ELSEIF  "$(CFG)" == "QSPak32ForNAVMSE - Win32 ALPHA SymTrial Release"

DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\qstructs.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# End Target
# End Project
