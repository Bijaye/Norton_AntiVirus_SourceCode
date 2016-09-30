# Microsoft Developer Studio Project File - Name="verv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=verv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "verv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "verv.mak" CFG="verv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "verv - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "verv - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Avis100/Verv", ANUAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "verv - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "NOASM" /D "VERV_CODO" /D "USE_STRUCTURE" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
TargetPath=.\Release\verv.exe
SOURCE=$(InputPath)
PostBuild_Cmds=xcopy $(TargetPath) D:\is\progs\Stage\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "verv - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "NOASM" /D "VERV_CODO" /D "USE_STRUCTURE" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "verv - Win32 Release"
# Name "verv - Win32 Debug"
# Begin Source File

SOURCE=.\src\allocsafe.cpp
# End Source File
# Begin Source File

SOURCE=.\src\allocsafe.h
# End Source File
# Begin Source File

SOURCE=.\src\autoverv.cpp
# End Source File
# Begin Source File

SOURCE=.\src\autoverv.h
# End Source File
# Begin Source File

SOURCE=.\src\avfileio.cpp
# End Source File
# Begin Source File

SOURCE=.\src\avfileio.h
# End Source File
# Begin Source File

SOURCE=.\src\boundary.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common.h
# End Source File
# Begin Source File

SOURCE=.\src\compute_offset.cpp
# End Source File
# Begin Source File

SOURCE=.\src\crc_setup.cpp
# End Source File
# Begin Source File

SOURCE=.\src\create_invariant.cpp
# End Source File
# Begin Source File

SOURCE=.\src\decrypt.cpp
# End Source File
# Begin Source File

SOURCE=.\src\derive_correction.cpp
# End Source File
# Begin Source File

SOURCE=.\src\eliminate_duplicate_keys.cpp
# End Source File
# Begin Source File

SOURCE=.\src\entry.cpp
# End Source File
# Begin Source File

SOURCE=.\src\equivalent_keys.cpp
# End Source File
# Begin Source File

SOURCE=.\src\fileiterator.cpp
# End Source File
# Begin Source File

SOURCE=.\src\fileiterator.hpp
# End Source File
# Begin Source File

SOURCE=.\src\filesize.cpp
# End Source File
# Begin Source File

SOURCE=.\src\find_best_anchor.cpp
# End Source File
# Begin Source File

SOURCE=.\src\find_match_len.cpp
# End Source File
# Begin Source File

SOURCE=.\src\free_match.cpp
# End Source File
# Begin Source File

SOURCE=.\src\free_pos.cpp
# End Source File
# Begin Source File

SOURCE=.\src\get_file_type.cpp
# End Source File
# Begin Source File

SOURCE=.\src\greedy_cover.cpp
# End Source File
# Begin Source File

SOURCE=.\src\index_bigrams.cpp
# End Source File
# Begin Source File

SOURCE=.\src\lcs.cpp
# End Source File
# Begin Source File

SOURCE=.\src\make_key_list.cpp
# End Source File
# Begin Source File

SOURCE=.\src\make_map_list.cpp
# End Source File
# Begin Source File

SOURCE=.\src\make_recon_arrays.cpp
# End Source File
# Begin Source File

SOURCE=.\src\make_reconstruction_list.cpp
# End Source File
# Begin Source File

SOURCE=.\src\make_universal_goat.cpp
# End Source File
# Begin Source File

SOURCE=.\src\mem_err.cpp
# End Source File
# Begin Source File

SOURCE=.\src\patch_map_list.cpp
# End Source File
# Begin Source File

SOURCE=.\src\polymath.cpp
# End Source File
# Begin Source File

SOURCE=.\src\polymath.h
# End Source File
# Begin Source File

SOURCE=.\src\proto.h
# End Source File
# Begin Source File

SOURCE=.\src\reconstruct.cpp
# End Source File
# Begin Source File

SOURCE=.\src\reconstruct_status.cpp
# End Source File
# Begin Source File

SOURCE=.\src\write_bytes.cpp
# End Source File
# Begin Source File

SOURCE=.\src\write_bytes2.cpp
# End Source File
# Begin Source File

SOURCE=.\src\write_graphic.cpp
# End Source File
# Begin Source File

SOURCE=.\src\write_vds.cpp
# End Source File
# Begin Source File

SOURCE=.\src\write_vp.cpp
# End Source File
# End Target
# End Project
