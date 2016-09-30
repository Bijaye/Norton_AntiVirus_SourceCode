# Microsoft Developer Studio Project File - Name="Avenge" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Avenge - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Avenge.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avenge.mak" CFG="Avenge - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Avenge - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Avenge - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "SHRIKE_NewAP"
# PROP Scc_LocalPath "..\..\..\TRUNK\SOURCE\AVENGE\SRC"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Avenge - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Zd /O2 /I "..\..\include\src" /I "..\..\..\..\core\source\include\src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "SYM_WIN" /D "SYM_WIN32" /D "QCUST" /D "_WINDOWS" /D Q_VER=\"P\" /D "_X86" /D "_M_X86" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\..\lib.ira/Avenge.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib.ira\Avenge.lib"

!ELSEIF  "$(CFG)" == "Avenge - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /Zi /Od /I "..\..\include\src" /I "..\..\..\..\..\..\core\trunk\source\include\src" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "SYM_WIN" /D "SYM_WIN32" /D "SYM_DEBUG" /D "QCUST" /D "_WINDOWS" /D Q_VER=\"P\" /D "_X86" /D "_M_X86" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\..\lib.ida/Avenge.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"\build\navapi.devstudio\Avenge.lib"

!ENDIF 

# Begin Target

# Name "Avenge - Win32 Release"
# Name "Avenge - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ALGLOAD.CPP
# End Source File
# Begin Source File

SOURCE=.\ALGSCAN.CPP
# End Source File
# Begin Source File

SOURCE=.\AVCACHE.CPP
# End Source File
# Begin Source File

SOURCE=.\AVENGE.CPP
# End Source File
# Begin Source File

SOURCE=.\BOOLOAD.CPP
# End Source File
# Begin Source File

SOURCE=.\BOOTREP.CPP
# End Source File
# Begin Source File

SOURCE=.\CRCLOAD.CPP
# End Source File
# Begin Source File

SOURCE=.\CRCSCAN.CPP
# End Source File
# Begin Source File

SOURCE=.\DELENTRY.CPP
# End Source File
# Begin Source File

SOURCE=.\ENGCOPYF.CPP
# End Source File
# Begin Source File

SOURCE=.\FILEREP.CPP
# End Source File
# Begin Source File

SOURCE=.\FSTART.CPP
# End Source File
# Begin Source File

SOURCE=.\GETBULK.CPP
# End Source File
# Begin Source File

SOURCE=.\GETINFO.CPP
# End Source File
# Begin Source File

SOURCE=.\INFC.CPP
# End Source File
# Begin Source File

SOURCE=.\INFOAUX.CPP
# End Source File
# Begin Source File

SOURCE=.\MEMLOAD.CPP
# End Source File
# Begin Source File

SOURCE=.\MEMSCAN.CPP
# End Source File
# Begin Source File

SOURCE=.\NAMEC.CPP
# End Source File
# Begin Source File

SOURCE=.\REPARAUX.CPP
# End Source File
# Begin Source File

SOURCE=.\VDATFILE.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
