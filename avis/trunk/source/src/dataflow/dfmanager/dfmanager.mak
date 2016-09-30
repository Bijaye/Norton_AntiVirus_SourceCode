# Microsoft Developer Studio Generated NMAKE File, Based on DFManager.dsp
!IF "$(CFG)" == ""
CFG=DFManager - Win32 Debug IDE
!MESSAGE No configuration specified. Defaulting to DFManager - Win32 Debug IDE.
!ENDIF 

!IF "$(CFG)" != "DFManager - Win32 Release" && "$(CFG)" != "DFManager - Win32 Debug" && "$(CFG)" != "DFManager - Win32 Debug IDE" && "$(CFG)" != "DFManager - Win32 Release IDE"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "DFManager - Win32 Release"

OUTDIR=///LIBD///
INTDIR=///OBJD///\dataflow\DFManager
# Begin Custom Macros
OutDir=///LIBD///\ 
# End Custom Macros

ALL : "$(OUTDIR)\DFManager.lib"


CLEAN :
	-@erase "$(INTDIR)\dfdbrequests.obj"
	-@erase "$(INTDIR)\dfdbselect.obj"
	-@erase "$(INTDIR)\DFDirInfo.obj"
	-@erase "$(INTDIR)\dfmanager.obj"
	-@erase "$(INTDIR)\dfpackthread.obj"
	-@erase "$(INTDIR)\dfsample.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\DFManager.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\inc" /I "..\com" /I "..\dfgui" /I "..\..\avisdb" /I "..\..\avissendmail" /I "..\..\aviscommon" /I "..\..\avisfile" /I "..\..\avisfile\md5" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DFManager.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DFManager.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\DFManager.lib" /out:"///LIBD///\avisdfmg.lib" /out:"///LIBD///\avisdfmg.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dfdbrequests.obj" \
	"$(INTDIR)\dfdbselect.obj" \
	"$(INTDIR)\DFDirInfo.obj" \
	"$(INTDIR)\dfmanager.obj" \
	"$(INTDIR)\dfpackthread.obj" \
	"$(INTDIR)\dfsample.obj"

"$(OUTDIR)\DFManager.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug"

OUTDIR=///LIBD///
INTDIR=///OBJD///\dataflow\DFManager
# Begin Custom Macros
OutDir=///LIBD///\ 
# End Custom Macros

ALL : "$(OUTDIR)\DFManager.lib"


CLEAN :
	-@erase "$(INTDIR)\dfdbrequests.obj"
	-@erase "$(INTDIR)\dfdbselect.obj"
	-@erase "$(INTDIR)\DFDirInfo.obj"
	-@erase "$(INTDIR)\dfmanager.obj"
	-@erase "$(INTDIR)\dfpackthread.obj"
	-@erase "$(INTDIR)\dfsample.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\DFManager.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "..\inc" /I "..\com" /I "..\dfgui" /I "..\..\avisdb" /I "..\..\aviscommon" /I "..\..\avissendmail" /I "..\..\avisfile" /I "..\..\avisfile\md5" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DFManager.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DFManager.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\DFManager.lib" /out:"///LIBD///\avisdfmg.lib" /out:"///LIBD///\avisdfmg.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dfdbrequests.obj" \
	"$(INTDIR)\dfdbselect.obj" \
	"$(INTDIR)\DFDirInfo.obj" \
	"$(INTDIR)\dfmanager.obj" \
	"$(INTDIR)\dfpackthread.obj" \
	"$(INTDIR)\dfsample.obj"

"$(OUTDIR)\DFManager.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug IDE"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\lib\avisdfmg.lib" "$(OUTDIR)\DFManager.bsc"


CLEAN :
	-@erase "$(INTDIR)\dfdbrequests.obj"
	-@erase "$(INTDIR)\dfdbrequests.sbr"
	-@erase "$(INTDIR)\dfdbselect.obj"
	-@erase "$(INTDIR)\dfdbselect.sbr"
	-@erase "$(INTDIR)\DFDirInfo.obj"
	-@erase "$(INTDIR)\DFDirInfo.sbr"
	-@erase "$(INTDIR)\dfmanager.obj"
	-@erase "$(INTDIR)\dfmanager.sbr"
	-@erase "$(INTDIR)\dfpackthread.obj"
	-@erase "$(INTDIR)\dfpackthread.sbr"
	-@erase "$(INTDIR)\dfsample.obj"
	-@erase "$(INTDIR)\dfsample.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\DFManager.bsc"
	-@erase "..\..\..\lib\avisdfmg.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\avissendmail" /I "e:\newdev\src\dataflow\com" /I "e:\newdev\src\dataflow\dfgui" /I "e:\newdev\src\inc" /I "e:\newdev\src\avisdb" /I "e:\newdev\src\aviscommon" /I "e:\newdev\src\aviscommon\system" /I "e:\newdev\src\avidfile" /I "e:\newdev\src\avisfile\md5" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\DFManager.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DFManager.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dfdbrequests.sbr" \
	"$(INTDIR)\dfdbselect.sbr" \
	"$(INTDIR)\DFDirInfo.sbr" \
	"$(INTDIR)\dfmanager.sbr" \
	"$(INTDIR)\dfpackthread.sbr" \
	"$(INTDIR)\dfsample.sbr"

