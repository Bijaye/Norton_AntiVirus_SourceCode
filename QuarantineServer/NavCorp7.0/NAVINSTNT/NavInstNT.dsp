# Microsoft Developer Studio Project File - Name="NavInstNT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NavInstNT - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NavInstNT.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NavInstNT.mak" CFG="NavInstNT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NavInstNT - Win32 AlphaAnsiWinNTDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NavInstNT - Win32 AlphaAnsiWinNTRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NavInstNT - Win32 IntelAnsiWinNTDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NavInstNT - Win32 IntelAnsiWinNTRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NavInstNT - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NavInstNT - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "NavInstNT"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NavInstNT - Win32 AlphaAnsiWinNTDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AlphaAnsiWinNTDebug"
# PROP BASE Intermediate_Dir "AlphaAnsiWinNTDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "AlphaAnsiWinNTDebug"
# PROP Intermediate_Dir "AlphaAnsiWinNTDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\shared\include" /I "..\..\..\avcore\trunk\source\include\src" /I "..\..\..\core\trunk\source\include\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D Q_VER=\"N\" /D "_ALPHA_" /D "_M_ALPHA" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\shared\include" /i "..\..\shared\include" /i ".." /d "_DEBUG" /d "SYM_WIN" /d "SYM_WIN32" /d "SBCS_DLG" /d "QCUST" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib defutils.lib chan32a.lib progman.lib version.lib svcctrl.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\shared\lib\alphaansi\win32\debug" /libpath:"..\nobuilds\avcore\lib\AlphaUnicode\Win32\Debug" /libpath:"..\nobuilds\inetsdk\lib" /libpath:"..\nobuilds\avnoblds\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy AlphaAnsiWinNTDebug\NavInstNT.dll    ..\shared\bin\AlphaAnsi\WinNT\Debug	copy AlphaAnsiWinNTDebug\NavInstNT.lib    ..\shared\lib\AlphaAnsi\WinNT\Debug
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NavInstNT - Win32 AlphaAnsiWinNTRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AlphaAnsiWinNTRelease"
# PROP BASE Intermediate_Dir "AlphaAnsiWinNTRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "AlphaAnsiWinNTRelease"
# PROP Intermediate_Dir "AlphaAnsiWinNTRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\shared\include" /I "..\..\..\avcore\trunk\source\include\src" /I "..\..\..\core\trunk\source\include\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D Q_VER=\"N\" /D "_ALPHA_" /D "_M_ALPHA" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\shared\include" /i "..\..\shared\include" /i ".." /d "NDEBUG" /d "SYM_WIN" /d "SYM_WIN32" /d "SBCS_DLG" /d "QCUST" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib defutils.lib chan32a.lib progman.lib version.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"..\shared\lib\alphaansi\win32\release" /libpath:"..\nobuilds\avcore\lib\AlphaUnicode\Win32\Release" /libpath:"..\nobuilds\inetsdk\lib" /libpath:"..\nobuilds\avnoblds\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy AlphaAnsiWinNTRelease\NavInstNT.dll    ..\shared\bin\AlphaAnsi\WinNT\Release	copy AlphaAnsiWinNTRelease\NavInstNT.lib    ..\shared\lib\AlphaAnsi\WinNT\Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NavInstNT - Win32 IntelAnsiWinNTDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "IntelAnsiWinNTDebug"
# PROP BASE Intermediate_Dir "IntelAnsiWinNTDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "IntelAnsiWinNTDebug"
# PROP Intermediate_Dir "IntelAnsiWinNTDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /I "..\shared\include" /I "..\..\..\avcore\source\include\src" /I "..\..\..\core\source\include\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D Q_VER=\"N\" /D "_X86_" /D "_M_X86" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\shared\include" /i "..\..\shared\include" /i ".." /d "_DEBUG" /d "SYM_WIN" /d "SYM_WIN32" /d "SBCS_DLG" /d "QCUST" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib defutils.lib chan32i.lib progman.lib version.lib svcctrl.lib svcctrl.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\shared\lib\intelansi\win32\release" /libpath:"..\..\..\avcore\source\nobuilds\src"
# Begin Special Build Tool
OutDir=.\IntelAnsiWinNTDebug
SOURCE="$(InputPath)"
PostBuild_Cmds=Echo $(OutDir)	Echo Copying NavInstNT.dll to Shared\bin...	if not exist ..\Shared\bin\IntelAnsi\Win32\$(OutDir) md ..\Shared\bin\IntelAnsi\Win32\$(OutDir)	copy $(OutDir)\NavInstNT.dll ..\Shared\bin\IntelAnsi\Win32\$(OutDir)\NavInstNT.dll	Echo $(OutDir)	Echo Copying NavInstNT.lib to Shared\lib...	if not exist ..\Shared\lib\IntelAnsi\Win32\$(OutDir) md ..\Shared\lib\IntelAnsi\Win32\$(OutDir)	copy $(OutDir)\NavInstNT.lib ..\Shared\lib\IntelAnsi\Win32\$(OutDir)\NavInstNT.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NavInstNT - Win32 IntelAnsiWinNTRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "IntelAnsiWinNTRelease"
# PROP BASE Intermediate_Dir "IntelAnsiWinNTRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "IntelAnsiWinNTRelease"
# PROP Intermediate_Dir "IntelAnsiWinNTRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /I "..\shared\include" /I "..\..\..\avcore\source\include\src" /I "..\..\..\core\source\include\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D Q_VER=\"N\" /D "_X86_" /D "_M_X86" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\shared\include" /i "..\..\shared\include" /i ".." /d "NDEBUG" /d "SYM_WIN" /d "SYM_WIN32" /d "SBCS_DLG" /d "QCUST" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib defutils.lib chan32i.lib progman.lib version.lib svcctrl.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"..\shared\lib\intelansi\win32\release" /libpath:"..\..\..\avcore\source\nobuilds\src"
# Begin Special Build Tool
OutDir=.\IntelAnsiWinNTRelease
SOURCE="$(InputPath)"
PostBuild_Cmds=Echo $(OutDir)	Echo Copying NavInstNT.dll to Shared\bin...	if not exist ..\Shared\bin\IntelAnsi\Win32\$(OutDir) md ..\Shared\bin\IntelAnsi\Win32\$(OutDir)	copy $(OutDir)\NavInstNT.dll ..\Shared\bin\IntelAnsi\Win32\$(OutDir)\NavInstNT.dll	Echo $(OutDir)	Echo Copying NavInstNT.lib to Shared\lib...	if not exist ..\Shared\lib\IntelAnsi\Win32\$(OutDir) md ..\Shared\lib\IntelAnsi\Win32\$(OutDir)	copy $(OutDir)\NavInstNT.lib ..\Shared\lib\IntelAnsi\Win32\$(OutDir)\NavInstNT.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NavInstNT - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NavInstNT___Win32_Release"
# PROP BASE Intermediate_Dir "NavInstNT___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MT /W3 /GX /O2 /I "..\shared\include" /I "..\..\..\avcore\source\include\src" /I "..\..\..\core\source\include\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D Q_VER=\"N\" /D "_X86_" /D "_M_X86" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /I "..\..\..\core_technology\ldvp_shared\nobuilds\defutils\include" /I "..\..\..\core_technology\ldvp_shared\nobuilds\channel\include" /I "..\shared\include" /I "T:\source\include\src" /I "L:\source\include\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D Q_VER=\"N\" /D "_X86_" /D "_M_X86" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "..\shared\include" /i "..\..\shared\include" /i ".." /d "NDEBUG" /d "SYM_WIN" /d "SYM_WIN32" /d "SBCS_DLG" /d "QCUST" /d "_WINDOWS"
# ADD RSC /l 0x409 /i "..\..\shared\include" /d "NDEBUG" /d "SYM_WIN" /d "SYM_WIN32" /d "SBCS_DLG" /d "QCUST" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib defutils.lib chan32i.lib progman.lib version.lib svcctrl.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"..\shared\lib\intelansi\win32\release" /libpath:"..\..\..\avcore\source\nobuilds\src"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib defutils.lib chan32i.lib progman.lib version.lib svcctrl.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"..\..\..\core_technology\ldvp_shared\nobuilds\DEFUTILS\LIB.IRA" /libpath:"..\..\..\core_technology\ldvp_shared\nobuilds\channel\LIB.IRA" /libpath:"L:\source\lib.ira" /libpath:"T:\SOURCE\LIB.IRU"
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Cmds=Echo $(OutDir)	Echo Copying NavInstNT.dll to Shared\bin...	if not exist ..\Shared\bin\IntelAnsi\Win32\$(OutDir) md ..\Shared\bin\IntelAnsi\Win32\$(OutDir)	copy $(OutDir)\NavInstNT.dll ..\Shared\bin\IntelAnsi\Win32\$(OutDir)\NavInstNT.dll	Echo $(OutDir)	Echo Copying NavInstNT.lib to Shared\lib...	if not exist ..\Shared\lib\IntelAnsi\Win32\$(OutDir) md ..\Shared\lib\IntelAnsi\Win32\$(OutDir)	copy $(OutDir)\NavInstNT.lib ..\Shared\lib\IntelAnsi\Win32\$(OutDir)\NavInstNT.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NavInstNT - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "NavInstNT___Win32_Debug"
# PROP BASE Intermediate_Dir "NavInstNT___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /I "..\shared\include" /I "..\..\..\avcore\source\include\src" /I "..\..\..\core\source\include\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D Q_VER=\"N\" /D "_X86_" /D "_M_X86" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\core_technology\ldvp_shared\nobuilds\defutils\include" /I "..\..\..\core_technology\ldvp_shared\nobuilds\channel\include" /I "..\shared\include" /I "T:\source\include\src" /I "L:\source\include\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SYM_WIN" /D "SYM_WIN32" /D "_WINDLL" /D "QCUST" /D Q_VER=\"N\" /D "_X86_" /D "_M_X86" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "..\shared\include" /i "..\..\shared\include" /i ".." /d "_DEBUG" /d "SYM_WIN" /d "SYM_WIN32" /d "SBCS_DLG" /d "QCUST" /d "_WINDOWS"
# ADD RSC /l 0x409 /i "..\..\shared\include" /d "_DEBUG" /d "SYM_WIN" /d "SYM_WIN32" /d "SBCS_DLG" /d "QCUST" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib defutils.lib chan32i.lib progman.lib version.lib svcctrl.lib svcctrl.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\shared\lib\intelansi\win32\release" /libpath:"..\..\..\avcore\source\nobuilds\src"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib defutils.lib chan32i.lib progman.lib version.lib svcctrl.lib svcctrl.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\core_technology\ldvp_shared\nobuilds\DEFUTILS\LIB.IDA" /libpath:"..\..\..\core_technology\ldvp_shared\nobuilds\channel\LIB.IDA" /libpath:"L:\source\lib.ida" /libpath:"T:\SOURCE\LIB.IDU"
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=Echo $(OutDir)	Echo Copying NavInstNT.dll to Shared\bin...	if not exist ..\Shared\bin\IntelAnsi\Win32\$(OutDir) md ..\Shared\bin\IntelAnsi\Win32\$(OutDir)	copy $(OutDir)\NavInstNT.dll ..\Shared\bin\IntelAnsi\Win32\$(OutDir)\NavInstNT.dll	Echo $(OutDir)	Echo Copying NavInstNT.lib to Shared\lib...	if not exist ..\Shared\lib\IntelAnsi\Win32\$(OutDir) md ..\Shared\lib\IntelAnsi\Win32\$(OutDir)	copy $(OutDir)\NavInstNT.lib ..\Shared\lib\IntelAnsi\Win32\$(OutDir)\NavInstNT.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "NavInstNT - Win32 AlphaAnsiWinNTDebug"
# Name "NavInstNT - Win32 AlphaAnsiWinNTRelease"
# Name "NavInstNT - Win32 IntelAnsiWinNTDebug"
# Name "NavInstNT - Win32 IntelAnsiWinNTRelease"
# Name "NavInstNT - Win32 Release"
# Name "NavInstNT - Win32 Debug"
# Begin Source File

