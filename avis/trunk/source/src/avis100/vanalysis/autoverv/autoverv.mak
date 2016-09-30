# Microsoft Developer Studio Generated NMAKE File, Based on autoverv.dsp
!IF "$(CFG)" == ""
CFG=autoverv - Win32 Debug
!MESSAGE No configuration specified. Defaulting to autoverv - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "autoverv - Win32 Release" && "$(CFG)" !=\
 "autoverv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "autoverv.mak" CFG="autoverv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "autoverv - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "autoverv - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "autoverv - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "dummy" "$(OUTDIR)\autoverv.exe"

!ELSE 

ALL : "dummy" "$(OUTDIR)\autoverv.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\autoverv.obj"
	-@erase "$(INTDIR)\bigrams.obj"
	-@erase "$(INTDIR)\dotProduct.obj"
	-@erase "$(INTDIR)\encryption.obj"
	-@erase "$(INTDIR)\File.obj"
	-@erase "$(INTDIR)\FileClass.obj"
	-@erase "$(INTDIR)\key.obj"
	-@erase "$(INTDIR)\lcs.obj"
	-@erase "$(INTDIR)\loadsamples.obj"
	-@erase "$(INTDIR)\location.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\match.obj"
	-@erase "$(INTDIR)\newhandler.obj"
	-@erase "$(INTDIR)\polymath.obj"
	-@erase "$(INTDIR)\recon.obj"
	-@erase "$(INTDIR)\sample.obj"
	-@erase "$(INTDIR)\section.obj"
	-@erase "$(INTDIR)\segment.obj"
	-@erase "$(INTDIR)\test.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\verification.obj"
	-@erase "$(OUTDIR)\autoverv.exe"
	-@erase "$(OUTDIR)\autoverv.ilk"
	-@erase "dummy"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "_MBCS" /Fp"$(INTDIR)\autoverv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\autoverv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=buildtime.obj kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\autoverv.pdb" /machine:I386 /out:"$(OUTDIR)\autoverv.exe" 
LINK32_OBJS= \
	"$(INTDIR)\autoverv.obj" \
	"$(INTDIR)\bigrams.obj" \
	"$(INTDIR)\dotProduct.obj" \
	"$(INTDIR)\encryption.obj" \
	"$(INTDIR)\File.obj" \
	"$(INTDIR)\FileClass.obj" \
	"$(INTDIR)\key.obj" \
	"$(INTDIR)\lcs.obj" \
	"$(INTDIR)\loadsamples.obj" \
	"$(INTDIR)\location.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\match.obj" \
	"$(INTDIR)\newhandler.obj" \
	"$(INTDIR)\polymath.obj" \
	"$(INTDIR)\recon.obj" \
	"$(INTDIR)\sample.obj" \
	"$(INTDIR)\section.obj" \
	"$(INTDIR)\segment.obj" \
	"$(INTDIR)\test.obj" \
	"$(INTDIR)\verification.obj"

"$(OUTDIR)\autoverv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
   cl -nologo -c -Zl -Fo.\Release\buildtime.obj buildtime.cpp
	 $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

IntDir=.\Release
SOURCE=$(InputPath)

!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\autoverv.exe" "$(OUTDIR)\autoverv.bsc"

!ELSE 

