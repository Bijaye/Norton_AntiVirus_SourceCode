# Microsoft Developer Studio Project File - Name="DFGui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DFGui - Win32 Debug IDE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DFGui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DFGui.mak" CFG="DFGui - Win32 Debug IDE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DFGui - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DFGui - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DFGui - Win32 Debug IDE" (based on "Win32 (x86) Application")
!MESSAGE "DFGui - Win32 Release IDE" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DFGui - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "///OUTD///\nt"
# PROP Intermediate_Dir "///OBJD///\dataflow\avisdf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\inc" /I "..\dfgui" /I "..\com" /I "..\..\avisdb" /I "..\..\aviscommon" /I "..\..\avissendmail" /I "..\..\inc" /I "..\..\avisfile" /I "..\..\avisfile\md5" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /subsystem:windows /pdb:none /machine:I386 /out:"///OUTD///\nt/DFGui.exe" /out:"///OUTD///\nt/DFGui.exe" /out:"///OUTD///\nt/DFGui.exe" /out:"///OUTD///\nt/DFGui.exe" /out:"///OUTD///\nt/DFGui.exe" /out:"///OUTD///\nt/avisdf.exe" ///LIBD///\avisdfev.lib ///LIBD///\avisdfmg.lib ///LIBD///\AVISDFRL.lib ///LIBD///\AVISDB.lib ///LIBD///\AVISCOMMON.lib ///LIBD///\AVISFILE.lib ///LIBD///\avissendmail.lib

!ELSEIF  "$(CFG)" == "DFGui - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "///OUTD///\nt"
# PROP Intermediate_Dir "///OBJD///\dataflow\avisdf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /MDd /W3 /GX /Zi /Od /I "..\inc" /I "..\dfgui" /I "..\com" /I "..\..\avisdb" /I "..\..\aviscommon" /I "..\..\inc" /I "..\..\avisfile" /I "..\..\avisfile\md5" /I "..\..\avissendmail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /nologo
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /subsystem:windows /incremental:no /debug /machine:I386 /pdb:"///OUTD///\nt/DFGui.pdb" /out:"///OUTD///\nt/DFGui.exe" /pdb:"///OUTD///\nt/DFGui.pdb" /out:"///OUTD///\nt/DFGui.exe" /pdb:"///OUTD///\nt/DFGui.pdb" /out:"///OUTD///\nt/DFGui.exe" /out:"///OUTD///\nt/avisdf.exe" /pdb:"///OUTD///\nt/avisdf.pdb" /map:"///OBJD///\dataflow\DFGui/DFGui.map" ///LIBD///\avisdfev.lib ///LIBD///\avisdfmg.lib ///LIBD///\AVISDFRL.lib ///LIBD///\AVISDB.lib ///LIBD///\AVISCOMMON.lib ///LIBD///\AVISFILE.lib ///LIBD///\avissendmail.lib

!ELSEIF  "$(CFG)" == "DFGui - Win32 Debug IDE"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DFGui___"
# PROP BASE Intermediate_Dir "DFGui___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\dataflow\com" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\avissendmail" /I "e:\newdev\src\dataflow\com" /I "e:\newdev\src\avisdb" /I "e:\newdev\src\aviscommon" /I "e:\newdev\src\inc" /I "e:\newdev\src\AVISFILE" /I "e:\newdev\src\AVISFILE\MD5" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 e:\newdev\lib\avisdfrl.lib e:\newdev\lib\avissendmail.lib e:\newdev\lib\avisdfmg.lib e:\newdev\lib\avisdfev.lib e:\newdev\lib\avisdb.lib e:\newdev\lib\aviscommon.lib e:\newdev\lib\avisfile.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/avisdf.exe" /pdbtype:sept
# SUBTRACT LINK32 /incremental:no /map

!ELSEIF  "$(CFG)" == "DFGui - Win32 Release IDE"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DFGui__0"
# PROP BASE Intermediate_Dir "DFGui__0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\dataflow\com" /I "e:\newdev\src\avisdb" /I "e:\newdev\src\aviscommon" /I "e:\newdev\src\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\dfmanager\release\avisdfmg.lib e:\newdev\lib\avisdfrl.lib ..\dfeval\release\avisdfevl.lib /nologo /subsystem:windows /machine:I386 /out:"Release/avisdf.exe"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "DFGui - Win32 Release"
# Name "DFGui - Win32 Debug"
# Name "DFGui - Win32 Debug IDE"
# Name "DFGui - Win32 Release IDE"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\com\computemd5checksum.cpp
# End Source File
# Begin Source File

SOURCE=.\crashrecover.cpp
# End Source File
# Begin Source File

SOURCE=.\DFConfigureDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DFGui.cpp
# End Source File
# Begin Source File

SOURCE=.\DFGui.rc
# End Source File
# Begin Source File

SOURCE=.\DFGuiDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\DFGuiView.cpp
# End Source File
# Begin Source File

SOURCE=.\DFSampleAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\DFStatisticsDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\com\listvwex.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\com\paramvalue.cpp
# End Source File
# Begin Source File

SOURCE=.\PriorityDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\com\singleinstance.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\com\titletip.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\crashrecover.h
# End Source File
# Begin Source File

SOURCE=.\DFConfigureDialog.h
# End Source File
# Begin Source File

SOURCE=..\inc\dfdefaults.h
# End Source File
# Begin Source File

SOURCE=.\DFGui.h
# End Source File
# Begin Source File

SOURCE=.\DFGuiDoc.h
# End Source File
# Begin Source File

SOURCE=.\DFGuiView.h
# End Source File
# Begin Source File

SOURCE=.\DFSampleAttributes.h
# End Source File
# Begin Source File

SOURCE=.\DFStatisticsDialog.h
# End Source File
# Begin Source File

SOURCE=..\com\listctex.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\PriorityDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\com\singleinstance.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\avisdfmgr.ico
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=D:\extract\src\dataflow\dfgui\res\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\DFGui.ico
# End Source File
# Begin Source File

SOURCE=.\res\DFGui.rc2
# End Source File
# Begin Source File

SOURCE=.\res\DFGuiDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
