# Microsoft Developer Studio Generated NMAKE File, Based on AVISMonitorTest.dsp
!IF "$(CFG)" == ""
CFG=AVISMonitorTest - Win32 Debug
!MESSAGE No configuration specified. Defaulting to AVISMonitorTest - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AVISMonitorTest - Win32 Release" && "$(CFG)" != "AVISMonitorTest - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVISMonitorTest.mak" CFG="AVISMonitorTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVISMonitorTest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "AVISMonitorTest - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "AVISMonitorTest - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\AVISMonitorTest.exe"


CLEAN :
	-@erase "$(INTDIR)\AVISMonitorTest.obj"
	-@erase "$(INTDIR)\AVISMonitorTest.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AVISMonitorTest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\AVISMonitorLib" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\AVISMonitorTest.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AVISMonitorTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib AVISMonitorLib.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\AVISMonitorTest.pdb" /machine:I386 /out:"$(OUTDIR)\AVISMonitorTest.exe" /libpath:"..\..\..\rlib\ea" 
LINK32_OBJS= \
	"$(INTDIR)\AVISMonitorTest.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\AVISMonitorTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AVISMonitorTest - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\AVISMonitorTest.exe"


CLEAN :
	-@erase "$(INTDIR)\AVISMonitorTest.obj"
	-@erase "$(INTDIR)\AVISMonitorTest.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AVISMonitorTest.exe"
	-@erase "$(OUTDIR)\AVISMonitorTest.ilk"
	-@erase "$(OUTDIR)\AVISMonitorTest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\AVISMonitorLib" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\AVISMonitorTest.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AVISMonitorTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib AVISMonitorLib.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\AVISMonitorTest.pdb" /debug /machine:I386 /out:"$(OUTDIR)\AVISMonitorTest.exe" /pdbtype:sept /libpath:"..\..\..\dlib\ea" 
LINK32_OBJS= \
	"$(INTDIR)\AVISMonitorTest.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\AVISMonitorTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("AVISMonitorTest.dep")
!INCLUDE "AVISMonitorTest.dep"
!ELSE 
!MESSAGE Warning: cannot find "AVISMonitorTest.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AVISMonitorTest - Win32 Release" || "$(CFG)" == "AVISMonitorTest - Win32 Debug"
SOURCE=.\AVISMonitorTest.cpp

"$(INTDIR)\AVISMonitorTest.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISMonitorTest.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "AVISMonitorTest - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\..\AVISMonitorLib" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\AVISMonitorTest.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AVISMonitorTest.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AVISMonitorTest - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\AVISMonitorLib" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\AVISMonitorTest.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AVISMonitorTest.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

