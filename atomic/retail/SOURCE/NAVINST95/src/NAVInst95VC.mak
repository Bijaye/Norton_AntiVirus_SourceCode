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
!MESSAGE NMAKE /f "NAVInst.mak" CFG="NAVInst - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "NAVInst - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NAVInst - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE
!ERROR An invalid configuration is specified.
!ENDIF

!IF "$(OS)" == "Windows_95"
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

ALL : "$(OUTDIR)\NAVInst.dll"

CLEAN :
	-@erase "$(INTDIR)\CWSTRING.OBJ"
	-@erase "$(INTDIR)\ISREGUTI.OBJ"
	-@erase "$(INTDIR)\ISUTIL.OBJ"
	-@erase "$(INTDIR)\navinst.obj"
	-@erase "$(INTDIR)\NAVInst.res"
	-@erase "$(INTDIR)\navuninst.obj"
	-@erase "$(OUTDIR)\NAVInst.dll"
	-@erase "$(OUTDIR)\NAVInst.exp"
	-@erase "$(OUTDIR)\NAVInst.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MBCS" /D "SYM_WIN32" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MBCS"\
 /D "SYM_WIN32" /Fp"$(INTDIR)/NAVInst.pch" /YX /Fo"$(INTDIR)/" /c
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/NAVInst.bsc"
BSC32_SBRS= \

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib svcctrl.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib svcctrl.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/NAVInst.pdb" /machine:I386 /out:"$(OUTDIR)/NAVInst.dll"\
 /implib:"$(OUTDIR)/NAVInst.lib"
LINK32_OBJS= \
	"$(INTDIR)\CWSTRING.OBJ" \
	"$(INTDIR)\ISREGUTI.OBJ" \
	"$(INTDIR)\ISUTIL.OBJ" \
	"$(INTDIR)\navinst.obj" \
	"$(INTDIR)\NAVInst.res" \
	"$(INTDIR)\navuninst.obj"

"$(OUTDIR)\NAVInst.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "$(OUTDIR)\NAVInst.dll"

CLEAN :
	-@erase "$(INTDIR)\CWSTRING.OBJ"
	-@erase "$(INTDIR)\ISREGUTI.OBJ"
	-@erase "$(INTDIR)\ISUTIL.OBJ"
	-@erase "$(INTDIR)\navinst.obj"
	-@erase "$(INTDIR)\NAVInst.res"
	-@erase "$(INTDIR)\navuninst.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\NAVInst.dll"
	-@erase "$(OUTDIR)\NAVInst.exp"
	-@erase "$(OUTDIR)\NAVInst.ilk"
	-@erase "$(OUTDIR)\NAVInst.lib"
	-@erase "$(OUTDIR)\NAVInst.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "MBCS" /D "SYM_WIN32" /D "_LOGFILE_TRACE_" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "MBCS" /D "SYM_WIN32" /D "_LOGFILE_TRACE_" /Fp"$(INTDIR)/NAVInst.pch" /YX\
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/NAVInst.bsc"
BSC32_SBRS= \

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib svcctrl.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib svcctrl.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/NAVInst.pdb" /debug /machine:I386 /out:"$(OUTDIR)/NAVInst.dll"\
 /implib:"$(OUTDIR)/NAVInst.lib"
LINK32_OBJS= \
	"$(INTDIR)\CWSTRING.OBJ" \
	"$(INTDIR)\ISREGUTI.OBJ" \
	"$(INTDIR)\ISUTIL.OBJ" \
	"$(INTDIR)\navinst.obj" \
	"$(INTDIR)\NAVInst.res" \
	"$(INTDIR)\navuninst.obj"

"$(OUTDIR)\NAVInst.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\ISREGUTI.CPP
DEP_CPP_ISREG=\
	".\cwstring.h"\
	".\IsRegUti.h"\
	".\IsUtil.h"\
	".\NAVInst.h"\