"$(OUTDIR)\DFManager.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"e:\newdev\lib\avisdfmg.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dfdbrequests.obj" \
	"$(INTDIR)\dfdbselect.obj" \
	"$(INTDIR)\DFDirInfo.obj" \
	"$(INTDIR)\dfmanager.obj" \
	"$(INTDIR)\dfpackthread.obj" \
	"$(INTDIR)\dfsample.obj"

"..\..\..\lib\avisdfmg.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DFManager - Win32 Release IDE"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\avisdfmg.lib"


CLEAN :
	-@erase "$(INTDIR)\dfdbrequests.obj"
	-@erase "$(INTDIR)\dfdbselect.obj"
	-@erase "$(INTDIR)\DFDirInfo.obj"
	-@erase "$(INTDIR)\dfmanager.obj"
	-@erase "$(INTDIR)\dfpackthread.obj"
	-@erase "$(INTDIR)\dfsample.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\avisdfmg.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /X /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\dataflow\com" /I "e:\newdev\src\dataflow\dfgui" /I "e:\newdev\src\avisdb" /I "e:\newdev\src\aviscommon" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DFManager.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DFManager.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\avisdfmg.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dfdbrequests.obj" \
	"$(INTDIR)\dfdbselect.obj" \
	"$(INTDIR)\DFDirInfo.obj" \
	"$(INTDIR)\dfmanager.obj" \
	"$(INTDIR)\dfpackthread.obj" \
	"$(INTDIR)\dfsample.obj"

"$(OUTDIR)\avisdfmg.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("DFManager.dep")
!INCLUDE "DFManager.dep"
!ELSE 
!MESSAGE Warning: cannot find "DFManager.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "DFManager - Win32 Release" || "$(CFG)" == "DFManager - Win32 Debug" || "$(CFG)" == "DFManager - Win32 Debug IDE" || "$(CFG)" == "DFManager - Win32 Release IDE"
SOURCE=.\dfdbrequests.cpp

!IF  "$(CFG)" == "DFManager - Win32 Release"


"$(INTDIR)\dfdbrequests.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug"


"$(INTDIR)\dfdbrequests.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug IDE"


"$(INTDIR)\dfdbrequests.obj"	"$(INTDIR)\dfdbrequests.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Release IDE"


"$(INTDIR)\dfdbrequests.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dfdbselect.cpp

!IF  "$(CFG)" == "DFManager - Win32 Release"


"$(INTDIR)\dfdbselect.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug"


"$(INTDIR)\dfdbselect.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug IDE"


"$(INTDIR)\dfdbselect.obj"	"$(INTDIR)\dfdbselect.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Release IDE"


"$(INTDIR)\dfdbselect.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DFDirInfo.cpp

!IF  "$(CFG)" == "DFManager - Win32 Release"


"$(INTDIR)\DFDirInfo.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug"


"$(INTDIR)\DFDirInfo.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug IDE"


"$(INTDIR)\DFDirInfo.obj"	"$(INTDIR)\DFDirInfo.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Release IDE"


"$(INTDIR)\DFDirInfo.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dfmanager.cpp

!IF  "$(CFG)" == "DFManager - Win32 Release"


"$(INTDIR)\dfmanager.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug"


"$(INTDIR)\dfmanager.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug IDE"


"$(INTDIR)\dfmanager.obj"	"$(INTDIR)\dfmanager.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Release IDE"


"$(INTDIR)\dfmanager.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dfpackthread.cpp

!IF  "$(CFG)" == "DFManager - Win32 Release"


"$(INTDIR)\dfpackthread.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug"


"$(INTDIR)\dfpackthread.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug IDE"


"$(INTDIR)\dfpackthread.obj"	"$(INTDIR)\dfpackthread.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Release IDE"


"$(INTDIR)\dfpackthread.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dfsample.cpp

!IF  "$(CFG)" == "DFManager - Win32 Release"


"$(INTDIR)\dfsample.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug"


"$(INTDIR)\dfsample.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Debug IDE"


"$(INTDIR)\dfsample.obj"	"$(INTDIR)\dfsample.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DFManager - Win32 Release IDE"


"$(INTDIR)\dfsample.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

