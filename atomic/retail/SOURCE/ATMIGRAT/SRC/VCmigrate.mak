# Microsoft Developer Studio Generated NMAKE File, Based on Migrate.dsp
!IF "$(CFG)" == ""
CFG=Migrate - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Migrate - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Migrate - Win32 Release" && "$(CFG)" !=\
 "Migrate - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Migrate.mak" CFG="Migrate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Migrate - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Migrate - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Migrate - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Migrate.dll"

!ELSE 

ALL : "$(OUTDIR)\Migrate.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\CreatMig.obj"
	-@erase "$(INTDIR)\CRegent.obj"
	-@erase "$(INTDIR)\CWSTRING.OBJ"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\migrant.obj"
	-@erase "$(INTDIR)\migrate.obj"
	-@erase "$(INTDIR)\migrate.res"
	-@erase "$(INTDIR)\NavMig95.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\Migrate.dll"
	-@erase "$(OUTDIR)\Migrate.exp"
	-@erase "$(OUTDIR)\Migrate.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\Migrate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\migrate.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Migrate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\Migrate.pdb" /machine:I386 /def:".\migrate.def"\
 /out:"$(OUTDIR)\Migrate.dll" /implib:"$(OUTDIR)\Migrate.lib" 
DEF_FILE= \
	".\migrate.def"
LINK32_OBJS= \
	"$(INTDIR)\CreatMig.obj" \
	"$(INTDIR)\CRegent.obj" \
	"$(INTDIR)\CWSTRING.OBJ" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\migrant.obj" \
	"$(INTDIR)\migrate.obj" \
	"$(INTDIR)\migrate.res" \
	"$(INTDIR)\NavMig95.obj" \
	"$(INTDIR)\utils.obj"

"$(OUTDIR)\Migrate.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Migrate - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Migrate.dll"

!ELSE 

ALL : "$(OUTDIR)\Migrate.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\CreatMig.obj"
	-@erase "$(INTDIR)\CRegent.obj"
	-@erase "$(INTDIR)\CWSTRING.OBJ"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\migrant.obj"
	-@erase "$(INTDIR)\migrate.obj"
	-@erase "$(INTDIR)\migrate.res"
	-@erase "$(INTDIR)\NavMig95.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\Migrate.dll"
	-@erase "$(OUTDIR)\Migrate.exp"
	-@erase "$(OUTDIR)\Migrate.ilk"
	-@erase "$(OUTDIR)\Migrate.lib"
	-@erase "$(OUTDIR)\Migrate.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\Migrate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\migrate.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Migrate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\Migrate.pdb" /debug /machine:I386 /def:".\migrate.def"\
 /out:"$(OUTDIR)\Migrate.dll" /implib:"$(OUTDIR)\Migrate.lib" /pdbtype:sept 
DEF_FILE= \
	".\migrate.def"
LINK32_OBJS= \
	"$(INTDIR)\CreatMig.obj" \
	"$(INTDIR)\CRegent.obj" \
	"$(INTDIR)\CWSTRING.OBJ" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\migrant.obj" \
	"$(INTDIR)\migrate.obj" \
	"$(INTDIR)\migrate.res" \
	"$(INTDIR)\NavMig95.obj" \
	"$(INTDIR)\utils.obj"

"$(OUTDIR)\Migrate.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "Migrate - Win32 Release" || "$(CFG)" ==\
 "Migrate - Win32 Debug"
SOURCE=.\CreatMig.cpp
DEP_CPP_CREAT=\
	".\CreatMig.h"\
	".\cwstring.h"\
	".\migrant.h"\
	".\navmig95.h"\
	

"$(INTDIR)\CreatMig.obj" : $(SOURCE) $(DEP_CPP_CREAT) "$(INTDIR)"


SOURCE=.\CRegent.cpp
DEP_CPP_CREGE=\
	".\cregent.h"\
	

"$(INTDIR)\CRegent.obj" : $(SOURCE) $(DEP_CPP_CREGE) "$(INTDIR)"


SOURCE=.\CWSTRING.CPP
DEP_CPP_CWSTR=\
	".\cwstring.h"\
	

"$(INTDIR)\CWSTRING.OBJ" : $(SOURCE) $(DEP_CPP_CWSTR) "$(INTDIR)"


SOURCE=.\global.cpp
DEP_CPP_GLOBA=\
	".\cwstring.h"\
	".\global.h"\
	".\migrant.h"\
	

"$(INTDIR)\global.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"


SOURCE=.\migrant.cpp

!IF  "$(CFG)" == "Migrate - Win32 Release"

DEP_CPP_MIGRA=\
	".\cwstring.h"\
	".\global.h"\
	".\migrant.h"\
	".\utils.h"\
	

"$(INTDIR)\migrant.obj" : $(SOURCE) $(DEP_CPP_MIGRA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Migrate - Win32 Debug"

DEP_CPP_MIGRA=\
	".\cwstring.h"\
	".\global.h"\
	".\migrant.h"\
	".\utils.h"\
	

"$(INTDIR)\migrant.obj" : $(SOURCE) $(DEP_CPP_MIGRA) "$(INTDIR)"


!ENDIF 

SOURCE=.\migrate.cpp
DEP_CPP_MIGRAT=\
	".\CreatMig.h"\
	".\cregent.h"\
	".\cwstring.h"\
	".\global.h"\
	".\migrant.h"\
	".\migrate.h"\
	".\registry.h"\
	".\setupapi.h"\
	".\utils.h"\
	

"$(INTDIR)\migrate.obj" : $(SOURCE) $(DEP_CPP_MIGRAT) "$(INTDIR)"


SOURCE=.\migrate.rc

"$(INTDIR)\migrate.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\NavMig95.cpp

!IF  "$(CFG)" == "Migrate - Win32 Release"

DEP_CPP_NAVMI=\
	".\cregent.h"\
	".\cwstring.h"\
	".\global.h"\
	".\migrant.h"\
	".\navmig95.h"\
	".\registry.h"\
	".\utils.h"\
	

"$(INTDIR)\NavMig95.obj" : $(SOURCE) $(DEP_CPP_NAVMI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Migrate - Win32 Debug"

DEP_CPP_NAVMI=\
	".\cregent.h"\
	".\cwstring.h"\
	".\global.h"\
	".\migrant.h"\
	".\navmig95.h"\
	".\registry.h"\
	".\utils.h"\
	

"$(INTDIR)\NavMig95.obj" : $(SOURCE) $(DEP_CPP_NAVMI) "$(INTDIR)"


!ENDIF 

SOURCE=.\utils.cpp
DEP_CPP_UTILS=\
	".\cregent.h"\
	".\cwstring.h"\
	".\utils.h"\
	

"$(INTDIR)\utils.obj" : $(SOURCE) $(DEP_CPP_UTILS) "$(INTDIR)"



!ENDIF 

