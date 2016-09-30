# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=NAVInst - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to NAVInst - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "NAVInst - Win32 Release" && "$(CFG)" !=\
 "NAVInst - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "NAVInstNT.mak" CFG="NAVInst - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NAVInst - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NAVInst - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "NAVInst - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "NAVInst - Win32 Release"

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
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\NAVInstNT.dll"

CLEAN : 
	-@erase "$(INTDIR)\CRegent.obj"
	-@erase "$(INTDIR)\CUsgcnt.obj"
	-@erase "$(INTDIR)\CWSTRING.OBJ"
	-@erase "$(INTDIR)\ISUTIL.OBJ"
	-@erase "$(INTDIR)\navinst.obj"
	-@erase "$(INTDIR)\NAVInst.res"
	-@erase "$(INTDIR)\NAVInstMIF.obj"
	-@erase "$(INTDIR)\navuninst.obj"
	-@erase "$(OUTDIR)\NAVInstNT.dll"
	-@erase "$(OUTDIR)\NAVInstNT.exp"
	-@erase "$(OUTDIR)\NAVInstNT.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MBCS" /D "SYM_WIN32" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MBCS"\
 /D "SYM_WIN32" /Fp"$(INTDIR)/NAVInstNT.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/NAVInst.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/NAVInstNT.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib svcctrl.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib svcctrl.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/NAVInstNT.pdb" /machine:I386 /out:"$(OUTDIR)/NAVInstNT.dll"\
 /implib:"$(OUTDIR)/NAVInstNT.lib" 
LINK32_OBJS= \
	"$(INTDIR)\CRegent.obj" \
	"$(INTDIR)\CUsgcnt.obj" \
	"$(INTDIR)\CWSTRING.OBJ" \
	"$(INTDIR)\ISUTIL.OBJ" \
	"$(INTDIR)\navinst.obj" \
	"$(INTDIR)\NAVInst.res" \
	"$(INTDIR)\NAVInstMIF.obj" \
	"$(INTDIR)\navuninst.obj"

"$(OUTDIR)\NAVInstNT.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "NAVInst - Win32 Debug"

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
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\NAVInstNT.dll"

CLEAN : 
	-@erase "$(INTDIR)\CRegent.obj"
	-@erase "$(INTDIR)\CUsgcnt.obj"
	-@erase "$(INTDIR)\CWSTRING.OBJ"
	-@erase "$(INTDIR)\ISUTIL.OBJ"
	-@erase "$(INTDIR)\navinst.obj"
	-@erase "$(INTDIR)\NAVInst.res"
	-@erase "$(INTDIR)\NAVInstMIF.obj"
	-@erase "$(INTDIR)\navuninst.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\NAVInstNT.dll"
	-@erase "$(OUTDIR)\NAVInstNT.exp"
	-@erase "$(OUTDIR)\NAVInstNT.ilk"
	-@erase "$(OUTDIR)\NAVInstNT.lib"
	-@erase "$(OUTDIR)\NAVInstNT.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "MBCS" /D "SYM_WIN32" /D "_LOGFILE_TRACE_" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "MBCS" /D "SYM_WIN32" /D "_LOGFILE_TRACE_" /Fp"$(INTDIR)/NAVInstNT.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/NAVInst.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/NAVInstNT.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib svcctrl.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib svcctrl.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/NAVInstNT.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/NAVInstNT.dll" /implib:"$(OUTDIR)/NAVInstNT.lib" 
LINK32_OBJS= \
	"$(INTDIR)\CRegent.obj" \
	"$(INTDIR)\CUsgcnt.obj" \
	"$(INTDIR)\CWSTRING.OBJ" \
	"$(INTDIR)\ISUTIL.OBJ" \
	"$(INTDIR)\navinst.obj" \
	"$(INTDIR)\NAVInst.res" \
	"$(INTDIR)\NAVInstMIF.obj" \
	"$(INTDIR)\navuninst.obj"

