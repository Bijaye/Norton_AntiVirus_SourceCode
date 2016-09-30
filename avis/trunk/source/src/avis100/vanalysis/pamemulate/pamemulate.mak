# Microsoft Developer Studio Generated NMAKE File, Based on PAMemulate.dsp
!IF "$(CFG)" == ""
CFG=PAMemulate - Win32 Debug
!MESSAGE No configuration specified. Defaulting to PAMemulate - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "PAMemulate - Win32 Release" && "$(CFG)" !=\
 "PAMemulate - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PAMemulate.mak" CFG="PAMemulate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PAMemulate - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "PAMemulate - Win32 Debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "PAMemulate - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\PAMemulate.exe"

!ELSE 

ALL : "$(OUTDIR)\PAMemulate.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\analysis.obj"
	-@erase "$(INTDIR)\CACHE.OBJ"
	-@erase "$(INTDIR)\CALLFAKE.OBJ"
	-@erase "$(INTDIR)\CPU.OBJ"
	-@erase "$(INTDIR)\DATAFILE.OBJ"
	-@erase "$(INTDIR)\EXCLUDE.OBJ"
	-@erase "$(INTDIR)\EXEC.OBJ"
	-@erase "$(INTDIR)\FLAGS.OBJ"
	-@erase "$(INTDIR)\INSTR.OBJ"
	-@erase "$(INTDIR)\INSTR2.OBJ"
	-@erase "$(INTDIR)\MAIN.OBJ"
	-@erase "$(INTDIR)\OPCODELE.OBJ"
	-@erase "$(INTDIR)\PAGE.OBJ"
	-@erase "$(INTDIR)\PAMAPI.OBJ"
	-@erase "$(INTDIR)\REGION.OBJ"
	-@erase "$(INTDIR)\REPAIR.OBJ"
	-@erase "$(INTDIR)\SEARCH.OBJ"
	-@erase "$(INTDIR)\SIGEX.OBJ"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\PAMemulate.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /ML /W3 /GX /O2 /I "inc" /I "inc\w32" /D "NDEBUG" /D\
 "WIN32" /D "_CONSOLE" /D "_MBCS" /D "REPAIR_DEBUG" /D "BUILD_SET" /D "SYM_WIN"\
 /D "SYM_WIN32" /D "SYM_CPU_X86" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PAMemulate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\PAMemulate.pdb" /machine:I386 /out:"$(OUTDIR)\PAMemulate.exe" 
LINK32_OBJS= \
	"$(INTDIR)\analysis.obj" \
	"$(INTDIR)\CACHE.OBJ" \
	"$(INTDIR)\CALLFAKE.OBJ" \
	"$(INTDIR)\CPU.OBJ" \
	"$(INTDIR)\DATAFILE.OBJ" \
	"$(INTDIR)\EXCLUDE.OBJ" \
	"$(INTDIR)\EXEC.OBJ" \
	"$(INTDIR)\FLAGS.OBJ" \
	"$(INTDIR)\INSTR.OBJ" \
	"$(INTDIR)\INSTR2.OBJ" \
	"$(INTDIR)\MAIN.OBJ" \
	"$(INTDIR)\OPCODELE.OBJ" \
	"$(INTDIR)\PAGE.OBJ" \
	"$(INTDIR)\PAMAPI.OBJ" \
	"$(INTDIR)\REGION.OBJ" \
	"$(INTDIR)\REPAIR.OBJ" \
	"$(INTDIR)\SEARCH.OBJ" \
	"$(INTDIR)\SIGEX.OBJ"

"$(OUTDIR)\PAMemulate.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\PAMemulate.exe" "$(OUTDIR)\PAMemulate.bsc"

!ELSE 