ALL : "$(OUTDIR)\autoverv.exe" "$(OUTDIR)\autoverv.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\autoverv.obj"
	-@erase "$(INTDIR)\autoverv.sbr"
	-@erase "$(INTDIR)\bigrams.obj"
	-@erase "$(INTDIR)\bigrams.sbr"
	-@erase "$(INTDIR)\buildtime.obj"
	-@erase "$(INTDIR)\buildtime.sbr"
	-@erase "$(INTDIR)\dotProduct.obj"
	-@erase "$(INTDIR)\dotProduct.sbr"
	-@erase "$(INTDIR)\encryption.obj"
	-@erase "$(INTDIR)\encryption.sbr"
	-@erase "$(INTDIR)\File.obj"
	-@erase "$(INTDIR)\File.sbr"
	-@erase "$(INTDIR)\FileClass.obj"
	-@erase "$(INTDIR)\FileClass.sbr"
	-@erase "$(INTDIR)\key.obj"
	-@erase "$(INTDIR)\key.sbr"
	-@erase "$(INTDIR)\lcs.obj"
	-@erase "$(INTDIR)\lcs.sbr"
	-@erase "$(INTDIR)\loadsamples.obj"
	-@erase "$(INTDIR)\loadsamples.sbr"
	-@erase "$(INTDIR)\location.obj"
	-@erase "$(INTDIR)\location.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\map.sbr"
	-@erase "$(INTDIR)\match.obj"
	-@erase "$(INTDIR)\match.sbr"
	-@erase "$(INTDIR)\newhandler.obj"
	-@erase "$(INTDIR)\newhandler.sbr"
	-@erase "$(INTDIR)\polymath.obj"
	-@erase "$(INTDIR)\polymath.sbr"
	-@erase "$(INTDIR)\recon.obj"
	-@erase "$(INTDIR)\recon.sbr"
	-@erase "$(INTDIR)\sample.obj"
	-@erase "$(INTDIR)\sample.sbr"
	-@erase "$(INTDIR)\section.obj"
	-@erase "$(INTDIR)\section.sbr"
	-@erase "$(INTDIR)\segment.obj"
	-@erase "$(INTDIR)\segment.sbr"
	-@erase "$(INTDIR)\test.obj"
	-@erase "$(INTDIR)\test.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\verification.obj"
	-@erase "$(INTDIR)\verification.sbr"
	-@erase "$(OUTDIR)\autoverv.bsc"
	-@erase "$(OUTDIR)\autoverv.exe"
	-@erase "$(OUTDIR)\autoverv.ilk"
	-@erase "$(OUTDIR)\autoverv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "MYASSERT" /D "WIN32"\
 /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\autoverv.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\autoverv.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\autoverv.sbr" \
	"$(INTDIR)\bigrams.sbr" \
	"$(INTDIR)\buildtime.sbr" \
	"$(INTDIR)\dotProduct.sbr" \
	"$(INTDIR)\encryption.sbr" \
	"$(INTDIR)\File.sbr" \
	"$(INTDIR)\FileClass.sbr" \
	"$(INTDIR)\key.sbr" \
	"$(INTDIR)\lcs.sbr" \
	"$(INTDIR)\loadsamples.sbr" \
	"$(INTDIR)\location.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\map.sbr" \
	"$(INTDIR)\match.sbr" \
	"$(INTDIR)\newhandler.sbr" \
	"$(INTDIR)\polymath.sbr" \
	"$(INTDIR)\recon.sbr" \
	"$(INTDIR)\sample.sbr" \
	"$(INTDIR)\section.sbr" \
	"$(INTDIR)\segment.sbr" \
	"$(INTDIR)\test.sbr" \
	"$(INTDIR)\verification.sbr"

"$(OUTDIR)\autoverv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /verbose /incremental:yes\
 /pdb:"$(OUTDIR)\autoverv.pdb" /debug /machine:I386\
 /nodefaultlib:"D:\bchecker\bcinterf.lib" /out:"$(OUTDIR)\autoverv.exe"\
 /pdbtype:sept /libpath:"J:\bchecker" 
LINK32_OBJS= \
	"$(INTDIR)\autoverv.obj" \
	"$(INTDIR)\bigrams.obj" \
	"$(INTDIR)\buildtime.obj" \
	"$(INTDIR)\dotProduct.obj" \
	"$(INTDIR)\encryption.obj" \
	"$(INTDIR)\File.obj" \
	"$(INTDIR)\FileClass.obj" \
	"$(INTDIR)\key.obj" \
	"$(INTDIR)\lcs.obj" \
	"$(INTDIR)\loadsamples.obj" \
	"$(INTDIR)\location.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\match.obj" \
	"$(INTDIR)\newhandler.obj" \
	"$(INTDIR)\polymath.obj" \
	"$(INTDIR)\recon.obj" \
	"$(INTDIR)\sample.obj" \
	"$(INTDIR)\section.obj" \
	"$(INTDIR)\segment.obj" \
	"$(INTDIR)\test.obj" \
	"$(INTDIR)\verification.obj"

"$(OUTDIR)\autoverv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
   cl -nologo -c -Zl -Fo.\Debug\buildtime.obj buildtime.cpp
	 $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

IntDir=.\Debug
SOURCE=$(InputPath)

!ENDIF 


!IF "$(CFG)" == "autoverv - Win32 Release" || "$(CFG)" ==\
 "autoverv - Win32 Debug"
