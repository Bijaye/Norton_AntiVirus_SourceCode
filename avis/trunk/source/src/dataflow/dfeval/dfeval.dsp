# Microsoft Developer Studio Project File - Name="DFEval" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DFEval - Win32 Debug GUI
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dfeval.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dfeval.mak" CFG="DFEval - Win32 Debug GUI"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DFEval - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DFEval - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "DFEval - Win32 Debug GUI" (based on "Win32 (x86) Static Library")
!MESSAGE "DFEval - Win32 Release GUI" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DFEval - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "///LIBD///\"
# PROP Intermediate_Dir "///OBJD///\dataflow\dfeval"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\inc" /I "..\alm" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"///LIBD///\avisdfev.lib"

!ELSEIF  "$(CFG)" == "DFEval - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "///LIBD///\"
# PROP Intermediate_Dir "///OBJD///\dataflow\dfeval"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\inc" /I "..\alm" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"///LIBD///\avisdfev.lib"

!ELSEIF  "$(CFG)" == "DFEval - Win32 Debug GUI"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DFEval__"
# PROP BASE Intermediate_Dir "DFEval__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Z7 /Od /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\alm" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\dataflow\dfgui" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"e:\newdev\lib\avisdfev.lib"

!ELSEIF  "$(CFG)" == "DFEval - Win32 Release GUI"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DFEval_0"
# PROP BASE Intermediate_Dir "DFEval_0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\dataflow\dfgui" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"e:\newdev\lib\avisdfev.lib"

!ENDIF 

# Begin Target

# Name "DFEval - Win32 Release"
# Name "DFEval - Win32 Debug"
# Name "DFEval - Win32 Debug GUI"
# Name "DFEval - Win32 Release GUI"
# Begin Source File

SOURCE=.\DFEvalError.h
# End Source File
# Begin Source File

SOURCE=.\DFEvalState.cpp
# End Source File
# Begin Source File

SOURCE=..\inc\DFEvalState.h
# End Source File
# Begin Source File

SOURCE=.\DFEvalStatus.cpp
# End Source File
# Begin Source File

SOURCE=..\inc\DFEvalStatus.h
# End Source File
# Begin Source File

SOURCE=.\DFMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\inc\DFMatrix.h
# End Source File
# End Target
# End Project