ALL : "$(OUTDIR)\PAMemulate.exe" "$(OUTDIR)\PAMemulate.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\analysis.obj"
	-@erase "$(INTDIR)\analysis.sbr"
	-@erase "$(INTDIR)\CACHE.OBJ"
	-@erase "$(INTDIR)\CACHE.SBR"
	-@erase "$(INTDIR)\CALLFAKE.OBJ"
	-@erase "$(INTDIR)\CALLFAKE.SBR"
	-@erase "$(INTDIR)\CPU.OBJ"
	-@erase "$(INTDIR)\CPU.SBR"
	-@erase "$(INTDIR)\DATAFILE.OBJ"
	-@erase "$(INTDIR)\DATAFILE.SBR"
	-@erase "$(INTDIR)\EXCLUDE.OBJ"
	-@erase "$(INTDIR)\EXCLUDE.SBR"
	-@erase "$(INTDIR)\EXEC.OBJ"
	-@erase "$(INTDIR)\EXEC.SBR"
	-@erase "$(INTDIR)\FLAGS.OBJ"
	-@erase "$(INTDIR)\FLAGS.SBR"
	-@erase "$(INTDIR)\INSTR.OBJ"
	-@erase "$(INTDIR)\INSTR.SBR"
	-@erase "$(INTDIR)\INSTR2.OBJ"
	-@erase "$(INTDIR)\INSTR2.SBR"
	-@erase "$(INTDIR)\MAIN.OBJ"
	-@erase "$(INTDIR)\MAIN.SBR"
	-@erase "$(INTDIR)\OPCODELE.OBJ"
	-@erase "$(INTDIR)\OPCODELE.SBR"
	-@erase "$(INTDIR)\PAGE.OBJ"
	-@erase "$(INTDIR)\PAGE.SBR"
	-@erase "$(INTDIR)\PAMAPI.OBJ"
	-@erase "$(INTDIR)\PAMAPI.SBR"
	-@erase "$(INTDIR)\REGION.OBJ"
	-@erase "$(INTDIR)\REGION.SBR"
	-@erase "$(INTDIR)\REPAIR.OBJ"
	-@erase "$(INTDIR)\REPAIR.SBR"
	-@erase "$(INTDIR)\SEARCH.OBJ"
	-@erase "$(INTDIR)\SEARCH.SBR"
	-@erase "$(INTDIR)\SIGEX.OBJ"
	-@erase "$(INTDIR)\SIGEX.SBR"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\PAMemulate.bsc"
	-@erase "$(OUTDIR)\PAMemulate.exe"
	-@erase "$(OUTDIR)\PAMemulate.ilk"
	-@erase "$(OUTDIR)\PAMemulate.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Zp1 /MLd /W3 /Gm /Zi /Od /I "inc" /I "inc\w32" /D\
 "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "REPAIR_DEBUG" /D "BUILD_SET"\
 /D "SYM_WIN" /D "SYM_WIN32" /D "SYM_CPU_X86" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PAMemulate.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\analysis.sbr" \
	"$(INTDIR)\CACHE.SBR" \
	"$(INTDIR)\CALLFAKE.SBR" \
	"$(INTDIR)\CPU.SBR" \
	"$(INTDIR)\DATAFILE.SBR" \
	"$(INTDIR)\EXCLUDE.SBR" \
	"$(INTDIR)\EXEC.SBR" \
	"$(INTDIR)\FLAGS.SBR" \
	"$(INTDIR)\INSTR.SBR" \
	"$(INTDIR)\INSTR2.SBR" \
	"$(INTDIR)\MAIN.SBR" \
	"$(INTDIR)\OPCODELE.SBR" \
	"$(INTDIR)\PAGE.SBR" \
	"$(INTDIR)\PAMAPI.SBR" \
	"$(INTDIR)\REGION.SBR" \
	"$(INTDIR)\REPAIR.SBR" \
	"$(INTDIR)\SEARCH.SBR" \
	"$(INTDIR)\SIGEX.SBR"

"$(OUTDIR)\PAMemulate.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\PAMemulate.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\PAMemulate.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\analysis.obj" \
	"$(INTDIR)\CACHE.OBJ" \
	"$(INTDIR)\CALLFAKE.OBJ" \
	"$(INTDIR)\CPU.OBJ" \
	"$(INTDIR)\DATAFILE.OBJ" \
	"$(INTDIR)\EXCLUDE.OBJ" \
	"$(INTDIR)\EXEC.OBJ" \
	"$(INTDIR)\FLAGS.OBJ" \
	"$(INTDIR)\INSTR.OBJ" \
	"$(INTDIR)\INSTR2.OBJ" \
	"$(INTDIR)\MAIN.OBJ" \
	"$(INTDIR)\OPCODELE.OBJ" \
	"$(INTDIR)\PAGE.OBJ" \
	"$(INTDIR)\PAMAPI.OBJ" \
	"$(INTDIR)\REGION.OBJ" \
	"$(INTDIR)\REPAIR.OBJ" \
	"$(INTDIR)\SEARCH.OBJ" \
	"$(INTDIR)\SIGEX.OBJ"

"$(OUTDIR)\PAMemulate.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "PAMemulate - Win32 Release" || "$(CFG)" ==\
 "PAMemulate - Win32 Debug"
SOURCE=.\analysis.c

!IF  "$(CFG)" == "PAMemulate - Win32 Release"

DEP_CPP_ANALY=\
	".\analysis.h"\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_ANALY=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

"$(INTDIR)\analysis.obj" : $(SOURCE) $(DEP_CPP_ANALY) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"

DEP_CPP_ANALY=\
	".\analysis.h"\
	".\avendian.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	

