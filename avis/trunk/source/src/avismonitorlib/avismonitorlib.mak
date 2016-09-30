# Microsoft Developer Studio Generated NMAKE File, Based on AVISMonitorLib.dsp
!IF "$(CFG)" == ""
CFG=AVISMonitorLib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to AVISMonitorLib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AVISMonitorLib - Win32 Release" && "$(CFG)" != "AVISMonitorLib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVISMonitorLib.mak" CFG="AVISMonitorLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVISMonitorLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AVISMonitorLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AVISMonitorLib - Win32 Release"

OUTDIR=///LIBD///
INTDIR=///OBJD///\AVISMonitorLib
# Begin Custom Macros
OutDir=///LIBD///\ 
# End Custom Macros

ALL : "$(OUTDIR)\AVISMonitorLib.lib"


CLEAN :
	-@erase "$(INTDIR)\AVISMonitorLib.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AVISMonitorLib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AVISMonitorLib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\AVISMonitorLib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AVISMonitorLib.obj"

"$(OUTDIR)\AVISMonitorLib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AVISMonitorLib - Win32 Debug"

OUTDIR=///LIBD///
INTDIR=///OBJD///\AVISMonitorLib
# Begin Custom Macros
OutDir=///LIBD///\ 
# End Custom Macros

ALL : "$(OUTDIR)\AVISMonitorLib.lib"


CLEAN :
	-@erase "$(INTDIR)\AVISMonitorLib.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AVISMonitorLib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AVISMonitorLib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\AVISMonitorLib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AVISMonitorLib.obj"

"$(OUTDIR)\AVISMonitorLib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("AVISMonitorLib.dep")
!INCLUDE "AVISMonitorLib.dep"
!ELSE 
!MESSAGE Warning: cannot find "AVISMonitorLib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AVISMonitorLib - Win32 Release" || "$(CFG)" == "AVISMonitorLib - Win32 Debug"
SOURCE=.\AVISMonitorLib.c

"$(INTDIR)\AVISMonitorLib.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