SOURCE=.\autoverv.cpp
DEP_CPP_AUTOV=\
	".\autoverv.h"\
	".\polymath.h"\
	

!IF  "$(CFG)" == "autoverv - Win32 Release"


"$(INTDIR)\autoverv.obj" : $(SOURCE) $(DEP_CPP_AUTOV) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"


"$(INTDIR)\autoverv.obj"	"$(INTDIR)\autoverv.sbr" : $(SOURCE) $(DEP_CPP_AUTOV)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\bigrams.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_BIGRA=\
	".\autoverv.h"\
	".\bigrams.h"\
	".\encryption.h"\
	".\File.h"\
	".\location.h"\
	".\newhandler.h"\
	".\polymath.h"\
	

"$(INTDIR)\bigrams.obj" : $(SOURCE) $(DEP_CPP_BIGRA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_BIGRA=\
	".\autoverv.h"\
	".\bigrams.h"\
	".\encryption.h"\
	".\File.h"\
	".\location.h"\
	".\newhandler.h"\
	".\polymath.h"\
	

"$(INTDIR)\bigrams.obj"	"$(INTDIR)\bigrams.sbr" : $(SOURCE) $(DEP_CPP_BIGRA)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\buildtime.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

InputPath=.\buildtime.cpp

"dummy"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl -c buildtime.cpp

!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_BUILD=\
	".\buildtime.h"\
	

"$(INTDIR)\buildtime.obj"	"$(INTDIR)\buildtime.sbr" : $(SOURCE)\
 $(DEP_CPP_BUILD) "$(INTDIR)"


!ENDIF 

SOURCE=.\dotProduct.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_DOTPR=\
	".\assert.h"\
	".\autoverv.h"\
	".\bigrams.h"\
	".\break.h"\
	".\dotProduct.h"\
	".\encryption.h"\
	".\File.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\Sample.h"\
	".\segment.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\dotProduct.obj" : $(SOURCE) $(DEP_CPP_DOTPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_DOTPR=\
	".\assert.h"\
	".\autoverv.h"\
	".\bigrams.h"\
	".\break.h"\
	".\dotProduct.h"\
	".\encryption.h"\
	".\File.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\Sample.h"\
	".\segment.h"\
	

"$(INTDIR)\dotProduct.obj"	"$(INTDIR)\dotProduct.sbr" : $(SOURCE)\
 $(DEP_CPP_DOTPR) "$(INTDIR)"


!ENDIF 

SOURCE=.\encryption.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_ENCRY=\
	".\autoverv.h"\
	".\encryption.h"\
	".\polymath.h"\
	

"$(INTDIR)\encryption.obj" : $(SOURCE) $(DEP_CPP_ENCRY) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_ENCRY=\
	".\autoverv.h"\
	".\encryption.h"\
	".\polymath.h"\
	

"$(INTDIR)\encryption.obj"	"$(INTDIR)\encryption.sbr" : $(SOURCE)\
 $(DEP_CPP_ENCRY) "$(INTDIR)"


!ENDIF 

SOURCE=.\File.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_FILE_=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\key.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\section.h"\
	".\segment.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\File.obj" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_FILE_=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\key.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\section.h"\
	".\segment.h"\
	

"$(INTDIR)\File.obj"	"$(INTDIR)\File.sbr" : $(SOURCE) $(DEP_CPP_FILE_)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\FileClass.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_FILEC=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\FileClass.h"\
	".\key.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\recon.h"\
	".\Sample.h"\
	".\section.h"\
	".\segment.h"\
	".\test.h"\
	".\verification.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\FileClass.obj" : $(SOURCE) $(DEP_CPP_FILEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_FILEC=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\FileClass.h"\
	".\key.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\recon.h"\
	".\Sample.h"\
	".\section.h"\
	".\segment.h"\
	".\test.h"\
	".\verification.h"\
	

"$(INTDIR)\FileClass.obj"	"$(INTDIR)\FileClass.sbr" : $(SOURCE)\
 $(DEP_CPP_FILEC) "$(INTDIR)"


!ENDIF 

SOURCE=.\key.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_KEY_C=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\key.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\section.h"\
	".\segment.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\key.obj" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_KEY_C=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\key.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\section.h"\
	".\segment.h"\
	

"$(INTDIR)\key.obj"	"$(INTDIR)\key.sbr" : $(SOURCE) $(DEP_CPP_KEY_C)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\lcs.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_LCS_C=\
	".\assert.h"\
	".\autoverv.h"\
	".\bigrams.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\lcs.h"\
	".\location.h"\
	".\match.h"\
	".\newhandler.h"\
	".\polymath.h"\
	".\segment.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\lcs.obj" : $(SOURCE) $(DEP_CPP_LCS_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_LCS_C=\
	".\assert.h"\
	".\autoverv.h"\
	".\bigrams.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\lcs.h"\
	".\location.h"\
	".\match.h"\
	".\newhandler.h"\
	".\polymath.h"\
	".\segment.h"\
	

"$(INTDIR)\lcs.obj"	"$(INTDIR)\lcs.sbr" : $(SOURCE) $(DEP_CPP_LCS_C)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\loadsamples.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_LOADS=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\loadsamples.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\Sample.h"\
	".\segment.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\loadsamples.obj" : $(SOURCE) $(DEP_CPP_LOADS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_LOADS=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\loadsamples.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\Sample.h"\
	".\segment.h"\
	

"$(INTDIR)\loadsamples.obj"	"$(INTDIR)\loadsamples.sbr" : $(SOURCE)\
 $(DEP_CPP_LOADS) "$(INTDIR)"


!ENDIF 

SOURCE=.\location.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_LOCAT=\
	".\autoverv.h"\
	".\location.h"\
	".\polymath.h"\
	

"$(INTDIR)\location.obj" : $(SOURCE) $(DEP_CPP_LOCAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_LOCAT=\
	".\autoverv.h"\
	".\location.h"\
	".\polymath.h"\
	

"$(INTDIR)\location.obj"	"$(INTDIR)\location.sbr" : $(SOURCE) $(DEP_CPP_LOCAT)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\main.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_MAIN_=\
	".\assert.h"\
	".\autoverv.h"\
	".\bigrams.h"\
	".\break.h"\
	".\buildtime.h"\
	".\dotProduct.h"\
	".\encryption.h"\
	".\File.h"\
	".\FileClass.h"\
	".\key.h"\
	".\lcs.h"\
	".\loadsamples.h"\
	".\location.h"\
	".\match.h"\
	".\newhandler.h"\
	".\polymath.h"\
	".\recon.h"\
	".\Sample.h"\
	".\section.h"\
	".\segment.h"\
	".\test.h"\
	".\verification.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_MAIN_=\
	".\assert.h"\
	".\autoverv.h"\
	".\bigrams.h"\
	".\break.h"\
	".\buildtime.h"\
	".\dotProduct.h"\
	".\encryption.h"\
	".\File.h"\
	".\FileClass.h"\
	".\key.h"\
	".\lcs.h"\
	".\loadsamples.h"\
	".\location.h"\
	".\match.h"\
	".\newhandler.h"\
	".\polymath.h"\
	".\recon.h"\
	".\Sample.h"\
	".\section.h"\
	".\segment.h"\
	".\test.h"\
	".\verification.h"\
	

"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) $(DEP_CPP_MAIN_)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\map.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_MAP_C=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\location.h"\
	".\map.h"\
	".\polymath.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\map.obj" : $(SOURCE) $(DEP_CPP_MAP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_MAP_C=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\location.h"\
	".\map.h"\
	".\polymath.h"\
	

"$(INTDIR)\map.obj"	"$(INTDIR)\map.sbr" : $(SOURCE) $(DEP_CPP_MAP_C)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\match.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_MATCH=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\lcs.h"\
	".\location.h"\
	".\map.h"\
	".\match.h"\
	".\polymath.h"\
	".\segment.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\match.obj" : $(SOURCE) $(DEP_CPP_MATCH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_MATCH=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\lcs.h"\
	".\location.h"\
	".\map.h"\
	".\match.h"\
	".\polymath.h"\
	".\segment.h"\
	

"$(INTDIR)\match.obj"	"$(INTDIR)\match.sbr" : $(SOURCE) $(DEP_CPP_MATCH)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\newhandler.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_NEWHA=\
	".\assert.h"\
	".\break.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\newhandler.obj" : $(SOURCE) $(DEP_CPP_NEWHA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_NEWHA=\
	".\assert.h"\
	".\break.h"\
	

"$(INTDIR)\newhandler.obj"	"$(INTDIR)\newhandler.sbr" : $(SOURCE)\
 $(DEP_CPP_NEWHA) "$(INTDIR)"


!ENDIF 

SOURCE=.\polymath.cpp
DEP_CPP_POLYM=\
	".\polymath.h"\
	

!IF  "$(CFG)" == "autoverv - Win32 Release"


"$(INTDIR)\polymath.obj" : $(SOURCE) $(DEP_CPP_POLYM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"


"$(INTDIR)\polymath.obj"	"$(INTDIR)\polymath.sbr" : $(SOURCE) $(DEP_CPP_POLYM)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\recon.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_RECON=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\key.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\recon.h"\
	".\section.h"\
	".\segment.h"\
	".\verification.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\recon.obj" : $(SOURCE) $(DEP_CPP_RECON) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_RECON=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\key.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\recon.h"\
	".\section.h"\
	".\segment.h"\
	".\verification.h"\
	

"$(INTDIR)\recon.obj"	"$(INTDIR)\recon.sbr" : $(SOURCE) $(DEP_CPP_RECON)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\sample.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_SAMPL=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\Sample.h"\
	".\segment.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\sample.obj" : $(SOURCE) $(DEP_CPP_SAMPL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_SAMPL=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\Sample.h"\
	".\segment.h"\
	

"$(INTDIR)\sample.obj"	"$(INTDIR)\sample.sbr" : $(SOURCE) $(DEP_CPP_SAMPL)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\section.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_SECTI=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\FileClass.h"\
	".\key.h"\
	".\location.h"\
	".\map.h"\
	".\match.h"\
	".\polymath.h"\
	".\recon.h"\
	".\Sample.h"\
	".\section.h"\
	".\segment.h"\
	".\test.h"\
	".\verification.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\section.obj" : $(SOURCE) $(DEP_CPP_SECTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_SECTI=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\FileClass.h"\
	".\key.h"\
	".\location.h"\
	".\map.h"\
	".\match.h"\
	".\polymath.h"\
	".\recon.h"\
	".\Sample.h"\
	".\section.h"\
	".\segment.h"\
	".\test.h"\
	".\verification.h"\
	

"$(INTDIR)\section.obj"	"$(INTDIR)\section.sbr" : $(SOURCE) $(DEP_CPP_SECTI)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\segment.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_SEGME=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\location.h"\
	".\polymath.h"\
	".\segment.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\segment.obj" : $(SOURCE) $(DEP_CPP_SEGME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_SEGME=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\location.h"\
	".\polymath.h"\
	".\segment.h"\
	

"$(INTDIR)\segment.obj"	"$(INTDIR)\segment.sbr" : $(SOURCE) $(DEP_CPP_SEGME)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\test.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_TEST_=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\FileClass.h"\
	".\key.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\recon.h"\
	".\Sample.h"\
	".\section.h"\
	".\segment.h"\
	".\test.h"\
	".\verification.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\test.obj" : $(SOURCE) $(DEP_CPP_TEST_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_TEST_=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\FileClass.h"\
	".\key.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\recon.h"\
	".\Sample.h"\
	".\section.h"\
	".\segment.h"\
	".\test.h"\
	".\verification.h"\
	

"$(INTDIR)\test.obj"	"$(INTDIR)\test.sbr" : $(SOURCE) $(DEP_CPP_TEST_)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\verification.cpp

!IF  "$(CFG)" == "autoverv - Win32 Release"

DEP_CPP_VERIF=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\key.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\section.h"\
	".\segment.h"\
	".\verification.h"\
	{$(INCLUDE)}"assert.h"\
	

"$(INTDIR)\verification.obj" : $(SOURCE) $(DEP_CPP_VERIF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "autoverv - Win32 Debug"

DEP_CPP_VERIF=\
	".\assert.h"\
	".\autoverv.h"\
	".\break.h"\
	".\encryption.h"\
	".\File.h"\
	".\key.h"\
	".\location.h"\
	".\match.h"\
	".\polymath.h"\
	".\section.h"\
	".\segment.h"\
	".\verification.h"\
	

"$(INTDIR)\verification.obj"	"$(INTDIR)\verification.sbr" : $(SOURCE)\
 $(DEP_CPP_VERIF) "$(INTDIR)"


!ENDIF 


!ENDIF 