"$(INTDIR)\analysis.obj"	"$(INTDIR)\analysis.sbr" : $(SOURCE) $(DEP_CPP_ANALY)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\CACHE.C

!IF  "$(CFG)" == "PAMemulate - Win32 Release"

DEP_CPP_CACHE=\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_CACHE=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	".\symsync.h"\
	

"$(INTDIR)\CACHE.OBJ" : $(SOURCE) $(DEP_CPP_CACHE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"

DEP_CPP_CACHE=\
	".\avendian.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	

"$(INTDIR)\CACHE.OBJ"	"$(INTDIR)\CACHE.SBR" : $(SOURCE) $(DEP_CPP_CACHE)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\CALLFAKE.C

!IF  "$(CFG)" == "PAMemulate - Win32 Release"

DEP_CPP_CALLF=\
	".\avtypes.h"\
	".\callfake.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	
NODEP_CPP_CALLF=\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

"$(INTDIR)\CALLFAKE.OBJ" : $(SOURCE) $(DEP_CPP_CALLF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"

DEP_CPP_CALLF=\
	".\avtypes.h"\
	".\callfake.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	

"$(INTDIR)\CALLFAKE.OBJ"	"$(INTDIR)\CALLFAKE.SBR" : $(SOURCE) $(DEP_CPP_CALLF)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\CPU.C

!IF  "$(CFG)" == "PAMemulate - Win32 Release"

DEP_CPP_CPU_C=\
	".\analysis.h"\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_CPU_C=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

"$(INTDIR)\CPU.OBJ" : $(SOURCE) $(DEP_CPP_CPU_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"

DEP_CPP_CPU_C=\
	".\analysis.h"\
	".\avendian.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	

"$(INTDIR)\CPU.OBJ"	"$(INTDIR)\CPU.SBR" : $(SOURCE) $(DEP_CPP_CPU_C)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\DATAFILE.C

!IF  "$(CFG)" == "PAMemulate - Win32 Release"

DEP_CPP_DATAF=\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_DATAF=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

"$(INTDIR)\DATAFILE.OBJ" : $(SOURCE) $(DEP_CPP_DATAF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"

DEP_CPP_DATAF=\
	".\avendian.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	

"$(INTDIR)\DATAFILE.OBJ"	"$(INTDIR)\DATAFILE.SBR" : $(SOURCE) $(DEP_CPP_DATAF)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\EXCLUDE.C

!IF  "$(CFG)" == "PAMemulate - Win32 Release"

DEP_CPP_EXCLU=\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_EXCLU=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

"$(INTDIR)\EXCLUDE.OBJ" : $(SOURCE) $(DEP_CPP_EXCLU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"

DEP_CPP_EXCLU=\
	".\avendian.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	

"$(INTDIR)\EXCLUDE.OBJ"	"$(INTDIR)\EXCLUDE.SBR" : $(SOURCE) $(DEP_CPP_EXCLU)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\EXEC.C

!IF  "$(CFG)" == "PAMemulate - Win32 Release"

DEP_CPP_EXEC_=\
	".\analysis.h"\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_EXEC_=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

"$(INTDIR)\EXEC.OBJ" : $(SOURCE) $(DEP_CPP_EXEC_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"

DEP_CPP_EXEC_=\
	".\analysis.h"\
	".\avendian.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	

"$(INTDIR)\EXEC.OBJ"	"$(INTDIR)\EXEC.SBR" : $(SOURCE) $(DEP_CPP_EXEC_)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\FLAGS.C

!IF  "$(CFG)" == "PAMemulate - Win32 Release"

DEP_CPP_FLAGS=\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_FLAGS=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

"$(INTDIR)\FLAGS.OBJ" : $(SOURCE) $(DEP_CPP_FLAGS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"

DEP_CPP_FLAGS=\
	".\avendian.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	

"$(INTDIR)\FLAGS.OBJ"	"$(INTDIR)\FLAGS.SBR" : $(SOURCE) $(DEP_CPP_FLAGS)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\INSTR.C
DEP_CPP_INSTR=\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_INSTR=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

!IF  "$(CFG)" == "PAMemulate - Win32 Release"


"$(INTDIR)\INSTR.OBJ" : $(SOURCE) $(DEP_CPP_INSTR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"


"$(INTDIR)\INSTR.OBJ"	"$(INTDIR)\INSTR.SBR" : $(SOURCE) $(DEP_CPP_INSTR)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\INSTR2.C
DEP_CPP_INSTR2=\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_INSTR2=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

!IF  "$(CFG)" == "PAMemulate - Win32 Release"


"$(INTDIR)\INSTR2.OBJ" : $(SOURCE) $(DEP_CPP_INSTR2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"


"$(INTDIR)\INSTR2.OBJ"	"$(INTDIR)\INSTR2.SBR" : $(SOURCE) $(DEP_CPP_INSTR2)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\MAIN.C
DEP_CPP_MAIN_=\
	".\analysis.h"\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_MAIN_=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

!IF  "$(CFG)" == "PAMemulate - Win32 Release"


"$(INTDIR)\MAIN.OBJ" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"


"$(INTDIR)\MAIN.OBJ"	"$(INTDIR)\MAIN.SBR" : $(SOURCE) $(DEP_CPP_MAIN_)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\OPCODELE.C

!IF  "$(CFG)" == "PAMemulate - Win32 Release"


"$(INTDIR)\OPCODELE.OBJ" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"


"$(INTDIR)\OPCODELE.OBJ"	"$(INTDIR)\OPCODELE.SBR" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\PAGE.C
DEP_CPP_PAGE_=\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_PAGE_=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

!IF  "$(CFG)" == "PAMemulate - Win32 Release"


"$(INTDIR)\PAGE.OBJ" : $(SOURCE) $(DEP_CPP_PAGE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"


"$(INTDIR)\PAGE.OBJ"	"$(INTDIR)\PAGE.SBR" : $(SOURCE) $(DEP_CPP_PAGE_)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\PAMAPI.C
DEP_CPP_PAMAP=\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\ident.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_PAMAP=\
	".\cfcover.h"\
	".\drvseg.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	".\symsync.h"\
	

!IF  "$(CFG)" == "PAMemulate - Win32 Release"


"$(INTDIR)\PAMAPI.OBJ" : $(SOURCE) $(DEP_CPP_PAMAP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"


"$(INTDIR)\PAMAPI.OBJ"	"$(INTDIR)\PAMAPI.SBR" : $(SOURCE) $(DEP_CPP_PAMAP)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\REGION.C
DEP_CPP_REGIO=\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\region.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_REGIO=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

!IF  "$(CFG)" == "PAMemulate - Win32 Release"


"$(INTDIR)\REGION.OBJ" : $(SOURCE) $(DEP_CPP_REGIO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"


"$(INTDIR)\REGION.OBJ"	"$(INTDIR)\REGION.SBR" : $(SOURCE) $(DEP_CPP_REGIO)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\REPAIR.C
DEP_CPP_REPAI=\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\ident.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_REPAI=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

!IF  "$(CFG)" == "PAMemulate - Win32 Release"


"$(INTDIR)\REPAIR.OBJ" : $(SOURCE) $(DEP_CPP_REPAI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"


"$(INTDIR)\REPAIR.OBJ"	"$(INTDIR)\REPAIR.SBR" : $(SOURCE) $(DEP_CPP_REPAI)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\SEARCH.C
DEP_CPP_SEARC=\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_SEARC=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

!IF  "$(CFG)" == "PAMemulate - Win32 Release"


"$(INTDIR)\SEARCH.OBJ" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"


"$(INTDIR)\SEARCH.OBJ"	"$(INTDIR)\SEARCH.SBR" : $(SOURCE) $(DEP_CPP_SEARC)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\SIGEX.C
DEP_CPP_SIGEX=\
	".\avendian.h"\
	".\avtypes.h"\
	".\callfake.h"\
	".\cover.h"\
	".\cpu.h"\
	".\datafile.h"\
	".\exclude.h"\
	".\ident.h"\
	".\inc\dbcs_str.h"\
	".\inc\platform.h"\
	".\inc\platwin.h"\
	".\inc\symvxd.h"\
	".\inc\w32\file.h"\
	".\opcode.h"\
	".\page.h"\
	".\pamapi.h"\
	".\pamcache.h"\
	".\prepair.h"\
	".\profile.h"\
	".\proto.h"\
	".\search.h"\
	".\sigex.h"\
	".\types.h"\
	
NODEP_CPP_SIGEX=\
	".\cfcover.h"\
	".\inc\dosvmm.h"\
	".\inc\nlm_str.h"\
	".\inc\os2win.h"\
	".\inc\platmac.h"\
	".\inc\platnlm.h"\
	".\inc\platntk.h"\
	".\inc\platvxd.h"\
	

!IF  "$(CFG)" == "PAMemulate - Win32 Release"


"$(INTDIR)\SIGEX.OBJ" : $(SOURCE) $(DEP_CPP_SIGEX) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PAMemulate - Win32 Debug"


"$(INTDIR)\SIGEX.OBJ"	"$(INTDIR)\SIGEX.SBR" : $(SOURCE) $(DEP_CPP_SIGEX)\
 "$(INTDIR)"


!ENDIF 


!ENDIF 