SOURCE=.\CRegent.cpp
# End Source File
# Begin Source File

SOURCE=.\CRegent.h
# End Source File
# Begin Source File

SOURCE=.\CUsgcnt.cpp
# End Source File
# Begin Source File

SOURCE=.\CUsgcnt.h
# End Source File
# Begin Source File

SOURCE=.\CVERRSRC.CPP
# End Source File
# Begin Source File

SOURCE=.\CVERRSRC.H
# End Source File
# Begin Source File

SOURCE=.\CWSTRING.CPP
# End Source File
# Begin Source File

SOURCE=.\CWSTRING.H
# End Source File
# Begin Source File

SOURCE=.\ISUTIL.CPP
# End Source File
# Begin Source File

SOURCE=.\IsUtil.h
# End Source File
# Begin Source File

SOURCE=.\NAVINST.CPP
# End Source File
# Begin Source File

SOURCE=.\NAVINST.H
# End Source File
# Begin Source File

SOURCE=.\English\NAVInst.rc

!IF  "$(CFG)" == "NavInstNT - Win32 AlphaAnsiWinNTDebug"

# ADD BASE RSC /l 0x409 /i "English"
# SUBTRACT BASE RSC /i "..\shared\include" /i "..\..\shared\include" /i ".."
# ADD RSC /l 0x409 /i "English" /i ".\English"
# SUBTRACT RSC /i "..\shared\include" /i "..\..\shared\include" /i ".."