"$(OUTDIR)\NAVInstNT.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "NAVInst - Win32 Release"
# Name "NAVInst - Win32 Debug"

!IF  "$(CFG)" == "NAVInst - Win32 Release"

!ELSEIF  "$(CFG)" == "NAVInst - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\NAVInst.rc

"$(INTDIR)\NAVInst.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CWSTRING.CPP
DEP_CPP_CWSTR=\
	".\cwstring.h"\
	

"$(INTDIR)\CWSTRING.OBJ" : $(SOURCE) $(DEP_CPP_CWSTR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ISUTIL.CPP
DEP_CPP_ISUTI=\
	".\cregent.h"\
	".\cusgcnt.h"\
	".\cwstring.h"\
	".\IsRegUti.h"\
	".\IsUtil.h"\
	".\NAVInst.h"\
	{$(INCLUDE)}"\dbcs_str.h"\
	{$(INCLUDE)}"\DefUtils.h"\
	{$(INCLUDE)}"\dosvmm.h"\
	{$(INCLUDE)}"\drvseg.h"\
	{$(INCLUDE)}"\NAVSTART.H"\
	{$(INCLUDE)}"\navver.h"\
	{$(INCLUDE)}"\nlm_str.h"\
	{$(INCLUDE)}"\os2win.h"\
	{$(INCLUDE)}"\othunk.h"\
	{$(INCLUDE)}"\PLATFORM.H"\
	{$(INCLUDE)}"\platmac.h"\
	{$(INCLUDE)}"\platnlm.h"\
	{$(INCLUDE)}"\platntk.h"\
	{$(INCLUDE)}"\platvxd.h"\
	{$(INCLUDE)}"\platwin.h"\
	{$(INCLUDE)}"\symvxd.h"\
	
NODEP_CPP_ISUTI=\
	"L:\SOURCE\INCLUDE\SRC\ior.h"\
	"L:\SOURCE\INCLUDE\SRC\shell.h"\
	

"$(INTDIR)\ISUTIL.OBJ" : $(SOURCE) $(DEP_CPP_ISUTI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\navinst.cpp
DEP_CPP_NAVIN=\
	"..\..\INCLUDE\SRC\PROFILER.H"\
	"..\..\INCLUDE\SRC\TIMERCLI.H"\
	".\cwstring.h"\
	".\IsUtil.h"\
	".\NAVInst.h"\
	"T:\SOURCE\INCLUDE\SRC\avtypes.h"\
	{$(INCLUDE)}"\dbcs_str.h"\
	{$(INCLUDE)}"\DefUtils.h"\
	{$(INCLUDE)}"\dosvmm.h"\
	{$(INCLUDE)}"\drvseg.h"\
	{$(INCLUDE)}"\n30type.h"\
	{$(INCLUDE)}"\navapcom.h"\
	{$(INCLUDE)}"\NavAppIds.h"\
	{$(INCLUDE)}"\NAVSTART.H"\
	{$(INCLUDE)}"\navver.h"\
	{$(INCLUDE)}"\nlm_str.h"\
	{$(INCLUDE)}"\os2win.h"\
	{$(INCLUDE)}"\othunk.h"\
	{$(INCLUDE)}"\PLATFORM.H"\
	{$(INCLUDE)}"\platmac.h"\
	{$(INCLUDE)}"\platnlm.h"\
	{$(INCLUDE)}"\platntk.h"\
	{$(INCLUDE)}"\platvxd.h"\
	{$(INCLUDE)}"\platwin.h"\
	{$(INCLUDE)}"\svcctrl.h"\
	{$(INCLUDE)}"\symvxd.h"\
	
NODEP_CPP_NAVIN=\
	"L:\SOURCE\INCLUDE\SRC\ior.h"\
	"L:\SOURCE\INCLUDE\SRC\shell.h"\
	

"$(INTDIR)\navinst.obj" : $(SOURCE) $(DEP_CPP_NAVIN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\navuninst.cpp
DEP_CPP_NAVUN=\
	"..\..\INCLUDE\SRC\PROFILER.H"\
	"..\..\INCLUDE\SRC\TIMERCLI.H"\
	".\cregent.h"\
	".\cusgcnt.h"\
	".\cwstring.h"\
	".\IsUtil.h"\
	".\NAVInst.h"\
	".\NavInstMIF.h"\
	"T:\SOURCE\INCLUDE\SRC\avtypes.h"\
	{$(INCLUDE)}"\dbcs_str.h"\
	{$(INCLUDE)}"\DefUtils.h"\
	{$(INCLUDE)}"\dosvmm.h"\
	{$(INCLUDE)}"\drvseg.h"\
	{$(INCLUDE)}"\n30type.h"\
	{$(INCLUDE)}"\navapcom.h"\
	{$(INCLUDE)}"\NavAppIds.h"\
	{$(INCLUDE)}"\NAVSTART.H"\
	{$(INCLUDE)}"\navver.h"\
	{$(INCLUDE)}"\nlm_str.h"\
	{$(INCLUDE)}"\os2win.h"\
	{$(INCLUDE)}"\othunk.h"\
	{$(INCLUDE)}"\PLATFORM.H"\
	{$(INCLUDE)}"\platmac.h"\
	{$(INCLUDE)}"\platnlm.h"\
	{$(INCLUDE)}"\platntk.h"\
	{$(INCLUDE)}"\platvxd.h"\
	{$(INCLUDE)}"\platwin.h"\
	{$(INCLUDE)}"\svcctrl.h"\
	{$(INCLUDE)}"\symvxd.h"\
	
NODEP_CPP_NAVUN=\
	"L:\SOURCE\INCLUDE\SRC\ior.h"\
	"L:\SOURCE\INCLUDE\SRC\shell.h"\
	

"$(INTDIR)\navuninst.obj" : $(SOURCE) $(DEP_CPP_NAVUN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\NAVInstMIF.cpp
DEP_CPP_NAVINS=\
	".\NAVInst.h"\
	".\NavInstMIF.h"\
	{$(INCLUDE)}"\dbcs_str.h"\
	{$(INCLUDE)}"\DefUtils.h"\
	{$(INCLUDE)}"\dosvmm.h"\
	{$(INCLUDE)}"\drvseg.h"\
	{$(INCLUDE)}"\NAVSTART.H"\
	{$(INCLUDE)}"\navver.h"\
	{$(INCLUDE)}"\nlm_str.h"\
	{$(INCLUDE)}"\os2win.h"\
	{$(INCLUDE)}"\othunk.h"\
	{$(INCLUDE)}"\PLATFORM.H"\
	{$(INCLUDE)}"\platmac.h"\
	{$(INCLUDE)}"\platnlm.h"\
	{$(INCLUDE)}"\platntk.h"\
	{$(INCLUDE)}"\platvxd.h"\
	{$(INCLUDE)}"\platwin.h"\
	{$(INCLUDE)}"\symvxd.h"\
	
NODEP_CPP_NAVINS=\
	"L:\SOURCE\INCLUDE\SRC\ior.h"\
	"L:\SOURCE\INCLUDE\SRC\shell.h"\
	

"$(INTDIR)\NAVInstMIF.obj" : $(SOURCE) $(DEP_CPP_NAVINS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CUsgcnt.cpp
DEP_CPP_CUSGC=\
	".\cregent.h"\
	".\cusgcnt.h"\
	

"$(INTDIR)\CUsgcnt.obj" : $(SOURCE) $(DEP_CPP_CUSGC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CRegent.cpp
DEP_CPP_CREGE=\
	".\cregent.h"\
	

"$(INTDIR)\CRegent.obj" : $(SOURCE) $(DEP_CPP_CREGE) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
