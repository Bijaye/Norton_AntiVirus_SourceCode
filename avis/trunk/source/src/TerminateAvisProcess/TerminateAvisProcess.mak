# Microsoft Developer Studio Generated NMAKE File, Based on TerminateAvisProcess.dsp
!IF "$(CFG)" == ""
CFG=TerminateAvisProcess - Win32 Debug
!MESSAGE No configuration specified. Defaulting to TerminateAvisProcess - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "TerminateAvisProcess - Win32 Release" && "$(CFG)" != "TerminateAvisProcess - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TerminateAvisProcess.mak" CFG="TerminateAvisProcess - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TerminateAvisProcess - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "TerminateAvisProcess - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "TerminateAvisProcess - Win32 Release"

OUTDIR=///OUTD///\nt
INTDIR=///OBJD///\TerminateAvisProcess
# Begin Custom Macros
OutDir=///OUTD///\nt
# End Custom Macros

ALL : "$(OUTDIR)\TerminateAvisProcess.exe"


CLEAN :
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TerminateAvisProcess.obj"
	-@erase "$(INTDIR)\TerminateAvisProcess.pch"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\TerminateAvisProcess.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\TerminateAvisProcess.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TerminateAvisProcess.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib psapi.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\TerminateAvisProcess.pdb" /machine:I386 /out:"$(OUTDIR)\TerminateAvisProcess.exe" 
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TerminateAvisProcess.obj"

"$(OUTDIR)\TerminateAvisProcess.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TerminateAvisProcess - Win32 Debug"

OUTDIR=///OUTD///\nt
INTDIR=///OBJD///\TerminateAvisProcess
# Begin Custom Macros
OutDir=///OUTD///\nt
# End Custom Macros

ALL : "$(OUTDIR)\TerminateAvisProcess.exe"


CLEAN :
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TerminateAvisProcess.obj"
	-@erase "$(INTDIR)\TerminateAvisProcess.pch"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\TerminateAvisProcess.exe"
	-@erase "$(OUTDIR)\TerminateAvisProcess.ilk"
	-@erase "$(OUTDIR)\TerminateAvisProcess.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\TerminateAvisProcess.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TerminateAvisProcess.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib psapi.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\TerminateAvisProcess.pdb" /debug /machine:I386 /out:"$(OUTDIR)\TerminateAvisProcess.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TerminateAvisProcess.obj"

"$(OUTDIR)\TerminateAvisProcess.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
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
!IF EXISTS("TerminateAvisProcess.dep")
!INCLUDE "TerminateAvisProcess.dep"
!ELSE 
!MESSAGE Warning: cannot find "TerminateAvisProcess.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "TerminateAvisProcess - Win32 Release" || "$(CFG)" == "TerminateAvisProcess - Win32 Debug"
SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "TerminateAvisProcess - Win32 Release"

CPP_SWITCHES=/nologo /MDd /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\TerminateAvisProcess.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TerminateAvisProcess.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "TerminateAvisProcess - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\TerminateAvisProcess.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TerminateAvisProcess.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TerminateAvisProcess.cpp

"$(INTDIR)\TerminateAvisProcess.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TerminateAvisProcess.pch"



!ENDIF 

