# Microsoft Developer Studio Generated NMAKE File, Based on dfeval.dsp
!IF "$(CFG)" == ""
CFG=DFEval - Win32 Debug GUI
!MESSAGE No configuration specified. Defaulting to DFEval - Win32 Debug GUI.
!ENDIF 

!IF "$(CFG)" != "DFEval - Win32 Release" && "$(CFG)" != "DFEval - Win32 Debug" && "$(CFG)" != "DFEval - Win32 Debug GUI" && "$(CFG)" != "DFEval - Win32 Release GUI"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DFEval - Win32 Release"

OUTDIR=///LIBD///
INTDIR=///OBJD///\dataflow\dfeval
# Begin Custom Macros
OutDir=///LIBD///\ 
# End Custom Macros

ALL : "$(OUTDIR)\dfeval.lib"


CLEAN :
	-@erase "$(INTDIR)\DFEvalState.obj"
	-@erase "$(INTDIR)\DFEvalStatus.obj"
	-@erase "$(INTDIR)\DFMatrix.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dfeval.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\inc" /I "..\alm" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\dfeval.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dfeval.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dfeval.lib" /out:"///LIBD///\avisdfev.lib" 
LIB32_OBJS= \
	"$(INTDIR)\DFEvalState.obj" \
	"$(INTDIR)\DFEvalStatus.obj" \
	"$(INTDIR)\DFMatrix.obj"

"$(OUTDIR)\dfeval.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DFEval - Win32 Debug"

OUTDIR=///LIBD///
INTDIR=///OBJD///\dataflow\dfeval
# Begin Custom Macros
OutDir=///LIBD///\ 
# End Custom Macros

ALL : "$(OUTDIR)\dfeval.lib"


CLEAN :
	-@erase "$(INTDIR)\DFEvalState.obj"
	-@erase "$(INTDIR)\DFEvalStatus.obj"
	-@erase "$(INTDIR)\DFMatrix.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dfeval.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "..\inc" /I "..\alm" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\dfeval.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dfeval.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dfeval.lib" /out:"///LIBD///\avisdfev.lib" 
LIB32_OBJS= \
	"$(INTDIR)\DFEvalState.obj" \
	"$(INTDIR)\DFEvalStatus.obj" \
	"$(INTDIR)\DFMatrix.obj"

"$(OUTDIR)\dfeval.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DFEval - Win32 Debug GUI"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\lib\avisdfev.lib"


CLEAN :
	-@erase "$(INTDIR)\DFEvalState.obj"
	-@erase "$(INTDIR)\DFEvalStatus.obj"
	-@erase "$(INTDIR)\DFMatrix.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\lib\avisdfev.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\dataflow\dfgui" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\dfeval.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dfeval.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"e:\newdev\lib\avisdfev.lib" 
LIB32_OBJS= \
	"$(INTDIR)\DFEvalState.obj" \
	"$(INTDIR)\DFEvalStatus.obj" \
	"$(INTDIR)\DFMatrix.obj"

"..\..\..\lib\avisdfev.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DFEval - Win32 Release GUI"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\lib\avisdfev.lib"


CLEAN :
	-@erase "$(INTDIR)\DFEvalState.obj"
	-@erase "$(INTDIR)\DFEvalStatus.obj"
	-@erase "$(INTDIR)\DFMatrix.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\lib\avisdfev.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /O2 /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\dataflow\dfgui" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\dfeval.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dfeval.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"e:\newdev\lib\avisdfev.lib" 
LIB32_OBJS= \
	"$(INTDIR)\DFEvalState.obj" \
	"$(INTDIR)\DFEvalStatus.obj" \
	"$(INTDIR)\DFMatrix.obj"

"..\..\..\lib\avisdfev.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("dfeval.dep")
!INCLUDE "dfeval.dep"
!ELSE 
!MESSAGE Warning: cannot find "dfeval.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "DFEval - Win32 Release" || "$(CFG)" == "DFEval - Win32 Debug" || "$(CFG)" == "DFEval - Win32 Debug GUI" || "$(CFG)" == "DFEval - Win32 Release GUI"
SOURCE=.\DFEvalState.cpp

"$(INTDIR)\DFEvalState.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DFEvalStatus.cpp

"$(INTDIR)\DFEvalStatus.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DFMatrix.cpp

"$(INTDIR)\DFMatrix.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