!ELSEIF  "$(CFG)" == "NavInstNT - Win32 AlphaAnsiWinNTRelease"

# ADD BASE RSC /l 0x409 /i "English"
# SUBTRACT BASE RSC /i "..\shared\include" /i "..\..\shared\include" /i ".."
# ADD RSC /l 0x409 /i "English" /i ".\English"
# SUBTRACT RSC /i "..\shared\include" /i "..\..\shared\include" /i ".."

!ELSEIF  "$(CFG)" == "NavInstNT - Win32 IntelAnsiWinNTDebug"

# ADD BASE RSC /l 0x409 /i "English"
# SUBTRACT BASE RSC /i "..\shared\include" /i "..\..\shared\include" /i ".."
# ADD RSC /l 0x409 /i "English" /i ".\English"
# SUBTRACT RSC /i "..\shared\include" /i "..\..\shared\include" /i ".."

!ELSEIF  "$(CFG)" == "NavInstNT - Win32 IntelAnsiWinNTRelease"

# ADD BASE RSC /l 0x409 /i "English"
# SUBTRACT BASE RSC /i "..\shared\include" /i "..\..\shared\include" /i ".."
# ADD RSC /l 0x409 /i "English" /i ".\English"
# SUBTRACT RSC /i "..\shared\include" /i "..\..\shared\include" /i ".."

!ELSEIF  "$(CFG)" == "NavInstNT - Win32 Release"

# ADD BASE RSC /l 0x409 /i "English" /i ".\English"
# ADD RSC /l 0x409 /i "English" /i ".\English" /i "..\shared\include" /i ".."

!ELSEIF  "$(CFG)" == "NavInstNT - Win32 Debug"

# ADD BASE RSC /l 0x409 /i "English" /i ".\English"
# ADD RSC /l 0x409 /i "English" /i ".\English" /i "..\shared\include" /i ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NAVInstMIF.cpp
# End Source File
# Begin Source File

SOURCE=.\NAVInstMIF.h
# End Source File
# Begin Source File

SOURCE=.\navuninst.cpp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Target
# End Project
