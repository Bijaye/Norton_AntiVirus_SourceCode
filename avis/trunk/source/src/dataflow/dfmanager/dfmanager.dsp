# Microsoft Developer Studio Project File - Name="DFManager" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DFManager - Win32 Debug IDE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DFManager.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DFManager.mak" CFG="DFManager - Win32 Debug IDE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DFManager - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DFManager - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "DFManager - Win32 Debug IDE" (based on "Win32 (x86) Static Library")
!MESSAGE "DFManager - Win32 Release IDE" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DFManager - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "///LIBD///\"
# PROP Intermediate_Dir "///OBJD///\dataflow\DFManager"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\inc" /I "..\com" /I "..\dfgui" /I "..\..\avisdb" /I "..\..\avissendmail" /I "..\..\aviscommon" /I "..\..\avisfile" /I "..\..\avisfile\md5" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"///LIBD///\avisdfmg.lib" /out:"///LIBD///\avisdfmg.lib"

!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "///LIBD///\"
# PROP Intermediate_Dir "///OBJD///\dataflow\DFManager"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\inc" /I "..\com" /I "..\dfgui" /I "..\..\avisdb" /I "..\..\aviscommon" /I "..\..\avissendmail" /I "..\..\avisfile" /I "..\..\avisfile\md5" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"///LIBD///\avisdfmg.lib" /out:"///LIBD///\avisdfmg.lib"

!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug IDE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DFManage"
# PROP BASE Intermediate_Dir "DFManage"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Z7 /Od /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\alm" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\avissendmail" /I "e:\newdev\src\dataflow\com" /I "e:\newdev\src\dataflow\dfgui" /I "e:\newdev\src\inc" /I "e:\newdev\src\avisdb" /I "e:\newdev\src\aviscommon" /I "e:\newdev\src\aviscommon\system" /I "e:\newdev\src\avidfile" /I "e:\newdev\src\avisfile\md5" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"e:\newdev\lib\avisdfmg.lib"

!ELSEIF  "$(CFG)" == "DFManager - Win32 Release IDE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DFManag0"
# PROP BASE Intermediate_Dir "DFManag0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /X /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\dataflow\com" /I "e:\newdev\src\dataflow\dfgui" /I "e:\newdev\src\avisdb" /I "e:\newdev\src\aviscommon" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\avisdfmg.lib"

!ENDIF 

# Begin Target

# Name "DFManager - Win32 Release"
# Name "DFManager - Win32 Debug"
# Name "DFManager - Win32 Debug IDE"
# Name "DFManager - Win32 Release IDE"
# Begin Source File

SOURCE=.\dfdbrequests.cpp
# End Source File
# Begin Source File

SOURCE=.\dfdbselect.cpp
# End Source File
# Begin Source File

SOURCE=..\inc\dfdbselect.h
# End Source File
# Begin Source File

SOURCE=..\inc\dfdefaults.h
# End Source File
# Begin Source File

SOURCE=.\DFDirInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\inc\DFDirInfo.h
# End Source File
# Begin Source File

SOURCE=..\inc\dfjob.h
# End Source File
# Begin Source File

SOURCE=.\dfmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\inc\dfmanager.h
# End Source File
# Begin Source File

SOURCE=.\dfpackthread.cpp
# End Source File
# Begin Source File

SOURCE=.\dfsample.cpp
# End Source File
# Begin Source File

SOURCE=..\inc\dfsample.h
# End Source File
# End Target
# End Project