"$(INTDIR)\ISREGUTI.OBJ" : $(SOURCE) $(DEP_CPP_ISREG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ISUTIL.CPP
DEP_CPP_ISUTI=\
	".\cwstring.h"\
	".\IsRegUti.h"\
	".\IsUtil.h"\
	".\NAVInst.h"\


"$(INTDIR)\ISUTIL.OBJ" : $(SOURCE) $(DEP_CPP_ISUTI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\navinst.cpp

!IF  "$(CFG)" == "NAVInst - Win32 Release"

DEP_CPP_NAVIN=\
	".\cwstring.h"\
	".\NAVInst.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\dbcs_str.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\dosvmm.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\drvseg.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\OS2WIN.H"\
	"L:\SOURCE\INCLUDE\SRCQAKE\OTHUNK.H"\
	"L:\SOURCE\INCLUDE\SRCQAKE\platmac.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\PLATNLM.H"\
	"L:\SOURCE\INCLUDE\SRCQAKE\PLATNTK.H"\
	"L:\SOURCE\INCLUDE\SRCQAKE\PLATVXD.H"\
	"L:\SOURCE\INCLUDE\SRCQAKE\platwin.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\SYMVXD.H"\
	"S:\INCLUDE\SRC\PROFILER.H"\
	"S:\INCLUDE\SRC\TIMERCLI.H"\
	{$(INCLUDE)}"\NAVAPCOM.H"\
	{$(INCLUDE)}"\NAVSTART.H"\
	{$(INCLUDE)}"\PLATFORM.H"\
	{$(INCLUDE)}"\SVCCTRL.H"\


"$(INTDIR)\navinst.obj" : $(SOURCE) $(DEP_CPP_NAVIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "NAVInst - Win32 Debug"

DEP_CPP_NAVIN=\
	".\cwstring.h"\
	".\NAVInst.h"\
	"S:\INCLUDE\SRC\PROFILER.H"\
	"S:\INCLUDE\SRC\TIMERCLI.H"\
	{$(INCLUDE)}"\NAVAPCOM.H"\
	{$(INCLUDE)}"\NAVSTART.H"\
	{$(INCLUDE)}"\PLATFORM.H"\
	{$(INCLUDE)}"\SVCCTRL.H"\


"$(INTDIR)\navinst.obj" : $(SOURCE) $(DEP_CPP_NAVIN) "$(INTDIR)"


!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\navuninst.cpp

!IF  "$(CFG)" == "NAVInst - Win32 Release"

DEP_CPP_NAVUN=\
	".\cwstring.h"\
	".\IsRegUti.h"\
	".\IsUtil.h"\
	".\NAVInst.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\dbcs_str.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\dosvmm.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\drvseg.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\OS2WIN.H"\
	"L:\SOURCE\INCLUDE\SRCQAKE\OTHUNK.H"\
	"L:\SOURCE\INCLUDE\SRCQAKE\platmac.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\PLATNLM.H"\
	"L:\SOURCE\INCLUDE\SRCQAKE\PLATNTK.H"\
	"L:\SOURCE\INCLUDE\SRCQAKE\PLATVXD.H"\
	"L:\SOURCE\INCLUDE\SRCQAKE\platwin.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\SYMVXD.H"\
	"S:\INCLUDE\SRC\PROFILER.H"\
	"S:\INCLUDE\SRC\TIMERCLI.H"\
	{$(INCLUDE)}"\NAVAPCOM.H"\
	{$(INCLUDE)}"\PLATFORM.H"\
	{$(INCLUDE)}"\SVCCTRL.H"\


"$(INTDIR)\navuninst.obj" : $(SOURCE) $(DEP_CPP_NAVUN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "NAVInst - Win32 Debug"

DEP_CPP_NAVUN=\
	".\cwstring.h"\
	".\IsRegUti.h"\
	".\IsUtil.h"\
	".\NAVInst.h"\
	"S:\INCLUDE\SRC\PROFILER.H"\
	"S:\INCLUDE\SRC\TIMERCLI.H"\
	{$(INCLUDE)}"\NAVAPCOM.H"\
	{$(INCLUDE)}"\PLATFORM.H"\
	{$(INCLUDE)}"\SVCCTRL.H"\


"$(INTDIR)\navuninst.obj" : $(SOURCE) $(DEP_CPP_NAVUN) "$(INTDIR)"


!ENDIF

# End Source File
# End Target
# End Project
################################################################################